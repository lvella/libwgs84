/* Emulates f477.c behavior, by reading INPUT.DAT and outputig OUTF477.DAT */

#include <stdio.h>
#include <math.h>

#include "../wgs84.h"

int main()
{
	const double rad = M_PI / 180.0;
	FILE *in = fopen("INPUT.DAT", "r");
	FILE *out = fopen("OUTF477.DAT", "w");

	double flat, flon;

	while (2 == fscanf(in, "%lg %lg", &flat, &flon)) {
		double u = egm96_displacement(flat * rad, flon * rad);
		fprintf(out, "%14.7f %14.7f %10.7f\n", flat, flon, u);
	}

	fclose(in);
	fclose(out);
}
