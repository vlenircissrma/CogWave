/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////                            Author: Vincent Le Nir                                         ///////
///////                                 Royal Military Academy                                    ///////
///////           Department Communications, Information Systems & Sensors (CISS)                 ///////
///////                             30, Avenue de la Renaissance                                  ///////
///////                                B-1000 Brussels BELGIUM                                    ///////
///////                                   Tel: +3227426624                                        ///////
///////                                 email:vincent.lenir@rma.ac.be                             ///////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TEXT_TX_H
#define TEXT_TX_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include "ui_mainwindow.h"
#include <QThread>
#include <QFile>
#include <QTextStream>


class Text_TX: public QThread
{
    Q_OBJECT
public:
    Text_TX();
    void init_text(QString text);

protected:
    void run();


private:

    QString myText;
    QFile file;
    QTextStream out;

};


#endif // TEXT_H
