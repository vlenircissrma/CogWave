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

#include "video_rx.h"


Video_RX::Video_RX(){



    gst_init(NULL,NULL);


    pipeline = gst_pipeline_new("pipeline");
    file_src = gst_element_factory_make("filesrc","file_src");
    video_decoder = gst_element_factory_make("jpegdec","video_decoder");
    video_filter = gst_element_factory_make("ffmpegcolorspace","video_filter");
    video_sink = gst_element_factory_make("xvimagesink","video_sink");
    g_object_set(G_OBJECT(file_src),"location","video_outputpipe",NULL);
    gst_bin_add_many(GST_BIN(pipeline),file_src,video_decoder,video_filter,video_sink,NULL);
    gst_element_link_many(file_src,video_decoder,video_filter,video_sink,NULL);


    new_video_rx_stream=false;

}


void Video_RX::run(){




        emit displayed(video_sink,2);
        gst_element_set_state(GST_ELEMENT(pipeline),GST_STATE_PLAYING);
        gst_x_overlay_expose(GST_X_OVERLAY(video_sink));
        new_video_rx_stream=true;

}

void Video_RX::stop(){

         gst_element_set_state(GST_ELEMENT(pipeline),GST_STATE_NULL);
         gst_object_unref(pipeline);

}
