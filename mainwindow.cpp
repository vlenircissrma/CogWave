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
    connect(text_rx,SIGNAL(edited_text(QString)),this,SLOT(edit_text(QString)),Qt::QueuedConnection);
    connect(video_tx,SIGNAL(displayed(GstElement*,int)),this,SLOT(display(GstElement*,int)),Qt::QueuedConnection);
    connect(video_rx,SIGNAL(displayed(GstElement*,int)),this,SLOT(display(GstElement*,int)),Qt::QueuedConnection);


    //BlindOFDM
    waveform_tx = new BlindOFDM_TDD_Mode_TX(ui);
    waveform_rx = new BlindOFDM_TDD_Mode_RX(ui);
    connect(waveform_tx,SIGNAL(valuechanged(bool)),waveform_rx,SLOT(setvalue(bool)),Qt::QueuedConnection);
    connect(waveform_rx,SIGNAL(valuechanged(bool)),waveform_tx,SLOT(setvalue(bool)),Qt::DirectConnection);
    connect(waveform_rx,SIGNAL(valuechanged(int)),waveform_tx,SLOT(setvalue(int)),Qt::QueuedConnection);




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

}


void MainWindow::on_pushButton_clicked()
{
    if(ui->pushButton->text()=="Start Node"){


        //Start Multimedia
        video_rx->start();
        audio_rx->start();
        text_rx->start();

        //Start Waveform
        waveform_rx->start();
        waveform_tx->start();

        //Change Button Text
        ui->pushButton->setText("Stop Node");

    }
    else{

        //Stop Waveform
        //BlindOFDM
        waveform_rx->stop_signal=true;
        waveform_tx->stop_signal=true;
        waveform_rx->packets->file_close();

        //Stop Multimedia
        text_rx->close();

        //Change Button Text
        ui->pushButton->setText("Start Node");
    }

}

void MainWindow::on_pushButton_2_clicked()
{

    if(waveform_tx->noderunning==true){
        if(ui->pushButton_2->text()=="Start Video"){
            //Start Sending Video
            ui->pushButton_2->setText("Stop Video");
            video_tx->start();
            waveform_tx->state="SEND";

        }
        else{
            //Stop Sending Video
            ui->pushButton_2->setText("Start Video");
            video_tx->stop();
            delete video_tx;
            video_tx = new Video_TX();
            connect(video_tx,SIGNAL(displayed(GstElement*,int)),this,SLOT(display(GstElement*,int)),Qt::QueuedConnection);
            waveform_tx->packets->restart_video();
        }
    }

}


void MainWindow::on_pushButton_3_clicked()
{

    if(waveform_tx->noderunning==true){
        if(ui->pushButton_3->text()=="Start Audio"){
            //Start Sending Audio
            ui->pushButton_3->setText("Stop Audio");
            audio_tx->start();
            waveform_tx->state="SEND";

        }
        else{
            //Stop Sending Audio
            ui->pushButton_3->setText("Start Audio");
            audio_tx->stop();
            delete audio_tx;
            audio_tx = new Audio_TX();
            waveform_tx->packets->restart_audio();
        }
     }

}

void MainWindow::on_lineEdit_returnPressed()
{
    if(waveform_tx->noderunning==true){
        //Start Sending Text
        QString myText;
        myText = ui->lineEdit->text();
        ui->lineEdit->clear();
        ui->textEdit->setTextColor(QColor(Qt::black));
        ui->textEdit->append(myText);
        text_tx->init_text(myText);
        text_tx->start();
        waveform_tx->state="SEND";
    }

}

void MainWindow::on_lineEdit_2_returnPressed()
{
    waveform_tx->device->tx_rate=(ui->lineEdit_2->text().toDouble())*1.0e6;
    waveform_rx->device->tx_rate=(ui->lineEdit_2->text().toDouble())*1.0e6;

}

void MainWindow::on_lineEdit_3_returnPressed()
{

    waveform_tx->device->tx_freq=(ui->lineEdit_3->text().toDouble())*1.0e6;
    waveform_rx->device->tx_freq=(ui->lineEdit_3->text().toDouble())*1.0e6;


}

void MainWindow::on_lineEdit_4_returnPressed()
{

    waveform_tx->device->tx_gain=ui->lineEdit_4->text().toDouble();
    waveform_rx->device->tx_gain=ui->lineEdit_4->text().toDouble();

}

void MainWindow::on_lineEdit_5_returnPressed()
{

    waveform_tx->device->tx_amplitude=ui->lineEdit_5->text().toDouble();
    waveform_rx->device->tx_amplitude=ui->lineEdit_5->text().toDouble();

}

void MainWindow::on_lineEdit_6_returnPressed()
{

    waveform_tx->device->rx_rate=(ui->lineEdit_6->text().toDouble())*1.0e6;
    waveform_rx->device->rx_rate=(ui->lineEdit_6->text().toDouble())*1.0e6;


}

void MainWindow::on_lineEdit_7_returnPressed()
{

    waveform_tx->device->rx_freq=(ui->lineEdit_7->text().toDouble())*1.0e6;
    waveform_rx->device->rx_freq=(ui->lineEdit_7->text().toDouble())*1.0e6;


}

void MainWindow::on_lineEdit_8_returnPressed()
{
    waveform_tx->device->rx_gain=ui->lineEdit_8->text().toDouble();
    waveform_rx->device->rx_gain=ui->lineEdit_8->text().toDouble();

}

void MainWindow::on_lineEdit_9_returnPressed()
{
    waveform_tx->myaddress=ui->lineEdit_9->text().toInt();
    waveform_rx->myaddress=ui->lineEdit_9->text().toInt();

}

void MainWindow::on_lineEdit_10_returnPressed()
{
    waveform_tx->destaddress=ui->lineEdit_10->text().toInt();
    waveform_rx->destaddress=ui->lineEdit_10->text().toInt();

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
