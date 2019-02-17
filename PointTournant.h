#ifndef POINTTOURNANT_H
#define POINTTOURNANT_H

#include <string>
#include <vector>
#include <QStandardItemModel>

enum class TypedeChemin {         // peut être Goudron, Route très fréquentée, Chemin Pierre, Chemin Terre, Sentier, (Sable,Herbe pas encore)
    Goudron,
    Route_Frequent,
    Chemin_Pierre,
    Chemin_Terre,
    Sentier
    };


class PointTournant
{
    public:
        PointTournant();
        PointTournant(std::string nomPoint, double latPoint, double LongPoint, int altitude);

        virtual ~PointTournant();

        unsigned int Getm_Counter() { return m_Counter; }
        void Setm_Counter(unsigned int val) { m_Counter = val; }

        void ajoutePointConnecte (std::string nomPt, double dist, int denivMontant, TypedeChemin TdC);

        std::string NomPointTournant() const;
        double LatPointTournant() const;
        double LongPointTournant() const;
        int NbrPointsConnectes() const;
        std::string NomPointConnecte (int rang) const;
        double DistancePointConnecte(int rang) const;
        int gainAltitude(int rang) const;
        TypedeChemin typedeChemin (int rang) const;
        void affichePoint(QStandardItemModel *modele);
        void affichePremPointCnstr(QStandardItemModel *modele, std::map<TypedeChemin, bool> lTDC); // construction circuit : affiche le premier point


    protected:

    private:
        unsigned int m_Counter;
        std::string m_NomduPoint;       // nom du lieu
        double m_Lat, m_Long;           // Latitude et Longitude en degrés décimaux
        int m_altitude;                 // altitude en m
        std::vector<std::string> m_NomPointConnecte ;  // vecteur de tous les points connectés par un chemin direct
        std::vector<double> m_DistancePointConnecte ;   // vecteur parallèle des distances des points connectés
        std::vector<TypedeChemin> m_typeChemin;        // peut être Goudron, Chemin Pierre, Chemin Terre, Sentier
        std::vector<int> m_hausseAltitude;       //en m dénivellé montant

};

#endif // POINTTOURNANT_H
