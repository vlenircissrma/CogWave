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

#ifndef DADS_FDD_MODE_RX_H
#define DADS_FDD_MODE_RX_H
#include <QThread>
#include "ui_mainwindow.h"
#include "dads_modem.h"
#include "dads_uhddevice.h"
#include "dads_framing.h"
#include "waveform_rx.h"

class DADS_FDD_Mode_RX: public Waveform_RX
{
Q_OBJECT
public:
    DADS_FDD_Mode_RX(Ui_MainWindow *ui);
    DADS_Modem *dads;
    DADS_UHDDevice *device;
    DADS_Framing *packets;
    bool stop_signal;
    bool noderunning;
    int SF;
    int nb_bits;
    int Number_of_received_symbols;
    QString state;
    int myaddress;
    int destaddress;
    int Nfft;

public slots:
    void update_uhd();
signals:
    void updated_uhd();
    void valuechanged(int newvalue);
    void plotted(vec, int);
    void plotted(cvec, int);

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
    cvec rx_buff;
    QString number;
    bvec received_bits;
    bvec received_bits2;
    bool is_dads;
    bool is_synchronized;
    int nb_packet;
    int OF;
    int correction;

};

#endif // DADS_FDD_MODE_RX_H
