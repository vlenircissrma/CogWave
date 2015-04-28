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

#ifndef POINT_TO_POINT_TDD_TX_H
#define POINT_TO_POINT_TDD_TX_H
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
#include "Physical_Layer/sensing.h"
#include "Data_Link_Layer/data_link_layer_tx.h"
#include "Data_Link_Layer/packet.h"

class Point_to_Point_TDD_TX: public Data_Link_Layer_TX
{
Q_OBJECT
public:
    Point_to_Point_TDD_TX(Ui_MainWindow *ui, int fd_ext);
    Modem_DADS *dads;
    Modem_MCDAAOFDM *mcdaaofdm;
    Modem_BPSK *bpsk;
    Modem_GMSK *gmsk;
    Modem_QPSK *qpsk;
    Modem_CPFSK *cpfsk;
    Modem_DSSS *dsss;
    Modem_MCDADS *mcdads;
    Packet *packet;
    bool stop_signal;
    bool noderunning;
    QString state;
    int myaddress;
    int destaddress;
    double time_gap;
    int Number_of_received_symbols;


public slots:
    void update_uhd();
    void setvalue(bool value, double value2);
    void setvalue(int value);
    void setwaveform(int value);

signals:
    void updated_uhd();
    void valuechanged(bool newvalue);
    void valuechanged(double newvalue);

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
    double tx_timestamp;
    bool first_tx_timestamp;
    bool is_time_set;
    int tx_best_group;
    int waveform;
    int last_waveform;
    int ptr;
};

#endif // POINT_TO_POINT_TDD_TX_H
