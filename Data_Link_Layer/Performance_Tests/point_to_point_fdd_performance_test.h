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
#ifndef POINT_TO_POINT_FDD_PERFORMANCE_TEST_H
#define POINT_TO_POINT_FDD_PERFORMANCE_TEST_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include "Data_Link_Layer/point_to_point_fdd_rx.h"
#include "Data_Link_Layer/point_to_point_fdd_tx.h"

class Point_to_Point_FDD_Performance_Test: public QThread
{
public:
    Point_to_Point_FDD_Performance_Test(Ui_MainWindow *ui);
    int tun_alloc(char *dev);

private:
    vector<Point_to_Point_FDD_RX *> receivers;
    vector<Point_to_Point_FDD_TX *> transmitters;
    int number_transceivers;

protected:
    void run();
};

#endif // POINT_TO_POINT_FDD_PERFORMANCE_TEST_H
