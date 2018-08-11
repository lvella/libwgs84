#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <wgs84.h>

int main(int argc, char *argv[])
{
	if(argc < 4) {
		fprintf(stderr, "Missing arguments. Usage:\n"
			"%s <latitude> <longitude> <altitude>\n", argv[0]);
		return 1;
	}

	const long double to_rad = M_PI / 180.0l;

	const long double latitude = strtold(argv[1], NULL) * to_rad;
	const long double longitude = strtold(argv[2], NULL) * to_rad;
	const long double altitude = strtold(argv[3], NULL) * to_rad;

	long double point[3];
	wgs84_to_euclidean(latitude, longitude, altitude, point);

	printf("Euclidean coordinates:\nx = %.20Lf\ny = %.20Lf\nz = %.20Lf\n",
		point[0], point[1], point[2]);
}
