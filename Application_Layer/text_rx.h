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

#ifndef TEXT_RX_H
#define TEXT_RX_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include "ui_mainwindow.h"
#include <QThread>
#include <QFile>
#include <QTextStream>


class Text_RX: public QThread
{
    Q_OBJECT
public:
    Text_RX();
    void close();

protected:
    void run();

signals:
    void edited_text(QString);

private:

    QFile file2;
    QTextStream in;
    volatile bool stopped;
    QString last_text;
};


#endif // TEXT_H
