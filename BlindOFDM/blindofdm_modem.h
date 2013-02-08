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

#ifndef BLINDOFDM_MODEM_H
#define BLINDOFDM_MODEM_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include "blindofdm_multimediaread.h"


class BlindOFDM_Modem
{
public:
    BlindOFDM_Modem();
    int Nfft;
    int Ncp;
    int num_subchannels;
    int Number_of_OFDM_symbols;
    int Modulation_Order;


    cvec modulate_mask_bpsk(bvec data_packet, int best_group);
    cvec modulate_mask_qpsk(bvec data_packet, int best_group);
    cvec modulate_mask_gray_16qam(bvec data_packet, int best_group);
    cvec modulate_mask_quadrant_16qam(bvec data_packet, int best_group);
    cvec modulate_mask_gray_64qam(bvec data_packet, int best_group);
    cvec modulate_mask_gray_256qam(bvec data_packet, int best_group);
    cvec equalizer_second_power(cvec rx_buff, int best_group, vec &estimated_channel);
    cvec equalizer_fourth_power(cvec rx_buff, int best_group, vec &estimated_channel);
    bvec demodulate_mask_gray_256qam(cvec demodulated_ofdm_symbols, int best_group);
    bvec demodulate_mask_gray_64qam(cvec demodulated_ofdm_symbols, int best_group);
    bvec demodulate_mask_quadrant_16qam(cvec demodulated_ofdm_symbols, int best_group);
    bvec demodulate_mask_gray_16qam(cvec demodulated_ofdm_symbols, int best_group);
    bvec demodulate_mask_gray_qpsk(cvec demodulated_ofdm_symbols, int best_group);
    bvec demodulate_mask_gray_bpsk(cvec demodulated_ofdm_symbols, int best_group);
    bool preamble_detection_bpsk(bvec received_bits,bvec &received_bits2);
    bool preamble_detection_qpsk(bvec received_bits,bvec &received_bits2);
    bool preamble_detection_gray_16qam(bvec received_bits,bvec &received_bits2);
    bool preamble_detection_quadrant_16qam(bvec received_bits,bvec &received_bits2);
    bool preamble_detection_gray_64qam(bvec received_bits,bvec &received_bits2);
    bool preamble_detection_gray_256qam(bvec received_bits,bvec &received_bits2);
    bvec bpsk_differential_coding(bvec data_packet);
    bvec bpsk_differential_decoding(bvec received_bits);
    bvec qpsk_differential_coding(bvec data_packet);
    bvec qpsk_differential_decoding(bvec received_bits);
    bvec qam16_gray_differential_coding(bvec data_packet);
    bvec qam16_gray_differential_decoding(bvec received_bits);
    bvec qam16_quadrant_differential_coding(bvec data_packet);
    bvec qam16_quadrant_differential_decoding(bvec received_bits);
    bvec charvec2bvec(vector<char> input);
    void compare_bvec(bvec received_bits,bvec transmitted_bits);
    bool detection(cvec rx_buff);
    bool detection(cvec rx_buff,double &metric);
    int time_offset_estimate;

private:


    double ampl;
    double frequency_offset_estimate;
    bvec transmitted_bits;
    cvec mapped_transmitted_symbols;
    int Number_of_symbols;
    int Number_of_transmitted_symbols;







};

#endif // BLINDOFDM_MODEM_H

