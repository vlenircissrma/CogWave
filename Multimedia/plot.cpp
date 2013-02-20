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

#include "plot.h"


Plot::Plot(Ui_MainWindow *ui,int Nfft){



    gui=ui;

    curve.attach(gui->qwtPlot);
    curve2.attach(gui->qwtPlot_2);


    gui->qwtPlot->setTitle("Spectrum");
    gui->qwtPlot->setAxisScale(QwtPlot::yLeft,-100,0,0);
    gui->qwtPlot->setAxisScale(QwtPlot::xBottom,1,Nfft,0);
    gui->qwtPlot->setAxisTitle(QwtPlot::yLeft, "Power (dB)");
    gui->qwtPlot->setAxisTitle(QwtPlot::xBottom,"Frequency bin");

    gui->qwtPlot_2->setTitle("Constellation");
    gui->qwtPlot_2->setAxisScale(QwtPlot::yLeft,-1.5,1.5,0);
    gui->qwtPlot_2->setAxisScale(QwtPlot::xBottom,-1.5,1.5,0);
    gui->qwtPlot_2->setAxisTitle(QwtPlot::yLeft, "Quadrature");
    gui->qwtPlot_2->setAxisTitle(QwtPlot::xBottom,"In-Phase");


    gui->qwtPlot->replot();
    gui->qwtPlot_2->replot();

}


void Plot::Plot_data(vec ydata, int plot_number){

    double x[ydata.length()];
    double y[ydata.length()];

    if(plot_number==1){
        vec shifted_ydata(ydata.length());
        shifted_ydata.zeros();
        shifted_ydata=concat(ydata.get(ydata.length()/2,ydata.length()-1),ydata.get(0,ydata.length()/2-1));
        for (int i=0;i<ydata.length();i++){
               x[i]=i+1;
               y[i]= dB(1.0e-14+shifted_ydata.get(i));
        }
        curve.setData(x,y,ydata.length());
        gui->qwtPlot->replot();
    }

}
void Plot::Plot_data(cvec ydata, int plot_number){

    double x[ydata.length()];
    double y[ydata.length()];
    if(plot_number==2){
        curve2.setStyle(QwtPlotCurve::CurveStyle(4));
        for (int i=0;i<ydata.length();i++){
            x[i]=real(ydata.get(i));
            y[i]=imag(ydata.get(i));
        }
            curve2.setData(x,y,ydata.length());
            gui->qwtPlot_2->replot();
    }
}
