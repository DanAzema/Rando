#include "fonctionsgeo.h"
#include <cmath>

#define pi 3.14159265358979323846

double deg2rad(double deg)  //Convertit desa degrés décimaux en radians
{
    return (deg * pi / 180);
}


double rad2deg(double rad)  // convertit radians en degrés décimaux
{
    return (rad * 180 / pi);
}


/*::  Paramètres de la fonction :
        lat1, lon1 = Latitude et Longitude du point 1 (en degrés decimaux)
        lat2, lon2 = Latitude et Longitude du point 2 (en degrés decimaux)
      Latitudes Sud negatives,  longitudes Est positives
*/
/* La formule utilisée pour déterminer la distance la plus courte entre deux points sur le terrain (géodésique),
se rapproche du géoïde à une sphère de rayon R = 6372.795477598 km (rayon moyen quadrique),
de sorte que le calcul aurait pu une erreur de distance de 0,3%, en particulier dans les situations extrêmes, et pour de longues distances par le biais de divers parallèles.
Étant donné deux points A et B sur la sphère exprimé par la latitude (LAT) et la longitude (LON), vous devrez:
la distance (A, B) = R * arccos (sin (LATA) * sin (LATB) + cos (LATA) * cos (LATB) * cos (LonA-LonB))
Les angles utilisés sont exprimés en radians, la conversion entre les degrés et les radians est obtenue en multipliant l’angle par pi et en divisant par 180.
*/

double distance(double lat1, double lon1, double lat2, double lon2)
{
    double theta, dist;
    if ((lat1 == lat2) && (lon1 == lon2))
    {
        return 0;
    }
    else
    {
        theta = lon1 - lon2;
        dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));
        dist = acos(dist);
        dist = 6372.795477598 * dist; // rayon de la sphère en km
    }
    return (dist);
}

double distanceh(double lat1, double lon1, double lat2, double lon2)

/*
On peut déduire de la formule de Haversine pour calculer la distance d entre 2 points :

a = sin²(ΔlatDifference/2) + cos(lat1).cos(lt2).sin²(ΔlonDifference/2)
d = R.2.atan2(√a, √(1−a))

ΔlatDifference = lat1 – lat2 (difference de latitude)
ΔlonDifference = lon1 – lon2 (difference dew longitude)

R est le rayon de la terre  (6371 KM ou 6372,795)
*/

{
    double deltaLong, deltaLat, a,dist;
    if ((lat1 == lat2) && (lon1 == lon2))
    {
        return 0;
    }
    else
    {
        lat1 = deg2rad(lat1);
        lat2 = deg2rad(lat2);
        lon1 = deg2rad(lon1);
        lon2 = deg2rad(lon2);
        deltaLong = lon1 - lon2;
        deltaLat = lat1 - lat2;
        a = sin(deltaLat/2)*sin(deltaLat/2) + cos(lat1)*cos(lat2)*sin(deltaLong/2)*sin(deltaLong/2);
        dist = 2*atan2(sqrt(a),sqrt(1-a));

        dist = 6372.795477598 * dist; // rayon de la sphère en km
    }
    return (dist);
}


/*
Comment calculer la direction sachant 2 points ?

La formule pour calculer un azimut est azimuth=atan2(y,x)
avec x=cos(lat1)∗sin(lat2)−sin(lat1)∗cos(lat2)∗cos(long2−long1) et y=sin(long2−long1)∗cos(lat2)

L'angle est calculé en prenant pour base, le pole nord géographique.
*/

double direction (double lat1, double lon1, double lat2,double lon2)
{
    double x,y;
    double direction(0);
    x = cos (deg2rad(lat1)) * sin(deg2rad(lat2)) - sin(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(lon2) - deg2rad(lon1));
    y = sin(deg2rad(lon2)- deg2rad(lon1)) * cos(deg2rad(lat2));
    direction = atan2(y,x);
    direction = rad2deg(direction);
    if (direction < 0)
    {
        direction += 360;
    }
    return direction;

}
