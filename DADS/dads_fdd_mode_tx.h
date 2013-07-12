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

#ifndef DADS_FDD_MODE_TX_H
#define DADS_FDD_MODE_TX_H
#include <QThread>
#include "ui_mainwindow.h"
#include "dads_modem.h"
#include "dads_uhddevice.h"
#include "dads_framing.h"
#include "waveform_tx.h"

class DADS_FDD_Mode_TX: public Waveform_TX
{
Q_OBJECT
public:
    DADS_FDD_Mode_TX(Ui_MainWindow *ui);
    DADS_Modem *dads;
    DADS_UHDDevice *device;
    DADS_Framing *packets;
    bool stop_signal;
    bool noderunning;
    int nb_bits;
    QString state;
    int myaddress;
    int destaddress;

public slots:
    void update_uhd();
signals:
    void updated_uhd();


protected:
    void run();

private:
    Ui_MainWindow *gui;
    double txrate;
    double txfreq;
    double txgain;
    double txamplitude;
    double rxrate;
    double rxfreq;
    double rxgain;
    bvec data_packet;
    cvec tx_buff;
    QString number;
    int nb_read;


};

#endif // DADS_FDD_MODE_TX_H
