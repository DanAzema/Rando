#ifndef FONCTIONSGEO_H
#define FONCTIONSGEO_H

double deg2rad(double deg);   //Convertit desa degrés décimaux en radians
double rad2deg(double rad);   // convertit radians en degrés décimaux
double distance(double lat1, double lon1, double lat2, double lon2); // distance geo entre 2 points avec longitude et latitude en degrés décimaux
double distanceh(double lat1, double lon1, double lat2, double lon2); // idem avec formule havertine
double direction (double lat1, double lon1, double lat2,double lon2); // donne le cap géographique du point 1 au point 2


#endif // FONCTIONSGEO_H
