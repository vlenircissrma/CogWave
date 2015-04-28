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
#ifndef MODEM_MCDADS_H
#define MODEM_MCDADS_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;

class Modem_MCDADS
{
public:
    Modem_MCDADS();
    cvec modulate(bvec data_packet);
    bvec demodulate(cvec rx_buff, int OF, int &time_offset1, int &time_offset2, vec &received_symbols);
    bool preamble_detection(bvec received_bits,bvec &received_bits2, int &preamble_start);
    bool ack_detection(bvec received_bits,bvec &received_bits2, int &preamble_start);
    bvec charvec2bvec(vector<char> input);
    int delay;
    int SF;
    int nb_bits;
    int Nfft;
    int Ncp;
    ivec mask;
    int sum_mask;
    int num_subchannels;
private:
    int SF_pseudo;
    Spread_1d spread;


};

#endif // MODEM_MCDADS_H
