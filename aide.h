#ifndef AIDE_H
#define AIDE_H

#include <QWidget>
#include <QTextEdit>
#include <QCloseEvent>

const std::string FICHIER_AIDE = "./aide.mod.txt";
const std::string FICHIER_AIDE_MOD = "./aide.mod.txt";

namespace Ui {
class Aide;
}

class Aide : public QWidget
{
    Q_OBJECT

public:
    explicit Aide(QWidget *parent = nullptr);
    ~Aide();
    void afficheAide();

    void demanderSauvegarde(std::string raison);

private slots:
    void on_boutonAideOk_clicked();

    void boutonOUI();
    void boutonNON();

    void closeEvent(QCloseEvent *xClique);

private:
    Ui::Aide *ui;
};

#endif // AIDE_H
