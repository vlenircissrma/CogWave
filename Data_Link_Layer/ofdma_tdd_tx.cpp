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
#include "ofdma_tdd_tx.h"

OFDMA_TDD_TX::OFDMA_TDD_TX(Ui_MainWindow *ui)
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
    connect(this,SIGNAL(updated_uhd()),this,SLOT(update_uhd()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(valuechanged(double)),device,SLOT(setvalue(double)),Qt::BlockingQueuedConnection);
    waveform=2;
    last_waveform=0;
    all_subchannels_allocated=false;

}

void OFDMA_TDD_TX::setvalue(bool value, double value2){


    is_time_set=value;
    tx_timestamp=value2;
    device->tx_timestamp=value2+time_gap/2;
}

void OFDMA_TDD_TX::setvalue(int value){


    tx_best_group=value;
    if(value==mcdaaofdm->num_subchannels)
        all_subchannels_allocated=true;


}

void OFDMA_TDD_TX::setwaveform(int value){


    waveform=value;

}

void OFDMA_TDD_TX::update_uhd(){

    gui->lineEdit_2->setText(number.setNum(txrate/1.0e6));
    gui->lineEdit_3->setText(number.setNum(txfreq/1.0e6));
    gui->lineEdit_4->setText(number.setNum(txgain));
    gui->lineEdit_5->setText(number.setNum(txamplitude));
    gui->lineEdit_6->setText(number.setNum(rxrate/1.0e6));
    gui->lineEdit_7->setText(number.setNum(rxfreq/1.0e6));
    gui->lineEdit_8->setText(number.setNum(rxgain));

}


void OFDMA_TDD_TX::run(){

    stop_signal=false;
    noderunning=true;
    is_time_set=false;
    first_tx_timestamp=true;
    double previous_tx_timestamp=-1.0;

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
         int Nfft=mcdaaofdm->Nfft;
         int Ncp=mcdaaofdm->Ncp;
         int num_subchannels=mcdaaofdm->num_subchannels;
         int Number_of_OFDM_symbols=mcdaaofdm->Number_of_OFDM_symbols;
         int Modulation_Order=mcdaaofdm->Modulation_Order;

         int sum_mask=(Nfft-3*Nfft/16)/num_subchannels*Modulation_Order;
         int Number_of_received_symbols;
         if((Number_of_OFDM_symbols*Nfft/sum_mask)*sum_mask!=Number_of_OFDM_symbols*Nfft)
             Number_of_received_symbols=(Number_of_OFDM_symbols*Nfft/sum_mask+1)*(Nfft+Ncp);
         else
             Number_of_received_symbols=(Number_of_OFDM_symbols*Nfft/sum_mask)*(Nfft+Ncp);
         int number_of_slots=2;
         time_gap=number_of_slots*(Number_of_received_symbols/device->rx_rate);
         cout << "TIME GAP TX " << time_gap << endl;
         device->time_gap=time_gap;
         packet = new CogWave_Packet(Nfft*Number_of_OFDM_symbols);
         tx_best_group=0;

        }
        if(state=="SEND"){

            //Solution to be sure to send the packet only in one slot. Be careful, not all slots will be filled
            //if there is not enough processing power to compute one packet in the time_gap period.
            if((previous_tx_timestamp!=tx_timestamp)&&(!all_subchannels_allocated)){
                data_packet=packet->encode_packet(myaddress,destaddress,nb_read);
                if(nb_read>0){
                    previous_tx_timestamp=tx_timestamp;
                    if(last_waveform==2)
                        tx_buff=mcdaaofdm->modulate_mask_qpsk(data_packet,tx_best_group);
                    if(is_time_set==false){
                        tx_timestamp=(int(device->time()/time_gap)+1)*time_gap+time_gap/2;
                        is_time_set=true;
                    }
                    else{
                        tx_timestamp=tx_timestamp+time_gap+time_gap/2;
                    }
                    if(first_tx_timestamp==true){
                        first_tx_timestamp=false;
                        emit valuechanged(first_tx_timestamp);
                    }
                    tx_timestamp=device->sendsamples(tx_buff,tx_timestamp);
                    tx_timestamp=tx_timestamp-time_gap/2;
                }
            }
        }
        qApp->processEvents();
    }

    //finished
    std::cout << std::endl << "Done!" << std::endl << std::endl;
    noderunning=false;







}
