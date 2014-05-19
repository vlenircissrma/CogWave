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
#ifndef MODEM_GMSK_H
#define MODEM_GMSK_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;

class Modem_GMSK
{
public:
    Modem_GMSK();
    cvec modulate(bvec data_packet);
    bvec demodulate(cvec rx_buff, vec &out);
    bool preamble_detection(bvec received_bits,bvec &received_bits2, int &preamble_start);
    vec generate_gaussian_taps(unsigned samples_per_sym, unsigned L, double bt);
    vec mm_clock_recovery(vec received_samples);
    vec pfb_clock_recovery(vec received_samples);
    bvec charvec2bvec(vector<char> input);
    int nb_bits;
    int OF;
    double h_index;
    MA_Filter<double,double,double> shaper;
    Pulse_Shape<double,double,double> upsampled_shaper;

};

#endif // MODEM_GMSK_H
