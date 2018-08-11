# libwgs84

A simple library to calculate geoid displacement and other information from GPS coordinates.

There are many GPS coordinate systems, this library handles specifically the 1996 revision
of [WGS-84][2], which includes the geoid displacement calculation [EGM-96][3]. See [here][1] for a good introduction on the subject.

This is heavily based on [egm96-f477.c][4], which is, by itself, a direct
translation to C language of the original Fortran code found [here][3].

[1]: http://www.esri.com/news/arcuser/0703/geoid1of3.html
[2]: http://earth-info.nga.mil/GandG/publications/tr8350.2/tr8350_2.html
[3]: https://cddis.nasa.gov/926/egm96/egm96.html
[4]: https://sourceforge.net/projects/egm96-f477-c/