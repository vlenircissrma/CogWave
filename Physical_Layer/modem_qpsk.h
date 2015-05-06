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
#ifndef MODEM_QPSK_H
#define MODEM_QPSK_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include "injector_complex.h"
#include "sniffer_complex.h"
#include <gnuradio/top_block.h>
#include <gnuradio/filter/pfb_arb_resampler_ccf.h>
#include <gnuradio/digital/mpsk_receiver_cc.h>
#include <gnuradio/digital/fll_band_edge_cc.h>
#include <gnuradio/digital/pfb_clock_sync_ccf.h>
#include <gnuradio/digital/costas_loop_cc.h>

class Modem_QPSK
{
public:
    Modem_QPSK();
    vec generate_rrc_taps(double gain,double sampling_freq,double symbol_rate,double alpha,int ntaps);
    cvec modulate(bvec data_packet);
    bvec demodulate(cvec rx_buff, cvec &out);
    cvec costas_mm(cvec received_samples);
    cvec costas_pfb(cvec received_samples);
    bool preamble_detection(bvec received_bits,bvec &received_bits2, int &preamble_start);
    bool ack_detection(bvec received_bits,bvec &received_bits2, int &preamble_start);
    bvec charvec2bvec(vector<char> input);
    int nb_bits;
    double roll_off;
    int OF;
    Root_Raised_Cosine<double> rrc;
    MA_Filter<double,double,double> shaper;
    Pulse_Shape<double,double,double> upsampled_shaper;
    gr::top_block_sptr tb;

};

#endif // MODEM_QPSK_H

