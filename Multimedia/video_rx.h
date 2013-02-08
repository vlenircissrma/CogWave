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

#ifndef VIDEO_RX_H
#define VIDEO_RX_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include <gst/interfaces/xoverlay.h>
#include <QThread>
#include "ui_mainwindow.h"

class Video_RX : public QThread
{
    Q_OBJECT
public:
    Video_RX();
    void stop();
    bool new_video_rx_stream;

protected:
    void run();

signals:
    void displayed(GstElement*,int);

private:

    GstElement *pipeline;
    GstElement *file_src;
    GstElement *video_decoder;
    GstElement *video_filter;
    GstElement *video_sink;


};


#endif // VIDEO_H
