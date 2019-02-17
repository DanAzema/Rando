#ifndef FENPRINCIPALE_H
#define FENPRINCIPALE_H

#include <QMainWindow>
#include <QStringListModel>
#include "GraphPt.h"
#include "listcircuit.h"
#include "aide.h"
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QItemSelection>

#include <QtWebEngineWidgets/QtWebEngineWidgets>






const std::string VERSION = "Version 0.1-2019";


namespace Ui {
class FenPrincipale;
}

class FenPrincipale : public QMainWindow
{
    Q_OBJECT

public:
    explicit FenPrincipale(QWidget *parent = nullptr);
    ~FenPrincipale();
    void construireListeDesNomsDePoints();  // construit un QstringList avec les noms des points pour ensuite pouvoir choisir le point de départ dans la vue
    void indiquerUtilisateur(std::string raison);  // ouvre une fenêtre avec un titre "raison" et un bouton "noté" pour signaler indiquer quelquechose
    void afficheCheminRetourPlusCourt();        // demande le calcul du chemin et l'affiche
    void afficheModifCrctConst();           // affiche le circuit en construction qui vient d'être modifié
    void lancerDijkstra();                // récupérer pt dep et types de chemin pour calculer les plus courts
    void construireListeCircuitsFavoris();     // lire le fichier des circuits favoris et les stocker dans m_listCircuitsFavoris


public slots:
   void on_boutonLireGraphe_clicked();

private slots:
    std::string on_actionSelectionFichier_triggered();

    void on_boutonTrouverCircuits_clicked();

    void on_vueListeDesPoints_clicked(const QModelIndex &index);

    void on_choixDistanceMax_valueChanged(int value);

    void on_choixDistanceMin_valueChanged(int value);

    void on_vueCiruits_clicked(const QModelIndex &index);

    void on_boutonZoom_clicked();

    void on_boutonZoomMoins_clicked();
    void vueListeDesPoints_Changed(const QItemSelection &selection);
    void vueCiruits_Changed(const QItemSelection & selection);

    //void QItemSelectionModel::selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected );






    void on_actionZoom_IN_triggered();

    void on_actionZoom_OUT_triggered();

    void on_actionConstruire_Parcours_triggered();

    void on_vueConstruireCircuit_clicked(const QModelIndex &index);

    void on_actionVoir_Construction_triggered();

    void on_boutonReculer_clicked();

    void on_boutonNouveau_clicked();

    void on_boutonChPlusCourt_clicked();

    void on_actionA_Propos_triggered();

    void on_actionAide_triggered();

    void on_actionRafraichir_Circuit_triggered();

    void on_boutonFavoris_clicked();

    void on_actionParcours_Favoris_triggered();

    void on_actionQuitter_triggered();

    void on_actionVoir_les_Points_Tournant_triggered();

private:
    Ui::FenPrincipale *ui;
    QGraphicsScene *m_sceneCrct;

    GraphPt m_gPt;
    bool m_grapheInitialise =false;
    QStringList m_listeNomsPt;
    QStringListModel *m_modelePt;
    QStringListModel *m_modeleCrct = new QStringListModel();
    QStandardItemModel *m_modeleConsCrct = new QStandardItemModel(0,5);  //modèle avec 3 colonnes pour nom, distance, altitude
    ListCircuit *m_lstCrctTrouves;
    ListCircuit *m_lstCircuitsFavoris=new ListCircuit;
    bool m_modeFavoris = false;
    Circuit *m_circuitEnConstruction;
    bool m_constructionEnCours = false;
    std::map<TypedeChemin,bool> m_lTDC;
    /*
     * Ajouter une liste de parcours favoris, lue à partir d'un fichier
     *
     *
     *
     */

};

#endif // FENPRINCIPALE_H
