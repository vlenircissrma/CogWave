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
#ifndef DADS_MODEM_H
#define DADS_MODEM_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;

class DADS_Modem
{
public:
    DADS_Modem();
    cvec modulate(bvec data_packet);
    bvec demodulate(cvec rx_buff, int OF, int &time_offset);
    bool preamble_detection(bvec received_bits,bvec &received_bits2, int &preamble_start);
    bvec charvec2bvec(vector<char> input);
    int delay;
    int SF;
    int qf;
    int nb_bits;
    int time_offset_estimate;
private:
    int SF_pseudo;
    Spread_1d spread;

};

#endif // DADS_MODEM_H
