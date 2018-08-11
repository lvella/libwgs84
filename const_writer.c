/* Based on f477.c, from https://sourceforge.net/projects/egm96-f477-c/
   which is, in turn, based on F477.F, from
   http://earth-info.nga.mil/GandG/wgs84/gravitymod/egm96/egm96.html

   Preprocess the constant files CORCOEF and EGM96 to be included in
   compilation.
*/


#include<stdio.h>
#include<math.h>
#include <float.h>

#define l_value	(65341)
#define _361	(361)

static double cc[l_value + 1], cs[l_value + 1],
    hc[l_value + 1], hs[l_value + 1];

static int nmax;

static void ddump(const char *name, double* v, size_t bytesize)
{
    size_t size = bytesize / sizeof(double);

    char fname[50];
    snprintf(fname, 50, "%s.inc", name);

    FILE *fd = fopen(fname, "w");

    for(size_t j = 0; j < size; ++j) {
	fprintf(fd, "%.*g,\n", DBL_DECIMAL_DIG, v[j]);
    }

    fclose(fd);
}

static void dhcsin(unsigned nmax, double hc[l_value + 1], double hs[l_value + 1])
{
	FILE* f_12 = fopen("EGM96", "rb");	/*potential coefficient file */
	int n, m;
	double j2, j4, j6, j8, j10, c, s, ec, es;
/*the even degree zonal coefficients given below were computed for the
 wgs84(g873) system of constants and are identical to those values
 used in the NIMA gridding procedure. computed using subroutine
 grs written by N.K. PAVLIS*/
	j2 = 0.108262982131e-2;
	j4 = -.237091120053e-05;
	j6 = 0.608346498882e-8;
	j8 = -0.142681087920e-10;
	j10 = 0.121439275882e-13;
	m = ((nmax + 1) * (nmax + 2)) / 2;
	for (n = 1; n <= m; n++)
		hc[n] = hs[n] = 0;
	while (6 ==
	       fscanf(f_12, "%i %i %lf %lf %lf %lf", &n, &m, &c, &s, &ec,
		      &es)) {
		if (n > nmax) {
			continue;
		}
		n = (n * (n + 1)) / 2 + m + 1;
		hc[n] = c;
		hs[n] = s;
	}
	fclose(f_12);

	hc[4] += j2 / sqrt(5);
	hc[11] += j4 / 3;
	hc[22] += j6 / sqrt(13);
	hc[37] += j8 / sqrt(17);
	hc[56] += j10 / sqrt(21);
}

void init_arrays(void)
{
	int ig, i, n, m;
	double t1, t2;
	FILE *f_1 = fopen("CORCOEF", "rb");	/*correction coefficient file:
					   modified with 'sed -e"s/D/e/g"' to be read with fscanf */
	nmax = 360;
	for (i = 1; i <= l_value; i++)
		cc[i] = cs[i] = 0;
	while (4 == fscanf(f_1, "%i %i %lg %lg", &n, &m, &t1, &t2)) {
		ig = (n * (n + 1)) / 2 + m + 1;
		cc[ig] = t1;
		cs[ig] = t2;
	}
/*the correction coefficients are now read in*/
/*the potential coefficients are now read in and the reference
 even degree zonal harmonic coefficients removed to degree 6*/
	dhcsin(nmax, hc, hs);
	fclose(f_1);

	ddump("cc", cc, sizeof(cc));
	ddump("cs", cs, sizeof(cs));
	ddump("hc", hc, sizeof(hc));
	ddump("hs", hs, sizeof(hs));
}

int main(void)
{
	init_arrays();
}
