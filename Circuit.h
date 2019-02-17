#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <list>
#include <string>

#include "GraphPt.h"
#include <QGraphicsScene>
#include <QGraphicsTextItem>


class Circuit
{
    public:
        Circuit();
        Circuit(std::list<int>, GraphPt gPt);  // construit un circuit avec la liste de numéros de points
        virtual ~Circuit();
        void exporterCircuit(GraphPt gpt, std::string nomFichierCircuits);
        std::string chaineTiretCircuit(GraphPt gPt, int numero);
        int nbrOccurence(int n); // calcule le nombre de fois ou l'on trouve le point numéro n
        int nbrOccurence2(int n); // calcule le nombre de fois ou l'on trouve le point numéro n, avec la fonction count (ce nombre sera limité à un Max fixé à 2 pour permettre les allez-retour
        int nbrDoublesOccurences(int n); // calcule le nombre de doublets
        bool hoquetFinal(int n);  // tweste si les 4 derniers points quand on ajoute n font le hoquet
        bool pointe(int n); // teste si en ajoutant n on forme une pointe m-a-b-a-n
        bool pointeOuTriangleInverse(int n); // teste la séquence x-a-b-c-b-a-y ou xabcacby ou xabcdcbay
        bool dblpaire(int n);     // teste si on retrouve la combinaison a-b 2 fois en ajoutant n
        double augmenteLongueur (double longueurTroncon); // ajoute longueur Troncon à la longueur du circuit et reourne cette nouvelle longueur.
        int augmenteDenivele (int gainAlt);   // ajoute gainAlt au cumul des gain altitude du circuit et retourne cette nouvelle valeur
        int ajoutePt (int); //ajoute un point par son rang dans le graphe et retourne le nombre de points
        void ajouteNom(std::string nomCircuitFavori);           //  ajoute le nom du circuit
        std::string nomDuCircuit();                 // retourne le nom du circuit
        int ajoutePtDistGainAlt (int, GraphPt);  // ajoute un point et augmente longueur et gain altitude du circuit
        int enleverDernierPoint (GraphPt gPt);
        int dernierPt(); // retourne le numéro de Point Tournant dans le Graphe (= rang dans le fichier)
        int premPt();      // retourne le numéro du premier point
        int nbPoints();    // retourne le nombre de points du circuit
        double *minetmaxLatetLong(GraphPt gPt); //(en fait crée un tableau de 4 élémets) contenant les min et max des lat et long du circuit

        void dessineCircuit(GraphPt gPt, QGraphicsScene *scene);

        int affichePtSuivCnst(QStandardItemModel *modele, std::map<TypedeChemin, bool> lTDC, GraphPt *gPt); // affiche chemin en construction et connexions au dernier point

protected:

    private:
        std::list<int> m_lstPt;   // contient une liste de points repérés par leur rang dans Graph
        double m_longueurCircuit = 0;  //et la longueur du chemin ainsi défini
        int m_gainAltritude =0;      // et le cumul des dénivellés
        int m_nbPt = 0;     // nombre de points du circuit
        std::string m_nomCircuit;  // nom du circuit pour les parcours traditionels baptisés

};

#endif // CIRCUIT_H
