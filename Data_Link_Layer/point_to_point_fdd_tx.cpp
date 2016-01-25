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
#include "point_to_point_fdd_tx.h"

Point_to_Point_FDD_TX::Point_to_Point_FDD_TX(Ui_MainWindow *ui)
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
    if(gui->comboBox->currentText()=="L1:DSSS")
        waveform=7;
    if(gui->comboBox->currentText()=="L1:MCDADS")
        waveform=8;
    if(gui->comboBox->currentText()=="L1:OFDM")
        waveform=9;
    last_waveform=0;

}

void Point_to_Point_FDD_TX::setvalue(int value){


    tx_best_group=value;


}

void Point_to_Point_FDD_TX::update_uhd(){

    gui->lineEdit_2->setText(number.setNum(txrate/1.0e6));
    gui->lineEdit_3->setText(number.setNum(txfreq/1.0e6));
    gui->lineEdit_4->setText(number.setNum(txgain));
    gui->lineEdit_5->setText(number.setNum(txamplitude));
    gui->lineEdit_6->setText(number.setNum(rxrate/1.0e6));
    gui->lineEdit_7->setText(number.setNum(rxfreq/1.0e6));
    gui->lineEdit_8->setText(number.setNum(rxgain));

}


void Point_to_Point_FDD_TX::run(){

    stop_signal=false;
    noderunning=true;
    double previous_time_index=-1.0;
    double time_index=0.0;

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
         Number_of_received_symbols=SF*(nb_bits+1)*OF;
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
         packet = new CogWave_Packet(Nfft*Number_of_OFDM_symbols);
         tx_best_group=0;

        }
        if((last_waveform!=waveform)&&(waveform==3)){
            last_waveform=waveform;
            bpsk = new Modem_BPSK();
            int nb_bits=bpsk->nb_bits;
            int OF=bpsk->OF;
            Number_of_received_symbols=nb_bits*OF;
            packet = new CogWave_Packet(nb_bits);

        }
        if((last_waveform!=waveform)&&(waveform==4)){
            last_waveform=waveform;
            gmsk = new Modem_GMSK();
            int nb_bits=gmsk->nb_bits;
            int OF=gmsk->OF;
            Number_of_received_symbols=nb_bits*OF;
            packet = new CogWave_Packet(nb_bits);
        }
        if((last_waveform!=waveform)&&(waveform==5)){
            last_waveform=waveform;
            qpsk = new Modem_QPSK();
            int nb_bits=qpsk->nb_bits;
            int OF=qpsk->OF;
            Number_of_received_symbols=nb_bits*OF/2;
            packet = new CogWave_Packet(nb_bits);
        }
        if((last_waveform!=waveform)&&(waveform==6)){
            last_waveform=waveform;
            cpfsk = new Modem_CPFSK();
            int nb_bits=cpfsk->nb_bits;
            int OF=cpfsk->OF;
            Number_of_received_symbols=nb_bits*OF;
            packet = new CogWave_Packet(nb_bits);
        }
        if((last_waveform!=waveform)&&(waveform==7)){
            last_waveform=waveform;
            dsss = new Modem_DSSS();
            int nb_bits=dsss->nb_bits;
            int SF=dsss->SF;
            int OF=dsss->OF;
            Number_of_received_symbols=SF*nb_bits*OF;
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
         packet = new CogWave_Packet(nb_bits);

        }
        if((last_waveform!=waveform)&&(waveform==9)){
            last_waveform=waveform;
            ofdm = new Modem_OFDM();
            int nb_bits=ofdm->nb_bits;
            Number_of_received_symbols=ofdm->Number_of_received_symbols;
            packet = new CogWave_Packet(nb_bits);
        }
        if(state=="SEND"){
            //cout << "########### PROCESSING TX PACKET ########### " << device->time() << " #############" << endl;

            //Solution which send the packet in every slot until the packet gets updated.

            time_index=(floor(device->time()/(Number_of_received_symbols/rxrate)+0.5));
            if(previous_time_index!=time_index){
                data_packet=packet->encode_packet(myaddress,destaddress,nb_read);
                if(nb_read>0){
                    previous_time_index=time_index;
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
                    device->tx_buff2=tx_buff*device->tx_amplitude;
                    if((device->is_sending==false)&&(state=="SEND")){
                        device->is_sending=true;
                        device->start();
                    }
                }
            }
            if((gui->pushButton_3->text()=="Start Audio")&&(gui->pushButton_2->text()=="Start Video")&&(gui->pushButton_4->text()=="Start BER TX")&&(device->is_sending==true)){
                if(device->has_sent==true){
                    device->is_sending=false;
                    state="NOSTATE";
                }
            }
        }
        qApp->processEvents();
    }

    //finished
    std::cout << std::endl << "Done!" << std::endl << std::endl;
    noderunning=false;







}
