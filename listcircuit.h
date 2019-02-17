#ifndef LISTCIRCUIT_H
#define LISTCIRCUIT_H

#include "Circuit.h"



#include <list>
#include <string>

class ListCircuit
{
    public:
        ListCircuit();
        virtual ~ListCircuit();
        ListCircuit *construireCircuits(GraphPt gPt, std::string pointOrigine, double longueurMin, double longueurMax,
                                        std::vector<TypedeChemin> cheminsPermis, bool avecSortieFichier, int nbMaxDblOccur);
        std::string initialiserCircuitsFavoris( GraphPt gPt, std::string nomFicCFavoris);
        std::list<std::string> nomsDesCircuits (GraphPt gPt);
        std::list<std::string> representerCircuitsenChainesTirets(GraphPt gPt);
        Circuit circuitDeRang (int i);
        int nbCircuits();

    protected:

    private:
        std::list<Circuit> m_lstCircuits;

};

#endif // LISTCIRCUIT_H
