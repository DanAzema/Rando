#include "Circuit.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <array>

#include <QDebug>

using namespace std;

Circuit::Circuit()
{
    m_longueurCircuit=0;
}

Circuit::Circuit(std::list<int> lstPt, GraphPt gPt)
{
    m_longueurCircuit=0;
    for(list<int>::iterator it=lstPt.begin(); it!=lstPt.end();it++){
        this->ajoutePtDistGainAlt(*it,gPt);
    }
}

Circuit::~Circuit()
{
    //dtor
}

int Circuit::nbrOccurence(int n) //compte le nombre de fois où l'entier n est présent dans le circuit
{
    int nbOccur(0);
    for (std::list<int>::iterator occurIter = m_lstPt.begin(); occurIter != m_lstPt.end(); ++occurIter)
    {
        if(*occurIter==n)
            nbOccur++;
    }
    return nbOccur;

}


int Circuit::nbrOccurence2(int n) //compte le nombre de fois où l'entier n est présent dans le circuit
{
    return std::count(m_lstPt.begin(), m_lstPt.end(),n);
}

int Circuit::nbrDoublesOccurences(int n)  // compte le nb de doubles occurences dans le circuit
{
    map<int,int> occurences;
    int nDbl(0);
    for (std::list<int>::iterator it=m_lstPt.begin(); it !=m_lstPt.end(); it++)
    {
        occurences[*it]++;
    }
    for (std::map<int,int>::iterator itm=occurences.begin(); itm!=occurences.end(); itm++)
    {
        if (itm->second > 1)
            nDbl++;
    }

    return nDbl;
}



bool Circuit::hoquetFinal(int n)  // tweste si les 4 derniers points quand on ajoute n font le hoquet
                                // répétition i j i j
{
    int avantDernier(0), avavDernier(0), avavavDernier(0);
    std::list<int>::reverse_iterator it;
    if (m_lstPt.size() < 5) return false;
    it = m_lstPt.rbegin();
    it++;
    avantDernier = *it;
    it++;
    avavDernier = *it;
    it++;
    avavavDernier = *it;
    if (n==avavDernier && avantDernier==avavavDernier) return true;
    return false;
}

bool Circuit::pointe(int n)      // teste si en ajoutant n on forme une pointe m-a-b-a-n
{
    // teste une pointe m-a-b-a-n
    int m,a,b,c;
    std::list<int>::reverse_iterator it;
    if (m_lstPt.size() < 5) return false;
    it = m_lstPt.rbegin();
    it++;
    c = *it; it++;
    b = *it; it++;
    a = *it; it++;
    if (a != c)
        return false;
    m = *it;
    if (m == n)
        return false;
    return true;
}

bool Circuit::pointeOuTriangleInverse(int n)    // teste la séquence x-a-b-c-b-a-y ou xabcacby ou xabcdcbay
{
    // teste un double pointe m-a-b-c-b-a-n  // n est le dernier point
    int c,d,e,f,g,h,i,j,k,l;                    // la séquence sera stockée dans c-d-e-f-g-h-i-j-k-l-n (si nb points du circuit suffit)
    std::list<int>::reverse_iterator it;
    if (m_lstPt.size() < 8) return false;
    it = m_lstPt.rbegin();
    it++;
    l = *it; it++;
    k = *it; it++;
    j = *it; it++;
    i = *it; it++;
    h = *it; it++;
    g = *it;
    if (l == h && k==i && g!=n )   // séquence m a b c b a n
        return true;
    if (m_lstPt.size() < 9 ) return false;

    it++;
    f = *it; it++;
    e=*it;
    if (l==h && k==i && j==g && f!=n)  // séquence m a b c a c b n
        return true;
    if (l==f && k==g && j==h && e!=n)       // séquence m a b c d c b a n
        return true;
    if (m_lstPt.size()< 11) return false;
    it++;
    d=*it; it++;
    c=*it;
    if (c!=n && l==d && k==e && j==f && i==g) return true;

    return false;
}

bool Circuit::dblpaire(int n)     // teste si on retrouve la combinaison m-n 2 fois en ajoutant n
{
    int m(0);
    std::list<int>::reverse_iterator itf;
    std::list<int>::iterator it,it2;
    if (m_lstPt.size() < 5) return false;
    itf = m_lstPt.rbegin();
    itf++;
    m = *itf;
    std::array<int,2> seq={m,n};

    it2=--m_lstPt.end();it2--;
    it = std::search (m_lstPt.begin(),it2, seq.begin(),seq.end());  // chercfhe de it1 à it2-1 la séquence de it3 à it4
                                                                 // retourne un iterateur sur 1er élément de la séquence trouvée ou it2 si non trouvé
    if (it != it2) {
         return true;
    }
    return false;
}

void Circuit::exporterCircuit(GraphPt gPt, std::string nomFichierCircuits)
 {
    ofstream fluxC (nomFichierCircuits.c_str(), ios::app );

    if(fluxC)
    {
        for (list<int>::iterator it=m_lstPt.begin(); it!= m_lstPt.end(); ++it)
            fluxC << "<" << gPt.pointT(*it).NomPointTournant()<< "> " ;
        fluxC << " -> \t"<< m_longueurCircuit <<" km" <<endl;
    }
    else
    {
        cout << "ERREUR: Impossible d'ouvrir le fichier en écriture" << endl;
    }
}

string Circuit::chaineTiretCircuit(GraphPt gPt, int numero)  //construit une chaine des noms des points séparés par des tirets et prcédé d'un numero (si >0)
{
    string chaineCircuit;
    stringstream ss,ss2,ss1;
    ss.precision(4);
    ss << m_longueurCircuit;
    ss2 << m_gainAltritude;
                // on affiche #numero> en tête avant la liwste des points
    if (numero >0){
        ss1 << "#" << numero << "> ";
        chaineCircuit = ss1.str() ;
    }
                // ajoute la liste des points espacés par un tiret
    for (list<int>::iterator it=m_lstPt.begin(); it!= m_lstPt.end(); ++it)
         chaineCircuit = chaineCircuit + gPt.pointT(*it).NomPointTournant()+ "-" ;
                // on ajoute en fin la longueur en km et le gain d'altitude en m
    chaineCircuit = chaineCircuit + "->" + ss.str()  +" km";    // était to_string(m_longueurCircuit)
    chaineCircuit = chaineCircuit + "--" + ss2.str()  +" m";
    return chaineCircuit;
}




double Circuit::augmenteLongueur (double longueurTroncon)
{
    m_longueurCircuit += longueurTroncon;
    return m_longueurCircuit;
}

int Circuit::augmenteDenivele(int gainAlt)
{
    m_gainAltritude += gainAlt;
    return m_gainAltritude;
}

int Circuit::ajoutePt (int index)
{
    m_lstPt.push_back(index) ;

    m_nbPt +=1;
    return m_nbPt;
}

void Circuit::ajouteNom(string nomCircuitFavori)
{
    m_nomCircuit = nomCircuitFavori;
}

string Circuit::nomDuCircuit()
{
    return m_nomCircuit;
}

int Circuit::ajoutePtDistGainAlt(int index,  GraphPt gPt)
{
    m_lstPt.push_back(index) ;
    m_nbPt +=1;
    if (m_nbPt>1){
        PointTournant avDerPt = gPt.pointT(*--(--m_lstPt.end()));
        for (int i=0; i<avDerPt.NbrPointsConnectes();i++){
            if (gPt.trouveRangPoint(avDerPt.NomPointConnecte(i))== index ) {
                this->augmenteDenivele(avDerPt.gainAltitude(i));
                this->augmenteLongueur(avDerPt.DistancePointConnecte(i));
                break;
            }
        }
    }
    return m_nbPt;
}

int Circuit::enleverDernierPoint(GraphPt gPt)
{
    if (m_nbPt < 2) return -1;

    if (m_nbPt>1){
        PointTournant avDerPt = gPt.pointT(*--(--m_lstPt.end()));
        int indexDerPt = *--m_lstPt.end();
        for (int i=0; i<avDerPt.NbrPointsConnectes();i++){
            if (gPt.trouveRangPoint(avDerPt.NomPointConnecte(i))== indexDerPt ) {
                m_gainAltritude -= avDerPt.gainAltitude(i);
                m_longueurCircuit -= avDerPt.DistancePointConnecte(i);
                break;
            }
        }
    }
    m_lstPt.pop_back() ;
    m_nbPt -=1;
    return m_nbPt;
}

int Circuit::dernierPt()
{
    return m_lstPt.back();
}

int Circuit::premPt()
{
    return m_lstPt.front();
}

int Circuit::nbPoints()
{
    return m_nbPt;
}


double* Circuit::minetmaxLatetLong(GraphPt gPt){

    double *tableMinMax= new double[4];
    tableMinMax[0]= 100.0;  // min latitude
    tableMinMax[1]= -100.0;    // max latitude
    tableMinMax[2]=100.0;    // min longitude
    tableMinMax[3]=-100.0;    // max longitude

    for (list<int>::iterator it=m_lstPt.begin(); it!= m_lstPt.end(); ++it){
       if (gPt.pointT(*it).LatPointTournant()<tableMinMax[0])
           tableMinMax[0]=gPt.pointT(*it).LatPointTournant();
       if (gPt.pointT(*it).LatPointTournant()>tableMinMax[1])
           tableMinMax[1]=gPt.pointT(*it).LatPointTournant();
       if (gPt.pointT(*it).LongPointTournant()<tableMinMax[2])
           tableMinMax[2]=gPt.pointT(*it).LongPointTournant();
       if (gPt.pointT(*it).LongPointTournant()>tableMinMax[3])
           tableMinMax[3]=gPt.pointT(*it).LongPointTournant();
    }

       return tableMinMax;

}

void Circuit::dessineCircuit(GraphPt gPt, QGraphicsScene *scene)
{
    PointTournant pTn, pTs;
    QPen outlinePen(Qt::black);
    outlinePen.setWidth(2);
    double const echelle(300);
    double facteurEchelle;

    double x1(0),y1(0),x2(0),y2(0),xinit(0),yinit(0);
    double minLa(43),maxLa(44),minLo(1.40),maxLo(1.60);
    double *minMax = minetmaxLatetLong(gPt);
    minLa = minMax[0]; maxLa = minMax[1];
    minLo = minMax[2]; maxLo = minMax[3];
    facteurEchelle = min( echelle / (maxLa-minLa), echelle / (maxLo-minLo));
    //qDebug() << " Min Max LL" << minLa<<maxLa << minLo << maxLo;
    //qDebug() << "facteur echelle " << facteurEchelle;
    pTn = gPt.pointT(*m_lstPt.begin());

    xinit = (pTn.LongPointTournant()-minLo)*facteurEchelle;
    yinit = echelle - ((pTn.LatPointTournant()-minLa)*facteurEchelle);
    QGraphicsTextItem * grTNomP1 = new QGraphicsTextItem;
    grTNomP1->setPos(xinit,yinit);
    grTNomP1->setPlainText(QString::fromStdString(pTn.NomPointTournant()));
    scene->addItem(grTNomP1);

    //qDebug( ) << "1er Pt Circuit" << QString::fromStdString(pTn.NomPointTournant());
    for (list<int>::iterator it=++m_lstPt.begin(); it!= m_lstPt.end(); ++it){
        pTs = gPt.pointT(*it);
        //qDebug( ) << "Pt Suivant Circuit" << QString::fromStdString(pTs.NomPointTournant());
        // trace ligne de pTn à pTs avec échelle et affiche nom du point
        x1 = (pTn.LongPointTournant()-minLo)*facteurEchelle;

        y1 = echelle - ((pTn.LatPointTournant()-minLa)*facteurEchelle);
        x2 = (pTs.LongPointTournant()-minLo)*facteurEchelle;
        y2 = echelle - ((pTs.LatPointTournant()-minLa)*facteurEchelle);
        //qDebug() << "coord scene " << x1 <<y1 <<x2 << y2 ;

        scene->addLine(x1,y1,x2,y2,outlinePen);
        QGraphicsTextItem * grTNomP = new QGraphicsTextItem;
        grTNomP->setPos(x2,y2);
        grTNomP->setPlainText(QString::fromStdString(pTs.NomPointTournant()));
        scene->addItem(grTNomP);

        //scene->addText(QString::fromStdString(pTs.NomPointTournant()),QFont("Arial",11));

        pTn = pTs;
    }
}

int Circuit::affichePtSuivCnst(QStandardItemModel *modele, std::map<TypedeChemin, bool> lTDC, GraphPt *gPt)
{
    PointTournant pT,pTs,pTl;
    int rgLastPt(0);
    std::string nomPremPoint,nomPtSuiv,nomPtLast;
    QStandardItem *premPoint, *nomSuiv, *nomConn, *nomLast;
    modele->clear();
    modele->setColumnCount(2);
    QStandardItem *parentItm = modele->invisibleRootItem();

    pT = gPt->pointT(*m_lstPt.begin());
    nomPremPoint = pT.NomPointTournant();
    premPoint = new QStandardItem(QString::fromStdString(nomPremPoint));
    premPoint->setFlags(Qt::ItemIsEnabled);
    parentItm->appendRow(premPoint);
    for (list<int>::iterator it=++m_lstPt.begin(); it!= --m_lstPt.end(); ++it){
        pTs = gPt->pointT(*it);
        nomPtSuiv = pTs.NomPointTournant();
        nomSuiv = new QStandardItem(QString::fromStdString(nomPtSuiv));
        Qt::ItemFlags mesFlags;
        mesFlags = nomSuiv->flags();
        //nomSuiv->setFlags(nomSuiv->flags() & ~Qt::ItemIsEnabled);
        nomSuiv->setFlags(Qt::NoItemFlags);
        mesFlags = nomSuiv->flags();
        premPoint->appendRow(nomSuiv);
        rgLastPt = *it;
    }
    pTl = gPt->pointT(*--m_lstPt.end());
    nomPtLast = pTl.NomPointTournant();
    nomLast = new QStandardItem(QString::fromStdString(nomPtLast));
    nomLast->setFlags(Qt::ItemIsEnabled);

    premPoint->appendRow(nomLast);

    for (int i = 0;i<pTl.NbrPointsConnectes();i++){
        nomConn = new QStandardItem(QString::fromStdString(pTl.NomPointConnecte(i)));
        TypedeChemin tdc = pTl.typedeChemin(i);
        if (lTDC[tdc]){
            QStandardItem *distance = new QStandardItem(QLocale().toString( pTl.DistancePointConnecte(i),'f',2));
            QStandardItem *gainAlt = new QStandardItem(QLocale().toString(pTl.gainAltitude(i)));
            nomLast->appendRow({nomConn,distance,gainAlt});
            }
    }
    return rgLastPt;
}
