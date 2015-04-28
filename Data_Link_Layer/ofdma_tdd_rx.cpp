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
#include "ofdma_tdd_rx.h"

OFDMA_TDD_RX::OFDMA_TDD_RX(Ui_MainWindow *ui, int fd_ext)
{

    gui=ui;
    stop_signal=false;
#if HACKRF_ENABLED==1
    device = new HackRFDevice();
#endif
#if UHD_ENABLED==1
    device = new UHDDevice();
#endif
#if VIRTUAL_ENABLED==1
    device = new VirtualDevice();
#endif
#if SIMULATOR_ENABLED==1
    device = new SimulatorDevice();
#endif
    device->duplex_mode=1;
    device->init();
    txrate=device->tx_rate;
    txfreq=device->tx_freq;
    txgain=device->tx_gain;
    txamplitude=device->tx_amplitude;
    rxrate=device->rx_rate;
    rxfreq=device->rx_freq;
    rxgain=device->rx_gain;

    myaddress=1;
    destaddress=1;
    gui->lineEdit_2->setText(number.setNum(txrate/1.0e6));
    gui->lineEdit_3->setText(number.setNum(txfreq/1.0e6));
    gui->lineEdit_4->setText(number.setNum(txgain));
    gui->lineEdit_5->setText(number.setNum(txamplitude));
    gui->lineEdit_6->setText(number.setNum(rxrate/1.0e6));
    gui->lineEdit_7->setText(number.setNum(rxfreq/1.0e6));
    gui->lineEdit_8->setText(number.setNum(rxgain));
    gui->lineEdit_9->setText(number.setNum(myaddress));
    gui->lineEdit_10->setText(number.setNum(destaddress));

    connect(this,SIGNAL(valuechanged(double)),device,SLOT(setvalue(double)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(updated_uhd()),this,SLOT(update_uhd()),Qt::BlockingQueuedConnection);
    waveform=2;
    last_waveform=0;
    detected_group=0;
    estimated_throughput=0;
    previous_estimated_throughput=0;
    previous_time=0;
    Nfft=512;
    previous_packet_number=0;
    ptr=fd_ext;
}

void OFDMA_TDD_RX::update_uhd(){

    gui->lineEdit_2->setText(number.setNum(txrate/1.0e6));
    gui->lineEdit_3->setText(number.setNum(txfreq/1.0e6));
    gui->lineEdit_4->setText(number.setNum(txgain));
    gui->lineEdit_5->setText(number.setNum(txamplitude));
    gui->lineEdit_6->setText(number.setNum(rxrate/1.0e6));
    gui->lineEdit_7->setText(number.setNum(rxfreq/1.0e6));
    gui->lineEdit_8->setText(number.setNum(rxgain));

}

void OFDMA_TDD_RX::setvalue(bool value){


    first_tx_timestamp=value;


}

void OFDMA_TDD_RX::run(){

    stop_signal=false;
    noderunning=true;
    is_time_set=false;
    first_tx_timestamp=true;
    cvec previous_rx_buff;


    while(!stop_signal){


        if((txfreq!=device->tx_freq)||(txgain!=device->tx_gain)||(txamplitude!=device->tx_amplitude)||(rxfreq!=device->rx_freq)||(rxgain!=device->rx_gain)){

            device->init();
            txfreq=device->tx_freq;
            txgain=device->tx_gain;
            txamplitude=device->tx_amplitude;
            rxfreq=device->rx_freq;
            rxgain=device->rx_gain;

            emit updated_uhd();


        }
        if((last_waveform!=waveform)&&(waveform==2)){
            last_waveform=waveform;
            mcdaaofdm = new Modem_MCDAAOFDM();
            Nfft=mcdaaofdm->Nfft;
            Ncp=mcdaaofdm->Ncp;
            num_subchannels=mcdaaofdm->num_subchannels;
            int Number_of_OFDM_symbols=mcdaaofdm->Number_of_OFDM_symbols;
            int Modulation_Order=mcdaaofdm->Modulation_Order;
            sum_mask=(Nfft-3*Nfft/16)/num_subchannels*Modulation_Order;
            if((Number_of_OFDM_symbols*Nfft/sum_mask)*sum_mask!=Number_of_OFDM_symbols*Nfft)
                Number_of_received_symbols=(Number_of_OFDM_symbols*Nfft/sum_mask+1)*(Nfft+Ncp);
            else
                Number_of_received_symbols=(Number_of_OFDM_symbols*Nfft/sum_mask)*(Nfft+Ncp);
            int number_of_slots=2;
            time_gap=number_of_slots*(Number_of_received_symbols/rxrate);
            correction=0;
            cout << "TIME GAP RX " << time_gap << endl;
            device->time_gap=time_gap;
            packet = new Packet(Nfft*Number_of_OFDM_symbols);
            is_resynchronized=false;


        }

        //cout << "########### PROCESSING RX PACKET ########### " << device->time() << " #############" << endl;

        if((is_time_set==false)&&(first_tx_timestamp==true)){

            if(!device->isRunning()){
                rx_buff=device->readsamplesnow(Number_of_received_symbols);
            }
        }
        else{

            if(device->is_receiving==false){
                device->rx_buff.set_size(Number_of_received_symbols);
                device->rx_buff.zeros();
                device->is_receiving=true;
                if(first_tx_timestamp==false){
                    device->timestamp=(int(device->time()/time_gap)+1)*time_gap;
                }
                else{
                    device->timestamp=timestamp;
                }
                device->start();
            }

            rx_buff=device->rx_buff;

        }
        if((rx_buff!=previous_rx_buff)&&(rx_buff[0]!=0.0)){

            previous_rx_buff=rx_buff;

            //Calculate the spectrum
            spectrum_sensed=sensing->spectrum_block(rx_buff,Nfft);
            //Plot the spectrum
            emit plotted(spectrum_sensed,1);


            bool preamble_ok=false;
            int preamble_start=0;
            if(waveform==2){
                //Compute the best group of subcarriers
                rx_best_group=sensing->best_group_mask(spectrum_sensed,num_subchannels);
                //The following lines determine if there is an OFDM signal using part of the bandwidth on the different sub-channels.
                int i=0;
                ivec mask(Nfft);
                cvec rx_buff2(rx_buff.size());
                bool is_ofdm=false;
                double metric=0;
                ivec subchannel_preamble_vector(num_subchannels);
                ivec subchannel_packet_vector(num_subchannels);
                while(i<num_subchannels){
                    //cout << "SUBCHANNEL " << i << endl;
                        mask.zeros();
                        if(num_subchannels==1)
                            mask.replace_mid(Nfft/32,concat(ones_i(Nfft/2-3*Nfft/32),zeros_i(2*Nfft/16),ones_i(Nfft/2-3*Nfft/32)));
                        else{
                            if(i<num_subchannels/2)
                                mask.replace_mid(Nfft/32+i*(Nfft-3*Nfft/16)/num_subchannels,ones_i((Nfft-3*Nfft/16)/num_subchannels));
                            if(i>=num_subchannels/2)
                                mask.replace_mid(Nfft/32+2*Nfft/16+i*(Nfft-3*Nfft/16)/num_subchannels,ones_i((Nfft-3*Nfft/16)/num_subchannels));
                        }
                        rx_buff2.zeros();

                        for(int j=0;j<rx_buff2.size()/Nfft;j++){
                            rx_buff2.replace_mid(j*Nfft,ifft(elem_mult(to_cvec(mask),fft(rx_buff.get(j*Nfft,(j+1)*Nfft-1)))));
                        }
                        is_ofdm=mcdaaofdm->detection(rx_buff2,metric);
                        vec estimated_channel;
                        cvec demodulated_ofdm_symbols=mcdaaofdm->equalizer_fourth_power(rx_buff,i,estimated_channel);
                        cvec constellation;
                        received_bits=mcdaaofdm->demodulate_mask_gray_qpsk(demodulated_ofdm_symbols,i,constellation);
                        preamble_ok=mcdaaofdm->preamble_detection_qpsk(received_bits,received_bits2,preamble_start);
                        bool same_packet=false;
                        bool packet_ok=packet->decode_packet(received_bits2,myaddress,same_packet,ptr);
                        if(preamble_ok==true)
                            subchannel_preamble_vector[i]=1;
                        else
                            subchannel_preamble_vector[i]=0;
                        if(packet_ok==true)
                            subchannel_packet_vector[i]=1;
                        else
                            subchannel_packet_vector[i]=0;
                    i=i+1;
                }
                if(max(subchannel_packet_vector)==1){

                        detected_group=max_index(subchannel_packet_vector);
                        //Recalculate the time offset based on the sub-channel on which an OFDM signal has been detected
                        mask.zeros();
                        if(num_subchannels==1)
                            mask.replace_mid(Nfft/32,concat(ones_i(Nfft/2-3*Nfft/32),zeros_i(2*Nfft/16),ones_i(Nfft/2-3*Nfft/32)));
                        else{
                            if(detected_group<num_subchannels/2)
                                mask.replace_mid(Nfft/32+detected_group*(Nfft-3*Nfft/16)/num_subchannels,ones_i((Nfft-3*Nfft/16)/num_subchannels));
                            if(detected_group>=num_subchannels/2)
                                mask.replace_mid(Nfft/32+2*Nfft/16+detected_group*(Nfft-3*Nfft/16)/num_subchannels,ones_i((Nfft-3*Nfft/16)/num_subchannels));
                        }
                        rx_buff2.zeros();

                        for(int i=0;i<rx_buff2.size()/Nfft;i++){
                            rx_buff2.replace_mid(i*Nfft,ifft(elem_mult(to_cvec(mask),fft(rx_buff.get(i*Nfft,(i+1)*Nfft-1)))));
                        }
                        is_ofdm=mcdaaofdm->detection(rx_buff2,metric);
                        vec estimated_channel;
                        cvec demodulated_ofdm_symbols=mcdaaofdm->equalizer_fourth_power(rx_buff,detected_group,estimated_channel);
                        cvec constellation;
                        received_bits=mcdaaofdm->demodulate_mask_gray_qpsk(demodulated_ofdm_symbols,detected_group,constellation);
                        emit plotted(constellation,2);
                        preamble_ok=mcdaaofdm->preamble_detection_qpsk(received_bits,received_bits2,preamble_start);
                }
                else{
                    //Transfer an empty group
                    if(min(subchannel_preamble_vector)==0)
                        emit valuechanged(min_index(subchannel_preamble_vector));
                    else
                        emit valuechanged(num_subchannels);

                }

            }

            //cout << "PREAMBLE START " << preamble_start << endl;
            //cout << "TIME OFFSET ESTIMATE " << mcdaaofdm->time_offset_estimate << endl;
            //cout << "TIME OFFSET ESTIMATE " << time_offset_estimate << endl;
            if(preamble_ok==true){

                if(packet->is_ber_count==true)
                    gui->label_10->setText(number.setNum(packet->ber_count(received_bits2),'e',2));



               if((is_time_set==false)&&(first_tx_timestamp==true)){
                        double sync_time;
                        if(waveform==2){
                            if(mcdaaofdm->time_offset_estimate>Nfft-1)
                                    sync_time=(device->rx_md.time_spec).get_real_secs()+(mcdaaofdm->time_offset_estimate-(Nfft+Ncp))/rxrate+int(preamble_start/sum_mask)*(Nfft+Ncp)/rxrate;
                            else
                                    sync_time=(device->rx_md.time_spec).get_real_secs()+mcdaaofdm->time_offset_estimate/rxrate+int(preamble_start/sum_mask)*(Nfft+Ncp)/rxrate;
                        }
                        timestamp=sync_time;
                        is_time_set=true;
                        emit valuechanged(is_time_set,sync_time);
                        cout << "TIME HAS BEEN SET FOR SYNCHRONIZATION" << endl;

                }
                else{
                        is_time_set=true;
                        //Synchronization of the receive stream
                        if(waveform==2){
                            if(mcdaaofdm->time_offset_estimate>Nfft-1)
                                correction=(mcdaaofdm->time_offset_estimate-(Nfft+Ncp))/rxrate+int(preamble_start/sum_mask)*(Nfft+Ncp)/rxrate;
                            else
                                correction=(mcdaaofdm->time_offset_estimate)/rxrate+int(preamble_start/sum_mask)*(Nfft+Ncp)/rxrate;
                        }
                        emit valuechanged(correction);

                }
                bool same_packet=false;
                bool packet_ok=packet->decode_packet(received_bits2,myaddress,same_packet,ptr);
                if(packet_ok==true){
                    if(previous_packet_number+100<packet->packetnorx){
                        emit valuechanged(is_time_set,(device->rx_md.time_spec).get_real_secs()+correction);
                        previous_packet_number=packet->packetnorx;
                    }
                    if(is_resynchronized==false)
                             is_resynchronized=true;
                    if(waveform==2){
                        //Transfer the best group to TX
                        emit valuechanged(detected_group);
                    }

                    if(is_time_set){
                        //Throughput calculation
                        if(same_packet){
                            double current_time=device->time();
                            if(current_time>previous_time){
                                previous_estimated_throughput=estimated_throughput;
                                estimated_throughput=0.9*estimated_throughput;
                                previous_time=current_time;
                            }
                        }
                        else{
                            if(previous_time==0){
                                previous_time=device->time();
                            }
                            else{
                                double current_time=device->time();
                                if(current_time>previous_time){
                                    previous_estimated_throughput=estimated_throughput;
                                    estimated_throughput=0.9*estimated_throughput+0.1*received_bits2.size()/(current_time-previous_time);
                                    previous_time=current_time;
                                }
                            }
                        }
                    }


                }
                else{

                    if(is_time_set){
                        double current_time=device->time();
                        if(current_time>previous_time){
                            previous_estimated_throughput=estimated_throughput;
                            estimated_throughput=0.9*estimated_throughput;
                            previous_time=current_time;
                        }
                    }

                }

            }
            else{
                if(packet->is_ber_count==true)
                    gui->label_10->setText(number.setNum(packet->ber_count(received_bits2),'e',2));

                cout << "SOF NOT FOUND" << endl;

                if(waveform==2){
                    //Transfer the best group to TX
                    emit valuechanged(rx_best_group);
                }
                if((is_time_set)&&(is_resynchronized)){
                    double current_time=device->time();
                    if(current_time>previous_time){
                        previous_estimated_throughput=estimated_throughput;
                        estimated_throughput=0.9*estimated_throughput;
                        previous_time=current_time;
                    }
                }
            }
            cout << "Throughput: " << int(estimated_throughput/1000) << " kbps" << endl;
            qApp->processEvents();
        }
    }

    //finished
    noderunning=false;
    device->is_receiving=false;
    std::cout << std::endl << "Done!" << std::endl << std::endl;



}
