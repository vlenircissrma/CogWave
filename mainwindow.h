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
#include "Multimedia/video_tx.h"
#include "Multimedia/video_rx.h"
#include "Multimedia/audio_tx.h"
#include "Multimedia/audio_rx.h"
#include "Multimedia/text_tx.h"
#include "Multimedia/text_rx.h"
#include "Multimedia/plot.h"
#include "Common/waveform_rx.h"
#include "Common/waveform_tx.h"
#include "waveform_tdd_rx.h"
#include "waveform_tdd_tx.h"
#include "waveform_fdd_rx.h"
#include "waveform_fdd_tx.h"
#include "Modems/BER_Tests/dads_ber_test.h"
#include "Modems/BER_Tests/ofdma_ber_test.h"
#include "Modems/BER_Tests/bpsk_ber_test.h"
#include "Modems/BER_Tests/gmsk_ber_test.h"
#include "Modems/BER_Tests/qpsk_ber_test.h"
#include "Modems/BER_Tests/cpfsk_ber_test.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

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
    Waveform_RX *waveform_rx;
    Waveform_TX *waveform_tx;
    QString store_text;
    QString store_text2;


    //DADS_BER_Test *dads_ber_test;
    //BLINDOFDM_BER_Test *blindofdm_ber_test;
    //BPSK_BER_Test *bpsk_ber_test;
    //GMSK_BER_Test *gmsk_ber_test;
    //QPSK_BER_Test *qpsk_ber_test;
    //CPFSK_BER_Test *cpfsk_ber_test;

};

#endif // MAINWINDOW_H
