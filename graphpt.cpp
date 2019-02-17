#include "GraphPt.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

GraphPt::GraphPt()
{
    //ctor
}

GraphPt::~GraphPt()
{
    //dtor
}

PointTournant GraphPt::pointT(int rang)
{
    if (rang < int(m_pointT.size()) && rang>=0)
        return m_pointT[unsigned(rang)];
    else
        return PointTournant();
}

string GraphPt::parsePointsConnectes (string ligne, PointTournant& pt)
{
    string diagnostic;
    stringstream ss(ligne);
    string temp ;
    string nomPt;
    double dst (-1);
    int denivMontant (0);
    TypedeChemin tdc(TypedeChemin::Goudron);
    char lettreTypedeChemin('G');

    while (std::getline(ss,temp,'<') && ss.tellg()>=0 )
    {
        // getline(ss, temp, '<'); // positionne au prochain chevron ou signale la fin de la liogne

        std::getline(ss, nomPt, '='); // prend le nom du point jusqu'au signe =

        std::getline(ss, temp, ':'); // puis prend la distance sous forme chaine de ce poin connecté jusqu'au ":"
        dst = std::stod (temp);   // transforme le string en double

        std::getline(ss, temp, ';'); // puis prend le dénivellé montant sous forme chaine de ce point connecté jusqu'au ";"
        denivMontant = std::stoi (temp);   // transforme le string en entier

        std::getline(ss, temp, '>'); // puis prend le type de chemin jusqu'au ">"
        lettreTypedeChemin = temp.at(0);
        switch(lettreTypedeChemin)
        {
        case 'G':
            tdc=TypedeChemin::Goudron;
            break;
        case 'P':
            tdc = TypedeChemin::Chemin_Pierre;
            break;
        case 'T':
            tdc = TypedeChemin::Chemin_Terre;
            break;
        case 'S':
            tdc = TypedeChemin::Sentier;
            break;
        case 'F':
            tdc = TypedeChemin::Route_Frequent ;
            break;
        default:
            tdc = TypedeChemin::Goudron;
            diagnostic += " Mauvais type de Chemin pour " + nomPt + " dans " +pt.NomPointTournant() + "\n";
            break;
        }


        pt.ajoutePointConnecte(nomPt,dst,denivMontant,tdc); // insère le point connecté dans le point tournant
    }
    return diagnostic;
}

string GraphPt::initialiserGr(std::string nomFichierDesPt)
{
    string diagnostic;
    string nomPointX ("vide");
    double lonX(0), latX(0);
    int altx(0);
    int rangPointTournant (-1);

    std::ifstream fichier(nomFichierDesPt.c_str(), std::ios::in);
    if(fichier)

    {
        std::string ligne; //Une variable pour stocker les lignes lues
        std::string finLigne; // pour stocker la suite de la ligne après avoir edxtrait un nombre
        std::string::size_type pos,pos1,pos2; //index sur la ligne
        int i(0); // pour compter les lignes
        int nbLignesParPoint (3);   // chaque point est stocké sur 3 lignes
        while(getline(fichier, ligne)) //Tant qu'on n'est pas à la fin, on lit
        {
            switch (i % nbLignesParPoint)   // suivant le rang de la ligne pour chaque point
            {
            case 0:        // correspond au nom de point entre <>
                pos1 = ligne.find("<");
                if (pos1!=std::string::npos)
                    pos2 = ligne.find(">");
                else
                    pos2= std::string::npos;

                if (pos2 != std::string::npos)
                    nomPointX = ligne.substr(pos1+1,pos2-pos1-1) ;
                if  ((pos1==std::string::npos) || (pos2==std::string::npos)){
                    diagnostic += "Erreur : Nom Point incorrect  dans ligne :" + std::to_string(i) + " =" + ligne +" \n";
                    return diagnostic;
                }

                break;

            case 1:           // corrrespond à la longitude, latitude et altitude du point
                try {
                    // stod provoque exception invalid_argument
                    // si le string ne correspond pas à un double
                    latX = stod (ligne, &pos);
                  }
                  catch (const std::invalid_argument& ia) {
                      diagnostic += "Erreur : Invalid argument pour latitude " + nomPointX + '\n';
                      return diagnostic;
                      break;
                  }


                try {
                    lonX = stod (ligne.substr(pos),&pos1);
                  }
                  catch (const std::invalid_argument& ia) {
                      diagnostic += "Erreur : Invalid argument pour longitude " + nomPointX + '\n';
                      return  diagnostic;
                      break;
                  }
                finLigne = ligne.substr(pos);

                try {
                    // stoi provoque exception invalid_argument
                    // si le string ne correspond pas à un entier
                    altx = stoi(finLigne.substr(pos1));
                  }
                  catch (const std::invalid_argument& ia) {
                      diagnostic += "Invalid argument pour altitude " + nomPointX + '\n';
                  }


                break;

            case 2 :     // correspond à la liste des points connecttés qui seront analysé dans parsePointsConnectés
                break;

            default :
                diagnostic += "pb de switch lignes \n" ;
                break;
            }

            if (i % nbLignesParPoint==2)
            {
                PointTournant Ptx (nomPointX,latX,lonX,altx);

                diagnostic += parsePointsConnectes(ligne, Ptx);
                if(diagnostic.substr(0,5)== "Erreur") return diagnostic;

                m_pointT.push_back( Ptx);
                m_indexPoint[nomPointX] = ++rangPointTournant;

                m_etiquettes.push_back(infini);
                m_predecesseurs.push_back(-1);
                m_marques.push_back(false);

            }
            i += 1;

        }

        fichier.close();


        diagnostic += "Fin Fichier OK - Dernier Point : " + nomPointX +"\n";
    }
    else
    {
        diagnostic += "Erreur : Impossible ouvrir fichier des Points Géo \n" ;
    }

    return diagnostic;
}

void GraphPt::initialiserMapsDistTdc()
{
    for (int i=0; i<m_pointT.size(); i++){
        map<int,double> mapD;
        map<int,TypedeChemin> mapTdc;
        for (int j =0 ; j <m_pointT[i].NbrPointsConnectes();j++){

            int rgPtConn = m_indexPoint[m_pointT[i].NomPointConnecte(j)];
            mapD[rgPtConn] = m_pointT[i].DistancePointConnecte(j);
            mapTdc[rgPtConn]= m_pointT[i].typedeChemin(j);
        }
        m_arcsDist.push_back(mapD);
        m_arcsTdc.push_back(mapTdc);
    }
}



string GraphPt::listeNomsDesPointsTournants (string fichierNomdesPoints)
{

    ofstream fluxNoms (fichierNomdesPoints );
    unsigned long long nbPointsTournants (0);
    string diagnostic;

    nbPointsTournants = m_pointT.size();

    if(fluxNoms)
    {
        fluxNoms << "Liste des points importés, précédés du rang. " << endl;
        for (unsigned int i=0; i< nbPointsTournants; i++)
            fluxNoms << i << "--" <<m_pointT[i].NomPointTournant()<<endl;
    }
    else
    {
        diagnostic += "ERREUR: Impossible d'ouvrir le fichier en écriture \n" ;
    }
    return diagnostic;
}

string GraphPt::verifierCompletude()
{
    string diagnostic;
    for (unsigned int i=0; i<m_pointT.size(); i++)
    {
        for (int j=0; j<m_pointT[i].NbrPointsConnectes(); j++)
        {
            if (trouveRangPoint(m_pointT[i].NomPointConnecte(j)) <0)
            {
                diagnostic = "Le point connecté : " + m_pointT[i].NomPointConnecte(j) + "  du point " + to_string(i) + " n'est pas trouvé \n";
                return diagnostic;
            }
        }
    }
    return diagnostic;
}

string GraphPt::verifierCompletudeTotale()
{
    int rgPtConnecte(-1);
    unsigned int rgPtConn;
    string nomPtInitial;
    string nomPtConnecteIncident;
    string nomPtConnecteCible;
    double distPtConnecte (0);
    bool trouveReciproque(false);
    string diagnostic;
    for (unsigned int i=0; i<m_pointT.size(); i++)
    {
        nomPtInitial = m_pointT[i].NomPointTournant();
        for (int j=0; j<m_pointT[i].NbrPointsConnectes(); j++)
        {
            nomPtConnecteIncident = m_pointT[i].NomPointConnecte(j);
            rgPtConnecte = trouveRangPoint(m_pointT[i].NomPointConnecte(j));
            if (rgPtConnecte <0)
            {
                diagnostic = "Le point connecté : " + m_pointT[i].NomPointConnecte(j) + "  du point " + to_string(i) + " n'est pas trouvé \n" ;
                return diagnostic;
            }
            distPtConnecte = m_pointT[i].DistancePointConnecte(j);
            trouveReciproque = false;
            rgPtConn = unsigned(rgPtConnecte);
            int nbPtConn = m_pointT[rgPtConn].NbrPointsConnectes();
            for (int k=0; k< nbPtConn && !trouveReciproque;k++)
            {
                nomPtConnecteCible = m_pointT[rgPtConn].NomPointConnecte(k);
                if (m_pointT[i].NomPointTournant()== m_pointT[rgPtConn].NomPointConnecte(k))
                {
                    if (m_pointT[rgPtConn].DistancePointConnecte(k) != distPtConnecte)
                    {
                        diagnostic = "Différence de distance entre : " + m_pointT[i].NomPointConnecte(j)
                                + " et : " + m_pointT[rgPtConn].NomPointConnecte(k) +"\n";
                        return diagnostic;
                    }
                    else
                    {
                        trouveReciproque = true;
                        break;
                    }

                }

            }
            if ( !trouveReciproque)
            {
                diagnostic = diagnostic +"Pas de réciproque pour " + nomPtInitial +" et " + nomPtConnecteIncident + "\n";
            }
        }

    }
    return diagnostic;
}

int GraphPt::trouveRangPoint(std::string NomPt)
{
    int rangDuPoint(-1);
    for (unsigned int i=0; i<m_pointT.size(); i++)
    {
        if (m_pointT[i].NomPointTournant() == NomPt)
        {
            rangDuPoint = int(i);
            break;
        }
    }
    return rangDuPoint;
}

void GraphPt::exporterMap(string fichierMapdesPoints)
{
    ofstream fluxMap (fichierMapdesPoints );


    if(fluxMap)
    {
        for (map<string, int>::iterator it=m_indexPoint.begin(); it!= m_indexPoint.end(); ++it)
            fluxMap << it->second <<"\t<" << it->first << ">"  <<endl;
    }
    else
    {
        cout << "ERREUR: Impossible d'ouvrir le fichier en écriture" << endl;
    }
}

int GraphPt::indexDansGrapheduPt (std::string nomduPoint) // renvoie l'index d'un Pt grace à la map
{
    int i=-1;
    i= m_indexPoint [nomduPoint];
    return i;
}

int GraphPt::nbPointsTournants(){
    return int(m_pointT.size());

}

void GraphPt::dijkstra(unsigned int source, std::map<TypedeChemin, bool> mTdc)
{
    if (m_source == int(source) && mTdc == m_mapTypCh) return;
    double etiqMin;
    int x (-1);
    //initialiser Étiquettes à +infini et Prédécesseurs à -1
    for (unsigned int i=0; i<m_pointT.size();i++){
        m_etiquettes[i]=infini;
        m_predecesseurs[i]=-1;
        m_marques[i]=false;
    }
    m_etiquettes[source] = 0;
    m_predecesseurs[source] = int(source);
    do {
        // Recherche sommet x non marqué de plus petite étiquette
        etiqMin = infini;
        for(unsigned int y =0; y< m_pointT.size();y++){
            if (!m_marques[y] && m_etiquettes[y]<etiqMin){
                x = int(y);
                etiqMin = m_etiquettes[y];
            }
        }
        // mise à jour des successeurs non marqués de x
        if (etiqMin < infini){
            m_marques[unsigned(x)]=true;
            for (map<int,double>::iterator it =m_arcsDist[unsigned(x)].begin(); it !=m_arcsDist[unsigned(x)].end(); it++){
                int y = it->first;
                TypedeChemin tdc = m_arcsTdc[unsigned(x)][y];
                map<TypedeChemin,bool>::iterator trouve = mTdc.find(tdc);
                if(trouve!=mTdc.end() && !m_marques[unsigned(y)] && (m_etiquettes[unsigned(x)]+it->second)< m_etiquettes[unsigned(y)]){
                    m_etiquettes[y] = m_etiquettes[unsigned(x)]+it->second;
                    m_predecesseurs[y] = x;
                }
            }
        }

    }
    while (!(etiqMin == infini));

    m_mapTypCh = mTdc;
    m_source = int(source);

}

std::list<int> GraphPt::plusCourtChemin(int source, int dest)
{
    list<int> courtChemin;
    if (m_source != source) dijkstra(unsigned (source),m_mapTypCh);
    int a = dest;
    courtChemin.push_back(dest);
    while (a != source){
        courtChemin.push_front(m_predecesseurs[a]);
        a = m_predecesseurs[a];
    }
    return courtChemin;
}

std::list<int> GraphPt::plusCourtCheminSensRetour(int source, int dest)
{
    list<int> courtCheminInv;
    if (m_source != source) dijkstra(unsigned (source),m_mapTypCh);
    int a = dest;
    courtCheminInv.push_back(dest);
    while (a != source){
        courtCheminInv.push_back(m_predecesseurs[a]);
        a = m_predecesseurs[a];
    }
    return courtCheminInv;
}

double GraphPt::distanceCourtChemin(int source, int dest)
{
    if (m_source != source) dijkstra(unsigned(source),m_mapTypCh);
    return m_etiquettes[unsigned(dest)];
}








