#include "listcircuit.h"

#include <fstream>
#include <vector>

ListCircuit::ListCircuit()
{
    //ctor
}

ListCircuit::~ListCircuit()
{
    //dtor
}

ListCircuit* ListCircuit::construireCircuits(GraphPt gPt, std::string pointOrigine, double longueurMin,double longueurMax,
                                             std::vector<TypedeChemin> cheminsPermis,bool avecSortieFichier=false, int nbMaxDoublesOccur=0 )
{
    int maxNbrCircuits(10000);
    int nbCasTropLong (0);
    int indexPointOrigine(-1);
    const int nbMaxOccur(2);
    double longueurCourante(0);
    int gainAltCourant(0);
    int indexDansGraph, indexPtConn;
    Circuit crct,crctCourant;
    ListCircuit* lstCircTrouv = new ListCircuit;
    int nbCircuitTrouves (0);

    TypedeChemin typDC;
    std::vector<TypedeChemin>::iterator itTdC;
    bool bonTypedeChemin(false);

    std::string nomFichierCircuits (u8"Circuits-Trouv-"+std::to_string(longueurMin).substr(0,4)+std::to_string(longueurMax).substr(0,4)+".txt");
    if (avecSortieFichier){

        std::ofstream fluxC (nomFichierCircuits.c_str());
        if(fluxC)
        {
            fluxC << "Circuits entre " << longueurMin << " et " << longueurMax << " km" << std::endl;
            fluxC << "Nature de chemins :" ;
            for (itTdC = cheminsPermis.begin(); itTdC != cheminsPermis.end(); ++itTdC)
            {
                switch (*itTdC)
                {
                case TypedeChemin::Goudron :
                    fluxC << "Goudron " ; break;
                case TypedeChemin::Chemin_Pierre :
                    fluxC << u8"Chemin empierré "; break;
                case TypedeChemin::Chemin_Terre :
                    fluxC << " Chemin de Terre " ; break;
                case TypedeChemin::Sentier :
                    fluxC <<  " Sentier "; break;
                case TypedeChemin::Route_Frequent :
                    fluxC << " Route Fréquentée"; break;
                default :
                    break;
                }
            }
            fluxC << std::endl;
        }
    }

    indexPointOrigine = gPt.indexDansGrapheduPt(pointOrigine);
    if (indexPointOrigine == -1)
        return lstCircTrouv;
    crct.ajoutePt(indexPointOrigine);

    m_lstCircuits.push_back(crct);


    while ( ! m_lstCircuits.empty())            // on boucle tant que la liste n'est pas vide
    {
        if(nbCircuitTrouves>maxNbrCircuits)     // on arrête si on en a déjà trop (limite cause temps de calcul et occupation mémoire)
            return lstCircTrouv;
        crct = m_lstCircuits.back();            // prend le dernier parcours en cours de la liste et le stocke
        m_lstCircuits.pop_back();                // avant de l'enle3v3er de la liste
        indexDansGraph = crct.dernierPt();          // on va partir du dernier point du circuit pour aller plus loin
        for (int i=0; i<gPt.pointT(indexDansGraph).NbrPointsConnectes(); i++)  // pour chacun des Points connectés au dernier point du circuit
        {

            indexPtConn = gPt.indexDansGrapheduPt(gPt.pointT(indexDansGraph).NomPointConnecte(i));
            crctCourant = crct;
            crctCourant.ajoutePt(indexPtConn);
            longueurCourante = crctCourant.augmenteLongueur(gPt.pointT(indexDansGraph).DistancePointConnecte(i));
            gainAltCourant = crctCourant.augmenteDenivele(gPt.pointT(indexDansGraph).gainAltitude(i));
            typDC = gPt.pointT(indexDansGraph).typedeChemin(i);
            bonTypedeChemin=false;
            for (itTdC = cheminsPermis.begin(); itTdC != cheminsPermis.end();++itTdC)
            {
                if (*itTdC == typDC) bonTypedeChemin=true;
            }

            //itTdC = find(cheminsPermis.begin(), cheminsPermis.end(),typDC);
            //if (std::find(cheminsPermis.begin(), cheminsPermis.end(),typDC) != cheminsPermis.end())  // si le type de chemin n'est pas demandé
            //    break;
            if (!bonTypedeChemin)
            {}
            else if (crctCourant.nbrOccurence2(indexPtConn)>nbMaxOccur || longueurCourante>longueurMax)
            {}
            //else if (crctCourant.hoquetFinal(indexPtConn))    // dblpaire détecte la séquence final i-j-i-j donc hoquet inutile
            //{}
            else if (crctCourant.pointe(indexPtConn))
            {}
            else if (crctCourant.pointeOuTriangleInverse(indexPtConn))
            {}
            else if (crctCourant.dblpaire(indexPtConn))
            {}
            else if (crctCourant.nbrDoublesOccurences(indexPtConn) > nbMaxDoublesOccur )
            {}
            else if (gPt.distanceCourtChemin(indexPointOrigine, indexPtConn) + longueurCourante > longueurMax)   // teste si on peut revenir sans dépasser la longueur max
            {
              nbCasTropLong++;
            }
            else if (indexPtConn == indexPointOrigine)
            {
                if (longueurCourante<longueurMin)
                {}
                else
                {

                    nbCircuitTrouves +=1;
                    //circuit trouvé à exporter
                    if(avecSortieFichier) crctCourant.exporterCircuit(gPt,nomFichierCircuits);
                    lstCircTrouv->m_lstCircuits.push_back(crctCourant);
                }

            }
            else
            {
                m_lstCircuits.push_back(crctCourant);
            }

        }
    }
    return lstCircTrouv;
}

std::string ListCircuit::initialiserCircuitsFavoris(GraphPt gPt, std::string nomFicCFavoris)
{
    std::string diagnostic;
    std::string nomPointX ("vide");
    std::string nomCircuitFavori;
    int nbPF(0);        // pour compter les parcours favoris
    int indexPt;

    std::ifstream fichier(nomFicCFavoris.c_str(), std::ios::in);
    if(fichier) {

        std::string ligne; //Une variable pour stocker les lignes lues
        std::string debLigne; // pour stocker les 2 premiers car de la ligne pour détection des commentaires
        std::string::size_type pos,pos1,pos2; //index sur la ligne

        while(getline(fichier, ligne)) {        //Tant qu'on n'est pas à la fin, on lit ligne par ligne

            pos = ligne.find("//");
            debLigne = ligne.substr(0,2);
            if (pos==std::string::npos) {

                // ce n'est pas  une ligne de commentaire
                Circuit *crctCourant = new Circuit;
                pos1 = ligne.find("<");
                if (pos1!=std::string::npos)
                    pos2 = ligne.find(">");
                else
                    pos2= std::string::npos;

                if (pos2 != std::string::npos){
                    nomCircuitFavori = ligne.substr(pos1+1,pos2-pos1-1) ;
                    crctCourant->ajouteNom(nomCircuitFavori);
                }
                if  ((pos1==std::string::npos) || (pos2==std::string::npos)){
                    diagnostic += "Erreur : Pas de Nom de Parcours Favori pour le candidat :" + std::to_string(nbPF) + " =" + ligne +" \n";
                    return diagnostic;
                }
                do {
                    ligne = ligne.substr(pos2+1);
                    pos1 = ligne.find("<");
                    if (pos1!=std::string::npos)
                        pos2 = ligne.find(">");
                    else
                        pos2= std::string::npos;

                    if (pos2 != std::string::npos){
                        nomPointX = ligne.substr(pos1+1,pos2-pos1-1) ;
                        indexPt = gPt.trouveRangPoint(nomPointX);
                        if (indexPt!=-1)
                            crctCourant->ajoutePtDistGainAlt(indexPt,gPt);
                        else {
                            diagnostic += "Un des points du circuit " + nomFicCFavoris + "n'existe pas";
                        }

                    }


                }
                while (pos2!= std::string::npos);
                // on a extrait tous les points possibles
                if(crctCourant->nbPoints()>0) {
                    m_lstCircuits.push_back(*crctCourant);
                }
            }

        }
        // fin du fichier

    }
    else {
        diagnostic += "Impossible ouvrir fichier des Circuits Favoris \n" ;
    }

    return diagnostic;
}

std::list<std::string> ListCircuit::nomsDesCircuits(GraphPt gPt)
{
    std::list<std::string> lstNomsC;
    Circuit crctCourant;
    int numeroCircuit(0);
    std::string nomF;
    for(std::list<Circuit>::iterator it=m_lstCircuits.begin(); it!= m_lstCircuits.end(); ++it){
       crctCourant = *it;
       numeroCircuit++;
       nomF =  crctCourant.nomDuCircuit();
       lstNomsC.push_back(nomF);
    }
    return lstNomsC;
}

std::list<std::string> ListCircuit::representerCircuitsenChainesTirets(GraphPt gPt)
{
    std::list<std::string> lstCrctTirets;
    std::string chaineCrctCourant;
    int numeroCircuit(0);
    Circuit crctCourant;
    for(std::list<Circuit>::iterator it=m_lstCircuits.begin(); it!= m_lstCircuits.end(); ++it){
       crctCourant = *it;
       numeroCircuit++;
       chaineCrctCourant =  crctCourant.chaineTiretCircuit(gPt,numeroCircuit);
       lstCrctTirets.push_back(chaineCrctCourant);
    }


    return lstCrctTirets;
}

Circuit ListCircuit::circuitDeRang (int rang){
    std::list<Circuit>::iterator it = m_lstCircuits.begin();
    for (int i = 0; i<rang; i++)
        it++;
    return *it;
}

int ListCircuit::nbCircuits()
{
    return int(m_lstCircuits.size());
}
