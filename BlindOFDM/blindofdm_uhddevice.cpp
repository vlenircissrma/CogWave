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

#include "blindofdm_uhddevice.h"



BlindOFDM_UHDDevice::BlindOFDM_UHDDevice()
{
    //Create the USRP
    string args="";
    usrp = uhd::usrp::multi_usrp::make(args);
    timeout=1.0;
    tx_rate=2.0e6;
    tx_freq=433.92e6;
    tx_gain=usrp->get_tx_gain_range().stop()/2;
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
    timestamp=0;
    tx_timestamp=0;
    time_gap=0;
    correction=0;
    previous_correction=0;
    has_sent=false;
    tx_buff.set_size(1);
    tx_buff.zeros();
    rx_buff_size=0;
    rx_buff.set_size(1);
    rx_buff.zeros();
    sync_time=0;
    sync_time2=0;




}

void BlindOFDM_UHDDevice::setvalue(double value){


    correction=value;
    //cout << "################ CORRECTION " << correction << endl;

}



void BlindOFDM_UHDDevice::init()
{


    usrp->set_tx_rate(tx_rate);
    cout << "Setting TX Rate " << usrp->get_tx_rate() << endl;
    tx_rate=usrp->get_tx_rate();

    usrp->set_tx_freq(tx_freq);
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

double BlindOFDM_UHDDevice::time()
{
  return usrp->get_time_now().get_real_secs();
}

void BlindOFDM_UHDDevice::set_time(double sync_time)
{
    usrp->set_time_now(uhd::time_spec_t(sync_time+time()));
}

cvec BlindOFDM_UHDDevice::readsamples(int Nsamples,double timestamp)
{


    //uhd::rx_metadata_t rx_md;
    uhd::stream_args_t stream_args("fc64");
    uhd::rx_streamer::sptr rx_stream = usrp->get_rx_stream(stream_args);
    cvec rx_buff;
    cvec rx_buff2;
    int num_rx_samps=0;
    int lost_samples=500;
     //Receiving mode
     uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);
     //allocate buffer with data to receive
     rx_buff.set_size(lost_samples+Nsamples);
     rx_buff.zeros();
     rx_buff2.set_size(Nsamples);
     rx_buff2.zeros();
     stream_cmd.num_samps = rx_buff.size();
     stream_cmd.stream_now = false;
     stream_cmd.time_spec = uhd::time_spec_t(timestamp-lost_samples/rx_rate);
     usrp->issue_stream_cmd(stream_cmd);
     num_rx_samps=rx_stream->recv(&rx_buff(0), rx_buff.size(), rx_md, timestamp-lost_samples/rx_rate,false);
     rx_buff2=rx_buff.get(lost_samples,lost_samples+Nsamples-1);
     //cout << "First receive time sample " <<((rx_md.time_spec).get_real_secs()+lost_samples/rx_rate)*1.0e9 << " nanoseconds" << endl;
     //if(num_rx_samps==lost_samples+Nsamples)
     //     cout << "Number of samples acquired by the Rx streamer " << rx_buff2.size() << endl;
     //else
     //     cout << "Error in the number of samples acquired by the Rx streamer " <<  endl;

     return rx_buff2;

}



void BlindOFDM_UHDDevice::rxerrors()
{
    if (rx_md.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT) {
        cout << "Timeout while streaming" << endl;
    }
    if (rx_md.error_code == uhd::rx_metadata_t::ERROR_CODE_OVERFLOW){
           cout <<
                "Got an overflow indication. Please consider the following:\n"
                "  Your write medium must sustain a rate of %fMB/s.\n"
                "  Dropped samples will not be written to the file.\n"
                "  Please modify this example for your purposes.\n"
                "  This message will not appear again.\n"
             << endl;
    }
    if (rx_md.error_code != uhd::rx_metadata_t::ERROR_CODE_NONE){
     cout << "unknown error code" << endl;
    }
}


void BlindOFDM_UHDDevice::sendsamples(cvec tx_buff,double timestamp){


    int num_tx_samps=0;

    tx_buff=tx_buff*tx_amplitude;
    tx_md.start_of_burst = true;
    tx_md.end_of_burst = true;
    tx_md.has_time_spec = true;
    tx_md.time_spec = uhd::time_spec_t(timestamp);

    //cout << "The time is now " << time() << endl;

    if(time()>timestamp)
        cout << "Timestamp not valid" << endl;

    //cout << "before sending " << time();
    num_tx_samps=tx_stream->send(&tx_buff(0), tx_buff.size(), tx_md, timestamp+tx_buff.size()/tx_rate);
    //cout << "after sending " << time();
    //cout << "Number of samples sent by the Tx streamer " << num_tx_samps << endl;


}


void BlindOFDM_UHDDevice::run(){

    if(is_sending){
        int num_tx_samps=0;

        while(is_sending){

            //cout << "Send PACKET at time " << tx_timestamp << endl;
            tx_md.start_of_burst = true;
            tx_md.end_of_burst = true;
            tx_md.has_time_spec = true;
            tx_md.time_spec = uhd::time_spec_t(tx_timestamp);
            if(time()>tx_timestamp){
                cout << "WRONG TX TIMESTAMP!!!!!!!!!!!!!!!!" << endl;
                //break;
                while(time()>tx_timestamp)
                        tx_timestamp=tx_timestamp+time_gap;
            }
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

        //uhd::rx_metadata_t rx_md;
        uhd::stream_args_t stream_args("fc64");
        uhd::rx_streamer::sptr rx_stream = usrp->get_rx_stream(stream_args);
        cvec rx_buff2;
        int num_rx_samps=0;
        int lost_samples=500;
        //Receiving mode
        uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);
        //allocate buffer with data to receive


         while(is_receiving){
             if(rx_buff_size!=rx_buff.size()){
                 rx_buff_size=rx_buff.size();
                 rx_buff2.set_size(lost_samples+rx_buff_size);
                 rx_buff2.zeros();
                 stream_cmd.num_samps = rx_buff2.size();
                 stream_cmd.stream_now = false;
             }
             //cout << "Receive PACKET at time " << timestamp << endl;
             timestamp=timestamp+time_gap;
             if((previous_correction!=correction)&&(abs(previous_correction-correction)*rx_rate>1)){
                  timestamp=timestamp+correction;
                  previous_correction=correction;

             }
            if(time()>timestamp){
                cout << "WRONG RX TIMESTAMP!!!!!!!!!!!!!!!!" << endl;
                //break;
                while(time()>timestamp)
                        timestamp=timestamp+time_gap;

            }
            stream_cmd.time_spec = uhd::time_spec_t(timestamp-lost_samples/rx_rate);
            usrp->issue_stream_cmd(stream_cmd);
            num_rx_samps=rx_stream->recv(&rx_buff2(0), rx_buff2.size(), rx_md, timestamp-lost_samples/rx_rate,false);
            rx_buff=rx_buff2.get(lost_samples,lost_samples+rx_buff_size-1);

         }

    }


}


void BlindOFDM_UHDDevice::txerrors(double timestamp){

    std::cout << std::endl << "Waiting for async burst ACK... " << std::flush;
    uhd::async_metadata_t async_md;
    bool got_async_burst_ack = false;
    //loop through all messages for the ACK packet (may have underflow messages in queue)
    while (not got_async_burst_ack and usrp->get_device()->recv_async_msg(async_md, timestamp)){
        got_async_burst_ack = (async_md.event_code == uhd::async_metadata_t::EVENT_CODE_BURST_ACK);
    }
    std::cout << (got_async_burst_ack? "success" : "fail") << std::endl;


}
