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

#ifndef MODEM_MCDAAOFDM_H
#define MODEM_MCDAAOFDM_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;


class Modem_MCDAAOFDM
{
public:
    Modem_MCDAAOFDM();
    int Nfft;
    int Ncp;
    int num_subchannels;
    int Number_of_OFDM_symbols;
    int Modulation_Order;
    int time_offset_estimate;
    double frequency_offset_estimate;


    cvec modulate_mask_qpsk(bvec data_packet, int best_group);
    cvec equalizer_fourth_power(cvec rx_buff, int best_group, vec &estimated_channel);
    bvec demodulate_mask_gray_qpsk(cvec demodulated_ofdm_symbols, int best_group, cvec &constellation);
    bool preamble_detection_qpsk(bvec received_bits,bvec &received_bits2, int &preamble_start);
    bool ack_detection_qpsk(bvec received_bits,bvec &received_bits2, int &preamble_start);
    bvec charvec2bvec(vector<char> input);
    bool detection(cvec rx_buff,double &metric);


};

#endif // Modem_MCDAAOFDM_H
