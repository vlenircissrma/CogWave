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

#include "video_tx.h"


Video_TX::Video_TX(){



    gst_init(NULL,NULL);


    //JPEG
    GstCaps *caps;
    pipeline = gst_pipeline_new("pipeline");
    video_src = gst_element_factory_make("v4l2src","video_src");
    caps=gst_caps_new_simple("video/x-raw-yuv","width",G_TYPE_INT,352,"height",G_TYPE_INT,288,"framerate",GST_TYPE_FRACTION,5,1,NULL);
    video_filter = gst_element_factory_make("ffmpegcolorspace","video_filter");
    video_tee = gst_element_factory_make("tee","video_tee");
    video_queue = gst_element_factory_make("queue","video_queue");
    video_sink = gst_element_factory_make("xvimagesink","video_sink");
    file_queue = gst_element_factory_make("queue","file_queue");
    video_encoder = gst_element_factory_make("jpegenc","video_encoder");
    g_object_set(G_OBJECT(video_encoder),"quality",16,NULL);
    file_sink = gst_element_factory_make("filesink","file_sink");
    g_object_set(G_OBJECT(file_sink),"location","video_inputpipe",NULL);
    g_object_set(G_OBJECT(file_sink),"buffer-mode",1,NULL);
    gst_bin_add_many(GST_BIN(pipeline),video_src,video_filter,video_tee,video_queue,video_sink,file_queue,video_encoder,file_sink,NULL);
    gst_element_link_filtered(video_src,video_filter,caps);
    gst_element_link(video_filter,video_tee);
    gst_element_link_many(video_tee,video_queue,video_sink,NULL);
    gst_element_link_many(video_tee,file_queue,video_encoder,file_sink,NULL);




}


void Video_TX::run(){


        emit displayed(video_sink,1);
        gst_element_set_state(GST_ELEMENT(pipeline),GST_STATE_PLAYING);



}

void Video_TX::stop(){

         gst_element_set_state(GST_ELEMENT(pipeline),GST_STATE_NULL);
         gst_object_unref(pipeline);

}
