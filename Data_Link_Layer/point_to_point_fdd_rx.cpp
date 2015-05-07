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
#include "point_to_point_fdd_rx.h"

Point_to_Point_FDD_RX::Point_to_Point_FDD_RX(Ui_MainWindow *ui, int fd_ext)
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
    device->duplex_mode=2;
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
    if((gui->comboBox->currentText()=="L1:DADS")||(gui->comboBox->currentText()=="L1:DADS-MCDAAOFDM"))
        waveform=1;
    if(gui->comboBox->currentText()=="L1:MCDAAOFDM")
        waveform=2;
    if(gui->comboBox->currentText()=="L1:BPSK")
        waveform=3;
    if(gui->comboBox->currentText()=="L1:GMSK")
        waveform=4;
    if(gui->comboBox->currentText()=="L1:QPSK")
        waveform=5;
    if(gui->comboBox->currentText()=="L1:CPFSK")
        waveform=6;
    if(gui->comboBox->currentText()=="L1:CORASMA")
        waveform=7;
    if(gui->comboBox->currentText()=="L1:NBWF")
        waveform=8;
    if(gui->comboBox->currentText()=="L1:DSSS")
        waveform=9;
    if(gui->comboBox->currentText()=="L1:MCDADS")
        waveform=10;
    if(gui->comboBox->currentText()=="L1:OFDM")
        waveform=11;
    last_waveform=0;
    detected_group=0;
    estimated_throughput=0;
    previous_estimated_throughput=0;
    previous_time=0;
    Nfft=64;
    ptr=fd_ext;

}

void Point_to_Point_FDD_RX::update_uhd(){

    gui->lineEdit_2->setText(number.setNum(txrate/1.0e6));
    gui->lineEdit_3->setText(number.setNum(txfreq/1.0e6));
    gui->lineEdit_4->setText(number.setNum(txgain));
    gui->lineEdit_5->setText(number.setNum(txamplitude));
    gui->lineEdit_6->setText(number.setNum(rxrate/1.0e6));
    gui->lineEdit_7->setText(number.setNum(rxfreq/1.0e6));
    gui->lineEdit_8->setText(number.setNum(rxgain));

}

void Point_to_Point_FDD_RX::run(){

    stop_signal=false;
    noderunning=true;
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

        if((last_waveform!=waveform)&&(waveform==1)){
            last_waveform=waveform;
            dads = new Modem_DADS();
            int nb_bits=dads->nb_bits;
            SF=dads->SF;
            OF=rxrate/txrate;
            Number_of_received_symbols=SF*(nb_bits+1)*OF;
            packet = new Packet(nb_bits);
            correction=0;
            is_resynchronized=false;
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
            correction=0;
            packet = new Packet(Nfft*Number_of_OFDM_symbols);
            is_resynchronized=false;


        }
        if((last_waveform!=waveform)&&(waveform==3)){
            last_waveform=waveform;
            bpsk = new Modem_BPSK();
            int nb_bits=bpsk->nb_bits;
            OF=bpsk->OF;
            Number_of_received_symbols=nb_bits*OF;
            packet = new Packet(nb_bits);
            correction=0;
            is_resynchronized=false;
        }
        if((last_waveform!=waveform)&&(waveform==4)){
            last_waveform=waveform;
            gmsk = new Modem_GMSK();
            int nb_bits=gmsk->nb_bits;
            OF=gmsk->OF;
            Number_of_received_symbols=nb_bits*OF;
            packet = new Packet(nb_bits);
            correction=0;
            is_resynchronized=false;
        }
        if((last_waveform!=waveform)&&(waveform==5)){
            last_waveform=waveform;
            qpsk = new Modem_QPSK();
            int nb_bits=qpsk->nb_bits;
            OF=qpsk->OF;
            Number_of_received_symbols=nb_bits*OF/2;
            packet = new Packet(nb_bits);
            correction=0;
            is_resynchronized=false;
        }
        if((last_waveform!=waveform)&&(waveform==6)){
            last_waveform=waveform;
            cpfsk = new Modem_CPFSK();
            int nb_bits=cpfsk->nb_bits;
            OF=cpfsk->OF;
            Number_of_received_symbols=nb_bits*OF;
            packet = new Packet(nb_bits);
            correction=0;
            is_resynchronized=false;
        }
        if((last_waveform!=waveform)&&(waveform==7)){
            last_waveform=waveform;
            corasma = new Modem_CORASMA();
            Nfft=corasma->Nfft;
            Ncp=corasma->Ncp;
            bitspersymbol=corasma->bitspersymbol;
            int nb_bits=corasma->nb_bits;
            int nb_symbols=corasma->nb_symbols;
            Number_of_received_symbols=nb_symbols;
            packet = new Packet(nb_bits);
            correction=0;
            is_resynchronized=false;
        }
        if((last_waveform!=waveform)&&(waveform==8)){
            last_waveform=waveform;
            nbwf = new Modem_NBWF();
            Nfft=nbwf->Nfft;
            int nb_bits=nbwf->nb_bits;
            Number_of_received_symbols=nbwf->nb_symbols;
            packet = new Packet(nb_bits);
            correction=0;
            is_resynchronized=false;
        }
        if((last_waveform!=waveform)&&(waveform==9)){
            last_waveform=waveform;
            dsss = new Modem_DSSS();
            int nb_bits=dsss->nb_bits;
            SF=dsss->SF;
            OF=dsss->OF;
            Number_of_received_symbols=SF*nb_bits*OF;
            packet = new Packet(nb_bits);
            correction=0;
            is_resynchronized=false;
        }
        if((last_waveform!=waveform)&&(waveform==10)){
            last_waveform=waveform;
            mcdads = new Modem_MCDADS();
            int nb_bits=mcdads->nb_bits;
            SF=mcdads->SF;
            OF=rxrate/txrate;
            Nfft=mcdads->Nfft;
            Ncp=mcdads->Ncp;
            sum_mask=mcdads->sum_mask;
            if((SF*(nb_bits+1)*OF/sum_mask)*sum_mask!=SF*(nb_bits+1)*OF)
                Number_of_received_symbols=(SF*(nb_bits+1)*OF/sum_mask+1)*(Nfft+Ncp);
            else
                Number_of_received_symbols=(SF*(nb_bits+1)*OF/sum_mask)*(Nfft+Ncp);
            packet = new Packet(nb_bits);
            correction=0;
            is_resynchronized=false;
        }
        if((last_waveform!=waveform)&&(waveform==11)){
            last_waveform=waveform;
            ofdm = new Modem_OFDM();
            int nb_bits=ofdm->nb_bits;
            Nfft=ofdm->fft_len;
            Ncp=ofdm->cp_len;
            Number_of_received_symbols=ofdm->Number_of_received_symbols;
            packet = new Packet(nb_bits);
            correction=0;
            is_resynchronized=false;
        }
        //cout << "########### PROCESSING RX PACKET ########### " << device->time() << " #############" << endl;

        if(device->is_receiving==false){
            device->rx_buff.set_size(Number_of_received_symbols);
            device->rx_buff.zeros();
            device->is_receiving=true;
            device->start();
        }
        device->mutex.lock();
        rx_buff=device->rx_buff;
        device->mutex.unlock();
        if((rx_buff!=previous_rx_buff)&&(rx_buff[0]!=0.0)){

            previous_rx_buff=rx_buff;

            //Calculate the spectrum
            spectrum_sensed=sensing->spectrum_block(rx_buff,Nfft);
            //Plot the spectrum
            emit plotted(spectrum_sensed,1);


            bool preamble_ok=false;
            int preamble_start=0;
            int time_offset_estimate=0;
            int ofdm_time_offset_estimate=0;
            int dads_time_offset_estimate=0;
            if(waveform==1){
                vec constellation;
                received_bits=dads->demodulate(rx_buff,OF,time_offset_estimate,constellation);
                emit plotted(to_cvec(constellation),2);
                preamble_ok=dads->preamble_detection(received_bits,received_bits2,preamble_start);
            }
            if(waveform==2){
                //Compute the best group of subcarriers
                rx_best_group=sensing->best_group_mask(spectrum_sensed,num_subchannels);
                //The following lines determine if there is an OFDM signal using part of the bandwidth on the different sub-channels.
                int i=0;
                ivec mask(Nfft);
                cvec rx_buff2(rx_buff.size());
                bool is_ofdm=false;
                double metric=0;
                double detected_metric=0;
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
                    //cout << "subchannel " << i << " with metric " << metric << endl;
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
                    is_ofdm=mcdaaofdm->detection(rx_buff2,metric);
                    vec estimated_channel;
                    cvec demodulated_ofdm_symbols=mcdaaofdm->equalizer_fourth_power(rx_buff,detected_group,estimated_channel);
                    cvec constellation;
                    received_bits=mcdaaofdm->demodulate_mask_gray_qpsk(demodulated_ofdm_symbols,detected_group,constellation);
                    emit plotted(constellation,2);
                    preamble_ok=mcdaaofdm->preamble_detection_qpsk(received_bits,received_bits2,preamble_start);

                }


            }
            if(waveform==3){
                cvec constellation;
                received_bits=bpsk->demodulate(rx_buff,constellation);
                emit plotted(constellation,2);
                preamble_ok=bpsk->preamble_detection(received_bits,received_bits2,preamble_start);
            }
            if(waveform==4){
                vec constellation;
                received_bits=gmsk->demodulate(rx_buff,constellation);
                emit plotted(to_cvec(constellation),2);
                preamble_ok=gmsk->preamble_detection(received_bits,received_bits2,preamble_start);
            }
            if(waveform==5){
                cvec constellation;
                received_bits=qpsk->demodulate(rx_buff,constellation);
                emit plotted(constellation,2);
                preamble_ok=qpsk->preamble_detection(received_bits,received_bits2,preamble_start);
            }
            if(waveform==6){
                vec constellation;
                received_bits=cpfsk->demodulate(rx_buff,constellation);
                emit plotted(constellation,2);
                preamble_ok=cpfsk->preamble_detection(received_bits,received_bits2,preamble_start);
            }
            if(waveform==7){
                cvec constellation;
                received_bits=corasma->demodulate(rx_buff,constellation,time_offset_estimate);
                emit plotted(constellation,2);
                preamble_ok=corasma->preamble_detection(received_bits,received_bits2,preamble_start);
            }
            if(waveform==8){
                received_bits=nbwf->demodulate(rx_buff,time_offset_estimate);
                preamble_ok=nbwf->preamble_detection(received_bits,received_bits2,preamble_start);
            }
            if(waveform==9){
                cvec constellation;
                received_bits=dsss->demodulate(rx_buff,time_offset_estimate,constellation);
                emit plotted(constellation,2);
                preamble_ok=dsss->preamble_detection(received_bits,received_bits2,preamble_start);
            }
            if(waveform==10){
                vec constellation;
                received_bits=mcdads->demodulate(rx_buff,OF,ofdm_time_offset_estimate,dads_time_offset_estimate,constellation);
                emit plotted(constellation,2);
                preamble_ok=mcdads->preamble_detection(received_bits,received_bits2,preamble_start);
            }
            if(waveform==11){
                cvec constellation;
                received_bits=ofdm->demodulate(concat(previous_rx_buff,rx_buff),constellation);
                emit plotted(constellation,2);
                preamble_ok=ofdm->preamble_detection(received_bits,received_bits2,preamble_start);
            }
            //cout << "PREAMBLE START " << preamble_start << endl;
            //cout << "TIME OFFSET ESTIMATE " << mcdaaofdm->time_offset_estimate << endl;
            //cout << "TIME OFFSET ESTIMATE " << time_offset_estimate << endl;
            //cout <<  ofdm_time_offset_estimate << endl;
            //cout << dads_time_offset_estimate << endl;

            if(preamble_ok==true){
                if(packet->is_ber_count==true)
                    gui->label_10->setText(number.setNum(packet->ber_count(received_bits2),'e',2));

                //Synchronization of the receive stream
                if(waveform==1)
                    correction=Number_of_received_symbols+(preamble_start*SF+time_offset_estimate-64*SF);
                if(waveform==2){
                    if(mcdaaofdm->time_offset_estimate>Nfft-1)
                        correction=Number_of_received_symbols+(mcdaaofdm->time_offset_estimate-(Nfft+Ncp))+int(preamble_start/sum_mask)*(Nfft+Ncp);
                    else
                        correction=Number_of_received_symbols+(mcdaaofdm->time_offset_estimate)+int(preamble_start/sum_mask)*(Nfft+Ncp);
                }
                if(waveform==3)
                    correction=Number_of_received_symbols+(preamble_start*OF-64*OF);
                if(waveform==4)
                    correction=Number_of_received_symbols+(preamble_start*OF-64*OF);
                if(waveform==5)
                    correction=Number_of_received_symbols+(preamble_start*OF-64*OF)/2;
                if(waveform==6)
                    correction=Number_of_received_symbols+(preamble_start*OF-64*OF);
                if(waveform==7){
                    if(time_offset_estimate>Nfft-1)
                        correction=Number_of_received_symbols+(time_offset_estimate-(Nfft+Ncp))+int(preamble_start/(Nfft/2*bitspersymbol))*2*(Nfft+Ncp);
                    else
                        correction=Number_of_received_symbols+(time_offset_estimate)+int(preamble_start/(Nfft/2*bitspersymbol))*2*(Nfft+Ncp);
                }
                if(waveform==8)
                    correction=Number_of_received_symbols+(time_offset_estimate-2056);
                if(waveform==9)
                    correction=Number_of_received_symbols+(preamble_start*SF+time_offset_estimate-64*SF);
                if(waveform==10){
                    if(ofdm_time_offset_estimate>Nfft-1)
                        correction=Number_of_received_symbols+(ofdm_time_offset_estimate-(Nfft+Ncp))+int((preamble_start*SF+dads_time_offset_estimate-64*SF)/Nfft)*(Nfft+Ncp);
                    else
                        correction=Number_of_received_symbols+(ofdm_time_offset_estimate)+int((preamble_start*SF+dads_time_offset_estimate-64*SF)/Nfft)*(Nfft+Ncp);
                }
                emit valuechanged(correction);


                bool same_packet=false;
                bool packet_ok=packet->decode_packet(received_bits2,myaddress,same_packet,ptr);
                if(packet_ok==true){
                    if(is_resynchronized==false)
                             is_resynchronized=true;
                    if(waveform==2){
                        //Transfer the best group to TX
                        emit valuechanged(detected_group);
                    }
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
                else{

                    double current_time=device->time();
                    if(current_time>previous_time){
                        previous_estimated_throughput=estimated_throughput;
                        estimated_throughput=0.9*estimated_throughput;
                        previous_time=current_time;
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
                if(is_resynchronized){
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
