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

#ifndef WAVEFORM_FDD_TX_H
#define WAVEFORM_FDD_TX_H
#include <QThread>
#include "ui_mainwindow.h"
#include "Modems/modem_dads.h"
#include "Modems/modem_ofdma.h"
#include "Modems/modem_bpsk.h"
#include "Modems/modem_gmsk.h"
#include "Modems/modem_qpsk.h"
#include "Modems/modem_cpfsk.h"
#include "Common/uhddevice.h"
#include "Common/packet.h"
#include "Common/waveform_tx.h"

class Waveform_FDD_TX: public Waveform_TX
{
Q_OBJECT
public:
    Waveform_FDD_TX(Ui_MainWindow *ui);
    Modem_DADS *dads;
    Modem_OFDMA *blindofdm;
    Modem_BPSK *bpsk;
    Modem_GMSK *gmsk;
    Modem_QPSK *qpsk;
    Modem_CPFSK *cpfsk;
    UHDDevice *device;
    Packet *packet;
    bool stop_signal;
    bool noderunning;
    QString state;
    int myaddress;
    int destaddress;
    int Number_of_received_symbols;

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
    int tx_best_group;
    int waveform;
    int last_waveform;

};

#endif // WAVEFORM_TDD_TX_H
