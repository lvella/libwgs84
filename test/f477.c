/* Emulates f477.c behavior, by reading INPUT.DAT and outputig OUTF477.DAT */

#include <stdio.h>
#include <math.h>

#include <wgs84.h>

int main(int argc, char *argv[])
{
	if(argc < 2) {
		fprintf(stderr, "Missing argument. Usage:\n"
			"%s INPUT.DAT [OUTF477.DAT]\n", argv[0]);
		return 1;
	}

	const char *outfname;
	if(argc > 2) {
		outfname = argv[2];
	} else {
		outfname = "OUTF477.DAT";
	}

	const double rad = M_PI / 180.0;
	FILE *in = fopen(argv[1], "r");
	FILE *out = fopen(outfname, "w");

	double flat, flon;

	while (2 == fscanf(in, "%lg %lg", &flat, &flon)) {
		double u = egm96_displacement(flat * rad, flon * rad);
		fprintf(out, "%14.7f %14.7f %10.7f\n", flat, flon, u);
	}

	fclose(in);
	fclose(out);
}
