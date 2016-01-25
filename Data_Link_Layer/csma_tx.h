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

#ifndef CSMA_TX_H
#define CSMA_TX_H
#include <QThread>
#include "ui_mainwindow.h"
#include "Physical_Layer/modem_dads.h"
#include "Physical_Layer/modem_mcdaaofdm.h"
#include "Physical_Layer/modem_bpsk.h"
#include "Physical_Layer/modem_gmsk.h"
#include "Physical_Layer/modem_qpsk.h"
#include "Physical_Layer/modem_cpfsk.h"
#include "Physical_Layer/modem_dsss.h"
#include "Physical_Layer/modem_mcdads.h"
#include "Physical_Layer/modem_ofdm.h"
#include "Physical_Layer/sensing.h"
#include "Data_Link_Layer/data_link_layer_tx.h"
#include "Application_Layer/cogwave_packet.h"

class CSMA_TX: public Data_Link_Layer_TX
{
Q_OBJECT
public:
    CSMA_TX(Ui_MainWindow *ui);
    Modem_DADS *dads;
    Modem_MCDAAOFDM *mcdaaofdm;
    Modem_BPSK *bpsk;
    Modem_GMSK *gmsk;
    Modem_QPSK *qpsk;
    Modem_CPFSK *cpfsk;
    Modem_DSSS *dsss;
    Modem_MCDADS *mcdads;
    Modem_OFDM *ofdm;
    CogWave_Packet *packet;
    bool stop_signal;
    bool noderunning;
    QString state;
    int myaddress;
    int destaddress;
    int Number_of_received_symbols;
    void sendack();

public slots:
    void update_uhd();
    void setvalue(int value);
    void triggerack(int packetnorx);
    void signal_detected(bool newvalue);

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
    int ack_notx;
    bool ack_sent;
    int ack_norx;
    string csma_type;
    bool carrier_sensed;
    double probability_sending;
    int ptr;
};

#endif // CSMA_TX_H
