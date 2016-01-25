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
#include "hackrfdevice.h"

HackRFDevice::HackRFDevice()
{

    tx_rate=2.0e6;
    tx_freq=433.92e6;
    tx_gain=0;
    rx_rate=2.0e6;
    rx_freq=433.92e6;
    rx_gain=0;
    tx_amplitude=1.0;



    timer.start();
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

int hackrf_rx_count=0;
int hackrf_rx_buffer_size=0;
std::vector<int8_t> hackrf_rx_buf(0);
int hackrf_tx_count=0;
int hackrf_tx_buffer_size=0;
std::vector<int8_t> hackrf_tx_buf(0);

int HackRFDevice::callback_rx(hackrf_transfer* transfer){

    int bytes_to_read;
    if(transfer->valid_length<hackrf_rx_buffer_size-hackrf_rx_count)
        bytes_to_read=transfer->valid_length;
    else
        bytes_to_read=hackrf_rx_buffer_size-hackrf_rx_count;
    if(bytes_to_read!=0){
       for (int i=0; i<bytes_to_read; i++){
           hackrf_rx_buf[i+hackrf_rx_count] = (int8_t)transfer->buffer[i];
       }
       hackrf_rx_count = hackrf_rx_count + bytes_to_read;
    }
    return(0);

}

int HackRFDevice::callback_tx(hackrf_transfer* transfer){

    if(hackrf_tx_buffer_size!=0){
        for (int i=0; i<hackrf_tx_buffer_size; i++){
            transfer->buffer[i]=(uint8_t)hackrf_tx_buf[i];
        }
        hackrf_tx_count = hackrf_tx_count + hackrf_tx_buffer_size;
    }
    return(0);


}

cvec HackRFDevice::readsamplesnow(int Nsamples){


    hackrf=NULL;
    hackrf_init();
    hackrf_open(&hackrf);
    hackrf_rx_count=0;
    hackrf_rx_buffer_size=Nsamples*2;
    hackrf_rx_buf.resize(Nsamples*2);
    hackrf_set_freq(hackrf,uint64_t(rx_freq));
    hackrf_set_sample_rate(hackrf,rx_rate);
    hackrf_compute_baseband_filter_bw(uint32_t(0.75*rx_rate));
    hackrf_set_amp_enable(hackrf,rx_gain);
    hackrf_set_lna_gain(hackrf,uint32_t(16));
    hackrf_set_vga_gain(hackrf,uint32_t(20));
    hackrf_start_rx(hackrf,callback_rx,NULL);
    while (hackrf_is_streaming (hackrf) != HACKRF_TRUE){
        cout << "waiting for streaming..." << endl;
    }
    while(hackrf_is_streaming (hackrf)==HACKRF_TRUE){
       if(hackrf_rx_count==Nsamples*2){
       break;
       }
    }
    cvec buffer;
    buffer.set_size(Nsamples);
    for (int i=0;i<Nsamples;i++) {
        buffer[i]=complex<double>(((double)hackrf_rx_buf[(i<<1)])/128.0,((double)hackrf_rx_buf[(i<<1)+1])/128.0);
    }

    hackrf_stop_rx(hackrf);
    hackrf_close(hackrf);
    hackrf_exit();

    return buffer;

}

void HackRFDevice::sendsamplesnow(cvec tx_buff){


    int Nsamples=tx_buff.size();
    hackrf=NULL;
    hackrf_init();
    hackrf_open(&hackrf);
    hackrf_set_freq(hackrf,uint64_t(tx_freq));
    hackrf_set_sample_rate(hackrf,tx_rate);
    hackrf_compute_baseband_filter_bw(uint32_t(0.75*tx_rate));
    hackrf_set_amp_enable(hackrf,tx_gain);
    hackrf_set_lna_gain(hackrf,uint32_t(16));
    hackrf_set_vga_gain(hackrf,uint32_t(20));

    hackrf_tx_count=0;
    hackrf_tx_buffer_size=Nsamples*2;
    hackrf_tx_buf.resize(Nsamples*2);

    for (int i=0;i<Nsamples;i++) {

        hackrf_tx_buf[(i<<1)]=(int8_t)(real(tx_buff[i])*128.0);
        hackrf_tx_buf[(i<<1)+1]=(int8_t)(imag(tx_buff[i])*128.0);
    }

    hackrf_start_tx(hackrf,callback_tx,NULL);

    while (hackrf_is_streaming (hackrf) != HACKRF_TRUE){
        cout << "waiting for streaming..." << endl;
    }
    while(hackrf_is_streaming (hackrf)==HACKRF_TRUE){
       if(hackrf_tx_count==Nsamples*2){
       break;
       }
    }
    hackrf_stop_tx(hackrf);
    hackrf_close(hackrf);
    hackrf_exit();
}

void HackRFDevice::waiting_time(double time_value){


    usleep(time_value*1.0e6);

}
void HackRFDevice::setvalue(double value){


    correction=value;

}


void HackRFDevice::init()
{

}

double HackRFDevice::time()
{
    return double(timer.nsecsElapsed())/1.0e9;
}


cvec HackRFDevice::readsamples(int Nsamples, double timestamp){

    if(time()>timestamp){
         cout << "WRONG RX TIMESTAMP!!!!!!!!!!!!!!!!" << endl;
         while(time()>timestamp)
                 timestamp=timestamp+time_gap;
    }
    hackrf=NULL;
    hackrf_init();
    hackrf_open(&hackrf);
    hackrf_set_freq(hackrf,uint64_t(rx_freq));
    hackrf_set_sample_rate(hackrf,rx_rate);
    hackrf_compute_baseband_filter_bw(uint32_t(0.75*rx_rate));
    hackrf_set_amp_enable(hackrf,rx_gain);
    hackrf_set_lna_gain(hackrf,uint32_t(16));
    hackrf_set_vga_gain(hackrf,uint32_t(20));

    hackrf_rx_count=0;
    hackrf_rx_buffer_size=Nsamples*2;
    hackrf_rx_buf.resize(Nsamples*2);
    hackrf_start_rx(hackrf,callback_rx,NULL);
    while (hackrf_is_streaming (hackrf) != HACKRF_TRUE){
        cout << "waiting for streaming..." << endl;
    }
    while(hackrf_is_streaming (hackrf)==HACKRF_TRUE){
       if(hackrf_rx_count==Nsamples*2){
       break;
       }
    }
    cvec buffer;
    buffer.set_size(Nsamples);
    for (int i=0;i<Nsamples;i++) {
        buffer[i]=complex<double>(((double)hackrf_rx_buf[(i<<1)])/128.0,((double)hackrf_rx_buf[(i<<1)+1])/128.0);
    }

    hackrf_stop_rx(hackrf);
    hackrf_close(hackrf);
    hackrf_exit();
    return buffer;

}

double HackRFDevice::sendsamples(cvec tx_buff, double timestamp){

    if(time()>timestamp){
        //cout << "WRONG TX TIMESTAMP!!!!!!!!!!!!!!!!" << endl;
        while(time()>timestamp){
                timestamp=timestamp+time_gap;
        }
    }

    int Nsamples=tx_buff.size();
    hackrf=NULL;
    hackrf_init();
    hackrf_open(&hackrf);
    hackrf_set_freq(hackrf,uint64_t(tx_freq));
    hackrf_set_sample_rate(hackrf,tx_rate);
    hackrf_compute_baseband_filter_bw(uint32_t(0.75*tx_rate));
    hackrf_set_amp_enable(hackrf,tx_gain);
    hackrf_set_lna_gain(hackrf,uint32_t(16));
    hackrf_set_vga_gain(hackrf,uint32_t(20));

    hackrf_tx_count=0;
    hackrf_tx_buffer_size=Nsamples*2;
    hackrf_tx_buf.resize(Nsamples*2);

    for (int i=0;i<Nsamples;i++) {

        hackrf_tx_buf[(i<<1)]=(int8_t)(real(tx_buff[i])*128.0);
        hackrf_tx_buf[(i<<1)+1]=(int8_t)(imag(tx_buff[i])*128.0);
    }

    hackrf_start_tx(hackrf,callback_tx,NULL);

    while (hackrf_is_streaming (hackrf) != HACKRF_TRUE){
        cout << "waiting for streaming..." << endl;
    }
    while(hackrf_is_streaming (hackrf)==HACKRF_TRUE){
       if(hackrf_tx_count==Nsamples*2){
       break;
       }
    }
    hackrf_stop_tx(hackrf);
    hackrf_close(hackrf);
    hackrf_exit();
    return timestamp;

}

void HackRFDevice::run(){

    if(duplex_mode==1){

        if(is_sending){
            while(is_sending){
                if(time()>tx_timestamp){
                    cout << "WRONG TX TIMESTAMP!!!!!!!!!!!!!!!!" << endl;
                    while(time()>tx_timestamp)
                            tx_timestamp=tx_timestamp+time_gap;
                }
                int Nsamples=tx_buff.size();
                hackrf=NULL;
                hackrf_init();
                hackrf_open(&hackrf);
                hackrf_set_freq(hackrf,uint64_t(tx_freq));
                hackrf_set_sample_rate(hackrf,tx_rate);
                hackrf_compute_baseband_filter_bw(uint32_t(0.75*tx_rate));
                hackrf_set_amp_enable(hackrf,tx_gain);
                hackrf_set_lna_gain(hackrf,uint32_t(16));
                hackrf_set_vga_gain(hackrf,uint32_t(20));

                hackrf_tx_count=0;
                hackrf_tx_buffer_size=Nsamples*2;
                hackrf_tx_buf.resize(Nsamples*2);

                for (int i=0;i<Nsamples;i++) {

                    hackrf_tx_buf[(i<<1)]=(int8_t)(real(tx_buff[i])*128.0);
                    hackrf_tx_buf[(i<<1)+1]=(int8_t)(imag(tx_buff[i])*128.0);
                }

                hackrf_start_tx(hackrf,callback_tx,NULL);

                while (hackrf_is_streaming (hackrf) != HACKRF_TRUE){
                    cout << "waiting for streaming..." << endl;
                }
                while(hackrf_is_streaming (hackrf)==HACKRF_TRUE){
                   if(hackrf_tx_count==Nsamples*2){
                   break;
                   }
                }
                hackrf_stop_tx(hackrf);
                hackrf_close(hackrf);
                hackrf_exit();
                if(tx_buff2!=tx_buff){
                    tx_buff=tx_buff2;
                }
                tx_timestamp=tx_timestamp+time_gap;
                has_sent=true;
            }
            has_sent=false;


        }
        if(is_receiving){

            cvec rx_buff2;
             while(is_receiving){
                 if(rx_buff_size!=rx_buff.size()){
                     rx_buff_size=rx_buff.size();
                     rx_buff2.set_size(rx_buff_size);
                     rx_buff2.zeros();
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
                int Nsamples=rx_buff2.size();
                hackrf=NULL;
                hackrf_init();
                hackrf_open(&hackrf);
                hackrf_set_freq(hackrf,uint64_t(rx_freq));
                hackrf_set_sample_rate(hackrf,rx_rate);
                hackrf_compute_baseband_filter_bw(uint32_t(0.75*rx_rate));
                hackrf_set_amp_enable(hackrf,rx_gain);
                hackrf_set_lna_gain(hackrf,uint32_t(16));
                hackrf_set_vga_gain(hackrf,uint32_t(20));

                hackrf_rx_count=0;
                hackrf_rx_buffer_size=Nsamples*2;
                hackrf_rx_buf.resize(Nsamples*2);
                hackrf_start_rx(hackrf,callback_rx,NULL);
                while (hackrf_is_streaming (hackrf) != HACKRF_TRUE){
                    cout << "waiting for streaming..." << endl;
                }
                while(hackrf_is_streaming (hackrf)==HACKRF_TRUE){
                   if(hackrf_rx_count==rx_buff_size*2){
                   break;
                   }
                }
                for (int i=0;i<Nsamples;i++) {
                    rx_buff2[i]=complex<double>(((double)hackrf_rx_buf[(i<<1)])/128.0,((double)hackrf_rx_buf[(i<<1)+1])/128.0);
                }

                hackrf_stop_rx(hackrf);
                hackrf_close(hackrf);
                hackrf_exit();
                mutex.lock();
                rx_buff=rx_buff2;
                mutex.unlock();

             }

        }
    }

    if(duplex_mode==2){

        if(is_sending){
            while(is_sending){
                int Nsamples=tx_buff.size();
                hackrf=NULL;
                hackrf_init();
                hackrf_open(&hackrf);
                hackrf_set_freq(hackrf,uint64_t(tx_freq));
                hackrf_set_sample_rate(hackrf,tx_rate);
                hackrf_compute_baseband_filter_bw(uint32_t(0.75*tx_rate));
                hackrf_set_amp_enable(hackrf,tx_gain);
                hackrf_set_lna_gain(hackrf,uint32_t(16));
                hackrf_set_vga_gain(hackrf,uint32_t(20));

                hackrf_tx_count=0;
                hackrf_tx_buffer_size=Nsamples*2;
                hackrf_tx_buf.resize(Nsamples*2);

                for (int i=0;i<Nsamples;i++) {

                    hackrf_tx_buf[(i<<1)]=(int8_t)(real(tx_buff[i])*128.0);
                    hackrf_tx_buf[(i<<1)+1]=(int8_t)(imag(tx_buff[i])*128.0);
                }

                hackrf_start_tx(hackrf,callback_tx,NULL);

                while (hackrf_is_streaming (hackrf) != HACKRF_TRUE){
                    cout << "waiting for streaming..." << endl;
                }
                while(hackrf_is_streaming (hackrf)==HACKRF_TRUE){
                   if(hackrf_tx_count==Nsamples*2){
                   break;
                   }
                }
                hackrf_stop_tx(hackrf);
                hackrf_close(hackrf);
                hackrf_exit();
                //cout << "Number of samples sent by the Tx streamer in loop " << num_tx_samps << endl;
                if(tx_buff2!=tx_buff){
                   tx_buff=tx_buff2;
                }
                has_sent=true;
            }
            has_sent=false;

        }
        if(is_receiving){

            int rx_buff_size=rx_buff.size();
            rx_buff2.set_size(rx_buff_size);
            rx_buff2.zeros();
            //Receiving mode
            while(is_receiving){
                if((previous_correction!=correction)&&(abs(previous_correction-correction)>10)&&(abs(rx_buff_size-correction)>10)){
                     rx_buff2.set_size(correction);
                     int Nsamples=rx_buff2.size();
                     hackrf=NULL;
                     hackrf_init();
                     hackrf_open(&hackrf);
                     hackrf_set_freq(hackrf,uint64_t(rx_freq));
                     hackrf_set_sample_rate(hackrf,rx_rate);
                     hackrf_compute_baseband_filter_bw(uint32_t(0.75*rx_rate));
                     hackrf_set_amp_enable(hackrf,rx_gain);
                     hackrf_set_lna_gain(hackrf,uint32_t(16));
                     hackrf_set_vga_gain(hackrf,uint32_t(20));

                     hackrf_rx_count=0;
                     hackrf_rx_buffer_size=Nsamples*2;
                     hackrf_rx_buf.resize(Nsamples*2);
                     hackrf_start_rx(hackrf,callback_rx,NULL);
                     while (hackrf_is_streaming (hackrf) != HACKRF_TRUE){
                         cout << "waiting for streaming..." << endl;
                     }
                     while(hackrf_is_streaming (hackrf)==HACKRF_TRUE){
                        if(hackrf_rx_count==rx_buff_size*2){
                        break;
                        }
                     }
                     for (int i=0;i<Nsamples;i++) {
                         rx_buff2[i]=complex<double>(((double)hackrf_rx_buf[(i<<1)])/128.0,((double)hackrf_rx_buf[(i<<1)+1])/128.0);
                     }

                     hackrf_stop_rx(hackrf);
                     hackrf_close(hackrf);
                     hackrf_exit();
                     if(correction>rx_buff_size)
                         rx_buff=rx_buff2.get(0,rx_buff_size-1);
                     else
                         rx_buff=concat(rx_buff2,zeros_c(rx_buff_size-correction));
                     rx_buff2.set_size(rx_buff_size);
                     previous_correction=correction;
                }

                int Nsamples=rx_buff2.size();
                hackrf=NULL;
                hackrf_init();
                hackrf_open(&hackrf);
                hackrf_set_freq(hackrf,uint64_t(rx_freq));
                hackrf_set_sample_rate(hackrf,rx_rate);
                hackrf_compute_baseband_filter_bw(uint32_t(0.75*rx_rate));
                hackrf_set_amp_enable(hackrf,rx_gain);
                hackrf_set_lna_gain(hackrf,uint32_t(16));
                hackrf_set_vga_gain(hackrf,uint32_t(20));

                hackrf_rx_count=0;
                hackrf_rx_buffer_size=Nsamples*2;
                hackrf_rx_buf.resize(Nsamples*2);
                hackrf_start_rx(hackrf,callback_rx,NULL);
                while (hackrf_is_streaming (hackrf) != HACKRF_TRUE){
                    cout << "waiting for streaming..." << endl;
                }
                while(hackrf_is_streaming (hackrf)==HACKRF_TRUE){
                   if(hackrf_rx_count==rx_buff_size*2){
                   break;
                   }
                }
                for (int i=0;i<Nsamples;i++) {
                    rx_buff2[i]=complex<double>(((double)hackrf_rx_buf[(i<<1)])/128.0,((double)hackrf_rx_buf[(i<<1)+1])/128.0);
                }

                hackrf_stop_rx(hackrf);
                hackrf_close(hackrf);
                hackrf_exit();
                rx_buff=rx_buff2;

            }

        }

    }


}
