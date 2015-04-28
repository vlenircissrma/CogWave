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

#ifndef AUDIO_TX_H
#define AUDIO_TX_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
#include <gst/interfaces/xoverlay.h>
using namespace std;
using namespace itpp;
#include <QThread>

class Audio_TX : public QThread
{
    Q_OBJECT
public:
    Audio_TX();
    void stop();

protected:
    void run();

private:

    GstElement *pipeline;
    GstElement *audio_src;
    GstElement *audio_tee;
    GstElement *audio_queue;
    GstElement *audio_sink;
    GstElement *file_queue;
    GstElement *audio_encoder;
    GstElement *file_sink;


};


#endif // AUDIO_TX_H
