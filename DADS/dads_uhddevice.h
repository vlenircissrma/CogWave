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

#ifndef DADS_UHDDEVICE_H
#define DADS_UHDDEVICE_H

#include <uhd/usrp/multi_usrp.hpp>
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include <QThread>

class DADS_UHDDevice : public QThread
{
Q_OBJECT
public:
    DADS_UHDDevice();
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
    cvec rx_buff2;
    bool is_receiving;
    uhd::rx_metadata_t rx_md;
    double correction;
    double previous_correction;
    volatile bool has_sent;





protected:
    void run();

public slots:
    void setvalue(int value);

private:
    double timeout;
    uhd::usrp::multi_usrp::sptr usrp;
    uhd::tx_streamer::sptr tx_stream;
    uhd::rx_streamer::sptr rx_stream;
    uhd::tx_metadata_t tx_md;







};

#endif // DADS_UHDDEVICE_H
