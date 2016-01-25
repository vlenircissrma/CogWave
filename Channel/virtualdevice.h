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

#ifndef VIRTUALDEVICE_H
#define VIRTUALDEVICE_H
#define VIRTUAL_ENABLED 0
#include <uhd/usrp/multi_usrp.hpp>
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include <QThread>
#include <QElapsedTimer>
#include <Channel/channel_models.h>
#include <QMutex>
#include <boost/circular_buffer.hpp>

class VirtualDevice : public QThread
{
Q_OBJECT
public:
    VirtualDevice();
    void init();
    double time();
    cvec readsamplesnow(int Nsamples);
    double sendsamples(cvec tx_buff,double timestamp);
    void sendsamplesnow(cvec tx_buff);
    cvec readsamples(int Nsamples,double timestamp);
    void send(cvec tx_buff, double tx_timestamp);
    void send(cvec tx_buff);
    cvec recv(int rx_size, double rx_timestamp);
    cvec recv(int rx_size);
    void waiting_time(double time_value);
    double tx_rate;
    double tx_freq;
    double tx_gain;
    double tx_amplitude;
    double rx_rate;
    double rx_freq;
    double rx_gain;
    cvec tx_buff;
    cvec tx_buff2;
    bool is_sending;
    cvec rx_buff;
    cvec rx_buff2;
    bool is_receiving;
    double correction;
    double previous_correction;
    volatile bool has_sent;
    double timestamp;
    double tx_timestamp;
    double time_gap;
    int rx_buff_size;
    int duplex_mode;
    QElapsedTimer timer;    
    uhd::rx_metadata_t rx_md;
    Channel_Models *channel_model;
    AWGN_Channel awgn_noise;
    QMutex mutex;
    QMutex mutex2;
    int my_address;
    bool is_synchronized;

protected:
    void run();

signals:
    void send_vector(timestamp_data tmp);

public slots:
    void setvalue(double value);
    void recv_vector(timestamp_data tmp);

private:
    double timeout;
    vector<timestamp_data> time_vector;
    //boost::circular_buffer<timestamp_data> time_vector;
    double rx_timestamp;
    double tx_timestamp2;


};

#endif // VIRTUALDEVICE_H
