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

#ifndef AUDIO_RX_H
#define AUDIO_RX_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
#include <gst/interfaces/xoverlay.h>
using namespace std;
using namespace itpp;
#include <QThread>

class Audio_RX : public QThread
{
    Q_OBJECT
public:
    Audio_RX();
    void stop();
    bool new_audio_rx_stream;

protected:
    void run();


private:

    GstElement *pipeline;
    GstElement *file_src;
    GstElement *audio_decoder;
    GstElement *audio_sink;



};


#endif // AUDIO_RX_H
