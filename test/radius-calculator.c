#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <wgs84.h>

int main(int argc, char *argv[])
{
	if(argc < 3) {
		fprintf(stderr, "Missing arguments. Usage:\n"
			"%s <latitude> <longitude>\n", argv[0]);
		return 1;
	}

	const double to_rad = M_PI / 180.0;

	const double latitude = atof(argv[1]) * to_rad;
	const double longitude = atof(argv[2]) * to_rad;

	const long double wgs84 = wgs84_ellipsoid_geocentric_radius(latitude);
	const double egm96 = egm96_displacement(latitude, longitude);
	printf("WGS84 ellipsoid radius: %Lf km\n"
		"EGM96 displacement: %f m\n"
		"Sum: %Lf km\n", wgs84/1000.0l, egm96,
		(wgs84 + egm96) / 1000.0l);
}
