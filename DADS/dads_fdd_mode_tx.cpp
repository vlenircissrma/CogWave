#include "dads_fdd_mode_tx.h"

DADS_FDD_Mode_TX::DADS_FDD_Mode_TX(Ui_MainWindow *ui)
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
    packets = new DADS_Framing(nb_bits);
    connect(this,SIGNAL(updated_uhd()),this,SLOT(update_uhd()),Qt::BlockingQueuedConnection);



}

void DADS_FDD_Mode_TX::update_uhd(){

    gui->lineEdit_2->setText(number.setNum(txrate/1.0e6));
    gui->lineEdit_3->setText(number.setNum(txfreq/1.0e6));
    gui->lineEdit_4->setText(number.setNum(txgain));
    gui->lineEdit_5->setText(number.setNum(txamplitude));
    gui->lineEdit_6->setText(number.setNum(rxrate/1.0e6));
    gui->lineEdit_7->setText(number.setNum(rxfreq/1.0e6));
    gui->lineEdit_8->setText(number.setNum(rxgain));

}


void DADS_FDD_Mode_TX::run(){

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

            if(state=="SEND"){
            //cout << "########### PROCESSING TX PACKET ########### " << device->time() << " #############" << endl;


            //Encode the PACKET with the bit vector and the power allocation
            data_packet=packets->encode_frame(myaddress,destaddress,nb_read);

            //cout << nb_read << endl;

            if(nb_read!=0){
                //modulate the PACKET
                tx_buff=dads->modulate(data_packet);

                device->tx_buff2=tx_buff*device->tx_amplitude;

                if((device->is_sending==false)&&(state=="SEND")){
                    device->is_sending=true;
                    device->start();

                }

            }
            if((gui->pushButton_3->text()=="Start Audio")&&(gui->pushButton_2->text()=="Start Video")&&(device->is_sending==true)){
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
