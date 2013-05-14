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

#include "blindofdm_tdd_mode_tx.h"


BlindOFDM_TDD_Mode_TX::BlindOFDM_TDD_Mode_TX(Ui_MainWindow *ui)
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
    time_gap=number_of_slots*(Number_of_received_symbols/device->rx_rate);
    cout << "TIME GAP " << time_gap << endl;
    packets = new BlindOFDM_Framing(Nfft,Number_of_OFDM_symbols);
    state="SENSING";
    nb_read=0;
    first_tx_timestamp=true;
    is_time_set=false;
    tx_best_group=0;

    connect(this,SIGNAL(updated_uhd()),this,SLOT(update_uhd()),Qt::BlockingQueuedConnection);



}

void BlindOFDM_TDD_Mode_TX::update_uhd(){

    gui->lineEdit_2->setText(number.setNum(txrate/1.0e6));
    gui->lineEdit_3->setText(number.setNum(txfreq/1.0e6));
    gui->lineEdit_4->setText(number.setNum(txgain));
    gui->lineEdit_5->setText(number.setNum(txamplitude));
    gui->lineEdit_6->setText(number.setNum(rxrate/1.0e6));
    gui->lineEdit_7->setText(number.setNum(rxfreq/1.0e6));
    gui->lineEdit_8->setText(number.setNum(rxgain));

}


void BlindOFDM_TDD_Mode_TX::setvalue(bool value){


    is_time_set=value;


}

void BlindOFDM_TDD_Mode_TX::setvalue(int value){


    tx_best_group=value;

}


void BlindOFDM_TDD_Mode_TX::init(){

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


void BlindOFDM_TDD_Mode_TX::run(){

    stop_signal=false;
    noderunning=true;

    while(!stop_signal){


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
            if(is_time_set==false)
                tx_timestamp=(int(device->time()/time_gap)+1)*time_gap+time_gap/2;
            else
                tx_timestamp=(int(device->time()/time_gap)+1)*time_gap;
            device->tx_timestamp=tx_timestamp;
            device->time_gap=time_gap;





        }


            if(state=="SEND"){
            //cout << "########### PROCESSING TX PACKET ########### " << device->time() << " #############" << endl;


            //Encode the PACKET with the bit vector and the power allocation
            data_packet=packets->encode_frame(myaddress,destaddress,tx_best_group,device->tx_timestamp,nb_read);

            if(Modulation_Order==1){
                //DBPSK
                //data_packet2=blindofdm->bpsk_differential_coding(data_packet);
                //BPSK
                data_packet2=data_packet;
            }
            if(Modulation_Order==2){
                //DQPSK
                //data_packet2=blindofdm->qpsk_differential_coding(data_packet);
                //QPSK
                data_packet2=data_packet;
            }
            if(Modulation_Order==4){
                //D16QAM
                //data_packet2=blindofdm->qam16_gray_differential_coding(data_packet);
                //data_packet2=blindofdm->qam16_quadrant_differential_coding(data_packet);
                //16QAM
                data_packet2=data_packet;
            }
            if(Modulation_Order==6){
                //64QAM
                data_packet2=data_packet;
            }
            if(Modulation_Order==8){
                //256QAM
                data_packet2=data_packet;
            }


            if(nb_read!=0){

                //modulate the PACKET
                if(Modulation_Order==1){
                    tx_buff=blindofdm->modulate_mask_bpsk(data_packet2,tx_best_group);
                    /*vec estimated_channel;
                    blindofdm->demodulate_mask_bpsk_tracking(tx_buff,tx_best_group,data_packet,estimated_channel,constellation);
                    cout << sum(to_ivec(data_packet+data_packet2.get(0,data_packet.size()-1))) << endl;
                    emit plotted(constellation,7);*/
                }
                if(Modulation_Order==2){
                    tx_buff=blindofdm->modulate_mask_qpsk(data_packet2,tx_best_group);
                    /*vec estimated_channel;
                    cvec constellation;
                    cvec demodulated_ofdm_symbols=blindofdm->equalizer_fourth_power(tx_buff,tx_best_group,estimated_channel);
                    bvec received_bits=blindofdm->demodulate_mask_gray_qpsk(demodulated_ofdm_symbols,tx_best_group,constellation);
                    cout << sum(to_ivec(received_bits+data_packet2.get(0,received_bits.size()-1))) << endl;
                    int src_adress;
                    int start_frame=128;
                    double sync_time=0.0;
                    bool packet_ok=packets->decode_frame(received_bits,myaddress,src_adress,tx_best_group,start_frame,sync_time,num_subchannels);*/

                }
                if(Modulation_Order==4){
                    //Quadrant 16QAM
                    //tx_buff=blindofdm->modulate_mask_quadrant_16qam(data_packet2,tx_best_group);
                    /*bool is_ofdm=blindofdm->detection(tx_buff);
                    vec estimated_channel;
                    blindofdm->demodulate_mask_quadrant_16qam_tracking(tx_buff,tx_best_group,data_packet,estimated_channel);
                    cout << sum(to_ivec(data_packet+data_packet2.get(0,data_packet.size()-1))) << endl;*/


                    //Gray 16QAM
                    tx_buff=blindofdm->modulate_mask_gray_16qam(data_packet2,tx_best_group);
                    /*vec estimated_channel;;
                    blindofdm->demodulate_mask_gray_16qam_tracking(tx_buff,tx_best_group,data_packet,estimated_channel,constellation);
                    cout << sum(to_ivec(data_packet+data_packet2.get(0,data_packet.size()-1))) << endl;
                    emit plotted(constellation,7);*/
                }
                if(Modulation_Order==6){
                    //Gray 64QAM
                    tx_buff=blindofdm->modulate_mask_gray_64qam(data_packet2,tx_best_group);
                    /*vec estimated_channel;;
                    blindofdm->demodulate_mask_gray_64qam_tracking(tx_buff,tx_best_group,data_packet,estimated_channel,constellation);
                    cout << sum(to_ivec(data_packet+data_packet2.get(0,data_packet.size()-1))) << endl;
                    emit plotted(constellation,7);*/
                }
                if(Modulation_Order==8){

                    //Gray 256QAM
                    tx_buff=blindofdm->modulate_mask_gray_256qam(data_packet2,tx_best_group);
                    /*vec estimated_channel;;
                    blindofdm->demodulate_mask_gray_256qam_tracking(tx_buff,tx_best_group,data_packet,estimated_channel,constellation);
                    cout << sum(to_ivec(data_packet+data_packet2.get(0,data_packet.size()-1))) << endl;
                    emit plotted(constellation,7);*/
                }

                device->tx_buff2=tx_buff*device->tx_amplitude;
                if((device->is_sending==false)&&(state=="SEND")){
                    if(first_tx_timestamp==true){
                        first_tx_timestamp=false;
                        emit valuechanged(first_tx_timestamp);
                    }

                    if(is_time_set==false)
                        tx_timestamp=(int(device->time()/time_gap)+1)*time_gap+time_gap/2;
                    else
                        tx_timestamp=(int(device->time()/time_gap)+1)*time_gap;

                    device->is_sending=true;
                    device->tx_timestamp=tx_timestamp;
                    device->time_gap=time_gap;
                    device->start();

                }

            }
            if((gui->pushButton_3->text()=="Start Audio")&&(gui->pushButton_2->text()=="Start Video")&&(device->is_sending==true)){
                if(device->has_sent==true){
                    state="NOSTATE";
                    device->is_sending=false;
                }
            }

        }
        qApp->processEvents();
    }

    //finished
    std::cout << std::endl << "Done!" << std::endl << std::endl;
    noderunning=false;







}
