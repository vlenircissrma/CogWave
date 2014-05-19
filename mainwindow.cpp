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
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    //Multimedia
    video_tx = new Video_TX();
    video_rx = new Video_RX();
    audio_tx = new Audio_TX();
    audio_rx = new Audio_RX();
    text_tx = new Text_TX();
    text_rx = new Text_RX();
    connect(text_rx,SIGNAL(edited_text(QString)),this,SLOT(edit_text(QString)),Qt::BlockingQueuedConnection);
    connect(video_tx,SIGNAL(displayed(GstElement*,int)),this,SLOT(display(GstElement*,int)),Qt::BlockingQueuedConnection);
    connect(video_rx,SIGNAL(displayed(GstElement*,int)),this,SLOT(display(GstElement*,int)),Qt::BlockingQueuedConnection);

    //dads_ber_test = new DADS_BER_Test();
    //blindofdm_ber_test = new BLINDOFDM_BER_Test();
    //bpsk_ber_test = new BPSK_BER_Test();
    //gmsk_ber_test = new GMSK_BER_Test();
    //qpsk_ber_test = new QPSK_BER_Test();
    //cpfsk_ber_test = new CPFSK_BER_Test();


}


MainWindow::~MainWindow()
{
    delete ui;
    delete video_tx;
    delete video_rx;
    delete audio_tx;
    delete audio_rx;
    delete text_tx;
    delete text_rx;
    delete waveform_tx;
    delete waveform_rx;
    delete plot;

}


void MainWindow::on_pushButton_clicked()
{
    if(ui->pushButton->text()=="Start Node"){


        //Start Multimedia
        video_rx->start();
        audio_rx->start();
        text_rx->start();

        store_text2=ui->comboBox_2->currentText();
        if(store_text2=="TDD"){
            //Start Waveform TDD
            waveform_tx = new Waveform_TDD_TX(ui);
            waveform_rx = new Waveform_TDD_RX(ui);
            connect(waveform_tx,SIGNAL(valuechanged(bool)),waveform_rx,SLOT(setvalue(bool)),Qt::BlockingQueuedConnection);
            connect(waveform_rx,SIGNAL(valuechanged(bool,double)),waveform_tx,SLOT(setvalue(bool,double)),Qt::BlockingQueuedConnection);
            connect(waveform_rx,SIGNAL(valuechanged(int)),waveform_tx,SLOT(setvalue(int)),Qt::BlockingQueuedConnection);
            connect(waveform_rx,SIGNAL(waveformset(int)),waveform_tx,SLOT(setwaveform(int)),Qt::BlockingQueuedConnection);

            plot = new Plot(ui,((Waveform_TDD_RX*)waveform_rx)->Nfft);
            qRegisterMetaType<vec>("vec");
            qRegisterMetaType<vec>("cvec");
            connect(waveform_rx,SIGNAL(plotted(vec,int)),this,SLOT(plotdata(vec,int)),Qt::BlockingQueuedConnection);
            connect(waveform_rx,SIGNAL(plotted(cvec,int)),this,SLOT(plotdata(cvec,int)),Qt::BlockingQueuedConnection);
        }
        if(store_text2=="FDD"){
            //Start Waveform FDD
            waveform_tx = new Waveform_FDD_TX(ui);
            waveform_rx = new Waveform_FDD_RX(ui);
            plot = new Plot(ui,((Waveform_FDD_RX*)waveform_rx)->Nfft);
            qRegisterMetaType<vec>("vec");
            qRegisterMetaType<vec>("cvec");
            connect(waveform_rx,SIGNAL(plotted(vec,int)),this,SLOT(plotdata(vec,int)),Qt::BlockingQueuedConnection);
            connect(waveform_rx,SIGNAL(plotted(cvec,int)),this,SLOT(plotdata(cvec,int)),Qt::BlockingQueuedConnection);
        }

        store_text=ui->comboBox->currentText();
        waveform_rx->start();
        waveform_tx->start();

        //Change Button Text
        ui->pushButton->setText("Stop Node");

    }
    else{
        if(store_text2=="TDD"){
            ((Waveform_TDD_RX*)waveform_rx)->stop_signal=true;
            ((Waveform_TDD_TX*)waveform_tx)->stop_signal=true;
            ((Waveform_TDD_RX*)waveform_rx)->packet->file_close();
        }
        if(store_text2=="FDD"){
            ((Waveform_FDD_RX*)waveform_rx)->stop_signal=true;
            ((Waveform_FDD_TX*)waveform_tx)->stop_signal=true;
            ((Waveform_FDD_RX*)waveform_rx)->packet->file_close();
        }
        //Stop Multimedia
        text_rx->close();

        //Change Button Text
        ui->pushButton->setText("Start Node");
    }

}

void MainWindow::on_pushButton_2_clicked()
{
    if(store_text2=="TDD"){

        if(((Waveform_TDD_TX*)waveform_tx)->noderunning==true){
            if(ui->pushButton_2->text()=="Start Video"){
                //Start Sending Video
                ui->pushButton_2->setText("Stop Video");
                video_tx->start();
                ((Waveform_TDD_TX*)waveform_tx)->state="SEND";

            }
            else{
                //Stop Sending Video
                ui->pushButton_2->setText("Start Video");
                video_tx->stop();
                delete video_tx;
                video_tx = new Video_TX();
                connect(video_tx,SIGNAL(displayed(GstElement*,int)),this,SLOT(display(GstElement*,int)),Qt::BlockingQueuedConnection);
                ((Waveform_TDD_TX*)waveform_tx)->packet->restart_video();
            }
        }
    }
    if(store_text2=="FDD"){

        if(((Waveform_FDD_TX*)waveform_tx)->noderunning==true){
            if(ui->pushButton_2->text()=="Start Video"){
                //Start Sending Video
                ui->pushButton_2->setText("Stop Video");
                video_tx->start();
                ((Waveform_FDD_TX*)waveform_tx)->state="SEND";

            }
            else{
                //Stop Sending Video
                ui->pushButton_2->setText("Start Video");
                video_tx->stop();
                delete video_tx;
                video_tx = new Video_TX();
                connect(video_tx,SIGNAL(displayed(GstElement*,int)),this,SLOT(display(GstElement*,int)),Qt::BlockingQueuedConnection);
                ((Waveform_FDD_TX*)waveform_tx)->packet->restart_video();
            }
        }
    }

}


void MainWindow::on_pushButton_3_clicked()
{
    if(store_text2=="TDD"){

        if(((Waveform_TDD_TX*)waveform_tx)->noderunning==true){
            if(ui->pushButton_3->text()=="Start Audio"){
                //Start Sending Audio
                ui->pushButton_3->setText("Stop Audio");
                audio_tx->start();
                ((Waveform_TDD_TX*)waveform_tx)->state="SEND";

            }
            else{
                //Stop Sending Audio
                ui->pushButton_3->setText("Start Audio");
                audio_tx->stop();
                delete audio_tx;
                audio_tx = new Audio_TX();
                ((Waveform_TDD_TX*)waveform_tx)->packet->restart_audio();
            }
        }
    }
    if(store_text2=="FDD"){

        if(((Waveform_FDD_TX*)waveform_tx)->noderunning==true){
            if(ui->pushButton_3->text()=="Start Audio"){
                //Start Sending Audio
                ui->pushButton_3->setText("Stop Audio");
                audio_tx->start();
                ((Waveform_FDD_TX*)waveform_tx)->state="SEND";

            }
            else{
                //Stop Sending Audio
                ui->pushButton_3->setText("Start Audio");
                audio_tx->stop();
                delete audio_tx;
                audio_tx = new Audio_TX();
                ((Waveform_FDD_TX*)waveform_tx)->packet->restart_audio();
            }
        }
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    if(store_text2=="TDD"){

        if(((Waveform_TDD_TX*)waveform_tx)->noderunning==true){
            if(ui->pushButton_4->text()=="Start BER TX"){
                //Start Sending BER
                ui->pushButton_4->setText("Stop BER TX");
                ((Waveform_TDD_TX*)waveform_tx)->packet->is_ber_count=true;
                ((Waveform_TDD_TX*)waveform_tx)->state="SEND";

            }
            else{
                //Stop Sending BER
                ((Waveform_TDD_TX*)waveform_tx)->packet->is_ber_count=false;
                ui->pushButton_4->setText("Start BER TX");
            }
        }
    }
    if(store_text2=="FDD"){

        if(((Waveform_FDD_TX*)waveform_tx)->noderunning==true){
            if(ui->pushButton_4->text()=="Start BER TX"){
                //Start Sending BER
                ui->pushButton_4->setText("Stop BER TX");
                ((Waveform_FDD_TX*)waveform_tx)->packet->is_ber_count=true;
                ((Waveform_FDD_TX*)waveform_tx)->state="SEND";

            }
            else{
                //Stop Sending BER
                ((Waveform_FDD_TX*)waveform_tx)->packet->is_ber_count=false;
                ui->pushButton_4->setText("Start BER TX");
            }
        }
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    if(store_text2=="TDD"){

        if(((Waveform_TDD_RX*)waveform_rx)->noderunning==true){
            if(ui->pushButton_5->text()=="Start BER RX"){
                //Start Receiving BER
                ui->pushButton_5->setText("Stop BER RX");
                ((Waveform_TDD_RX*)waveform_rx)->packet->is_ber_count=true;

            }
            else{
                //Stop Receiving BER
                ((Waveform_TDD_RX*)waveform_rx)->packet->is_ber_count=false;
                ui->pushButton_5->setText("Start BER RX");
            }
        }
    }
    if(store_text2=="FDD"){

        if(((Waveform_FDD_RX*)waveform_rx)->noderunning==true){
            if(ui->pushButton_5->text()=="Start BER RX"){
                //Start Receiving BER
                ui->pushButton_5->setText("Stop BER RX");
                ((Waveform_FDD_RX*)waveform_rx)->packet->is_ber_count=true;

            }
            else{
                //Stop Receiving BER
                ((Waveform_FDD_RX*)waveform_rx)->packet->is_ber_count=false;
                ui->pushButton_5->setText("Start BER RX");
            }
        }
    }

}

void MainWindow::on_lineEdit_returnPressed()
{

    if(store_text2=="TDD"){


        if(((Waveform_TDD_TX*)waveform_tx)->noderunning==true){
            //Start Sending Text
            QString myText;
            myText = ui->lineEdit->text();
            ui->lineEdit->clear();
            ui->textEdit->setTextColor(QColor(Qt::black));
            ui->textEdit->append(myText);
            text_tx->init_text(myText);
            text_tx->start();
            ((Waveform_TDD_TX*)waveform_tx)->state="SEND";
        }
    }

    if(store_text2=="FDD"){


        if(((Waveform_FDD_TX*)waveform_tx)->noderunning==true){
            //Start Sending Text
            QString myText;
            myText = ui->lineEdit->text();
            ui->lineEdit->clear();
            ui->textEdit->setTextColor(QColor(Qt::black));
            ui->textEdit->append(myText);
            text_tx->init_text(myText);
            text_tx->start();
            ((Waveform_FDD_TX*)waveform_tx)->state="SEND";
        }
    }

}

void MainWindow::on_lineEdit_2_returnPressed()
{

    if(store_text2=="TDD"){

        ((Waveform_TDD_TX*)waveform_tx)->device->tx_rate=(ui->lineEdit_2->text().toDouble())*1.0e6;
        ((Waveform_TDD_RX*)waveform_rx)->device->tx_rate=(ui->lineEdit_2->text().toDouble())*1.0e6;

    }
    if(store_text2=="FDD"){

        ((Waveform_FDD_TX*)waveform_tx)->device->tx_rate=(ui->lineEdit_2->text().toDouble())*1.0e6;
        ((Waveform_FDD_RX*)waveform_rx)->device->tx_rate=(ui->lineEdit_2->text().toDouble())*1.0e6;

    }
}

void MainWindow::on_lineEdit_3_returnPressed()
{
    if(store_text2=="TDD"){

        ((Waveform_TDD_TX*)waveform_tx)->device->tx_freq=(ui->lineEdit_3->text().toDouble())*1.0e6;
        ((Waveform_TDD_RX*)waveform_rx)->device->tx_freq=(ui->lineEdit_3->text().toDouble())*1.0e6;

    }
    if(store_text2=="FDD"){

        ((Waveform_FDD_TX*)waveform_tx)->device->tx_freq=(ui->lineEdit_3->text().toDouble())*1.0e6;
        ((Waveform_FDD_RX*)waveform_rx)->device->tx_freq=(ui->lineEdit_3->text().toDouble())*1.0e6;

    }

}

void MainWindow::on_lineEdit_4_returnPressed()
{
    if(store_text2=="TDD"){

        ((Waveform_TDD_TX*)waveform_tx)->device->tx_gain=ui->lineEdit_4->text().toDouble();
        ((Waveform_TDD_RX*)waveform_rx)->device->tx_gain=ui->lineEdit_4->text().toDouble();

    }
    if(store_text2=="FDD"){

        ((Waveform_FDD_TX*)waveform_tx)->device->tx_gain=ui->lineEdit_4->text().toDouble();
        ((Waveform_FDD_RX*)waveform_rx)->device->tx_gain=ui->lineEdit_4->text().toDouble();

    }

}

void MainWindow::on_lineEdit_5_returnPressed()
{
    if(store_text2=="TDD"){

        ((Waveform_TDD_TX*)waveform_tx)->device->tx_amplitude=ui->lineEdit_5->text().toDouble();
        ((Waveform_TDD_RX*)waveform_rx)->device->tx_amplitude=ui->lineEdit_5->text().toDouble();


    }
    if(store_text2=="FDD"){

        ((Waveform_FDD_TX*)waveform_tx)->device->tx_amplitude=ui->lineEdit_5->text().toDouble();
        ((Waveform_FDD_RX*)waveform_rx)->device->tx_amplitude=ui->lineEdit_5->text().toDouble();


    }

}

void MainWindow::on_lineEdit_6_returnPressed()
{
    if(store_text2=="TDD"){

        ((Waveform_TDD_TX*)waveform_tx)->device->rx_rate=(ui->lineEdit_6->text().toDouble())*1.0e6;
        ((Waveform_TDD_RX*)waveform_rx)->device->rx_rate=(ui->lineEdit_6->text().toDouble())*1.0e6;

    }
    if(store_text2=="FDD"){

        ((Waveform_FDD_TX*)waveform_tx)->device->rx_rate=(ui->lineEdit_6->text().toDouble())*1.0e6;
        ((Waveform_FDD_RX*)waveform_rx)->device->rx_rate=(ui->lineEdit_6->text().toDouble())*1.0e6;

    }


}

void MainWindow::on_lineEdit_7_returnPressed()
{
    if(store_text2=="TDD"){

        ((Waveform_TDD_TX*)waveform_tx)->device->rx_freq=(ui->lineEdit_7->text().toDouble())*1.0e6;
        ((Waveform_TDD_RX*)waveform_rx)->device->rx_freq=(ui->lineEdit_7->text().toDouble())*1.0e6;

    }
    if(store_text2=="FDD"){

        ((Waveform_FDD_TX*)waveform_tx)->device->rx_freq=(ui->lineEdit_7->text().toDouble())*1.0e6;
        ((Waveform_FDD_RX*)waveform_rx)->device->rx_freq=(ui->lineEdit_7->text().toDouble())*1.0e6;

    }

}

void MainWindow::on_lineEdit_8_returnPressed()
{
    if(store_text2=="TDD"){

        ((Waveform_TDD_TX*)waveform_tx)->device->rx_gain=ui->lineEdit_8->text().toDouble();
        ((Waveform_TDD_RX*)waveform_rx)->device->rx_gain=ui->lineEdit_8->text().toDouble();

    }
    if(store_text2=="FDD"){

        ((Waveform_FDD_TX*)waveform_tx)->device->rx_gain=ui->lineEdit_8->text().toDouble();
        ((Waveform_FDD_RX*)waveform_rx)->device->rx_gain=ui->lineEdit_8->text().toDouble();

    }


}

void MainWindow::on_lineEdit_9_returnPressed()
{
    if(store_text2=="TDD"){

        ((Waveform_TDD_TX*)waveform_tx)->myaddress=ui->lineEdit_9->text().toInt();
        ((Waveform_TDD_RX*)waveform_rx)->myaddress=ui->lineEdit_9->text().toInt();

    }
    if(store_text2=="FDD"){

        ((Waveform_FDD_TX*)waveform_tx)->myaddress=ui->lineEdit_9->text().toInt();
        ((Waveform_FDD_RX*)waveform_rx)->myaddress=ui->lineEdit_9->text().toInt();

    }


}

void MainWindow::on_lineEdit_10_returnPressed()
{
    if(store_text2=="TDD"){

        ((Waveform_TDD_TX*)waveform_tx)->destaddress=ui->lineEdit_10->text().toInt();
        ((Waveform_TDD_RX*)waveform_rx)->destaddress=ui->lineEdit_10->text().toInt();

    }
    if(store_text2=="FDD"){

        ((Waveform_FDD_TX*)waveform_tx)->destaddress=ui->lineEdit_10->text().toInt();
        ((Waveform_FDD_RX*)waveform_rx)->destaddress=ui->lineEdit_10->text().toInt();

    }

}

void MainWindow::edit_text(QString line){

        ui->textEdit->setTextColor(QColor(Qt::red));
        ui->textEdit->insertPlainText(" ");
        ui->textEdit->insertPlainText(line);
        ui->textEdit->insertPlainText("\n");

}

void MainWindow::display(GstElement *video_sink, int window_display){


    if(window_display==1){

        unsigned long win_id=ui->widget->winId();
        QApplication::syncX();
        gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(video_sink),win_id);

    }
    if(window_display==2){

        unsigned long win_id=ui->widget_2->winId();
        QApplication::syncX();
        gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(video_sink),win_id);

    }

}

void MainWindow::plotdata(vec ydata, int window){
    plot->Plot_data(ydata,window);

}

void MainWindow::plotdata(cvec ydata, int window){
    plot->Plot_data(ydata,window);

}

void MainWindow::on_comboBox_activated(const QString &arg1)
{
    if((store_text!=ui->comboBox->currentText())&&(ui->pushButton->text()=="Stop Node")){
        ui->comboBox->setCurrentIndex(ui->comboBox->findText(store_text));
        ui->comboBox->setItemText(ui->comboBox->currentIndex(),store_text);
    }
}

void MainWindow::on_comboBox_2_activated(const QString &arg1)
{
    if((store_text2!=ui->comboBox_2->currentText())&&(ui->pushButton->text()=="Stop Node")){
        ui->comboBox_2->setCurrentIndex(ui->comboBox_2->findText(store_text2));
        ui->comboBox_2->setItemText(ui->comboBox_2->currentIndex(),store_text2);
    }
}
