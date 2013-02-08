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

#ifndef BLINDOFDM_UHDDEVICE_H
#define BLINDOFDM_UHDDEVICE_H

#include <uhd/usrp/multi_usrp.hpp>
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include <QThread>

class BlindOFDM_UHDDevice : public QThread
{
Q_OBJECT
public:
    BlindOFDM_UHDDevice();
    void init();
    double time();
    void set_time(double sync_time);
    cvec readsamples(int Nsamples,double timestamp);
    void rxerrors();
    void sendsamples(cvec tx_buff,double timestamp);
    void txerrors(double timestamp);
    void stop();
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
    bool is_receiving;
    int Nsamples;
    int rx_buff_size;
    uhd::rx_metadata_t rx_md;
    double timestamp;
    double tx_timestamp;
    double time_gap;
    double correction;
    double previous_correction;
    volatile bool has_sent;
    double sync_time;
    double sync_time2;

public slots:
    void setvalue(double value);

protected:
    void run();


private:
    double timeout;
    uhd::usrp::multi_usrp::sptr usrp;
    uhd::tx_streamer::sptr tx_stream;
    uhd::rx_streamer::sptr rx_stream;
    uhd::tx_metadata_t tx_md;







};

#endif // BLINDOFDM_UHDDEVICE_H
