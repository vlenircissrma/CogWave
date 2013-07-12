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

#include "blindofdm_tdd_mode_rx.h"


BlindOFDM_TDD_Mode_RX::BlindOFDM_TDD_Mode_RX(Ui_MainWindow *ui)
{
    gui=ui;
    stop_signal=false;
    device = new BlindOFDM_UHDDevice();
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
    blindofdm = new BlindOFDM_Modem();
    Nfft=blindofdm->Nfft;
    Ncp=blindofdm->Ncp;
    num_subchannels=blindofdm->num_subchannels;
    Number_of_OFDM_symbols=blindofdm->Number_of_OFDM_symbols;
    Modulation_Order=blindofdm->Modulation_Order;

    sum_mask=(Nfft-3*Nfft/16)/num_subchannels*Modulation_Order;
    if((Number_of_OFDM_symbols*Nfft/sum_mask)*sum_mask!=Number_of_OFDM_symbols*Nfft)
        Number_of_received_symbols=(Number_of_OFDM_symbols*Nfft/sum_mask+1)*(Nfft+Ncp);
    else
        Number_of_received_symbols=(Number_of_OFDM_symbols*Nfft/sum_mask)*(Nfft+Ncp);
    number_of_slots=2;
    time_gap=number_of_slots*(Number_of_received_symbols/rxrate);
    correction=0;
    cout << "TIME GAP " << time_gap << endl;
    packets = new BlindOFDM_Framing(Nfft,Number_of_OFDM_symbols);
    sensing = new BlindOFDM_Sensing();
    start_frame=0;
    sync_time=0;
    is_time_set=false;
    first_tx_timestamp=true;
    is_synchronized=false;


    connect(this,SIGNAL(updated_uhd()),this,SLOT(update_uhd()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(valuechanged(double)),device,SLOT(setvalue(double)),Qt::BlockingQueuedConnection);

}

void BlindOFDM_TDD_Mode_RX::update_uhd(){

    gui->lineEdit_2->setText(number.setNum(txrate/1.0e6));
    gui->lineEdit_3->setText(number.setNum(txfreq/1.0e6));
    gui->lineEdit_4->setText(number.setNum(txgain));
    gui->lineEdit_5->setText(number.setNum(txamplitude));
    gui->lineEdit_6->setText(number.setNum(rxrate/1.0e6));
    gui->lineEdit_7->setText(number.setNum(rxfreq/1.0e6));
    gui->lineEdit_8->setText(number.setNum(rxgain));

}

void BlindOFDM_TDD_Mode_RX::init(){

    if(Nfft<4)
        Nfft=4;
    cout << "Setting FFT Size " << Nfft << endl;
    if(Ncp<0)
        Ncp=0;
    if(Ncp>Nfft/4)
        Ncp=Nfft/4;
    cout << "Setting CP Size " << Ncp << endl;
    if(num_subchannels<=0)
        num_subchannels=1;
    if(num_subchannels>Nfft)
        num_subchannels=Nfft;
    if(((num_subchannels/2)*2!=num_subchannels)&&(num_subchannels!=1))
        num_subchannels=num_subchannels-1;
    cout << "Setting Number of Subchannels " <<  num_subchannels << endl;
    if(((Number_of_OFDM_symbols/8)*8)!=Number_of_OFDM_symbols)
        Number_of_OFDM_symbols=(Number_of_OFDM_symbols/8)*8;
    cout << "Setting Number of OFDM symbols " <<  Number_of_OFDM_symbols << endl;


}

void BlindOFDM_TDD_Mode_RX::setvalue(bool value){


    first_tx_timestamp=value;


}

void BlindOFDM_TDD_Mode_RX::run(){

    stop_signal=false;
    noderunning=true;
    bool packet_ok;


    while(!stop_signal){

        packet_ok=false;

        if((txrate!=device->tx_rate)||(txfreq!=device->tx_freq)||(txgain!=device->tx_gain)||(txamplitude!=device->tx_amplitude)||(rxrate!=device->rx_rate)||(rxfreq!=device->rx_freq)||(rxgain!=device->rx_gain)){

            device->init();
            txrate=device->tx_rate;
            txfreq=device->tx_freq;
            txgain=device->tx_gain;
            txamplitude=device->tx_amplitude;
            rxrate=device->rx_rate;
            rxfreq=device->rx_freq;
            rxgain=device->rx_gain;

            emit updated_uhd();

            time_gap=number_of_slots*(Number_of_received_symbols/rxrate);
            cout << "TIME GAP " << time_gap << endl;
            device->timestamp=(int(device->time()/time_gap)+1)*time_gap;
            device->time_gap=time_gap;


        }


            //cout << "########### PROCESSING RX PACKET ########### " << device->time() << " #############" << endl;

            if((is_time_set==false)&&(first_tx_timestamp==true)){

                timestamp=double(int(device->time()*100)+12)/100;
                rx_buff=device->readsamples(Number_of_received_symbols,timestamp);

            }
            else{

                if(device->is_receiving==false){
                    device->rx_buff.set_size(Number_of_received_symbols);
                    device->rx_buff.zeros();
                    device->is_receiving=true;
                    if(first_tx_timestamp==false)
                        device->timestamp=(int(device->time()/time_gap)+1)*time_gap;
                    else
                        device->timestamp=timestamp;
                    device->time_gap=time_gap;
                    device->start();
                }

                rx_buff=device->rx_buff;

            }

            //Calculate the spectrum
            spectrum_sensed=sensing->spectrum_block(rx_buff,Nfft);

            //Plot the spectrum
            emit plotted(spectrum_sensed,1);

            //Check UHD errors
            device->rxerrors();

            //Compute the best group of subcarriers
            tx_best_group=sensing->best_group_mask(spectrum_sensed,num_subchannels);

            //Detect if there is a OFDM signal in the received samples
            //The following line determine if there is an OFDM signal using the whole bandwidth
            //is_ofdm=blindofdm->detection(rx_buff);

            //The following lines determine if there is an OFDM signal using part of the bandwidth on the different sub-channels.
            int i=0;
            ivec mask(Nfft);
            cvec rx_buff2(rx_buff.size());
            is_ofdm=false;
            double metric=0;
            double detected_metric=0;
            int detected_group=0;
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
                    is_ofdm=blindofdm->detection(rx_buff2,metric);
                    if(is_ofdm){
                        if(detected_metric==0){
                            detected_group=i;
                            detected_metric=metric;
                        }
                        else{
                            if(metric>detected_metric){
                                detected_group=i;
                                detected_metric=metric;
                            }
                        }
                    }


                i=i+1;
            }
            if(detected_metric!=0){

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
                is_ofdm=blindofdm->detection(rx_buff2);

                //Demodulate the signal for the detected sub-channel
                int src_adress=0;
                start_frame=0;
                if(Modulation_Order==1){
                    cvec demodulated_ofdm_symbols=blindofdm->equalizer_second_power(rx_buff,detected_group,estimated_channel);
                    received_bits=blindofdm->demodulate_mask_gray_bpsk(demodulated_ofdm_symbols,detected_group,constellation);
                }
                if(Modulation_Order==2){
                    cvec demodulated_ofdm_symbols=blindofdm->equalizer_fourth_power(rx_buff,detected_group,estimated_channel);
                    received_bits=blindofdm->demodulate_mask_gray_qpsk(demodulated_ofdm_symbols,detected_group,constellation);
                }
                if(Modulation_Order==4){

                    //Quadrant 16QAM
                    //cvec demodulated_ofdm_symbols=blindofdm->equalizer_fourth_power(rx_buff,detected_group,estimated_channel);
                    //received_bits=blindofdm->demodulate_mask_quadrant_16qam(demodulated_ofdm_symbols,detected_group,constellation);
                    //Gray 16QAM
                    cvec demodulated_ofdm_symbols=blindofdm->equalizer_fourth_power(rx_buff,detected_group,estimated_channel);
                    received_bits=blindofdm->demodulate_mask_gray_16qam(demodulated_ofdm_symbols,detected_group,constellation);
                }
                if(Modulation_Order==6){
                    //Gray 64QAM
                    cvec demodulated_ofdm_symbols=blindofdm->equalizer_fourth_power(rx_buff,detected_group,estimated_channel);
                    received_bits=blindofdm->demodulate_mask_gray_64qam(demodulated_ofdm_symbols,detected_group,constellation);
                }
                if(Modulation_Order==8){
                    //Gray 256QAM
                    cvec demodulated_ofdm_symbols=blindofdm->equalizer_fourth_power(rx_buff,detected_group,estimated_channel);
                    received_bits=blindofdm->demodulate_mask_gray_256qam(demodulated_ofdm_symbols,detected_group,constellation);
                }
                emit plotted(constellation,2);
                if(Modulation_Order==1){
                    //DBPSK
                    //received_bits2=blindofdm->bpsk_differential_decoding(received_bits);
                    //BPSK
                    bool preamble_ok=blindofdm->preamble_detection_bpsk(received_bits,received_bits2);
                }
                if(Modulation_Order==2){
                    //DQPSK
                    //received_bits2=blindofdm->qpsk_differential_decoding(received_bits);
                    //QPSK
                    bool preamble_ok=blindofdm->preamble_detection_qpsk(received_bits,received_bits2);
                }
                if(Modulation_Order==4){
                    //D16QAM
                    //received_bits2=blindofdm->qam16_gray_differential_decoding(received_bits);
                    //received_bits2=blindofdm->qam16_quadrant_differential_decoding(received_bits);
                    //16QAM
                    bool preamble_ok=blindofdm->preamble_detection_gray_16qam(received_bits,received_bits2);
                    //bool preamble_ok=blindofdm->preamble_detection_quadrant_16qam(received_bits,received_bits2);
                }
                if(Modulation_Order==6){
                    //64QAM
                    bool preamble_ok=blindofdm->preamble_detection_gray_64qam(received_bits,received_bits2);
                }
                if(Modulation_Order==8){
                    //256AM
                    bool preamble_ok=blindofdm->preamble_detection_gray_256qam(received_bits,received_bits2);
                }
                packet_ok=packets->decode_frame(received_bits2,myaddress,src_adress,tx_best_group,start_frame,sync_time,num_subchannels);

                if((packet_ok==true)&&(tx_best_group>=0)&&(tx_best_group<num_subchannels)){

                    //Transfer the best group to TX
                    emit valuechanged(tx_best_group);

                    //cout << "TIME " << sync_time << endl;
                    //cout << "START FRAME " << start_frame << endl;
                    //cout << "TIME OFFSET "  << blindofdm->time_offset_estimate << endl;
                    if((is_time_set==false)&&(first_tx_timestamp==true)){
                        if(blindofdm->time_offset_estimate>Nfft-1){
                                device->set_time(sync_time-((device->rx_md.time_spec).get_real_secs()+(blindofdm->time_offset_estimate-(Nfft+Ncp))/rxrate+int((start_frame*8)/sum_mask)*(Nfft+Ncp)/rxrate));
                        }
                        else{
                                device->set_time(sync_time-((device->rx_md.time_spec).get_real_secs()+blindofdm->time_offset_estimate/rxrate+int((start_frame*8)/sum_mask)*(Nfft+Ncp)/rxrate));
                        }

                        is_time_set=true;
                        emit valuechanged(is_time_set);
                        timestamp=sync_time;
                        device->timestamp=timestamp;
                        //cout << "TIME HAS BEEN SET FOR SYNCHRONIZATION" << endl;
                    }
                    else{//Synchronization of the receive stream
                        if(blindofdm->time_offset_estimate>Nfft-1){
                                correction=(blindofdm->time_offset_estimate-(Nfft+Ncp))/rxrate+int((start_frame*8)/sum_mask)*(Nfft+Ncp)/rxrate;
                        }
                        else{
                                correction=(blindofdm->time_offset_estimate)/rxrate+int((start_frame*8)/sum_mask)*(Nfft+Ncp)/rxrate;
                        }
                        emit valuechanged(correction);
                    }
                }
                if(packet_ok==false){
                    cout << "SOF NOT FOUND" << endl;
                    //cout << "TIME OFFSET "  << blindofdm->time_offset_estimate << endl;
                    //device->correction=blindofdm->time_offset_estimate/rxrate;
                }

            }
            else{
                //Transfer the best group to TX
                emit valuechanged(tx_best_group);
            }

        qApp->processEvents();
    }

    //finished
    noderunning=false;
    device->is_receiving=false;
    std::cout << std::endl << "Done!" << std::endl << std::endl;






}
