#include "aide.h"
#include "ui_aide.h"

#include <QFile>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextStream>

Aide::Aide(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Aide)
{
    ui->setupUi(this);
    afficheAide();
}

Aide::~Aide()
{
    delete ui;
}

void Aide::afficheAide()
{
    ui->textEditAide->setText("Commencer en cliquant sur Lire le Graphe");
    QFile fichierAide(FICHIER_AIDE.c_str());
    if(!fichierAide.open(QIODevice::ReadOnly))
        QMessageBox::information(nullptr,"ERREUR",fichierAide.errorString()); // ouverture d'un Qmsgbox si mon fichier est null
    else {
        QTextStream in(&fichierAide);
        //in.setCodec("ISO 8859-1");
        ui->textEditAide->setPlainText(in.readAll());
    }


}

void Aide::on_boutonAideOk_clicked()
{
    QTextDocument *docAide = ui->textEditAide->document();
    bool docAideChange = docAide->isModified();
    if (docAideChange)
        demanderSauvegarde("Enregistrer les Modifications ?");
    close();
}

void Aide::boutonOUI()
{
    // sauvegarder le texte dans le fichier
    QFile fichierAideMod(FICHIER_AIDE_MOD.c_str());
    if(!fichierAideMod.open(QIODevice::ReadWrite))
        QMessageBox::information(nullptr,"ERREUR",fichierAideMod.errorString()); // ouverture d'un Qmsgbox si mon fichier est null
    else {
        QTextStream out(&fichierAideMod);
        //QString *textMod = new QString( ui->textEditAide->toPlainText());
        // out.setString(textMod);
        //out.setCodec("ISO 8859-1");
        out << ui->textEditAide->toPlainText();
    }

    close();
}

void Aide::boutonNON()
{
    close();
}

void Aide::closeEvent(QCloseEvent *xClique)
{
    // Do something
    // demanderSauvegarde("Sauvegarder ?");
    xClique->accept();
}


void Aide::demanderSauvegarde(std::string raison)
{
    QWidget *fenDemUtil = new QWidget;
    fenDemUtil->setWindowTitle(QObject::tr(raison.c_str()));
    fenDemUtil->setFixedSize(300,150);


    QLabel *lRaison = new QLabel(fenDemUtil);
    lRaison->setText(tr(raison.c_str()));
    lRaison->setFont(QFont("Comic Sans MS", 13));
    lRaison->setAlignment(Qt::AlignCenter);
    //lRaison->move(50,50);
    lRaison->adjustSize();
    lRaison->setFrameStyle(QFrame::Panel | QFrame::Sunken);
            // centrer l'affichage
    int offsetx = (fenDemUtil->width() - lRaison->width()) / 2 ;
    int offsety = (fenDemUtil->height() - lRaison->height()) / 2;
    lRaison->setGeometry(offsetx, offsety,lRaison-> width(),lRaison-> height());
    QPushButton *boutonOUI = new QPushButton("Oui",fenDemUtil);
    boutonOUI->setFont(QFont("Comic Sans MS", 14));
    QPushButton *boutonNON = new QPushButton("NON",fenDemUtil);
    boutonNON->setFont(QFont("Comic Sans MS", 14));

    QVBoxLayout *vLayout = new QVBoxLayout;
    QHBoxLayout *hLayout = new QHBoxLayout;
    vLayout->addWidget(lRaison);
    vLayout->addItem(hLayout);
    hLayout->addWidget(boutonOUI);
    hLayout->addWidget(boutonNON);
    fenDemUtil->setLayout(vLayout);
              // Connexion du clic du bouton à la fermeture de de la fenètre
    QObject::connect(boutonOUI, SIGNAL(clicked()), this, SLOT(boutonOUI()));
    QObject::connect(boutonOUI, SIGNAL(clicked()), fenDemUtil, SLOT(close()));
    QObject::connect(boutonNON, SIGNAL(clicked()), this, SLOT(boutonNON()));
    QObject::connect(boutonNON, SIGNAL(clicked()), fenDemUtil, SLOT(close()));

    fenDemUtil->setWindowModality(Qt::ApplicationModal);      // on attend fermeture pour continuer
    fenDemUtil->show();

}
