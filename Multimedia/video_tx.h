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

#ifndef VIDEO_TX_H
#define VIDEO_TX_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include <gst/interfaces/xoverlay.h>
#include <QThread>

class Video_TX : public QThread
{
    Q_OBJECT
public:
    Video_TX();
    void stop();

protected:
    void run();

signals:
    void displayed(GstElement*,int);

private:

    GstElement *pipeline;
    GstElement *video_src;
    GstElement *video_tee;
    GstElement *video_queue;
    GstElement *video_filter;
    GstElement *video_rate;
    GstElement *video_sink;
    GstElement *file_queue;
    GstElement *video_encoder;
    GstElement *file_sink;


};


#endif // VIDEO_H
