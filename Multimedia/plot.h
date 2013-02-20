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

#ifndef PLOT_H
#define PLOT_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include "qwt_plot_curve.h"
#include "ui_mainwindow.h"

class Plot
{
public:
    Plot(Ui_MainWindow *ui,int Nfft);
    void Plot_data(vec ydata, int window);
    void Plot_data(cvec ydata, int window);

private:
    QwtPlotCurve curve;
    QwtPlotCurve curve2;
    Ui_MainWindow *gui;

};

#endif // PLOT_H
