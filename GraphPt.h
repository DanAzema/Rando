#ifndef GRAPHPT_H
#define GRAPHPT_H

#include <string>
#include <vector>
#include <map>
#include "PointTournant.h"

class GraphPt
{
    public:
        GraphPt();
        virtual ~GraphPt();
        std::string initialiserGr(std::string nomFichierDesPt);
        void initialiserMapsDistTdc();              // crée les valeurs des map de distance et de Type de chemin pour les points connectés
        std::string parsePointsConnectes(std::string ligne, PointTournant& pt);
        std::string listeNomsDesPointsTournants (std::string fichierNomdesPoints);
        std::string verifierCompletude();                  // vérifie que tout point connecté existe bien dans l'ensemble des points tournants
        std::string verifierCompletudeTotale();            // vérifie points connectés symétriques
        int trouveRangPoint(std::string NomPt);         // recherche un point par son nom et renvoie son rang ou -1 si non trouvé
        void exporterMap(std::string fichierMapdesPoints);  // écrit la Map dans un fichier (pour vérification)
        int indexDansGrapheduPt (std::string nomduPoint); // renvoie l'index d'un Pt grace à la map
        int nbPointsTournants();                           // renvoie le nombre de Pt grace à la taille du vecteur
        void dijkstra(unsigned int source, std::map<TypedeChemin,bool>);                      // construit vle tableqau des distances etb prédecesseurs pour le point source
        std::list<int> plusCourtChemin (int source, int dest);         // retourne la liste des points de source à dest
        std::list<int> plusCourtCheminSensRetour (int source, int dest);  // retourne le chemin dans l'ordre inverse
        double distanceCourtChemin (int source, int dest);          // retourne la distance au plus court
        int infini = 7777;                                 // valeur infini pour algo du oplus court chemin (une grande valeur quelconque supérieure à toute distance possible)


        PointTournant pointT(int rang);             // renvoie le point T de rang demandé (rang dans le vecteur)

    protected:

    private:
        std::vector<PointTournant> m_pointT;
        std::map<std::string,int> m_indexPoint;     // map permettant de retrouver le rang du point par son nom
        std::vector<double> m_etiquettes;      // tbleau stockant les étiquesttes des sommets pour algorithme de Dijkstra du plus court chemein
        std::vector<int> m_predecesseurs;    // stocke les valeurs de prédÉcesseurs pour retrouver le plus court chemin
        std::vector<bool> m_marques;         // stocke le marquage des sommets pour recherche plus court
        std::vector<std::map<int,double>> m_arcsDist;  //stocke le graphe avec pour chaque index de point une map donnant la distance pour chaque point connecté
        std::vector<std::map<int,TypedeChemin>> m_arcsTdc;      // stocke pour chaque point du graphe une map des types de chemein des points connectés

        int m_source=-1;            //contient le point source pour le calcul du plus court chemin, ou -1 si calcul non fait
        std::map<TypedeChemin,bool> m_mapTypCh;  // contient la map de Types de XChemins pour laquelle Dijstra a été calculé
        /*
         * A faire : ajouter une table des arêtes : cela pourraut être une map indexé par la npaire de sommets et avec pour valeur
         *      - la distance
         *      -le gain altitude
         *      -le type de chemin.
         *
         *
         *
         *
         */

};

#endif // GRAPHPT_H
