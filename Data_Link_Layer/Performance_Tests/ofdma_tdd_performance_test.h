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
#ifndef OFDMA_TDD_PERFORMANCE_TEST_H
#define OFDMA_TDD_PERFORMANCE_TEST_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include "Data_Link_Layer/ofdma_tdd_rx.h"
#include "Data_Link_Layer/ofdma_tdd_tx.h"

class OFDMA_TDD_Performance_Test: public QThread
{
public:
    OFDMA_TDD_Performance_Test(Ui_MainWindow *ui);
    int tun_alloc(char *dev);

private:
    vector<OFDMA_TDD_RX *> receivers;
    vector<OFDMA_TDD_TX *> transmitters;
    int number_transceivers;

protected:
    void run();
};

#endif // OFDMA_TDD_PERFORMANCE_TEST_H
