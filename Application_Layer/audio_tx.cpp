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

#include "audio_tx.h"


Audio_TX::Audio_TX(){



    gst_init(NULL,NULL);

    pipeline = gst_pipeline_new("pipeline");
    audio_src = gst_element_factory_make("alsasrc","audio_src");
    audio_tee = gst_element_factory_make("tee","audio_tee");
    audio_queue = gst_element_factory_make("queue","audio_queue");
    audio_sink = gst_element_factory_make("alsasink","audio_sink");
    g_object_set(G_OBJECT(audio_sink),"sync",FALSE,NULL);
    file_queue = gst_element_factory_make("queue","file_queue");
    audio_encoder = gst_element_factory_make("lamemp3enc","audio_encoder");
    g_object_set(G_OBJECT(audio_encoder),"target",1,NULL);
    g_object_set(G_OBJECT(audio_encoder),"bitrate",32,NULL);
    g_object_set(G_OBJECT(audio_encoder),"cbr",TRUE,NULL);
    file_sink = gst_element_factory_make("filesink","file_sink");
    g_object_set(G_OBJECT(file_sink),"location","audio_inputpipe",NULL);
    g_object_set(G_OBJECT(file_sink),"buffer-mode",2,NULL);
    gst_bin_add_many(GST_BIN(pipeline),audio_src,audio_tee,audio_queue,audio_sink,file_queue,audio_encoder,file_sink,NULL);
    gst_element_link(audio_src,audio_tee);
    gst_element_link_many(audio_tee,audio_queue,audio_sink,NULL);
    gst_element_link_many(audio_tee,file_queue,audio_encoder,file_sink,NULL);




}


void Audio_TX::run(){


        gst_element_set_state(GST_ELEMENT(pipeline),GST_STATE_PLAYING);



}

void Audio_TX::stop(){

         gst_element_set_state(GST_ELEMENT(pipeline),GST_STATE_NULL);
         gst_object_unref(pipeline);

}
