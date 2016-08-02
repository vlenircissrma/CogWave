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
#include "tdma_tdd_tx.h"

TDMA_TDD_TX::TDMA_TDD_TX(Ui_MainWindow *ui)
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
#if MIOT_ENABLED==1
    device = new MIOTDevice();
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
    if(gui->comboBox->currentText()=="L1:DSSS")
        waveform=7;
    if(gui->comboBox->currentText()=="L1:MCDADS")
        waveform=8;
    if(gui->comboBox->currentText()=="L1:OFDM")
        waveform=9;
    last_waveform=0;
    tdma_slots=4;
    all_slots_allocated=false;
    index_slot=0;

}

void TDMA_TDD_TX::setvalue(bool value, double value2){


    is_time_set=value;
    tx_timestamp=value2;
    device->tx_timestamp=value2+time_gap/2;
}

void TDMA_TDD_TX::setvalue(int value){


    tx_best_group=value;


}

void TDMA_TDD_TX::setwaveform(int value){


    waveform=value;

}

void TDMA_TDD_TX::update_uhd(){

    gui->lineEdit_2->setText(number.setNum(txrate/1.0e6));
    gui->lineEdit_3->setText(number.setNum(txfreq/1.0e6));
    gui->lineEdit_4->setText(number.setNum(txgain));
    gui->lineEdit_5->setText(number.setNum(txamplitude));
    gui->lineEdit_6->setText(number.setNum(rxrate/1.0e6));
    gui->lineEdit_7->setText(number.setNum(rxfreq/1.0e6));
    gui->lineEdit_8->setText(number.setNum(rxgain));

}

void TDMA_TDD_TX::slot_allocated(int value){

    if(value==tdma_slots)
        all_slots_allocated=true;
    index_slot=value;
    cout << "index slot " << index_slot << endl;

}

void TDMA_TDD_TX::run(){

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
        if((last_waveform!=waveform)&&(waveform==1)){
         last_waveform=waveform;
         dads = new Modem_DADS();
         int nb_bits=dads->nb_bits;
         int SF=dads->SF;
         int OF=rxrate/txrate;
         Number_of_received_symbols=SF*nb_bits*OF+SF;
         time_gap=tdma_slots*(Number_of_received_symbols/rxrate);
         cout << "TIME GAP TX " << time_gap << endl;
         device->time_gap=time_gap;
         packet = new CogWave_Packet(nb_bits);

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
         if((Number_of_OFDM_symbols*Nfft/sum_mask)*sum_mask!=Number_of_OFDM_symbols*Nfft)
             Number_of_received_symbols=(Number_of_OFDM_symbols*Nfft/sum_mask+1)*(Nfft+Ncp);
         else
             Number_of_received_symbols=(Number_of_OFDM_symbols*Nfft/sum_mask)*(Nfft+Ncp);
         time_gap=tdma_slots*(Number_of_received_symbols/device->rx_rate);
         cout << "TIME GAP TX " << time_gap << endl;
         device->time_gap=time_gap;
         packet = new CogWave_Packet(Nfft*Number_of_OFDM_symbols);
         tx_best_group=0;

        }
        if((last_waveform!=waveform)&&(waveform==3)){
            last_waveform=waveform;
            bpsk = new Modem_BPSK();
            int nb_bits=bpsk->nb_bits;
            int OF=bpsk->OF;
            Number_of_received_symbols=nb_bits*OF;
            time_gap=tdma_slots*(Number_of_received_symbols/rxrate);
            cout << "TIME GAP TX " << time_gap << endl;
            device->time_gap=time_gap;
            packet = new CogWave_Packet(nb_bits);

        }
        if((last_waveform!=waveform)&&(waveform==4)){
            last_waveform=waveform;
            gmsk = new Modem_GMSK();
            int nb_bits=gmsk->nb_bits;
            int OF=gmsk->OF;
            Number_of_received_symbols=nb_bits*OF;
            time_gap=tdma_slots*(Number_of_received_symbols/rxrate);
            cout << "TIME GAP TX " << time_gap << endl;
            device->time_gap=time_gap;
            packet = new CogWave_Packet(nb_bits);
        }
        if((last_waveform!=waveform)&&(waveform==5)){
            last_waveform=waveform;
            qpsk = new Modem_QPSK();
            int nb_bits=qpsk->nb_bits;
            int OF=qpsk->OF;
            Number_of_received_symbols=nb_bits*OF/2;
            int tdma_slots=2;
            time_gap=tdma_slots*(Number_of_received_symbols/rxrate);
            cout << "TIME GAP TX " << time_gap << endl;
            device->time_gap=time_gap;
            packet = new CogWave_Packet(nb_bits);
        }
        if((last_waveform!=waveform)&&(waveform==6)){
            last_waveform=waveform;
            cpfsk = new Modem_CPFSK();
            int nb_bits=cpfsk->nb_bits;
            int OF=cpfsk->OF;
            Number_of_received_symbols=nb_bits*OF;
            time_gap=tdma_slots*(Number_of_received_symbols/rxrate);
            cout << "TIME GAP TX " << time_gap << endl;
            device->time_gap=time_gap;
            packet = new CogWave_Packet(nb_bits);
        }
        if((last_waveform!=waveform)&&(waveform==7)){
            last_waveform=waveform;
            dsss = new Modem_DSSS();
            int nb_bits=dsss->nb_bits;
            int OF=dsss->OF;
            int SF=dsss->SF;
            Number_of_received_symbols=SF*nb_bits*OF;
            time_gap=tdma_slots*(Number_of_received_symbols/rxrate);
            cout << "TIME GAP TX " << time_gap << endl;
            device->time_gap=time_gap;
            packet = new CogWave_Packet(nb_bits);

        }
        if((last_waveform!=waveform)&&(waveform==8)){
         last_waveform=waveform;
         mcdads = new Modem_MCDADS();
         int nb_bits=mcdads->nb_bits;
         int SF=mcdads->SF;
         int OF=rxrate/txrate;
         int Nfft=mcdads->Nfft;
         int Ncp=mcdads->Ncp;
         int sum_mask=mcdads->sum_mask;
         if((SF*(nb_bits+1)*OF/sum_mask)*sum_mask!=SF*(nb_bits+1)*OF)
             Number_of_received_symbols=(SF*(nb_bits+1)*OF/sum_mask+1)*(Nfft+Ncp);
         else
             Number_of_received_symbols=(SF*(nb_bits+1)*OF/sum_mask)*(Nfft+Ncp);
         time_gap=tdma_slots*(Number_of_received_symbols/rxrate);
         cout << "TIME GAP TX " << time_gap << endl;
         device->time_gap=time_gap;
         packet = new CogWave_Packet(nb_bits);

        }
        if((last_waveform!=waveform)&&(waveform==9)){
            last_waveform=waveform;
            ofdm = new Modem_OFDM();
            int nb_bits=ofdm->nb_bits;
            Number_of_received_symbols=ofdm->Number_of_received_symbols;
            int number_of_slots=2;
            time_gap=number_of_slots*(Number_of_received_symbols/rxrate);
            cout << "TIME GAP TX " << time_gap << endl;
            device->time_gap=time_gap;
            packet = new CogWave_Packet(nb_bits);
        }
        if(state=="SEND"){
            //cout << "########### PROCESSING TX PACKET ########### " << device->time() << " #############" << endl;

            //Solution to be sure to send the packet only in one slot. Be careful, not all slots will be filled
            //if there is not enough processing power to compute one packet in the time_gap period.
            if((previous_tx_timestamp!=tx_timestamp)&&(!all_slots_allocated)){
                data_packet=packet->encode_packet(myaddress,destaddress,nb_read);
                if(nb_read>0){
                    previous_tx_timestamp=tx_timestamp;
                    if(last_waveform==1)
                        tx_buff=dads->modulate(data_packet);
                    if(last_waveform==2)
                        tx_buff=mcdaaofdm->modulate_mask_qpsk(data_packet,tx_best_group);
                    if(last_waveform==3)
                        tx_buff=bpsk->modulate(data_packet);
                    if(last_waveform==4)
                        tx_buff=gmsk->modulate(data_packet);
                    if(last_waveform==5)
                        tx_buff=qpsk->modulate(data_packet);
                    if(last_waveform==6)
                        tx_buff=cpfsk->modulate(data_packet);
                    if(last_waveform==7)
                        tx_buff=dsss->modulate(data_packet);
                    if(last_waveform==8)
                        tx_buff=mcdads->modulate(data_packet);
                    if(last_waveform==9)
                        tx_buff=ofdm->modulate(data_packet);
                    if(is_time_set==false){
                        tx_timestamp=(int(device->time()/time_gap)+1)*time_gap+time_gap/2;
                        is_time_set=true;
                    }
                    else{
                        tx_timestamp=tx_timestamp+time_gap+index_slot*time_gap/tdma_slots+time_gap/2;
                    }
                    if(first_tx_timestamp==true){
                        first_tx_timestamp=false;
                        emit valuechanged(first_tx_timestamp);
                    }
                    tx_timestamp=device->sendsamples(tx_buff,tx_timestamp);
                    tx_timestamp=tx_timestamp-time_gap/2-index_slot*time_gap/tdma_slots;
                }
            }



        }
        qApp->processEvents();
    }

    //finished
    std::cout << std::endl << "Done!" << std::endl << std::endl;
    noderunning=false;







}
