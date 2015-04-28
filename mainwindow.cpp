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
#include <net/if.h>
#include <linux/if_tun.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

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
    //mcdaaofdm_ber_test = new MCDAAOFDM_BER_Test();
    //bpsk_ber_test = new BPSK_BER_Test();
    //gmsk_ber_test = new GMSK_BER_Test();
    //qpsk_ber_test = new QPSK_BER_Test();
    //cpfsk_ber_test = new CPFSK_BER_Test();
    //dsss_ber_test = new DSSS_BER_Test();
    //mcdads_ber_test = new MCDADS_BER_Test();
    //mccdm_ber_test = new MCCDM_BER_Test();
    //ofdm_ber_test = new OFDM_BER_Test();

    //aloha_performance_test = new ALOHA_Performance_Test(ui);
    //aloha_performance_test->start();
    //csma_performance_test = new CSMA_Performance_Test(ui);
    //csma_performance_test->start();
    //ofdma_tdd_performance_test = new OFDMA_TDD_Performance_Test(ui);
    //ofdma_tdd_performance_test->start();
    //point_to_point_fdd_performance_test = new Point_to_Point_FDD_Performance_Test(ui);
    //point_to_point_fdd_performance_test->start();
    //point_to_point_tdd_performance_test = new Point_to_Point_TDD_Performance_Test(ui);
    //point_to_point_tdd_performance_test->start();
    //tdma_tdd_performance_test = new TDMA_TDD_Performance_Test(ui);
    //tdma_tdd_performance_test->start();
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
    delete data_link_layer_tx;
    delete data_link_layer_rx;
    delete plot;

}

int MainWindow::tun_alloc(char *dev)
{
    struct ifreq ifr;
    int fd, err;

    if( (fd = open("/dev/net/tun", O_RDWR)) < 0 )
       return -1;

    memset(&ifr, 0, sizeof(ifr));

    /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
     *        IFF_TAP   - TAP device
     *
     *        IFF_NO_PI - Do not provide packet information
     */
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    if( *dev )
       strncpy(ifr.ifr_name, dev, IFNAMSIZ);

    if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ){
       ::close(fd);
       return err;
    }
    return fd;
}

void MainWindow::on_pushButton_clicked()
{
    if(ui->pushButton->text()=="Start Node"){

        int fd_ext=tun_alloc((char*)"tun0");
        popen("ifconfig tun0 192.168.1.1 netmask 255.255.255.0 broadcast 192.168.1.255","r");

        //Start Multimedia
        video_rx->start();
        audio_rx->start();
        text_rx->start();

        store_text2=ui->comboBox_2->currentText();
        if(store_text2=="L2:Point to Point TDD"){
            //Start Waveform TDD
            data_link_layer_tx = new Point_to_Point_TDD_TX(ui,fd_ext);
            data_link_layer_rx = new Point_to_Point_TDD_RX(ui,fd_ext);
            connect(data_link_layer_tx,SIGNAL(valuechanged(bool)),data_link_layer_rx,SLOT(setvalue(bool)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(valuechanged(bool,double)),data_link_layer_tx,SLOT(setvalue(bool,double)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(valuechanged(int)),data_link_layer_tx,SLOT(setvalue(int)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(waveformset(int)),data_link_layer_tx,SLOT(setwaveform(int)),Qt::BlockingQueuedConnection);
            plot = new Plot(ui,((Point_to_Point_TDD_RX*)data_link_layer_rx)->Nfft);
            qRegisterMetaType<vec>("vec");
            qRegisterMetaType<cvec>("cvec");
            connect(data_link_layer_rx,SIGNAL(plotted(vec,int)),this,SLOT(plotdata(vec,int)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(plotted(cvec,int)),this,SLOT(plotdata(cvec,int)),Qt::BlockingQueuedConnection);
            #if VIRTUAL_ENABLED==1 || SIMULATOR_ENABLED ==1
            qRegisterMetaType<timestamp_data>("timestamp_data");
            connect(data_link_layer_tx->device,SIGNAL(send_vector(timestamp_data)),data_link_layer_rx->device,SLOT(recv_vector(timestamp_data)),Qt::BlockingQueuedConnection);
            #endif
        }
        if(store_text2=="L2:Point to Point FDD"){
            //Start Waveform FDD
            data_link_layer_tx = new Point_to_Point_FDD_TX(ui,fd_ext);
            data_link_layer_rx = new Point_to_Point_FDD_RX(ui,fd_ext);
            plot = new Plot(ui,((Point_to_Point_FDD_RX*)data_link_layer_rx)->Nfft);
            qRegisterMetaType<vec>("vec");
            qRegisterMetaType<cvec>("cvec");
            connect(data_link_layer_rx,SIGNAL(plotted(vec,int)),this,SLOT(plotdata(vec,int)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(plotted(cvec,int)),this,SLOT(plotdata(cvec,int)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(valuechanged(int)),data_link_layer_tx,SLOT(setvalue(int)),Qt::BlockingQueuedConnection);
            #if VIRTUAL_ENABLED==1 || SIMULATOR_ENABLED ==1
            qRegisterMetaType<timestamp_data>("timestamp_data");
            connect(data_link_layer_tx->device,SIGNAL(send_vector(timestamp_data)),data_link_layer_rx->device,SLOT(recv_vector(timestamp_data)),Qt::BlockingQueuedConnection);
            #endif
        }
        if(store_text2=="L2:ALOHA"){
            data_link_layer_tx = new ALOHA_TX(ui,fd_ext);
            data_link_layer_rx = new ALOHA_RX(ui,fd_ext);
            connect(data_link_layer_rx,SIGNAL(valuechanged(int)),data_link_layer_tx,SLOT(setvalue(int)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(sendack(int)),data_link_layer_tx,SLOT(triggerack(int)),Qt::BlockingQueuedConnection);
            plot = new Plot(ui,((ALOHA_RX*)data_link_layer_rx)->Nfft);
            qRegisterMetaType<vec>("vec");
            qRegisterMetaType<cvec>("cvec");
            connect(data_link_layer_rx,SIGNAL(plotted(vec,int)),this,SLOT(plotdata(vec,int)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(plotted(cvec,int)),this,SLOT(plotdata(cvec,int)),Qt::BlockingQueuedConnection);
            #if VIRTUAL_ENABLED==1 || SIMULATOR_ENABLED ==1
            qRegisterMetaType<timestamp_data>("timestamp_data");
            connect(data_link_layer_tx->device,SIGNAL(send_vector(timestamp_data)),data_link_layer_rx->device,SLOT(recv_vector(timestamp_data)),Qt::BlockingQueuedConnection);
            #endif

        }
        if(store_text2=="L2:CSMA"){
            data_link_layer_tx = new CSMA_TX(ui,fd_ext);
            data_link_layer_rx = new CSMA_RX(ui,fd_ext);
            connect(data_link_layer_rx,SIGNAL(valuechanged(int)),data_link_layer_tx,SLOT(setvalue(int)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(sendack(int)),data_link_layer_tx,SLOT(triggerack(int)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(signal_detect(bool)),data_link_layer_tx,SLOT(signal_detected(bool)),Qt::BlockingQueuedConnection);
            plot = new Plot(ui,((CSMA_RX*)data_link_layer_rx)->Nfft);
            qRegisterMetaType<vec>("vec");
            qRegisterMetaType<cvec>("cvec");
            connect(data_link_layer_rx,SIGNAL(plotted(vec,int)),this,SLOT(plotdata(vec,int)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(plotted(cvec,int)),this,SLOT(plotdata(cvec,int)),Qt::BlockingQueuedConnection);
            #if VIRTUAL_ENABLED==1 || SIMULATOR_ENABLED ==1
            qRegisterMetaType<timestamp_data>("timestamp_data");
            connect(data_link_layer_tx->device,SIGNAL(send_vector(timestamp_data)),data_link_layer_rx->device,SLOT(recv_vector(timestamp_data)),Qt::BlockingQueuedConnection);
            #endif

        }
        if(store_text2=="L2:TDMA TDD"){
            //Start Waveform TDD
            data_link_layer_tx = new TDMA_TDD_TX(ui,fd_ext);
            data_link_layer_rx = new TDMA_TDD_RX(ui,fd_ext);
            connect(data_link_layer_tx,SIGNAL(valuechanged(bool)),data_link_layer_rx,SLOT(setvalue(bool)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(valuechanged(bool,double)),data_link_layer_tx,SLOT(setvalue(bool,double)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(valuechanged(int)),data_link_layer_tx,SLOT(setvalue(int)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(waveformset(int)),data_link_layer_tx,SLOT(setwaveform(int)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(slot_allocation(int)),data_link_layer_tx,SLOT(slot_allocated(int)),Qt::BlockingQueuedConnection);
            plot = new Plot(ui,((TDMA_TDD_RX*)data_link_layer_rx)->Nfft);
            qRegisterMetaType<vec>("vec");
            qRegisterMetaType<cvec>("cvec");
            connect(data_link_layer_rx,SIGNAL(plotted(vec,int)),this,SLOT(plotdata(vec,int)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(plotted(cvec,int)),this,SLOT(plotdata(cvec,int)),Qt::BlockingQueuedConnection);
            #if VIRTUAL_ENABLED==1 || SIMULATOR_ENABLED ==1
            qRegisterMetaType<timestamp_data>("timestamp_data");
            connect(data_link_layer_tx->device,SIGNAL(send_vector(timestamp_data)),data_link_layer_rx->device,SLOT(recv_vector(timestamp_data)),Qt::BlockingQueuedConnection);
            #endif

        }
        if(store_text2=="L2:OFDMA TDD"){
            ui->comboBox->setCurrentIndex(ui->comboBox->findText("L1:MCDAAOFDM"));
            //Start Waveform TDD
            data_link_layer_tx = new OFDMA_TDD_TX(ui,fd_ext);
            data_link_layer_rx = new OFDMA_TDD_RX(ui,fd_ext);
            connect(data_link_layer_tx,SIGNAL(valuechanged(bool)),data_link_layer_rx,SLOT(setvalue(bool)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(valuechanged(bool,double)),data_link_layer_tx,SLOT(setvalue(bool,double)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(valuechanged(int)),data_link_layer_tx,SLOT(setvalue(int)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(waveformset(int)),data_link_layer_tx,SLOT(setwaveform(int)),Qt::BlockingQueuedConnection);
            plot = new Plot(ui,((OFDMA_TDD_RX*)data_link_layer_rx)->Nfft);
            qRegisterMetaType<vec>("vec");
            qRegisterMetaType<cvec>("cvec");
            connect(data_link_layer_rx,SIGNAL(plotted(vec,int)),this,SLOT(plotdata(vec,int)),Qt::BlockingQueuedConnection);
            connect(data_link_layer_rx,SIGNAL(plotted(cvec,int)),this,SLOT(plotdata(cvec,int)),Qt::BlockingQueuedConnection);
            #if VIRTUAL_ENABLED==1 || SIMULATOR_ENABLED ==1
            qRegisterMetaType<timestamp_data>("timestamp_data");
            connect(data_link_layer_tx->device,SIGNAL(send_vector(timestamp_data)),data_link_layer_rx->device,SLOT(recv_vector(timestamp_data)),Qt::BlockingQueuedConnection);
            #endif
        }
        store_text=ui->comboBox->currentText();
        data_link_layer_rx->start();
        data_link_layer_tx->start();

        //Change Button Text
        ui->pushButton->setText("Stop Node");

    }
    else{
        if(store_text2=="L2:Point to Point TDD"){
            ((Point_to_Point_TDD_RX*)data_link_layer_rx)->stop_signal=true;
            ((Point_to_Point_TDD_TX*)data_link_layer_tx)->stop_signal=true;
            ((Point_to_Point_TDD_RX*)data_link_layer_rx)->packet->file_close();
        }
        if(store_text2=="L2:Point to Point FDD"){
            ((Point_to_Point_FDD_RX*)data_link_layer_rx)->stop_signal=true;
            ((Point_to_Point_FDD_TX*)data_link_layer_tx)->stop_signal=true;
            ((Point_to_Point_FDD_RX*)data_link_layer_rx)->packet->file_close();
        }
        if(store_text2=="L2:ALOHA"){
            ((ALOHA_RX*)data_link_layer_rx)->stop_signal=true;
            ((ALOHA_TX*)data_link_layer_tx)->stop_signal=true;
            ((ALOHA_RX*)data_link_layer_rx)->packet->file_close();
        }
        if(store_text2=="L2:CSMA"){
            ((CSMA_RX*)data_link_layer_rx)->stop_signal=true;
            ((CSMA_TX*)data_link_layer_tx)->stop_signal=true;
            ((CSMA_RX*)data_link_layer_rx)->packet->file_close();
        }
        if(store_text2=="L2:TDMA TDD"){
            ((TDMA_TDD_RX*)data_link_layer_rx)->stop_signal=true;
            ((TDMA_TDD_TX*)data_link_layer_tx)->stop_signal=true;
            ((TDMA_TDD_RX*)data_link_layer_rx)->packet->file_close();
        }
        if(store_text2=="L2:OFDMA TDD"){
            ((OFDMA_TDD_RX*)data_link_layer_rx)->stop_signal=true;
            ((OFDMA_TDD_TX*)data_link_layer_tx)->stop_signal=true;
            ((OFDMA_TDD_RX*)data_link_layer_rx)->packet->file_close();
        }
        //Stop Multimedia
        text_rx->close();

        //Change Button Text
        ui->pushButton->setText("Start Node");
    }

}

void MainWindow::on_pushButton_2_clicked()
{
    if(store_text2=="L2:Point to Point TDD"){

        if(((Point_to_Point_TDD_TX*)data_link_layer_tx)->noderunning==true){
            if(ui->pushButton_2->text()=="Start Video"){
                //Start Sending Video
                ui->pushButton_2->setText("Stop Video");
                video_tx->start();
                ((Point_to_Point_TDD_TX*)data_link_layer_tx)->state="SEND";

            }
            else{
                //Stop Sending Video
                ui->pushButton_2->setText("Start Video");
                video_tx->stop();
                delete video_tx;
                video_tx = new Video_TX();
                connect(video_tx,SIGNAL(displayed(GstElement*,int)),this,SLOT(display(GstElement*,int)),Qt::BlockingQueuedConnection);
                ((Point_to_Point_TDD_TX*)data_link_layer_tx)->packet->restart_video();
            }
        }
    }
    if(store_text2=="L2:Point to Point FDD"){

        if(((Point_to_Point_FDD_TX*)data_link_layer_tx)->noderunning==true){
            if(ui->pushButton_2->text()=="Start Video"){
                //Start Sending Video
                ui->pushButton_2->setText("Stop Video");
                video_tx->start();
                ((Point_to_Point_FDD_TX*)data_link_layer_tx)->state="SEND";

            }
            else{
                //Stop Sending Video
                ui->pushButton_2->setText("Start Video");
                video_tx->stop();
                delete video_tx;
                video_tx = new Video_TX();
                connect(video_tx,SIGNAL(displayed(GstElement*,int)),this,SLOT(display(GstElement*,int)),Qt::BlockingQueuedConnection);
                ((Point_to_Point_FDD_TX*)data_link_layer_tx)->packet->restart_video();
            }
        }
    }
    if(store_text2=="L2:ALOHA"){

        if(((ALOHA_TX*)data_link_layer_tx)->noderunning==true){
            if(ui->pushButton_2->text()=="Start Video"){
                //Start Sending Video
                ui->pushButton_2->setText("Stop Video");
                video_tx->start();
                ((ALOHA_TX*)data_link_layer_tx)->state="SEND";

            }
            else{
                //Stop Sending Video
                ui->pushButton_2->setText("Start Video");
                video_tx->stop();
                delete video_tx;
                video_tx = new Video_TX();
                connect(video_tx,SIGNAL(displayed(GstElement*,int)),this,SLOT(display(GstElement*,int)),Qt::BlockingQueuedConnection);
                ((ALOHA_TX*)data_link_layer_tx)->packet->restart_video();
            }
        }
    }
    if(store_text2=="L2:CSMA"){

        if(((CSMA_TX*)data_link_layer_tx)->noderunning==true){
            if(ui->pushButton_2->text()=="Start Video"){
                //Start Sending Video
                ui->pushButton_2->setText("Stop Video");
                video_tx->start();
                ((CSMA_TX*)data_link_layer_tx)->state="SEND";

            }
            else{
                //Stop Sending Video
                ui->pushButton_2->setText("Start Video");
                video_tx->stop();
                delete video_tx;
                video_tx = new Video_TX();
                connect(video_tx,SIGNAL(displayed(GstElement*,int)),this,SLOT(display(GstElement*,int)),Qt::BlockingQueuedConnection);
                ((CSMA_TX*)data_link_layer_tx)->packet->restart_video();
            }
        }
    }
    if(store_text2=="L2:TDMA TDD"){

        if(((TDMA_TDD_TX*)data_link_layer_tx)->noderunning==true){
            if(ui->pushButton_2->text()=="Start Video"){
                //Start Sending Video
                ui->pushButton_2->setText("Stop Video");
                video_tx->start();
                ((TDMA_TDD_TX*)data_link_layer_tx)->state="SEND";

            }
            else{
                //Stop Sending Video
                ui->pushButton_2->setText("Start Video");
                video_tx->stop();
                delete video_tx;
                video_tx = new Video_TX();
                connect(video_tx,SIGNAL(displayed(GstElement*,int)),this,SLOT(display(GstElement*,int)),Qt::BlockingQueuedConnection);
                ((TDMA_TDD_TX*)data_link_layer_tx)->packet->restart_video();
            }
        }
    }
    if(store_text2=="L2:OFDMA TDD"){

        if(((OFDMA_TDD_TX*)data_link_layer_tx)->noderunning==true){
            if(ui->pushButton_2->text()=="Start Video"){
                //Start Sending Video
                ui->pushButton_2->setText("Stop Video");
                video_tx->start();
                ((OFDMA_TDD_TX*)data_link_layer_tx)->state="SEND";

            }
            else{
                //Stop Sending Video
                ui->pushButton_2->setText("Start Video");
                video_tx->stop();
                delete video_tx;
                video_tx = new Video_TX();
                connect(video_tx,SIGNAL(displayed(GstElement*,int)),this,SLOT(display(GstElement*,int)),Qt::BlockingQueuedConnection);
                ((OFDMA_TDD_TX*)data_link_layer_tx)->packet->restart_video();
            }
        }
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    if(store_text2=="L2:Point to Point TDD"){

        if(((Point_to_Point_TDD_TX*)data_link_layer_tx)->noderunning==true){
            if(ui->pushButton_3->text()=="Start Audio"){
                //Start Sending Audio
                ui->pushButton_3->setText("Stop Audio");
                audio_tx->start();
                ((Point_to_Point_TDD_TX*)data_link_layer_tx)->state="SEND";

            }
            else{
                //Stop Sending Audio
                ui->pushButton_3->setText("Start Audio");
                audio_tx->stop();
                delete audio_tx;
                audio_tx = new Audio_TX();
                ((Point_to_Point_TDD_TX*)data_link_layer_tx)->packet->restart_audio();
            }
        }
    }
    if(store_text2=="L2:Point to Point FDD"){

        if(((Point_to_Point_FDD_TX*)data_link_layer_tx)->noderunning==true){
            if(ui->pushButton_3->text()=="Start Audio"){
                //Start Sending Audio
                ui->pushButton_3->setText("Stop Audio");
                audio_tx->start();
                ((Point_to_Point_FDD_TX*)data_link_layer_tx)->state="SEND";

            }
            else{
                //Stop Sending Audio
                ui->pushButton_3->setText("Start Audio");
                audio_tx->stop();
                delete audio_tx;
                audio_tx = new Audio_TX();
                ((Point_to_Point_FDD_TX*)data_link_layer_tx)->packet->restart_audio();
            }
        }
    }
    if(store_text2=="L2:ALOHA"){

        if(((ALOHA_TX*)data_link_layer_tx)->noderunning==true){
            if(ui->pushButton_3->text()=="Start Audio"){
                //Start Sending Audio
                ui->pushButton_3->setText("Stop Audio");
                audio_tx->start();
                ((ALOHA_TX*)data_link_layer_tx)->state="SEND";

            }
            else{
                //Stop Sending Audio
                ui->pushButton_3->setText("Start Audio");
                audio_tx->stop();
                delete audio_tx;
                audio_tx = new Audio_TX();
                ((ALOHA_TX*)data_link_layer_tx)->packet->restart_audio();
            }
        }
    }
    if(store_text2=="L2:CSMA"){

        if(((CSMA_TX*)data_link_layer_tx)->noderunning==true){
            if(ui->pushButton_3->text()=="Start Audio"){
                //Start Sending Audio
                ui->pushButton_3->setText("Stop Audio");
                audio_tx->start();
                ((CSMA_TX*)data_link_layer_tx)->state="SEND";

            }
            else{
                //Stop Sending Audio
                ui->pushButton_3->setText("Start Audio");
                audio_tx->stop();
                delete audio_tx;
                audio_tx = new Audio_TX();
                ((CSMA_TX*)data_link_layer_tx)->packet->restart_audio();
            }
        }
    }
    if(store_text2=="L2:TDMA TDD"){

        if(((TDMA_TDD_TX*)data_link_layer_tx)->noderunning==true){
            if(ui->pushButton_3->text()=="Start Audio"){
                //Start Sending Audio
                ui->pushButton_3->setText("Stop Audio");
                audio_tx->start();
                ((TDMA_TDD_TX*)data_link_layer_tx)->state="SEND";

            }
            else{
                //Stop Sending Audio
                ui->pushButton_3->setText("Start Audio");
                audio_tx->stop();
                delete audio_tx;
                audio_tx = new Audio_TX();
                ((TDMA_TDD_TX*)data_link_layer_tx)->packet->restart_audio();
            }
        }
    }
    if(store_text2=="L2:OFDMA TDD"){

        if(((OFDMA_TDD_TX*)data_link_layer_tx)->noderunning==true){
            if(ui->pushButton_3->text()=="Start Audio"){
                //Start Sending Audio
                ui->pushButton_3->setText("Stop Audio");
                audio_tx->start();
                ((OFDMA_TDD_TX*)data_link_layer_tx)->state="SEND";

            }
            else{
                //Stop Sending Audio
                ui->pushButton_3->setText("Start Audio");
                audio_tx->stop();
                delete audio_tx;
                audio_tx = new Audio_TX();
                ((OFDMA_TDD_TX*)data_link_layer_tx)->packet->restart_audio();
            }
        }
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    if(store_text2=="L2:Point to Point TDD"){

        if(((Point_to_Point_TDD_TX*)data_link_layer_tx)->noderunning==true){
            if(ui->pushButton_4->text()=="Start BER TX"){
                //Start Sending BER
                ui->pushButton_4->setText("Stop BER TX");
                ((Point_to_Point_TDD_TX*)data_link_layer_tx)->packet->is_ber_count=true;
                ((Point_to_Point_TDD_TX*)data_link_layer_tx)->state="SEND";

            }
            else{
                //Stop Sending BER
                ((Point_to_Point_TDD_TX*)data_link_layer_tx)->packet->is_ber_count=false;
                ui->pushButton_4->setText("Start BER TX");
            }
        }
    }
    if(store_text2=="L2:Point to Point FDD"){

        if(((Point_to_Point_FDD_TX*)data_link_layer_tx)->noderunning==true){
            if(ui->pushButton_4->text()=="Start BER TX"){
                //Start Sending BER
                ui->pushButton_4->setText("Stop BER TX");
                ((Point_to_Point_FDD_TX*)data_link_layer_tx)->packet->is_ber_count=true;
                ((Point_to_Point_FDD_TX*)data_link_layer_tx)->state="SEND";

            }
            else{
                //Stop Sending BER
                ((Point_to_Point_FDD_TX*)data_link_layer_tx)->packet->is_ber_count=false;
                ui->pushButton_4->setText("Start BER TX");
            }
        }
    }
    if(store_text2=="L2:ALOHA"){

        if(((ALOHA_TX*)data_link_layer_tx)->noderunning==true){
            if(ui->pushButton_4->text()=="Start BER TX"){
                //Start Sending BER
                ui->pushButton_4->setText("Stop BER TX");
                ((ALOHA_TX*)data_link_layer_tx)->packet->is_ber_count=true;
                ((ALOHA_TX*)data_link_layer_tx)->state="SEND";

            }
            else{
                //Stop Sending BER
                ((ALOHA_TX*)data_link_layer_tx)->packet->is_ber_count=false;
                ui->pushButton_4->setText("Start BER TX");
            }
        }
    }
    if(store_text2=="L2:CSMA"){

        if(((CSMA_TX*)data_link_layer_tx)->noderunning==true){
            if(ui->pushButton_4->text()=="Start BER TX"){
                //Start Sending BER
                ui->pushButton_4->setText("Stop BER TX");
                ((CSMA_TX*)data_link_layer_tx)->packet->is_ber_count=true;
                ((CSMA_TX*)data_link_layer_tx)->state="SEND";

            }
            else{
                //Stop Sending BER
                ((CSMA_TX*)data_link_layer_tx)->packet->is_ber_count=false;
                ui->pushButton_4->setText("Start BER TX");
            }
        }
    }
    if(store_text2=="L2:TDMA TDD"){

        if(((TDMA_TDD_TX*)data_link_layer_tx)->noderunning==true){
            if(ui->pushButton_4->text()=="Start BER TX"){
                //Start Sending BER
                ui->pushButton_4->setText("Stop BER TX");
                ((TDMA_TDD_TX*)data_link_layer_tx)->packet->is_ber_count=true;
                ((TDMA_TDD_TX*)data_link_layer_tx)->state="SEND";

            }
            else{
                //Stop Sending BER
                ((TDMA_TDD_TX*)data_link_layer_tx)->packet->is_ber_count=false;
                ui->pushButton_4->setText("Start BER TX");
            }
        }
    }
    if(store_text2=="L2:OFDMA TDD"){

        if(((OFDMA_TDD_TX*)data_link_layer_tx)->noderunning==true){
            if(ui->pushButton_4->text()=="Start BER TX"){
                //Start Sending BER
                ui->pushButton_4->setText("Stop BER TX");
                ((OFDMA_TDD_TX*)data_link_layer_tx)->packet->is_ber_count=true;
                ((OFDMA_TDD_TX*)data_link_layer_tx)->state="SEND";

            }
            else{
                //Stop Sending BER
                ((OFDMA_TDD_TX*)data_link_layer_tx)->packet->is_ber_count=false;
                ui->pushButton_4->setText("Start BER TX");
            }
        }
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    if(store_text2=="L2:Point to Point TDD"){

        if(((Point_to_Point_TDD_RX*)data_link_layer_rx)->noderunning==true){
            if(ui->pushButton_5->text()=="Start BER RX"){
                //Start Receiving BER
                ui->pushButton_5->setText("Stop BER RX");
                ((Point_to_Point_TDD_RX*)data_link_layer_rx)->packet->is_ber_count=true;

            }
            else{
                //Stop Receiving BER
                ((Point_to_Point_TDD_RX*)data_link_layer_rx)->packet->is_ber_count=false;
                ui->pushButton_5->setText("Start BER RX");
            }
        }
    }
    if(store_text2=="L2:Point to Point FDD"){

        if(((Point_to_Point_FDD_RX*)data_link_layer_rx)->noderunning==true){
            if(ui->pushButton_5->text()=="Start BER RX"){
                //Start Receiving BER
                ui->pushButton_5->setText("Stop BER RX");
                ((Point_to_Point_FDD_RX*)data_link_layer_rx)->packet->is_ber_count=true;

            }
            else{
                //Stop Receiving BER
                ((Point_to_Point_FDD_RX*)data_link_layer_rx)->packet->is_ber_count=false;
                ui->pushButton_5->setText("Start BER RX");
            }
        }
    }
    if(store_text2=="L2:ALOHA"){

        if(((ALOHA_RX*)data_link_layer_rx)->noderunning==true){
            if(ui->pushButton_5->text()=="Start BER RX"){
                //Start Receiving BER
                ui->pushButton_5->setText("Stop BER RX");
                ((ALOHA_RX*)data_link_layer_rx)->packet->is_ber_count=true;

            }
            else{
                //Stop Receiving BER
                ((ALOHA_RX*)data_link_layer_rx)->packet->is_ber_count=false;
                ui->pushButton_5->setText("Start BER RX");
            }
        }
    }
    if(store_text2=="L2:CSMA"){

        if(((CSMA_RX*)data_link_layer_rx)->noderunning==true){
            if(ui->pushButton_5->text()=="Start BER RX"){
                //Start Receiving BER
                ui->pushButton_5->setText("Stop BER RX");
                ((CSMA_RX*)data_link_layer_rx)->packet->is_ber_count=true;

            }
            else{
                //Stop Receiving BER
                ((CSMA_RX*)data_link_layer_rx)->packet->is_ber_count=false;
                ui->pushButton_5->setText("Start BER RX");
            }
        }
    }
    if(store_text2=="L2:TDMA TDD"){

        if(((TDMA_TDD_RX*)data_link_layer_rx)->noderunning==true){
            if(ui->pushButton_5->text()=="Start BER RX"){
                //Start Receiving BER
                ui->pushButton_5->setText("Stop BER RX");
                ((TDMA_TDD_RX*)data_link_layer_rx)->packet->is_ber_count=true;

            }
            else{
                //Stop Receiving BER
                ((TDMA_TDD_RX*)data_link_layer_rx)->packet->is_ber_count=false;
                ui->pushButton_5->setText("Start BER RX");
            }
        }
    }
    if(store_text2=="L2:OFDMA TDD"){

        if(((OFDMA_TDD_RX*)data_link_layer_rx)->noderunning==true){
            if(ui->pushButton_5->text()=="Start BER RX"){
                //Start Receiving BER
                ui->pushButton_5->setText("Stop BER RX");
                ((OFDMA_TDD_RX*)data_link_layer_rx)->packet->is_ber_count=true;

            }
            else{
                //Stop Receiving BER
                ((OFDMA_TDD_RX*)data_link_layer_rx)->packet->is_ber_count=false;
                ui->pushButton_5->setText("Start BER RX");
            }
        }
    }
}

void MainWindow::on_lineEdit_returnPressed()
{

    if(store_text2=="L2:Point to Point TDD"){


        if(((Point_to_Point_TDD_TX*)data_link_layer_tx)->noderunning==true){
            //Start Sending Text
            QString myText;
            myText = ui->lineEdit->text();
            ui->lineEdit->clear();
            ui->textEdit->setTextColor(QColor(Qt::black));
            ui->textEdit->append(myText);
            text_tx->init_text(myText);
            text_tx->start();
            ((Point_to_Point_TDD_TX*)data_link_layer_tx)->state="SEND";
        }
    }

    if(store_text2=="L2:Point to Point FDD"){


        if(((Point_to_Point_FDD_TX*)data_link_layer_tx)->noderunning==true){
            //Start Sending Text
            QString myText;
            myText = ui->lineEdit->text();
            ui->lineEdit->clear();
            ui->textEdit->setTextColor(QColor(Qt::black));
            ui->textEdit->append(myText);
            text_tx->init_text(myText);
            text_tx->start();
            ((Point_to_Point_FDD_TX*)data_link_layer_tx)->state="SEND";
        }
    }
    if(store_text2=="L2:ALOHA"){


        if(((ALOHA_TX*)data_link_layer_tx)->noderunning==true){
            //Start Sending Text
            QString myText;
            myText = ui->lineEdit->text();
            ui->lineEdit->clear();
            ui->textEdit->setTextColor(QColor(Qt::black));
            ui->textEdit->append(myText);
            text_tx->init_text(myText);
            text_tx->start();
            ((ALOHA_TX*)data_link_layer_tx)->state="SEND";
        }
    }
    if(store_text2=="L2:CSMA"){


        if(((CSMA_TX*)data_link_layer_tx)->noderunning==true){
            //Start Sending Text
            QString myText;
            myText = ui->lineEdit->text();
            ui->lineEdit->clear();
            ui->textEdit->setTextColor(QColor(Qt::black));
            ui->textEdit->append(myText);
            text_tx->init_text(myText);
            text_tx->start();
            ((CSMA_TX*)data_link_layer_tx)->state="SEND";
        }
    }
    if(store_text2=="L2:TDMA TDD"){


        if(((TDMA_TDD_TX*)data_link_layer_tx)->noderunning==true){
            //Start Sending Text
            QString myText;
            myText = ui->lineEdit->text();
            ui->lineEdit->clear();
            ui->textEdit->setTextColor(QColor(Qt::black));
            ui->textEdit->append(myText);
            text_tx->init_text(myText);
            text_tx->start();
            ((TDMA_TDD_TX*)data_link_layer_tx)->state="SEND";
        }
    }
    if(store_text2=="L2:OFDMA TDD"){


        if(((OFDMA_TDD_TX*)data_link_layer_tx)->noderunning==true){
            //Start Sending Text
            QString myText;
            myText = ui->lineEdit->text();
            ui->lineEdit->clear();
            ui->textEdit->setTextColor(QColor(Qt::black));
            ui->textEdit->append(myText);
            text_tx->init_text(myText);
            text_tx->start();
            ((OFDMA_TDD_TX*)data_link_layer_tx)->state="SEND";
        }
    }
}

void MainWindow::on_lineEdit_2_returnPressed()
{

    if(store_text2=="L2:Point to Point TDD"){

        ((Point_to_Point_TDD_TX*)data_link_layer_tx)->device->tx_rate=(ui->lineEdit_2->text().toDouble())*1.0e6;
        ((Point_to_Point_TDD_RX*)data_link_layer_rx)->device->tx_rate=(ui->lineEdit_2->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:Point to Point FDD"){

        ((Point_to_Point_FDD_TX*)data_link_layer_tx)->device->tx_rate=(ui->lineEdit_2->text().toDouble())*1.0e6;
        ((Point_to_Point_FDD_RX*)data_link_layer_rx)->device->tx_rate=(ui->lineEdit_2->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:ALOHA"){

        ((ALOHA_TX*)data_link_layer_tx)->device->tx_rate=(ui->lineEdit_2->text().toDouble())*1.0e6;
        ((ALOHA_RX*)data_link_layer_rx)->device->tx_rate=(ui->lineEdit_2->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:CSMA"){

        ((CSMA_TX*)data_link_layer_tx)->device->tx_rate=(ui->lineEdit_2->text().toDouble())*1.0e6;
        ((CSMA_RX*)data_link_layer_rx)->device->tx_rate=(ui->lineEdit_2->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:TDMA TDD"){

        ((TDMA_TDD_TX*)data_link_layer_tx)->device->tx_rate=(ui->lineEdit_2->text().toDouble())*1.0e6;
        ((TDMA_TDD_RX*)data_link_layer_rx)->device->tx_rate=(ui->lineEdit_2->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:OFDMA TDD"){

        ((OFDMA_TDD_TX*)data_link_layer_tx)->device->tx_rate=(ui->lineEdit_2->text().toDouble())*1.0e6;
        ((OFDMA_TDD_RX*)data_link_layer_rx)->device->tx_rate=(ui->lineEdit_2->text().toDouble())*1.0e6;

    }
}

void MainWindow::on_lineEdit_3_returnPressed()
{
    if(store_text2=="L2:Point to Point TDD"){

        ((Point_to_Point_TDD_TX*)data_link_layer_tx)->device->tx_freq=(ui->lineEdit_3->text().toDouble())*1.0e6;
        ((Point_to_Point_TDD_RX*)data_link_layer_rx)->device->tx_freq=(ui->lineEdit_3->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:Point to Point FDD"){

        ((Point_to_Point_FDD_TX*)data_link_layer_tx)->device->tx_freq=(ui->lineEdit_3->text().toDouble())*1.0e6;
        ((Point_to_Point_FDD_RX*)data_link_layer_rx)->device->tx_freq=(ui->lineEdit_3->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:ALOHA"){

        ((ALOHA_TX*)data_link_layer_tx)->device->tx_freq=(ui->lineEdit_3->text().toDouble())*1.0e6;
        ((ALOHA_RX*)data_link_layer_rx)->device->tx_freq=(ui->lineEdit_3->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:CSMA"){

        ((CSMA_TX*)data_link_layer_tx)->device->tx_freq=(ui->lineEdit_3->text().toDouble())*1.0e6;
        ((CSMA_RX*)data_link_layer_rx)->device->tx_freq=(ui->lineEdit_3->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:TDMA TDD"){

        ((TDMA_TDD_TX*)data_link_layer_tx)->device->tx_freq=(ui->lineEdit_3->text().toDouble())*1.0e6;
        ((TDMA_TDD_RX*)data_link_layer_rx)->device->tx_freq=(ui->lineEdit_3->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:OFDMA TDD"){

        ((OFDMA_TDD_TX*)data_link_layer_tx)->device->tx_freq=(ui->lineEdit_3->text().toDouble())*1.0e6;
        ((OFDMA_TDD_RX*)data_link_layer_rx)->device->tx_freq=(ui->lineEdit_3->text().toDouble())*1.0e6;

    }
}

void MainWindow::on_lineEdit_4_returnPressed()
{
    if(store_text2=="L2:Point to Point TDD"){

        ((Point_to_Point_TDD_TX*)data_link_layer_tx)->device->tx_gain=ui->lineEdit_4->text().toDouble();
        ((Point_to_Point_TDD_RX*)data_link_layer_rx)->device->tx_gain=ui->lineEdit_4->text().toDouble();

    }
    if(store_text2=="L2:Point to Point FDD"){

        ((Point_to_Point_FDD_TX*)data_link_layer_tx)->device->tx_gain=ui->lineEdit_4->text().toDouble();
        ((Point_to_Point_FDD_RX*)data_link_layer_rx)->device->tx_gain=ui->lineEdit_4->text().toDouble();

    }
    if(store_text2=="L2:ALOHA"){

        ((ALOHA_TX*)data_link_layer_tx)->device->tx_gain=ui->lineEdit_4->text().toDouble();
        ((ALOHA_RX*)data_link_layer_rx)->device->tx_gain=ui->lineEdit_4->text().toDouble();

    }
    if(store_text2=="L2:CSMA"){

        ((CSMA_TX*)data_link_layer_tx)->device->tx_gain=ui->lineEdit_4->text().toDouble();
        ((CSMA_RX*)data_link_layer_rx)->device->tx_gain=ui->lineEdit_4->text().toDouble();

    }
    if(store_text2=="L2:TDMA TDD"){

        ((TDMA_TDD_TX*)data_link_layer_tx)->device->tx_gain=ui->lineEdit_4->text().toDouble();
        ((TDMA_TDD_RX*)data_link_layer_rx)->device->tx_gain=ui->lineEdit_4->text().toDouble();

    }
    if(store_text2=="L2:OFDMA TDD"){

        ((OFDMA_TDD_TX*)data_link_layer_tx)->device->tx_gain=ui->lineEdit_4->text().toDouble();
        ((OFDMA_TDD_RX*)data_link_layer_rx)->device->tx_gain=ui->lineEdit_4->text().toDouble();

    }
}

void MainWindow::on_lineEdit_5_returnPressed()
{
    if(store_text2=="L2:Point to Point TDD"){

        ((Point_to_Point_TDD_TX*)data_link_layer_tx)->device->tx_amplitude=ui->lineEdit_5->text().toDouble();
        ((Point_to_Point_TDD_RX*)data_link_layer_rx)->device->tx_amplitude=ui->lineEdit_5->text().toDouble();


    }
    if(store_text2=="L2:Point to Point FDD"){

        ((Point_to_Point_FDD_TX*)data_link_layer_tx)->device->tx_amplitude=ui->lineEdit_5->text().toDouble();
        ((Point_to_Point_FDD_RX*)data_link_layer_rx)->device->tx_amplitude=ui->lineEdit_5->text().toDouble();


    }
    if(store_text2=="L2:ALOHA"){

        ((ALOHA_TX*)data_link_layer_tx)->device->tx_amplitude=ui->lineEdit_5->text().toDouble();
        ((ALOHA_RX*)data_link_layer_rx)->device->tx_amplitude=ui->lineEdit_5->text().toDouble();


    }
    if(store_text2=="L2:CSMA"){

        ((CSMA_TX*)data_link_layer_tx)->device->tx_amplitude=ui->lineEdit_5->text().toDouble();
        ((CSMA_RX*)data_link_layer_rx)->device->tx_amplitude=ui->lineEdit_5->text().toDouble();


    }
    if(store_text2=="L2:TDMA TDD"){

        ((TDMA_TDD_TX*)data_link_layer_tx)->device->tx_amplitude=ui->lineEdit_5->text().toDouble();
        ((TDMA_TDD_RX*)data_link_layer_rx)->device->tx_amplitude=ui->lineEdit_5->text().toDouble();


    }
    if(store_text2=="L2:OFDMA TDD"){

        ((OFDMA_TDD_TX*)data_link_layer_tx)->device->tx_amplitude=ui->lineEdit_5->text().toDouble();
        ((OFDMA_TDD_RX*)data_link_layer_rx)->device->tx_amplitude=ui->lineEdit_5->text().toDouble();


    }
}

void MainWindow::on_lineEdit_6_returnPressed()
{
    if(store_text2=="L2:Point to Point TDD"){

        ((Point_to_Point_TDD_TX*)data_link_layer_tx)->device->rx_rate=(ui->lineEdit_6->text().toDouble())*1.0e6;
        ((Point_to_Point_TDD_RX*)data_link_layer_rx)->device->rx_rate=(ui->lineEdit_6->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:Point to Point FDD"){

        ((Point_to_Point_FDD_TX*)data_link_layer_tx)->device->rx_rate=(ui->lineEdit_6->text().toDouble())*1.0e6;
        ((Point_to_Point_FDD_RX*)data_link_layer_rx)->device->rx_rate=(ui->lineEdit_6->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:ALOHA"){

        ((ALOHA_TX*)data_link_layer_tx)->device->rx_rate=(ui->lineEdit_6->text().toDouble())*1.0e6;
        ((ALOHA_RX*)data_link_layer_rx)->device->rx_rate=(ui->lineEdit_6->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:CSMA"){

        ((CSMA_TX*)data_link_layer_tx)->device->rx_rate=(ui->lineEdit_6->text().toDouble())*1.0e6;
        ((CSMA_RX*)data_link_layer_rx)->device->rx_rate=(ui->lineEdit_6->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:TDMA TDD"){

        ((TDMA_TDD_TX*)data_link_layer_tx)->device->rx_rate=(ui->lineEdit_6->text().toDouble())*1.0e6;
        ((TDMA_TDD_RX*)data_link_layer_rx)->device->rx_rate=(ui->lineEdit_6->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:OFDMA TDD"){

        ((OFDMA_TDD_TX*)data_link_layer_tx)->device->rx_rate=(ui->lineEdit_6->text().toDouble())*1.0e6;
        ((OFDMA_TDD_RX*)data_link_layer_rx)->device->rx_rate=(ui->lineEdit_6->text().toDouble())*1.0e6;

    }
}

void MainWindow::on_lineEdit_7_returnPressed()
{
    if(store_text2=="L2:Point to Point TDD"){

        ((Point_to_Point_TDD_TX*)data_link_layer_tx)->device->rx_freq=(ui->lineEdit_7->text().toDouble())*1.0e6;
        ((Point_to_Point_TDD_RX*)data_link_layer_rx)->device->rx_freq=(ui->lineEdit_7->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:Point to Point FDD"){

        ((Point_to_Point_FDD_TX*)data_link_layer_tx)->device->rx_freq=(ui->lineEdit_7->text().toDouble())*1.0e6;
        ((Point_to_Point_FDD_RX*)data_link_layer_rx)->device->rx_freq=(ui->lineEdit_7->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:ALOHA"){

        ((ALOHA_TX*)data_link_layer_tx)->device->rx_freq=(ui->lineEdit_7->text().toDouble())*1.0e6;
        ((ALOHA_RX*)data_link_layer_rx)->device->rx_freq=(ui->lineEdit_7->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:CSMA"){

        ((CSMA_TX*)data_link_layer_tx)->device->rx_freq=(ui->lineEdit_7->text().toDouble())*1.0e6;
        ((CSMA_RX*)data_link_layer_rx)->device->rx_freq=(ui->lineEdit_7->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:TDMA TDD"){

        ((TDMA_TDD_TX*)data_link_layer_tx)->device->rx_freq=(ui->lineEdit_7->text().toDouble())*1.0e6;
        ((TDMA_TDD_RX*)data_link_layer_rx)->device->rx_freq=(ui->lineEdit_7->text().toDouble())*1.0e6;

    }
    if(store_text2=="L2:OFDMA TDD"){

        ((OFDMA_TDD_TX*)data_link_layer_tx)->device->rx_freq=(ui->lineEdit_7->text().toDouble())*1.0e6;
        ((OFDMA_TDD_RX*)data_link_layer_rx)->device->rx_freq=(ui->lineEdit_7->text().toDouble())*1.0e6;

    }
}

void MainWindow::on_lineEdit_8_returnPressed()
{
    if(store_text2=="L2:Point to Point TDD"){

        ((Point_to_Point_TDD_TX*)data_link_layer_tx)->device->rx_gain=ui->lineEdit_8->text().toDouble();
        ((Point_to_Point_TDD_RX*)data_link_layer_rx)->device->rx_gain=ui->lineEdit_8->text().toDouble();

    }
    if(store_text2=="L2:Point to Point FDD"){

        ((Point_to_Point_FDD_TX*)data_link_layer_tx)->device->rx_gain=ui->lineEdit_8->text().toDouble();
        ((Point_to_Point_FDD_RX*)data_link_layer_rx)->device->rx_gain=ui->lineEdit_8->text().toDouble();

    }
    if(store_text2=="L2:ALOHA"){

        ((ALOHA_TX*)data_link_layer_tx)->device->rx_gain=ui->lineEdit_8->text().toDouble();
        ((ALOHA_RX*)data_link_layer_rx)->device->rx_gain=ui->lineEdit_8->text().toDouble();

    }
    if(store_text2=="L2:CSMA"){

        ((CSMA_TX*)data_link_layer_tx)->device->rx_gain=ui->lineEdit_8->text().toDouble();
        ((CSMA_RX*)data_link_layer_rx)->device->rx_gain=ui->lineEdit_8->text().toDouble();

    }
    if(store_text2=="L2:TDMA TDD"){

        ((TDMA_TDD_TX*)data_link_layer_tx)->device->rx_gain=ui->lineEdit_8->text().toDouble();
        ((TDMA_TDD_RX*)data_link_layer_rx)->device->rx_gain=ui->lineEdit_8->text().toDouble();

    }
    if(store_text2=="L2:OFDMA TDD"){

        ((OFDMA_TDD_TX*)data_link_layer_tx)->device->rx_gain=ui->lineEdit_8->text().toDouble();
        ((OFDMA_TDD_RX*)data_link_layer_rx)->device->rx_gain=ui->lineEdit_8->text().toDouble();

    }
}

void MainWindow::on_lineEdit_9_returnPressed()
{
    if(store_text2=="L2:Point to Point TDD"){

        ((Point_to_Point_TDD_TX*)data_link_layer_tx)->myaddress=ui->lineEdit_9->text().toInt();
        ((Point_to_Point_TDD_RX*)data_link_layer_rx)->myaddress=ui->lineEdit_9->text().toInt();

    }
    if(store_text2=="L2:Point to Point FDD"){

        ((Point_to_Point_FDD_TX*)data_link_layer_tx)->myaddress=ui->lineEdit_9->text().toInt();
        ((Point_to_Point_FDD_RX*)data_link_layer_rx)->myaddress=ui->lineEdit_9->text().toInt();

    }
    if(store_text2=="L2:ALOHA"){

        ((ALOHA_TX*)data_link_layer_tx)->myaddress=ui->lineEdit_9->text().toInt();
        ((ALOHA_RX*)data_link_layer_rx)->myaddress=ui->lineEdit_9->text().toInt();

    }
    if(store_text2=="L2:CSMA"){

        ((CSMA_TX*)data_link_layer_tx)->myaddress=ui->lineEdit_9->text().toInt();
        ((CSMA_RX*)data_link_layer_rx)->myaddress=ui->lineEdit_9->text().toInt();

    }
    if(store_text2=="L2:TDMA TDD"){

        ((TDMA_TDD_TX*)data_link_layer_tx)->myaddress=ui->lineEdit_9->text().toInt();
        ((TDMA_TDD_RX*)data_link_layer_rx)->myaddress=ui->lineEdit_9->text().toInt();

    }
    if(store_text2=="L2:OFDMA TDD"){

        ((OFDMA_TDD_TX*)data_link_layer_tx)->myaddress=ui->lineEdit_9->text().toInt();
        ((OFDMA_TDD_RX*)data_link_layer_rx)->myaddress=ui->lineEdit_9->text().toInt();

    }
}

void MainWindow::on_lineEdit_10_returnPressed()
{
    if(store_text2=="L2:Point to Point TDD"){

        ((Point_to_Point_TDD_TX*)data_link_layer_tx)->destaddress=ui->lineEdit_10->text().toInt();
        ((Point_to_Point_TDD_RX*)data_link_layer_rx)->destaddress=ui->lineEdit_10->text().toInt();

    }
    if(store_text2=="L2:Point to Point FDD"){

        ((Point_to_Point_FDD_TX*)data_link_layer_tx)->destaddress=ui->lineEdit_10->text().toInt();
        ((Point_to_Point_FDD_RX*)data_link_layer_rx)->destaddress=ui->lineEdit_10->text().toInt();

    }
    if(store_text2=="L2:ALOHA"){

        ((ALOHA_TX*)data_link_layer_tx)->destaddress=ui->lineEdit_10->text().toInt();
        ((ALOHA_RX*)data_link_layer_rx)->destaddress=ui->lineEdit_10->text().toInt();

    }
    if(store_text2=="L2:CSMA"){

        ((CSMA_TX*)data_link_layer_tx)->destaddress=ui->lineEdit_10->text().toInt();
        ((CSMA_RX*)data_link_layer_rx)->destaddress=ui->lineEdit_10->text().toInt();

    }
    if(store_text2=="L2:TDMA TDD"){

        ((TDMA_TDD_TX*)data_link_layer_tx)->destaddress=ui->lineEdit_10->text().toInt();
        ((TDMA_TDD_RX*)data_link_layer_rx)->destaddress=ui->lineEdit_10->text().toInt();

    }
    if(store_text2=="L2:OFDMA TDD"){

        ((OFDMA_TDD_TX*)data_link_layer_tx)->destaddress=ui->lineEdit_10->text().toInt();
        ((OFDMA_TDD_RX*)data_link_layer_rx)->destaddress=ui->lineEdit_10->text().toInt();

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
