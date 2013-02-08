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

#ifndef BLINDOFDM_TDD_MODE_TX_H
#define BLINDOFDM_TDD_MODE_TX_H
#include <QThread>
#include "ui_mainwindow.h"
#include "blindofdm_modem.h"
#include "blindofdm_uhddevice.h"
#include "blindofdm_framing.h"

class BlindOFDM_TDD_Mode_TX: public QThread
{
Q_OBJECT
public:
    BlindOFDM_TDD_Mode_TX(Ui_MainWindow *ui);
    void init();
    BlindOFDM_Modem *blindofdm;
    BlindOFDM_UHDDevice *device;
    BlindOFDM_Framing *packets;
    bool stop_signal;
    bool noderunning;
    int Nfft;
    int Ncp;
    int num_subchannels;
    int Number_of_OFDM_symbols;
    int number_of_slots;
    double time_gap;
    QString state;
    int myaddress;
    int destaddress;


protected:
    void run();


public slots:
    void update_uhd();
    void setvalue(bool value);
    void setvalue(int value);
signals:
    void updated_uhd();
    void valuechanged(bool newvalue);



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
    bvec data_packet2;
    int tx_best_group;
    int nb_read;
    cvec tx_buff;
    double tx_timestamp;
    QString number;
    bool is_time_set;
};

#endif // BLINDOFDM_TDD_MODE_TX_H

