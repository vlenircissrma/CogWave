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
#include "BlindOFDM/blindofdm_tdd_mode_rx.h"
#include "BlindOFDM/blindofdm_tdd_mode_tx.h"
#include "DADS/dads_ber_test.h"
#include "DADS/dads_fdd_mode_tx.h"
#include "DADS/dads_fdd_mode_rx.h"
#include "waveform_rx.h"
#include "waveform_tx.h"

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

};

#endif // MAINWINDOW_H
