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
#ifndef TDMA_TDD_PERFORMANCE_TEST_H
#define TDMA_TDD_PERFORMANCE_TEST_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include "Data_Link_Layer/tdma_tdd_rx.h"
#include "Data_Link_Layer/tdma_tdd_tx.h"

class TDMA_TDD_Performance_Test: public QThread
{
public:
    TDMA_TDD_Performance_Test(Ui_MainWindow *ui);
    int tun_alloc(char *dev);

private:
    vector<TDMA_TDD_RX *> receivers;
    vector<TDMA_TDD_TX *> transmitters;
    int number_transceivers;

protected:
    void run();
};

#endif // TDMA_TDD_PERFORMANCE_TEST_H
