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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Application_Layer/video_tx.h"
#include "Application_Layer/video_rx.h"
#include "Application_Layer/audio_tx.h"
#include "Application_Layer/audio_rx.h"
#include "Application_Layer/text_tx.h"
#include "Application_Layer/text_rx.h"
#include "Application_Layer/plot.h"
#include "Data_Link_Layer/data_link_layer_rx.h"
#include "Data_Link_Layer/data_link_layer_tx.h"
#include "Data_Link_Layer/point_to_point_tdd_rx.h"
#include "Data_Link_Layer/point_to_point_tdd_tx.h"
#include "Data_Link_Layer/point_to_point_fdd_rx.h"
#include "Data_Link_Layer/point_to_point_fdd_tx.h"
#include "Data_Link_Layer/aloha_rx.h"
#include "Data_Link_Layer/aloha_tx.h"
#include "Data_Link_Layer/csma_rx.h"
#include "Data_Link_Layer/csma_tx.h"
#include "Data_Link_Layer/tdma_tdd_rx.h"
#include "Data_Link_Layer/tdma_tdd_tx.h"
#include "Data_Link_Layer/ofdma_tdd_rx.h"
#include "Data_Link_Layer/ofdma_tdd_tx.h"
#include "Data_Link_Layer/Performance_Tests/aloha_performance_test.h"
#include "Data_Link_Layer/Performance_Tests/csma_performance_test.h"
#include "Data_Link_Layer/Performance_Tests/ofdma_tdd_performance_test.h"
#include "Data_Link_Layer/Performance_Tests/point_to_point_fdd_performance_test.h"
#include "Data_Link_Layer/Performance_Tests/point_to_point_tdd_performance_test.h"
#include "Data_Link_Layer/Performance_Tests/tdma_tdd_performance_test.h"
#include "Physical_Layer/BER_Tests/dads_ber_test.h"
#include "Physical_Layer/BER_Tests/mcdaaofdm_ber_test.h"
#include "Physical_Layer/BER_Tests/bpsk_ber_test.h"
#include "Physical_Layer/BER_Tests/gmsk_ber_test.h"
#include "Physical_Layer/BER_Tests/qpsk_ber_test.h"
#include "Physical_Layer/BER_Tests/cpfsk_ber_test.h"
#include "Physical_Layer/BER_Tests/mcdads_ber_test.h"
#include "Physical_Layer/BER_Tests/mccdm_ber_test.h"
#include "Physical_Layer/BER_Tests/ofdm_ber_test.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int tun_alloc(char *dev);

public slots:
    void edit_text(QString line);
    void display(GstElement *video_sink, int window_display);
    void plotdata(vec ydata, int window);
    void plotdata(cvec ydata, int window);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_lineEdit_returnPressed();
    void on_lineEdit_2_returnPressed();
    void on_lineEdit_3_returnPressed();
    void on_lineEdit_4_returnPressed();
    void on_lineEdit_5_returnPressed();
    void on_lineEdit_6_returnPressed();
    void on_lineEdit_7_returnPressed();
    void on_lineEdit_8_returnPressed();
    void on_lineEdit_9_returnPressed();
    void on_lineEdit_10_returnPressed();
    void on_comboBox_activated(const QString &arg1);
    void on_comboBox_2_activated(const QString &arg1);

private:
    Ui::MainWindow *ui;
    Video_TX *video_tx;
    Video_RX *video_rx;
    Audio_TX *audio_tx;
    Audio_RX *audio_rx;
    Text_TX *text_tx;
    Text_RX *text_rx;
    Plot *plot;
    Data_Link_Layer_RX *data_link_layer_rx;
    Data_Link_Layer_TX *data_link_layer_tx;
    QString store_text;
    QString store_text2;


    //DADS_BER_Test *dads_ber_test;
    //MCDAAOFDM_BER_Test *mcdaaofdm_ber_test;
    //BPSK_BER_Test *bpsk_ber_test;
    //GMSK_BER_Test *gmsk_ber_test;
    //QPSK_BER_Test *qpsk_ber_test;
    //CPFSK_BER_Test *cpfsk_ber_test;
    //MCDADS_BER_Test *mcdads_ber_test;
    //MCCDM_BER_Test *mccdm_ber_test;
    //OFDM_BER_Test *ofdm_ber_test;

    ALOHA_Performance_Test *aloha_performance_test;
    CSMA_Performance_Test *csma_performance_test;
    OFDMA_TDD_Performance_Test *ofdma_tdd_performance_test;
    Point_to_Point_FDD_Performance_Test *point_to_point_fdd_performance_test;
    Point_to_Point_TDD_Performance_Test *point_to_point_tdd_performance_test;
    TDMA_TDD_Performance_Test *tdma_tdd_performance_test;

};

#endif // MAINWINDOW_H
