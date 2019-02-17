#include <iostream>

#include <locale>

#include "fenprincipale.h"
#include "ui_fenprincipale.h"
#include <QFileDialog>
#include <QListView>
#include <QMessageBox>
#include "PointTournant.h"
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QLocale>
#include <QDebug>
#include <QVariant>
#include <sstream>



FenPrincipale::FenPrincipale(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FenPrincipale)
{
    ui->setupUi(this);

    setlocale(LC_ALL, "");  // localiser à la valeur établie pour Windows.

    m_listeNomsPt << "Arming";
    m_modelePt = new QStringListModel(m_listeNomsPt,this);
    ui->vueListeDesPoints->setModel(m_modelePt);
    ui->vueConstruireCircuit->hide();




}

FenPrincipale::~FenPrincipale()
{
    delete ui;
}

void FenPrincipale::on_choixDistanceMax_valueChanged(int value)
{
    // empêcher la valeur Max d'être inférieur à la valeur Min
    if (ui->choixDistanceMax->value()<ui->choixDistanceMin->value())
        ui->choixDistanceMax->setValue(ui->choixDistanceMin->value());
    ui->distanceMaxChoisie->display(double(ui->choixDistanceMax->value())/10); // le slider contient un entier représentant des hectomètres, l'affichage lcd est en km
}

void FenPrincipale::on_choixDistanceMin_valueChanged(int value)
{
    // empêcher la valeur Min de dépasser la valeur Max
    if (ui->choixDistanceMin->value()>ui->choixDistanceMax->value())
        ui->choixDistanceMin->setValue(ui->choixDistanceMax->value());
    ui->distanceMinChoisie->display(double(ui->choixDistanceMin->value())/10);
}


void FenPrincipale::on_boutonLireGraphe_clicked()
{
    std::string nomFichierPt ("");
    std::string nomFichierDesNomsdePt ("NomsPointGeo.txt");
    std::string nomFichierMapdesPt ("MapPointsGeo.txt");
    std::string diagnostic;

    setlocale(LC_ALL, "");
    if(m_grapheInitialise) {
        indiquerUtilisateur("Graphe déjà initialisé");
        return;
    }

    nomFichierPt = ui->affichageNomFichier->text().toStdString();

    diagnostic = m_gPt.initialiserGr(nomFichierPt);

    ui->affichageDiagnostics->append(QString::fromStdString(diagnostic));
    if (diagnostic.substr(0,6) == "Erreur") return;

    m_gPt.listeNomsDesPointsTournants(nomFichierDesNomsdePt.c_str());
    m_gPt.exporterMap(nomFichierMapdesPt);

    diagnostic = m_gPt.verifierCompletude();
    if (diagnostic!="")
           ui->affichageDiagnostics->append(QString::fromStdString(diagnostic));

    diagnostic = m_gPt.verifierCompletudeTotale();
    if (diagnostic != "")
            ui->affichageDiagnostics->append(QString::fromStdString(diagnostic));

    construireListeDesNomsDePoints();
    m_gPt.initialiserMapsDistTdc();
    m_grapheInitialise = true;
    int nbP = int(m_gPt.nbPointsTournants());
    ui->lcdNbPoints->display(nbP);
    lancerDijkstra();

   return;

}

void FenPrincipale::construireListeCircuitsFavoris()
{
    std::string nomFichierCircuitsFavoris ("Parcours Favoris.txt");
    std::string diagnostic("");
    if (m_lstCircuitsFavoris->nbCircuits()>0) return;
    diagnostic = m_lstCircuitsFavoris->initialiserCircuitsFavoris(m_gPt,nomFichierCircuitsFavoris);
    if (diagnostic!="")
           ui->affichageDiagnostics->append(QString::fromStdString(diagnostic));




}

void FenPrincipale::construireListeDesNomsDePoints(){

    m_listeNomsPt.clear();
    for (int i=0;i<m_gPt.nbPointsTournants();i++){
        std::string nom = m_gPt.pointT(i).NomPointTournant();
        m_listeNomsPt << QString::fromStdString(nom);
    }
    m_modelePt->setStringList(m_listeNomsPt);

    connect(ui->vueListeDesPoints->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(vueListeDesPoints_Changed(QItemSelection)));


    m_modelePt->sort(0);
}


void FenPrincipale::indiquerUtilisateur(std::string raison)
{
    QWidget *fenIndiqueUtil = new QWidget;
    fenIndiqueUtil->setWindowTitle(QObject::tr(raison.c_str()));
    fenIndiqueUtil->setFixedSize(300,150);


    QLabel *lRaison = new QLabel(fenIndiqueUtil);
    lRaison->setText(tr(raison.c_str()));
    lRaison->setFont(QFont("Comic Sans MS", 13));
    lRaison->setAlignment(Qt::AlignCenter);
    //lRaison->move(50,50);
    lRaison->adjustSize();
    lRaison->setFrameStyle(QFrame::Panel | QFrame::Sunken);
            // centrer l'affichage
    int offsetx = (fenIndiqueUtil->width() - lRaison->width()) / 2 ;
    int offsety = (fenIndiqueUtil->height() - lRaison->height()) / 2;
    lRaison->setGeometry(offsetx, offsety,lRaison-> width(),lRaison-> height());
    QPushButton *bouton = new QPushButton("Compris",fenIndiqueUtil);
    bouton->setFont(QFont("Comic Sans MS", 14));
    //bouton->move(110, 110);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(lRaison);
    layout->addWidget(bouton);
    fenIndiqueUtil->setLayout(layout);
              // Connexion du clic du bouton à la fermeture de de la fenètre
    QObject::connect(bouton, SIGNAL(clicked()), fenIndiqueUtil, SLOT(close()));
    fenIndiqueUtil->setWindowModality(Qt::ApplicationModal);      // on attend fermeture pour continuer
    fenIndiqueUtil->show();
}

void FenPrincipale::afficheCheminRetourPlusCourt()
{
    int derPt, source;
    std::list<int> cheminRetour;
    std::string crctRetString;
    if (!m_constructionEnCours) {
        indiquerUtilisateur("Pas de circuit en cours de construction");
        return;
    }
    source = m_circuitEnConstruction->premPt();
    derPt = m_circuitEnConstruction->dernierPt();
    cheminRetour = m_gPt.plusCourtCheminSensRetour(source,derPt);
    Circuit *crctRet = new Circuit(cheminRetour,m_gPt);
    crctRetString =  crctRet->chaineTiretCircuit(m_gPt,0);  // param 0 pour ne pas mettre #num en tête
    ui->circuitChoisi->append(QString::fromStdString("RETOUR +Court : "+ crctRetString));
}

std::string FenPrincipale::on_actionSelectionFichier_triggered()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("Textes(*.txt *.doc)"));
    QString fichier;
   fichier = QFileDialog::getOpenFileName(this, tr("Open Points"),"./",tr("Fichiers Poins(*.txt)"));
   if (fichier != ""){
        ui->affichageNomFichier->setText(fichier);
        ui->affichageNomFichier->adjustSize();
   }

   return fichier.toStdString();
}



void FenPrincipale::on_boutonTrouverCircuits_clicked()
{

    ListCircuit lstCrct;
    bool avecResultFichier = false;
    int nbMaxDblOccur(100);
    if (ui->checkBoxAvecFichier->isChecked())
            avecResultFichier = true;
    nbMaxDblOccur = ui->choixNbDblOccur->value();

    std::list<std::string> circuitsFormatChaines;
    std::vector<TypedeChemin> tdc ;
    double minDist(5),maxDist(35);
    if(!FenPrincipale::m_grapheInitialise ){
        indiquerUtilisateur("Graphe non initialisé\nChoisir Paramètres puis\nCliquer LIRE LE GRAPHE");
        return;
    }
    ui->vueCiruits->show();                 // ces 2 vues sont superposées, on cache celle de dessus
    ui->vueConstruireCircuit->hide();
    ui->boutonReculer->hide();
    ui->boutonNouveau->hide();
    ui->boutonChPlusCourt->hide();

    if(ui->checkBoxRouteGoudronnee->isChecked()) tdc.push_back(TypedeChemin::Goudron);
    if(ui->checkBoxRteFrequentee->isChecked()) tdc.push_back(TypedeChemin::Route_Frequent);
    if (ui->checkBoxSentier->isChecked()) tdc.push_back(TypedeChemin::Sentier);
    if(ui->checkBoxCheminPierre->isChecked()) tdc.push_back(TypedeChemin::Chemin_Pierre);
    if(ui->checkBoxCheminDeTerre->isChecked()) tdc.push_back(TypedeChemin::Chemin_Terre);
    minDist = ui->distanceMinChoisie->value();
    maxDist = ui->distanceMaxChoisie->value();
    std::string pointDeDepart;
    pointDeDepart = ui->affichagePointDeDepart->text().toStdString();

    m_lstCrctTrouves = lstCrct.construireCircuits(m_gPt,pointDeDepart,minDist,maxDist,tdc,avecResultFichier, nbMaxDblOccur);
    m_modeFavoris = false;
    int nbCircuitstrouves (0);
    nbCircuitstrouves = m_lstCrctTrouves->nbCircuits();
    ui->lcdNbCircuits->display(nbCircuitstrouves);

    circuitsFormatChaines = m_lstCrctTrouves->representerCircuitsenChainesTirets(m_gPt);

    QStringList qlstCrct;

    ui->vueCiruits->setModel(m_modeleCrct);
    for (std::list<std::string>::iterator it=circuitsFormatChaines.begin(); it!=circuitsFormatChaines.end(); ++it){
        qlstCrct.append(QString::fromStdString(*it));
    }

        m_modeleCrct->setStringList( qlstCrct);
        connect(ui->vueCiruits->selectionModel(),
                SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                this, SLOT(vueCiruits_Changed(QItemSelection)));




}


void FenPrincipale::on_vueListeDesPoints_clicked(const QModelIndex &index)
{
    if(!FenPrincipale::m_grapheInitialise ){
        indiquerUtilisateur("Initialiser le Graphe");
        return;
    }

    QVariant elementSelectionne = m_modelePt->data(index, Qt::DisplayRole);
    ui->affichagePointDeDepart->setText( elementSelectionne.toString());
    QString nomPt = elementSelectionne.toString();
    int rgPt;
    rgPt = m_gPt.indexDansGrapheduPt(nomPt.toStdString()); // récupère son rang dans le graphe
    PointTournant Pt;
    Pt = m_gPt.pointT(rgPt);


    QStandardItemModel *modelePoint = new QStandardItemModel(0,4);  // le modèle comprend 4 colonnes pour les points connectés (Nom,dist,typ chemin, gain altitude)
    //modelePoint->setHorizontalHeaderLabels("Points Tournants"); //pb ?
    ui->vuePointTournant->setModel(modelePoint);
    Pt.affichePoint(modelePoint);

    ui->vuePointTournant->setColumnWidth(0,180); // la colonne 0 englobe depuis le parent jusqu'à tous les enfants pour la première donnée
    ui->vuePointTournant->setColumnWidth(1,70); // la colonne 1 est celle qui est déclarée colonne 1
    ui->vuePointTournant->setColumnWidth(2,110);
    ui->vuePointTournant->expandAll();
}

void FenPrincipale::vueListeDesPoints_Changed(const QItemSelection& selection)
{
    if(selection.indexes().isEmpty()) {
        qDebug() << " Liste Des Points vide";
     } else {
        //qDebug() << " sélection : " <<  selection.indexes().first();
        on_vueListeDesPoints_clicked(selection.indexes().first());
    }

}



void FenPrincipale::on_vueCiruits_clicked(const QModelIndex &index)
{
    QVariant elementSelectionne = m_modeleCrct->data(index, Qt::DisplayRole);
    //ui->circuitChoisi->setFont(QFont("Comic Sans MS", 11));
    ui->circuitChoisi->setText( elementSelectionne.toString());

    int rangC = index.row();
    Circuit circuitSel;
    if(m_modeFavoris){
        circuitSel = m_lstCircuitsFavoris->circuitDeRang(rangC);
        std::string favori = circuitSel.chaineTiretCircuit(m_gPt,rangC+1); // pour afficher, on numérote à partir de 1
        ui->circuitChoisi->setText(QString::fromStdString( favori));
    }
    else
        circuitSel = m_lstCrctTrouves->circuitDeRang(rangC);

    m_sceneCrct = new QGraphicsScene();
    ui->vueDessinCircuit->setScene(m_sceneCrct);

    circuitSel.dessineCircuit(m_gPt,m_sceneCrct);

}


void FenPrincipale::on_boutonZoom_clicked()
{
    ui->vueDessinCircuit->scale(1.2, 1.2);
}

void FenPrincipale::on_boutonZoomMoins_clicked()
{
    ui->vueDessinCircuit-> scale(1 / 1.2, 1 / 1.2);
}

void FenPrincipale::vueCiruits_Changed(const QItemSelection& selection)
{
    if(selection.indexes().isEmpty()) {
        qDebug() << " Liste vide";
     } else {
        //qDebug() << " sélection : " <<  selection.indexes().first();
        on_vueCiruits_clicked(selection.indexes().first());
    }
}

void FenPrincipale::on_actionZoom_IN_triggered()
{
   ui->vueDessinCircuit-> scale(1 / 1.2, 1 / 1.2);
}

void FenPrincipale::on_actionZoom_OUT_triggered()
{
    ui->vueDessinCircuit->scale(1.2, 1.2);
}

void FenPrincipale::on_actionConstruire_Parcours_triggered()
{
    ui->vueConstruireCircuit->show();
    ui->boutonReculer->show();
    ui->boutonNouveau->show();
    ui->boutonChPlusCourt->show();
    ui->circuitChoisi->clear();
    ui->vueConstruireCircuit->setColumnWidth(0,250); // la colonne 0 englobe depuis le parent jusqu'à tous les enfants pour la première donnée
    ui->vueConstruireCircuit->setColumnWidth(1,70); // la colonne 1 est celle qui est déclarée colonne 1

    if(!FenPrincipale::m_grapheInitialise ){
        indiquerUtilisateur("Initialiser le Graphe");
        return;
    }
    if (m_constructionEnCours){
        delete m_circuitEnConstruction;
    }
    m_circuitEnConstruction = new Circuit;
    m_constructionEnCours = true;

    // deb dijlstra
    QString nomPt = ui->affichagePointDeDepart->text();
    int rgPt;
    rgPt = m_gPt.indexDansGrapheduPt(nomPt.toStdString()); // récupère son rang dans le graphe

    m_lTDC.clear() ;            // pour conbtenir une map qui donne vrai si le tyupe de chemin est coché
    if (ui->checkBoxSentier->isChecked()) m_lTDC[TypedeChemin::Sentier]=true;
    if (ui->checkBoxCheminPierre->isChecked()) m_lTDC[TypedeChemin::Chemin_Pierre]=true;
    if (ui->checkBoxCheminDeTerre->isChecked()) m_lTDC[TypedeChemin::Chemin_Terre]=true;
    if (ui->checkBoxRteFrequentee->isChecked()) m_lTDC[TypedeChemin::Route_Frequent]=true;
    if (ui->checkBoxRouteGoudronnee->isChecked()) m_lTDC[TypedeChemin::Goudron]=true;

    m_gPt.dijkstra(unsigned(rgPt), m_lTDC);

    // fin
    PointTournant Pt;
    Pt = m_gPt.pointT(rgPt);
    m_circuitEnConstruction->ajoutePt(rgPt);


    //QStandardItemModel *m_modeleConsCrct = new QStandardItemModel();  // le modèle servira à afficher la suite des points
    ui->vueConstruireCircuit->setModel(m_modeleConsCrct);

    Pt.affichePremPointCnstr(m_modeleConsCrct,m_lTDC);
    ui->vueConstruireCircuit->expandAll();
}

void FenPrincipale::on_vueConstruireCircuit_clicked(const QModelIndex &index)
{
    std::string nomPtSuivChoisi;

    int rgPt;
    Qt::ItemFlags flags;
    PointTournant ptSuiv;

    flags = index.flags();
    if (!(flags & Qt::ItemIsSelectable)) return;

    QVariant elementSelectionne = m_modeleConsCrct->data(index, Qt::DisplayRole);  //

    nomPtSuivChoisi = elementSelectionne.toString().toStdString();
    rgPt =  m_gPt.indexDansGrapheduPt(nomPtSuivChoisi);
    ptSuiv = m_gPt.pointT(rgPt);
    m_circuitEnConstruction->ajoutePtDistGainAlt(rgPt,m_gPt);

    afficheModifCrctConst();
}

void FenPrincipale::afficheModifCrctConst(){
    std::string circuitEnConstTiret;

    m_circuitEnConstruction->affichePtSuivCnst(m_modeleConsCrct,m_lTDC,&m_gPt);
    ui->vueConstruireCircuit->expandAll();
    ui->vueConstruireCircuit->setColumnWidth(0,250); // la colonne 0 englobe depuis le parent jusqu'à tous les enfants pour la première donnée
    ui->vueConstruireCircuit->setColumnWidth(1,70); // la colonne 1 est celle qui est déclarée colonne 1

    circuitEnConstTiret = m_circuitEnConstruction->chaineTiretCircuit(m_gPt,0);
    ui->circuitChoisi->setText(QString::fromStdString( circuitEnConstTiret) );
    afficheCheminRetourPlusCourt();

    m_sceneCrct = new QGraphicsScene();
    ui->vueDessinCircuit->setScene(m_sceneCrct);
    m_circuitEnConstruction->dessineCircuit(m_gPt,m_sceneCrct);

}

void FenPrincipale::lancerDijkstra()
{
    QString nomPt = ui->affichagePointDeDepart->text();
    int rgPt;
    rgPt = m_gPt.indexDansGrapheduPt(nomPt.toStdString()); // récupère son rang dans le graphe

    m_lTDC.clear() ;            // pour conbtenir une map qui donne vrai si le tyupe de chemin est coché
    if (ui->checkBoxSentier->isChecked()) m_lTDC[TypedeChemin::Sentier]=true;
    if (ui->checkBoxCheminPierre->isChecked()) m_lTDC[TypedeChemin::Chemin_Pierre]=true;
    if (ui->checkBoxCheminDeTerre->isChecked()) m_lTDC[TypedeChemin::Chemin_Terre]=true;
    if (ui->checkBoxRteFrequentee->isChecked()) m_lTDC[TypedeChemin::Route_Frequent]=true;
    if (ui->checkBoxRouteGoudronnee->isChecked()) m_lTDC[TypedeChemin::Goudron]=true;

    m_gPt.dijkstra(unsigned(rgPt), m_lTDC);
}




void FenPrincipale::on_actionVoir_Construction_triggered()
{
    ui->vueConstruireCircuit->show();
    ui->boutonReculer->show();
    ui->boutonNouveau->show();
    ui->boutonChPlusCourt->show();
}

void FenPrincipale::on_boutonReculer_clicked()
{
    std::string circuitEnConstTiret;
    int nbPointsRestants (-1);

    if(!m_grapheInitialise ){
        indiquerUtilisateur("Graphe non initialisé");
        return;
    }
    if(!m_constructionEnCours){
        indiquerUtilisateur("Pas de construction sur le gril");
        return;
    }

    if(m_circuitEnConstruction->nbPoints()<2) {
        indiquerUtilisateur("Vous êtes au premier point !");
        return;
    }

    nbPointsRestants =  m_circuitEnConstruction->enleverDernierPoint(m_gPt);
    if (nbPointsRestants == -1) return;

    if (nbPointsRestants == 1) {
        on_actionConstruire_Parcours_triggered();
        return;
    }
    m_circuitEnConstruction->affichePtSuivCnst(m_modeleConsCrct,m_lTDC,&m_gPt);
    ui->vueConstruireCircuit->expandAll();
    ui->vueConstruireCircuit->setColumnWidth(0,250); // la colonne 0 englobe depuis le parent jusqu'à tous les enfants pour la première donnée
    ui->vueConstruireCircuit->setColumnWidth(1,70); // la colonne 1 est celle qui est déclarée colonne 1

    circuitEnConstTiret = m_circuitEnConstruction->chaineTiretCircuit(m_gPt,0);
    ui->circuitChoisi->setText(QString::fromStdString( circuitEnConstTiret) );

    m_sceneCrct = new QGraphicsScene();
    ui->vueDessinCircuit->setScene(m_sceneCrct);

    m_circuitEnConstruction->dessineCircuit(m_gPt,m_sceneCrct);


}

void FenPrincipale::on_boutonNouveau_clicked()
{
    on_actionConstruire_Parcours_triggered();
}

void FenPrincipale::on_boutonChPlusCourt_clicked()
{
    int derPt, source;
    std::list<int> cheminRetour;

    if(!m_grapheInitialise)
        {
            indiquerUtilisateur("Graphe non initialisé");
            return;
        }
    if(!m_constructionEnCours){
        indiquerUtilisateur("Pas de construction sur le gril");
        return;
    }

    if(m_circuitEnConstruction->nbPoints()<2){
        indiquerUtilisateur("Vous êtes au premier point");
        return;
    }
    source = m_circuitEnConstruction->premPt();
    derPt = m_circuitEnConstruction->dernierPt();
    cheminRetour = m_gPt.plusCourtCheminSensRetour(source,derPt);



    for (std::list<int>::iterator it =++cheminRetour.begin();it!= cheminRetour.end();it++) {
        m_circuitEnConstruction->ajoutePtDistGainAlt(*it,m_gPt);
    }
    afficheModifCrctConst();
}

void FenPrincipale::on_actionA_Propos_triggered()
{

        std::string apropos = "Bienvenue dans ce logiciel\nAvec les compliments de Dan\n";
        apropos += VERSION;
        QWidget *fenA_Propos = new QWidget;
        fenA_Propos->setWindowTitle(QObject::tr("A Propos"));
        fenA_Propos->setWindowIcon(QIcon(":/icones/icones/3687834-adventure-boot-footwear-hiking-walking_108818.png"));

        fenA_Propos->setFixedSize(400,200);
        QTextEdit *texteA_Propos = new QTextEdit(QString::fromStdString( apropos),fenA_Propos);
        texteA_Propos->setFont(QFont("Comic Sans MS", 13));
        texteA_Propos->resize(250,120);
        texteA_Propos->move(50,20);
        QPushButton *bouton = new QPushButton("Compris",fenA_Propos);
        bouton->setFont(QFont("Comic Sans MS", 14));
        bouton->move(110, 150);
                  // Connexion du clic du bouton à la fermeture de de la fenètre
        QObject::connect(bouton, SIGNAL(clicked()), fenA_Propos, SLOT(close()));
        fenA_Propos->setWindowModality(Qt::ApplicationModal);
        fenA_Propos->show();

}

void FenPrincipale::on_actionAide_triggered()
{
    Aide *randoAide = new Aide;
    randoAide->setWindowModality(Qt::ApplicationModal);
    randoAide->show();

}

void FenPrincipale::on_actionRafraichir_Circuit_triggered()
{
    on_actionVoir_Construction_triggered();
}

void FenPrincipale::on_boutonFavoris_clicked()
{
    if(!m_grapheInitialise){
            indiquerUtilisateur("Graphe non initialisé");
            return;
        }
    construireListeCircuitsFavoris();

    ui->vueCiruits->show();                 // ces 2 vues sont superposées, on cache celle de dessus
    ui->vueConstruireCircuit->hide();
    ui->boutonReculer->hide();
    ui->boutonNouveau->hide();
    ui->boutonChPlusCourt->hide();

    int nbCircuitsFavoris (0);
    nbCircuitsFavoris = m_lstCircuitsFavoris->nbCircuits();
    ui->lcdNbCircuits->display(nbCircuitsFavoris);

    std::list<std::string> nomDesCircuitsFavoris;
    nomDesCircuitsFavoris = m_lstCircuitsFavoris->nomsDesCircuits(m_gPt);

    QStringList qlstCrct;

    ui->vueCiruits->setModel(m_modeleCrct);
    int rg(0);
    for (std::list<std::string>::iterator it=nomDesCircuitsFavoris.begin(); it!=nomDesCircuitsFavoris.end(); ++it){
        rg++;
        std::stringstream ss;
        ss << rg;
        std::string nomFavNum = "#"+ ss.str()+ "- " + *it ;
        qlstCrct.append(QString::fromStdString(nomFavNum));
    }
    m_modeleCrct->setStringList( qlstCrct);
    connect(ui->vueCiruits->selectionModel(),
                SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                this, SLOT(vueCiruits_Changed(QItemSelection)));
    m_modeFavoris = true;

}


void FenPrincipale::on_actionParcours_Favoris_triggered()
{
    on_boutonFavoris_clicked();
}

void FenPrincipale::on_actionQuitter_triggered()
{
    close();
}

void FenPrincipale::on_actionVoir_les_Points_Tournant_triggered()
{
    QWebEngineView *view = new QWebEngineView();
    view->load(QUrl("https://drive.google.com/open?id=1E5nMVs7niRxLAUWvH6PrIrPn84E4hRvF&usp=sharing"));
    // https://drive.google.com/open?id=1E5nMVs7niRxLAUWvH6PrIrPn84E4hRvF&usp=sharing
    view->show();
}
