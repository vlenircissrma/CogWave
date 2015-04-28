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
#ifndef CSMA_PERFORMANCE_TEST_H
#define CSMA_PERFORMANCE_TEST_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include "Data_Link_Layer/csma_rx.h"
#include "Data_Link_Layer/csma_tx.h"

class CSMA_Performance_Test: public QThread
{
public:
    CSMA_Performance_Test(Ui_MainWindow *ui);
    int tun_alloc(char *dev);

private:
    vector<CSMA_RX *> receivers;
    vector<CSMA_TX *> transmitters;
    int number_transceivers;

protected:
    void run();
};

#endif // CSMA_PERFORMANCE_TEST_H
