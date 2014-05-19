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

#include "uhddevice.h"



UHDDevice::UHDDevice()
{
    //Create the USRP
    string args="";
    usrp = uhd::usrp::multi_usrp::make(args);
    timeout=1.0;
    tx_rate=2.0e6;
    tx_freq=433.92e6;
    tx_gain=usrp->get_tx_gain_range().stop();
    tx_amplitude=0.1;
    rx_rate=2.0e6;
    rx_freq=433.92e6;
    rx_gain=0;
    uhd::stream_args_t stream_args("fc64"); //complex doubles
    //create a transmit streamer
    tx_stream = usrp->get_tx_stream(stream_args);
    //create a receive streamer
    rx_stream = usrp->get_rx_stream(stream_args);
    usrp->set_time_now(uhd::time_spec_t(0.0));
    cout << "Setting device timestamp to 0" << endl;
    is_sending=false;
    is_receiving=false;
    correction=0;
    previous_correction=0;
    has_sent=false;
    tx_buff.set_size(1);
    tx_buff.zeros();
    rx_buff.set_size(1);
    rx_buff.zeros();
    timestamp=0;
    tx_timestamp=0;
    duplex_mode=1;


}


void UHDDevice::setvalue(double value){


    correction=value;
    //cout << "################ CORRECTION " << correction << endl;

}


void UHDDevice::init()
{


    usrp->set_tx_rate(tx_rate);
    cout << "Setting TX Rate " << usrp->get_tx_rate() << endl;
    tx_rate=usrp->get_tx_rate();

    uhd::tune_request_t tune_req(tx_freq,tx_rate);
    usrp->set_tx_freq(tune_req);
    cout << "Setting TX Freq " << usrp->get_tx_freq()<< endl;
    tx_freq=usrp->get_tx_freq();

    usrp->set_tx_gain(tx_gain);
    cout << "Setting TX Gain " << usrp->get_tx_gain() << endl;
    tx_gain=usrp->get_tx_gain();

    if(tx_amplitude<0)
        tx_amplitude=0;
    if(tx_amplitude>1)
        tx_amplitude=1;
    cout << "Setting TX Amplitude " << tx_amplitude << endl;

    usrp->set_rx_rate(rx_rate);
    cout << "Setting RX Rate " << usrp->get_rx_rate() << endl;
    rx_rate=usrp->get_rx_rate();

    usrp->set_rx_freq(rx_freq);
    cout << "Setting RX Freq " << usrp->get_rx_freq() << endl;
    rx_freq=usrp->get_rx_freq();

    usrp->set_rx_gain(rx_gain);
    cout << "Setting RX Gain " << usrp->get_rx_gain() << endl;
    rx_gain=usrp->get_rx_gain();

    cout << " Setting RX Antenna" << endl;
    usrp->set_rx_antenna("TX/RX");


}

double UHDDevice::time()
{
  return usrp->get_time_now().get_real_secs();
}

cvec UHDDevice::readsamplesnow(int Nsamples)
{

    cvec rx_buff;
    int num_rx_samps=0;

     //Receiving mode
     uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);
     //allocate buffer with data to receive
     rx_buff.set_size(Nsamples);
     rx_buff.zeros();
     stream_cmd.num_samps = rx_buff.size();
     stream_cmd.stream_now = true;
     usrp->issue_stream_cmd(stream_cmd);
     num_rx_samps=rx_stream->recv(&rx_buff(0), rx_buff.size(), rx_md);

     return rx_buff;

}

cvec UHDDevice::readsamples(int Nsamples,double timestamp)
{


    cvec rx_buff;
    int num_rx_samps=0;

     //Receiving mode
     uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);
     //allocate buffer with data to receive
     rx_buff.set_size(Nsamples);
     rx_buff.zeros();
     stream_cmd.num_samps = rx_buff.size();
     stream_cmd.stream_now = false;
     if(time()>timestamp){
         cout << "WRONG RX TIMESTAMP!!!!!!!!!!!!!!!!" << endl;
         while(time()>timestamp)
                 timestamp=timestamp+time_gap;
     }
     stream_cmd.time_spec = uhd::time_spec_t(timestamp);
     usrp->issue_stream_cmd(stream_cmd);
     num_rx_samps=rx_stream->recv(&rx_buff(0), rx_buff.size(), rx_md);

     return rx_buff;

}


double UHDDevice::sendsamples(cvec tx_buff,double timestamp){


    int num_tx_samps=0;

    tx_buff=tx_buff*tx_amplitude;
    tx_md.start_of_burst = true;
    tx_md.end_of_burst = true;
    tx_md.has_time_spec = true;
    if(time()>timestamp){
        //cout << "WRONG TX TIMESTAMP!!!!!!!!!!!!!!!!" << endl;
        while(time()>timestamp){
                timestamp=timestamp+time_gap;
        }
    }
    tx_md.time_spec = uhd::time_spec_t(timestamp);
    num_tx_samps=tx_stream->send(&tx_buff(0), tx_buff.size(), tx_md, timestamp+tx_buff.size()/tx_rate);
    return timestamp;

}

void UHDDevice::run(){

    if(duplex_mode==1){

        if(is_sending){
            int num_tx_samps=0;


            while(is_sending){
                tx_md.start_of_burst = true;
                tx_md.end_of_burst = true;
                tx_md.has_time_spec = true;
                if(time()>tx_timestamp){
                    cout << "WRONG TX TIMESTAMP!!!!!!!!!!!!!!!!" << endl;
                    while(time()>tx_timestamp)
                            tx_timestamp=tx_timestamp+time_gap;
                }
                tx_md.time_spec = uhd::time_spec_t(tx_timestamp);
                num_tx_samps=tx_stream->send(&tx_buff(0), tx_buff.size(), tx_md, tx_timestamp+tx_buff.size()/tx_rate);
                if(tx_buff2!=tx_buff){
                    tx_buff=tx_buff2;
                }
                tx_timestamp=tx_timestamp+time_gap;
                tx_md.start_of_burst = false;
                tx_md.end_of_burst = true;
                tx_md.has_time_spec = false;
                tx_stream->send("", 0, tx_md);
                has_sent=true;
            }
            has_sent=false;


        }
        if(is_receiving){

            cvec rx_buff2;
            int num_rx_samps=0;
            uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);
             while(is_receiving){
                 if(rx_buff_size!=rx_buff.size()){
                     rx_buff_size=rx_buff.size();
                     rx_buff2.set_size(rx_buff_size);
                     rx_buff2.zeros();
                     stream_cmd.num_samps = rx_buff2.size();
                     stream_cmd.stream_now = false;
                 }
                 timestamp=timestamp+time_gap;

                 if((previous_correction!=correction)&&(abs(previous_correction-correction)*rx_rate>0)){
                      timestamp=timestamp+correction;
                      previous_correction=correction;
                      //cout << "CORRECTION" << endl;

                 }
                if(time()>timestamp){
                    cout << "WRONG RX TIMESTAMP!!!!!!!!!!!!!!!!" << endl;
                    while(time()>timestamp)
                            timestamp=timestamp+time_gap;

                }
                stream_cmd.time_spec = uhd::time_spec_t(timestamp);
                usrp->issue_stream_cmd(stream_cmd);
                num_rx_samps=rx_stream->recv(&rx_buff2(0), rx_buff2.size(), rx_md, timestamp,false);
                rx_buff=rx_buff2;

             }

        }
    }

    if(duplex_mode==2){

        if(is_sending){
            int num_tx_samps=0;

            tx_md.start_of_burst = false;
            tx_md.end_of_burst = false;
            tx_md.has_time_spec = false;

            while(is_sending){
                   num_tx_samps=tx_stream->send(&tx_buff(0), tx_buff.size(), tx_md,tx_buff.size()/tx_rate);
                   //cout << "Number of samples sent by the Tx streamer in loop " << num_tx_samps << endl;
                   if(tx_buff2!=tx_buff){
                       tx_buff=tx_buff2;
                   }
                   has_sent=true;
            }
            has_sent=false;

            tx_md.start_of_burst = false;
            tx_md.end_of_burst = true;
            tx_md.has_time_spec = false;
            tx_stream->send("", 0, tx_md);


        }
        if(is_receiving){


            rx_buff2.set_size(rx_buff.size());
            rx_buff2.zeros();
            //uhd::rx_metadata_t rx_md;
            uhd::stream_args_t stream_args("fc64");
            uhd::rx_streamer::sptr rx_stream = usrp->get_rx_stream(stream_args);
            int num_rx_samps=0;
            //Receiving mode
            usrp->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
            while(is_receiving){
                if((previous_correction!=correction)&&(abs(previous_correction-correction)>10)){
                     rx_buff2.set_size(correction);
                     num_rx_samps=rx_stream->recv(&rx_buff2(0), rx_buff2.size(), rx_md);
                     rx_buff2.set_size(rx_buff.size());
                     previous_correction=correction;
                }

                num_rx_samps=rx_stream->recv(&rx_buff2(0), rx_buff2.size(), rx_md);
                rx_buff=rx_buff2;

            }
            usrp->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);

        }


    }


}

