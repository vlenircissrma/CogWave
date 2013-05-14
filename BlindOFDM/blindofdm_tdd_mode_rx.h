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
#ifndef BLINDOFDM_TDD_MODE_RX_H
#define BLINDOFDM_TDD_MODE_RX_H

#include <QThread>
#include "ui_mainwindow.h"
#include "blindofdm_modem.h"
#include "blindofdm_uhddevice.h"
#include "blindofdm_framing.h"
#include "blindofdm_sensing.h"

class BlindOFDM_TDD_Mode_RX: public QThread
{
Q_OBJECT
public:
    BlindOFDM_TDD_Mode_RX(Ui_MainWindow *ui);
    void init();
    BlindOFDM_Modem *blindofdm;
    BlindOFDM_UHDDevice *device;
    BlindOFDM_Framing *packets;
    BlindOFDM_Sensing *sensing;
    bool stop_signal;
    bool noderunning;
    int Nfft;
    int Ncp;
    int num_subchannels;
    int Number_of_OFDM_symbols;
    double time_gap;
    int number_of_slots;
    int myaddress;
    int destaddress;

protected:
    void run();

public slots:
    void update_uhd();
    void setvalue(bool value);


signals:
    void updated_uhd();
    void valuechanged(bool newvalue);
    void valuechanged(int newvalue);
    void valuechanged(double newvalue);
    void plotted(vec, int);
    void plotted(cvec, int);

private:
    Ui_MainWindow *gui;
    double txrate;
    double txfreq;
    double txgain;
    double txamplitude;
    double rxrate;
    double rxfreq;
    double rxgain;
    int sum_mask;
    int Number_of_received_symbols;
    int Modulation_Order;
    bool first_tx_timestamp;
    bvec data_packet;
    int tx_best_group;
    int previous_tx_best_group;
    int rx_best_group;
    bool is_time_set;
    double timestamp;
    cvec rx_buff;
    vec spectrum_sensed;
    bool is_ofdm;
    int start_frame;
    bvec received_bits;
    bvec received_bits2;
    vec estimated_channel;
    double sync_time;
    QString number;
    double correction;
    bool is_synchronized;
    int propagation_delay;
    cvec constellation;
};

#endif // BLINDOFDM_TDD_MODE_RX_H

