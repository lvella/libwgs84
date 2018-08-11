#pragma once

double egm96_displacement(double radians_latitude, double radians_longitude);

long double wgs84_ellipsoid_geocentric_radius(long double radians_latitude);

void wgs84_to_euclidean(long double rad_lat, long double rad_lon,
	long double height_from_geoid, long double point[3]);
