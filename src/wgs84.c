/* Based on f477.c, from https://sourceforge.net/projects/egm96-f477-c/
   which is, in turn, based on F477.F, from
   http://earth-info.nga.mil/GandG/wgs84/gravitymod/egm96/egm96.html

   Preprocess the constant files CORCOEF and EGM96 to be included in
   compilation.
*/

#include <math.h>

#include "wgs84.h"

// Constants defined on NGA Standard.
// http://earth-info.nga.mil/GandG/update/wgs84/NGA.STND.0036_1.0.0_WGS84.pdf
static const long double flattening = 1.0l / 298.257223563l;
static const long double semimajor = 6378137.0l;

// I don't know what this constant means, taken from original radgra().
// Is close to 2*flattening;
static const double e2 = .00669437999013;

static const unsigned l_value = 65341;
static const unsigned _361 = 361;

static const double cc[] = {
	#include "cc.inc"
};

static const double cs[] = {
	#include "cs.inc"
};

static const double hc[] = {
	#include "hc.inc"
};

static const double hs[] = {
	#include "hs.inc"
};

static double hundu(unsigned nmax, double p[l_value + 1],
		    double sinml[_361 + 1], double cosml[_361 + 1],
		    double gr, double re)
{
	/*constants for wgs84(g873);gm in units of m**3/s**2 */
	const double gm = .3986004418e15, ae = 6378137.;
	double arn, ar, ac, a, b, sum, sumc, sum2, tempc, temp;
	unsigned k;

	ar = ae / re;
	arn = ar;
	ac = a = b = 0;
	k = 3;

	for (unsigned n = 2; n <= nmax; n++) {
		arn *= ar;
		k++;
		sum = p[k] * hc[k];
		sumc = p[k] * cc[k];
		sum2 = 0;

		for (unsigned m = 1; m <= n; m++) {
			k++;
			tempc = cc[k] * cosml[m] + cs[k] * sinml[m];
			temp = hc[k] * cosml[m] + hs[k] * sinml[m];
			sumc += p[k] * tempc;
			sum += p[k] * temp;
		}

		ac += sumc;
		a += sum * arn;
	}
	ac += cc[1] + p[2] * cc[2] +
		p[3] * (cc[3] * cosml[1] + cs[3] * sinml[1]);

	/*add haco=ac/100 to convert height anomaly on the ellipsoid to
	   the undulation;
	   add -0.53m to make undulation refer to the wgs84 ellipsoid. */
	return a * gm / (gr * re) + ac / 100 - .53;
}

static void dscml(double rlon, unsigned nmax, double sinml[_361 + 1],
	   double cosml[_361 + 1])
{
	double a, b;

	a = sin(rlon);
	b = cos(rlon);

	sinml[1] = a;
	cosml[1] = b;
	sinml[2] = 2 * b * a;
	cosml[2] = 2 * b * b - 1;

	for (unsigned m = 3; m <= nmax; m++) {
		sinml[m] = 2 * b * sinml[m - 1] - sinml[m - 2];
		cosml[m] = 2 * b * cosml[m - 1] - cosml[m - 2];
	}
}

/*this subroutine computes  all normalized legendre function
in "rleg". order is always
m, and colatitude is always theta  (radians). maximum deg
is  nmx. all calculations in double precision.
ir  must be set to zero before the first call to this sub.
the dimensions of arrays  rleg must be at least equal to  nmx+1.
Original programmer :Oscar L. Colombo, Dept. of Geodetic Science
the Ohio State University, August 1980
ineiev: I removed the derivatives, for they are never computed here*/
static void legfdn(unsigned m, double theta, double rleg[_361 + 1],
		   unsigned nmx)
{
	double cothet, sithet, rlnn[_361 + 1];

	int nmx1 = nmx + 1, m1 = m + 1, m2 = m + 2, m3 = m + 3, n, n1, n2;

	cothet = cos(theta);
	sithet = sin(theta);

	/*compute the legendre functions */
	rlnn[1] = 1;
	rlnn[2] = sithet * sqrt(3);

	for (n1 = 3; n1 <= m1; n1++) {
		n = n1 - 1;
		n2 = 2 * n;
		rlnn[n1] = sqrt(n2 + 1) / sqrt(n2) * sithet * rlnn[n];
	}

	switch (m) {
	case 1:
		rleg[2] = rlnn[2];
		rleg[3] = sqrt(5) * cothet * rleg[2];
		break;
	case 0:
		rleg[1] = 1;
		rleg[2] = cothet * sqrt(3);
		break;
	}

	rleg[m1] = rlnn[m1];

	if (m2 <= nmx1) {
		rleg[m2] = sqrt(m1 * 2 + 1) * cothet * rleg[m1];
		if (m3 <= nmx1) {
			for (n1 = m3; n1 <= nmx1; n1++) {
				n = n1 - 1;
				if ((!m && n < 2) || (m == 1 && n < 3))
					continue;
				n2 = 2 * n;
				rleg[n1] =
				    sqrt(n2 + 1) / (sqrt(n + m) * sqrt(n - m))
				    * (sqrt(n2 - 1) * cothet * rleg[n1 - 1] -
				     sqrt(n + m - 1) * sqrt(n - m - 1) /
				     sqrt(n2 - 3) * rleg[n1 - 2]);
			}
		}
	}
}

static void to_geocentric(double lat, double lon,
	double slat2, double *rlat, double *re)
{
	double n, t2, x, y, z;
	n = semimajor / sqrt(1.0 - e2 * slat2);
	t2 = n * cos(lat);
	x = t2 * cos(lon);
	y = t2 * sin(lon);
	z = (n * (1.0 - e2)) * sin(lat);

	*re = sqrt(x * x + y * y + z * z);	/*compute the geocentric radius */
	*rlat = atan(z / sqrt(x * x + y * y));	/*compute the geocentric latitude */
}

static double gravity(double slat2)
{
	const double G = 9.7803253359, k = .00193185265246;

	/*compute normal gravity:units are m/sec**2 */
	return G * (1.0 + k * slat2) / sqrt(1.0 - e2 * slat2);
}

/* This subroutine computes geocentric distance to the point,
   the geocentric latitude, and an approximate value of normal
   gravity at the point based the constants of the wgs84(g873)
   system are used */
static void radgra(double lat, double lon, double *rlat, double *gr, double *re)
{
	const double slat2 = sin(lat) * sin(lat);
	to_geocentric(lat, lon, slat2, rlat, re);
	*gr = gravity(slat2);
}

/* Geoid displacement from reference ellipsis. */
double egm96_displacement(double rad_lat, double rad_lon)
{
	const int nmax = 360;
	const int k = nmax + 1;

	double p[l_value + 1], sinml[_361 + 1],
		cosml[_361 + 1], rleg[_361 + 1];

	double rlat, gr, re;
	int i, j, m;

	radgra(rad_lat, rad_lon, &rlat, &gr, &re);
	rlat = M_PI / 2 - rlat;
	for (j = 1; j <= k; j++) {
		m = j - 1;
		legfdn(m, rlat, rleg, nmax);
		for (i = j; i <= k; i++) {
			p[(i - 1) * i / 2 + m + 1] = rleg[i];
		}
	}
	dscml(rad_lon, nmax, sinml, cosml);
	return hundu(nmax, p, sinml, cosml, gr, re);
}

long double wgs84_ellipsoid_geocentric_radius(long double rad_lat)
{
	// We calculate the radius of a normalized ellipse, so a = 1,
	// and b is the is the ratio of semi-major over semi-minor:
	const long double b = 1.0l - flattening;

	// The radius, as given by https://planetcalc.com/7721/:
	const long double x = cosl(rad_lat);
	const long double y = b * sinl(rad_lat);

	return semimajor * sqrtl(
		(x*x + b*b*y*y) / (x*x + y*y)
	);
}

void wgs84_to_euclidean(long double rad_lat, long double rad_lon,
	long double height_from_geoid, long double point[3])
{
	// Formula taken from Wikipedia:
	// https://en.wikipedia.org/wiki/Reference_ellipsoid#Coordinates

	// Calculate normalized heigh above ellipsoid, so semimajor is 1.0:
	long double h = (egm96_displacement(rad_lat, rad_lon)
		+ height_from_geoid) / semimajor;

	// b is the is the ratio of semi-major over semi-minor:
	const long double b = 1.0l - flattening;

	const long double n = 1.0l / sqrtl(cosl(rad_lat) * cosl(rad_lat)
		+ b * b * sinl(rad_lat) * sinl(rad_lat));

	point[0] = ((n + h) * cosl(rad_lat) * cosl(rad_lon)) * semimajor;
	point[1] = ((n + h) * cosl(rad_lat) * sinl(rad_lon)) * semimajor;
	point[2] = ((b*b*n + h) * sinl(rad_lat)) * semimajor;
}
