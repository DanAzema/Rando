#include "PointTournant.h"
#include <QString>
#include <QStandardItem>
#include <QDebug>

using namespace std;

// Méthodes

PointTournant::PointTournant()
{
    //ctor
}
PointTournant::PointTournant(std::string nomPoint, double latPoint, double longPoint, int altitude) :
        m_NomduPoint(nomPoint), m_Lat(latPoint), m_Long (longPoint), m_altitude(altitude)
{
}

PointTournant::~PointTournant()
{
    //dtor
}

void PointTournant::ajoutePointConnecte (std::string nomPt, double dist, int denivMontant, TypedeChemin TdC)
{
    PointTournant::m_NomPointConnecte.push_back(nomPt);
    PointTournant::m_DistancePointConnecte.push_back(dist);
    PointTournant::m_hausseAltitude.push_back(denivMontant);
    PointTournant::m_typeChemin.push_back(TdC);
}


std::string PointTournant::NomPointTournant() const
{
    return m_NomduPoint;
}

double PointTournant::LatPointTournant () const
{
    return m_Lat;
}


double PointTournant::LongPointTournant() const
{
    return m_Long;
}


double PointTournant::DistancePointConnecte(int rang) const
{
    return m_DistancePointConnecte[unsigned (rang)];
}

int PointTournant::gainAltitude(int rang) const
{
    return m_hausseAltitude[unsigned (rang)];
}


string PointTournant::NomPointConnecte(int rang) const
{
    return m_NomPointConnecte[unsigned(rang)];
}

int PointTournant::NbrPointsConnectes() const
{
    return int(m_DistancePointConnecte.size());
}

TypedeChemin PointTournant::typedeChemin (int rang) const
{
    return m_typeChemin[unsigned(rang)];
}

void PointTournant::affichePoint(QStandardItemModel *modelePoint)
{
    QStandardItem *parentItm = modelePoint->invisibleRootItem();

    QStandardItem *pointT = new QStandardItem(QString::fromStdString(m_NomduPoint));
    parentItm->appendRow(pointT);
    QStandardItem *coord = new QStandardItem( "Coordonnées");
    pointT->appendRow(coord);

    QStandardItem *latitude = new QStandardItem ("Latitude");
    QStandardItem *valLatitude = new QStandardItem(QLocale().toString( m_Lat,'f',6));
    coord->appendRow({latitude,valLatitude});

    QStandardItem *longitude = new QStandardItem("Longitude");
    QStandardItem *valLongitude = new QStandardItem(QLocale().toString( m_Long,'f',6));
    coord->appendRow({longitude,valLongitude});

    QStandardItem *altitude = new QStandardItem("Altitude");
    QStandardItem *valAltitude = new QStandardItem(QLocale().toString( m_altitude));
    coord->appendRow({altitude,valAltitude});

    QStandardItem *connexions = new QStandardItem("Connexions");
    pointT->appendRow(connexions);

    for (unsigned int i = 0;i<m_NomPointConnecte.size();i++){
        QStandardItem *nom = new QStandardItem(QString::fromStdString(m_NomPointConnecte[i]));
        QStandardItem *distance = new QStandardItem(QLocale().toString( m_DistancePointConnecte[i],'f',2));
        QStandardItem *gainAlt = new QStandardItem(QLocale().toString(m_hausseAltitude[i]));
        TypedeChemin tdc = m_typeChemin[i];
        QString qTdc;
        switch (tdc) {
            case TypedeChemin::Goudron :
                qTdc = "Goudron";
                break;
            case TypedeChemin::Sentier :
                qTdc ="Sentier";
                break;
            case TypedeChemin::Chemin_Terre :
                qTdc ="Ch Terre";
                break;
            case TypedeChemin::Chemin_Pierre :
                qTdc = "Ch Pierre";
                break;
            case TypedeChemin::Route_Frequent :
                qTdc = "Route Fréquentée";
                break;

        }
        //qDebug() << "tdc = " << qTdc;
        QStandardItem *typChemin = new QStandardItem(qTdc);
        //typChemin->setCheckable(true);
        connexions ->appendRow({nom,distance,typChemin,gainAlt});
    }

    /*
    QModelIndex iNomPt = modelePoint->index(0,0);
    qDebug() << "valeur index(0,0)" << modelePoint->data(iNomPt);
    QModelIndex iCoord = modelePoint->index(0,0,iNomPt);
    qDebug() << "valeur index(1,0,iNomPt)" << modelePoint->data(iCoord);
    QModelIndex iConnx = modelePoint->index(1,0,iNomPt);
    QModelIndex iDist = modelePoint->index(0,1,iConnx);
    qDebug() << "valeur index(0,1,iDist)" << modelePoint->data(iDist);
    */

}

void PointTournant::affichePremPointCnstr(QStandardItemModel *modele, std::map<TypedeChemin, bool> lTDC)
{
    modele->clear();
    modele->setColumnCount(2);
    QStandardItem *parentItm = modele->invisibleRootItem();
    QStandardItem *nomPointT = new QStandardItem(QString::fromStdString(m_NomduPoint));
    nomPointT->setFlags(Qt::ItemIsEnabled);
    parentItm->appendRow(nomPointT);


    for (unsigned int i = 0;i<m_NomPointConnecte.size();i++){
        QStandardItem *nom = new QStandardItem(QString::fromStdString(m_NomPointConnecte[i]));
        TypedeChemin tdc = m_typeChemin[i];
        if (lTDC[tdc]){
            QStandardItem *distance = new QStandardItem(QLocale().toString( m_DistancePointConnecte[i],'f',2));
            QStandardItem *gainAlt = new QStandardItem(QLocale().toString(m_hausseAltitude[i]));
            nomPointT->appendRow({nom,distance,gainAlt});
        }
    //qDebug() << " Item du modele construit : " << nomPointT;
    //qDebug()<< " Item au rang 0 : "<< modele->item(0);
    }
}



