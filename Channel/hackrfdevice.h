#ifndef HACKRFDEVICE_H
#define HACKRFDEVICE_H
#define HACKRF_ENABLED 0
#include <libhackrf/hackrf.h>
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include <QThread>
#include <QElapsedTimer>
#include <QMutex>
#include <uhd/usrp/multi_usrp.hpp>
#include <Channel/channel_models.h>


class HackRFDevice : public QThread
{
Q_OBJECT
public:
    HackRFDevice();
    void init();
    double time();
    cvec readsamplesnow(int Nsamples);
    void sendsamplesnow(cvec tx_buff);
    static int callback_rx(hackrf_transfer* transfer);
    static int callback_tx(hackrf_transfer* transfer);
    double sendsamples(cvec tx_buff,double timestamp);
    cvec readsamples(int Nsamples,double timestamp);
    double tx_rate;
    double tx_freq;
    double tx_gain;
    double rx_rate;
    double rx_freq;
    double rx_gain;
    double tx_amplitude;
    void waiting_time(double time_value);
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
    QMutex mutex;
    uhd::rx_metadata_t rx_md;
    Channel_Models *channel_model;
    int my_address;
    bool is_synchronized;
protected:
    void run();

public slots:
    void setvalue(double value);

private:
    hackrf_device *hackrf;


};



#endif // HACKRFDEVICE_H

