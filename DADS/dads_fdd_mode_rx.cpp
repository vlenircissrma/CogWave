#include "dads_fdd_mode_rx.h"

DADS_FDD_Mode_RX::DADS_FDD_Mode_RX(Ui_MainWindow *ui)
{

    gui=ui;
    stop_signal=false;
    device = new DADS_UHDDevice();
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
    Nfft=512;
    gui->lineEdit_2->setText(number.setNum(txrate/1.0e6));
    gui->lineEdit_3->setText(number.setNum(txfreq/1.0e6));
    gui->lineEdit_4->setText(number.setNum(txgain));
    gui->lineEdit_5->setText(number.setNum(txamplitude));
    gui->lineEdit_6->setText(number.setNum(rxrate/1.0e6));
    gui->lineEdit_7->setText(number.setNum(rxfreq/1.0e6));
    gui->lineEdit_8->setText(number.setNum(rxgain));
    gui->lineEdit_9->setText(number.setNum(myaddress));
    gui->lineEdit_10->setText(number.setNum(destaddress));
    dads = new DADS_Modem();
    nb_bits=dads->nb_bits;
    SF=dads->SF;
    OF=rxrate/txrate;
    Number_of_received_symbols=SF*nb_bits*OF;
    packets = new DADS_Framing(nb_bits);
    connect(this,SIGNAL(updated_uhd()),this,SLOT(update_uhd()),Qt::BlockingQueuedConnection);
    is_synchronized=false;
    nb_packet=0;
    correction=0;
    connect(this,SIGNAL(valuechanged(int)),device,SLOT(setvalue(int)),Qt::BlockingQueuedConnection);

}

void DADS_FDD_Mode_RX::update_uhd(){

    gui->lineEdit_2->setText(number.setNum(txrate/1.0e6));
    gui->lineEdit_3->setText(number.setNum(txfreq/1.0e6));
    gui->lineEdit_4->setText(number.setNum(txgain));
    gui->lineEdit_5->setText(number.setNum(txamplitude));
    gui->lineEdit_6->setText(number.setNum(rxrate/1.0e6));
    gui->lineEdit_7->setText(number.setNum(rxfreq/1.0e6));
    gui->lineEdit_8->setText(number.setNum(rxgain));

}


void DADS_FDD_Mode_RX::run(){

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


        }

        //cout << "########### PROCESSING RX PACKET ########### " << device->time() << " #############" << endl;

        if(device->is_receiving==false){
            device->rx_buff.set_size(Number_of_received_symbols);
            device->rx_buff.zeros();
            device->is_receiving=true;
            device->start();
        }
        int src_adress=0;
        int time_offset_estimate;
        rx_buff=device->rx_buff;
        received_bits=dads->demodulate(rx_buff,OF,time_offset_estimate);
        int preamble_start;
        bool preamble_ok=dads->preamble_detection(received_bits,received_bits2,preamble_start);
        //cout << "PREAMBLE START " << preamble_start << endl;
        //cout << "TIME OFFSET ESTIMATE " << time_offset_estimate << endl;
        if(preamble_ok==true){
            bool packet_ok=packets->decode_frame(received_bits2,myaddress,src_adress);
            if(packet_ok==true){
                    correction=Number_of_received_symbols+preamble_start*SF+time_offset_estimate;
                    emit valuechanged(correction);
            }
            else{
                cout << "SOF NOT FOUND" << endl;
            }
        }



    qApp->processEvents();
    }

    //finished
    noderunning=false;
    device->is_receiving=false;
    std::cout << std::endl << "Done!" << std::endl << std::endl;



}
