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

#ifndef UHDDEVICE_H
#define UHDDEVICE_H
#define UHD_ENABLED 1
#include <uhd/usrp/multi_usrp.hpp>
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include <QThread>
#include <QMutex>
#include <Channel/channel_models.h>

class UHDDevice : public QThread
{
Q_OBJECT
public:
    UHDDevice();
    void init();
    double time();
    cvec readsamplesnow(int Nsamples);
    double sendsamples(cvec tx_buff,double timestamp);
    void sendsamplesnow(cvec tx_buff);
    cvec readsamples(int Nsamples,double timestamp);
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
    uhd::rx_metadata_t rx_md;
    double correction;
    double previous_correction;
    volatile bool has_sent;
    double timestamp;
    double tx_timestamp;
    double time_gap;
    int rx_buff_size;
    int duplex_mode;
    uhd::usrp::multi_usrp::sptr usrp;
    QMutex mutex;
    Channel_Models *channel_model;
    int my_address;
    bool is_synchronized;

protected:
    void run();

public slots:
    void setvalue(double value);

private:
    double timeout;
    uhd::tx_streamer::sptr tx_stream;
    uhd::rx_streamer::sptr rx_stream;
    uhd::tx_metadata_t tx_md;







};

#endif // UHDDEVICE_H
