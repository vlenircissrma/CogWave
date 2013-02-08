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

#include "audio_rx.h"


Audio_RX::Audio_RX(){



    gst_init(NULL,NULL);


    pipeline = gst_pipeline_new("pipeline");
    file_src = gst_element_factory_make("filesrc","file_src");
    g_object_set(G_OBJECT(file_src),"location","audio_outputpipe",NULL);
    audio_decoder = gst_element_factory_make("mad","audio_decoder");
    audio_sink = gst_element_factory_make("alsasink","audio_sink");
    g_object_set(G_OBJECT(audio_sink),"sync",FALSE,NULL);
    gst_bin_add_many(GST_BIN(pipeline),file_src,audio_decoder,audio_sink,NULL);
    gst_element_link_many(file_src,audio_decoder,audio_sink,NULL);


    new_audio_rx_stream=false;

}


void Audio_RX::run(){


        gst_element_set_state(GST_ELEMENT(pipeline),GST_STATE_PLAYING);
        new_audio_rx_stream=true;

}

void Audio_RX::stop(){


         gst_element_set_state(GST_ELEMENT(pipeline),GST_STATE_NULL);
         gst_object_unref(pipeline);

}
