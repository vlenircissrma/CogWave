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
#ifndef MODEM_CPFSK_H
#define MODEM_CPFSK_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include "injector_float.h"
#include "sniffer_float.h"
#include <gnuradio/top_block.h>
#include <gnuradio/digital/clock_recovery_mm_ff.h>
#include <gnuradio/digital/pfb_clock_sync_fff.h>

class Modem_CPFSK
{
public:
    Modem_CPFSK();
    cvec modulate(bvec data_packet);
    bvec demodulate(cvec rx_buff, vec &out);
    bool preamble_detection(bvec received_bits,bvec &received_bits2, int &preamble_start);
    bool ack_detection(bvec received_bits,bvec &received_bits2, int &preamble_start);
    vec generate_rectangular_taps(unsigned samples_per_sym, unsigned L);
    vec mm_clock_recovery(vec received_samples);
    vec pfb_clock_recovery(vec received_samples);
    bvec charvec2bvec(vector<char> input);
    int nb_bits;
    int OF;
    double h_index;
    Pulse_Shape<double,double,double> upsampled_shaper;
    gr::top_block_sptr tb;


};

#endif // MODEM_CPFSK_H
