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
#include "csma_tx.h"

CSMA_TX::CSMA_TX(Ui_MainWindow *ui, int fd_ext)
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
    last_waveform=0;
    ack_notx=-1;
    ack_norx=-1;
    ack_sent=1;
    csma_type="non-persistent";
    probability_sending=0.9;
    ptr=fd_ext;
}

void CSMA_TX::update_uhd(){

    gui->lineEdit_2->setText(number.setNum(txrate/1.0e6));
    gui->lineEdit_3->setText(number.setNum(txfreq/1.0e6));
    gui->lineEdit_4->setText(number.setNum(txgain));
    gui->lineEdit_5->setText(number.setNum(txamplitude));
    gui->lineEdit_6->setText(number.setNum(rxrate/1.0e6));
    gui->lineEdit_7->setText(number.setNum(rxfreq/1.0e6));
    gui->lineEdit_8->setText(number.setNum(rxgain));

}

void CSMA_TX::setvalue(int value){


    ack_notx=value;


}

void CSMA_TX::sendack(){


    bvec data_packet=packet->encode_ack(ack_norx,myaddress,destaddress);
    cvec tx_buff;
    if(waveform==1)
        tx_buff=dads->modulate(data_packet);
    if(waveform==2)
        tx_buff=mcdaaofdm->modulate_mask_qpsk(data_packet,tx_best_group);
    if(waveform==3)
        tx_buff=bpsk->modulate(data_packet);
    if(waveform==4)
        tx_buff=gmsk->modulate(data_packet);
    if(waveform==5)
        tx_buff=qpsk->modulate(data_packet);
    if(waveform==6)
        tx_buff=cpfsk->modulate(data_packet);
    if(waveform==7)
        tx_buff=dsss->modulate(data_packet);
    if(waveform==8)
        tx_buff=mcdads->modulate(data_packet);
    device->sendsamplesnow(tx_buff*device->tx_amplitude);

}

void CSMA_TX::signal_detected(bool newvalue){

    carrier_sensed=newvalue;

}
void CSMA_TX::triggerack(int packetnorx){


    ack_sent=0;
    ack_norx=packetnorx;

}

void CSMA_TX::run(){

    stop_signal=false;
    noderunning=true;
    double previous_time=0.0;
    int nb_retransmissions=0;

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
         packet = new Packet(nb_bits);

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
         packet = new Packet(Nfft*Number_of_OFDM_symbols);
         tx_best_group=0;

        }
        if((last_waveform!=waveform)&&(waveform==3)){
            last_waveform=waveform;
            bpsk = new Modem_BPSK();
            int nb_bits=bpsk->nb_bits;
            int OF=bpsk->OF;
            Number_of_received_symbols=nb_bits*OF;
            packet = new Packet(nb_bits);

        }
        if((last_waveform!=waveform)&&(waveform==4)){
            last_waveform=waveform;
            gmsk = new Modem_GMSK();
            int nb_bits=gmsk->nb_bits;
            int OF=gmsk->OF;
            Number_of_received_symbols=nb_bits*OF;
            packet = new Packet(nb_bits);
        }
        if((last_waveform!=waveform)&&(waveform==5)){
            last_waveform=waveform;
            qpsk = new Modem_QPSK();
            int nb_bits=qpsk->nb_bits;
            int OF=qpsk->OF;
            Number_of_received_symbols=nb_bits*OF/2;
            packet = new Packet(nb_bits);
        }
        if((last_waveform!=waveform)&&(waveform==6)){
            last_waveform=waveform;
            cpfsk = new Modem_CPFSK();
            int nb_bits=cpfsk->nb_bits;
            int OF=cpfsk->OF;
            Number_of_received_symbols=nb_bits*OF;
            packet = new Packet(nb_bits);
        }
        if((last_waveform!=waveform)&&(waveform==7)){
            last_waveform=waveform;
            dsss = new Modem_DSSS();
            int nb_bits=dsss->nb_bits;
            int SF=dsss->SF;
            int OF=dsss->OF;
            Number_of_received_symbols=SF*nb_bits*OF;
            packet = new Packet(nb_bits);
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
         packet = new Packet(nb_bits);

        }
        if(state=="SEND"){
            //cout << "########### PROCESSING TX PACKET ########### " << device->time() << " #############" << endl;

            //Solution which send the packet in every slot until the packet gets updated.


                data_packet=packet->encode_packet(myaddress,destaddress,nb_read,ptr);
                if(nb_read>0){
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
                    //fixed timeout to receive ack must be larger that the packet time
                    double timeout=4*tx_buff.size()/txrate;
                    if(csma_type=="1-persistent"){
                        //do not transmit until receiver idle
                        while(carrier_sensed) 0;
                    }
                    if(csma_type=="non-persistent"){
                        //wait a random time before sensing again (exponential backoff)
                        while(carrier_sensed){
                            device->waiting_time(tx_buff.size()/txrate*randi(0,pow(2,nb_retransmissions)-1));
                        }
                    }
                    if(csma_type=="p-persistent"){
                        //do not transmit until receiver idle
                        while(carrier_sensed) 0;
                        while(randu()<(1-probability_sending)){
                            device->waiting_time(tx_buff.size()/txrate);
                            while(carrier_sensed) 0;
                        }
                    }
                    //start time of the timeout
                    previous_time=device->time();
                    //send the packet now
                    device->sendsamplesnow(tx_buff*device->tx_amplitude);
                    //loop until ack received and number of retransmissions less than a fixed amount
                    while((packet->packetnotx-1!=ack_notx)&&(nb_retransmissions<40)){
                        //check if timeout and number of retransmissions less than a fixed amount
                        if(((device->time()-previous_time)>timeout)&&(nb_retransmissions<40)){
                            //the packet will be retransmitted, increment the variable
                            nb_retransmissions=nb_retransmissions+1;
                            if(csma_type=="1-persistent"){
                                //do not transmit until receiver idle
                                while(carrier_sensed) 0;

                            }
                            if(csma_type=="non-persistent"){
                                //wait a random time before sensing again (exponential backoff)
                                while(carrier_sensed){
                                    device->waiting_time(tx_buff.size()/txrate*randi(0,pow(2,nb_retransmissions)-1));
                                }
                            }
                            if(csma_type=="p-persistent"){
                                //do not transmit until receiver idle
                                while(carrier_sensed) 0;
                                double probability_sending=0.9;
                                while(randu()<(1-probability_sending)){
                                    device->waiting_time(tx_buff.size()/txrate);
                                    while(carrier_sensed) 0;
                                }
                            }
                            //start time of the timeout
                            previous_time=device->time();
                            //send the packet now
                            device->sendsamplesnow(tx_buff*device->tx_amplitude);
                        }
                        //send ack if receiver has decoded a packet from another node correctly (while sending, we might receive packets from other nodes)
                        else if(ack_sent==0){
                            sendack();
                            ack_sent=1;
                        }
                    }
                    //check if ack received, if not a new data packet will be transmitted
                    if(packet->packetnotx-1==ack_notx){
                        cout << "ACK OK - Packet Number " << ack_notx << endl;
                    }

                }



        }
        //send ack if receiver has decoded a packet from another node correctly (we received a packet from another node while not sending anything)
        if(ack_sent==0){
                sendack();
                ack_sent=1;
        }
        qApp->processEvents();
    }

    //finished
    std::cout << std::endl << "Done!" << std::endl << std::endl;
    noderunning=false;







}
