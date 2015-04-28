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
#ifndef ALOHA_PERFORMANCE_TEST_H
#define ALOHA_PERFORMANCE_TEST_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include "Data_Link_Layer/aloha_rx.h"
#include "Data_Link_Layer/aloha_tx.h"

class ALOHA_Performance_Test: public QThread
{
public:
    ALOHA_Performance_Test(Ui_MainWindow *ui);
    int tun_alloc(char *dev);

private:
    vector<ALOHA_RX *> receivers;
    vector<ALOHA_TX *> transmitters;
    int number_transceivers;

protected:
    void run();
};

#endif // ALOHA_PERFORMANCE_TEST_H
