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

#ifndef POINT_TO_POINT_TDD_RX_H
#define POINT_TO_POINT_TDD_RX_H
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
#include "Data_Link_Layer/data_link_layer_rx.h"
#include "Data_Link_Layer/packet.h"

class  Point_to_Point_TDD_RX: public Data_Link_Layer_RX
{
Q_OBJECT
public:
    Point_to_Point_TDD_RX(Ui_MainWindow *ui, int fd_ext);
    Modem_DADS *dads;
    Modem_MCDAAOFDM *mcdaaofdm;
    Modem_BPSK *bpsk;
    Modem_GMSK *gmsk;
    Modem_QPSK *qpsk;
    Modem_CPFSK *cpfsk;
    Modem_DSSS *dsss;
    Modem_MCDADS *mcdads;
    Packet *packet;
    Sensing *sensing;
    bool stop_signal;
    bool noderunning;
    int Nfft;
    int Ncp;
    int sum_mask;
    int SF;
    int num_subchannels;
    int Number_of_received_symbols;
    int myaddress;
    int destaddress;
    double time_gap;


public slots:
    void update_uhd();
    void setvalue(bool value);

signals:
    void updated_uhd();
    void valuechanged(bool newvalue,double newvalue2);
    void valuechanged(double newvalue);
    void valuechanged(int newvalue);
    void waveformset(int newvalue);
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
    double correction;
    double timestamp;
    bool first_tx_timestamp;
    bool is_time_set;
    int waveform;
    int last_waveform;
    vec spectrum_sensed;
    int OF;
    int rx_best_group;
    int detected_group;
    double estimated_throughput;
    double previous_estimated_throughput;
    double previous_time;
    bool is_resynchronized;
    short bitspersymbol;
    int previous_packet_number;
    int ptr;
};

#endif // POINT_TO_POINT_TDD_RX_H
