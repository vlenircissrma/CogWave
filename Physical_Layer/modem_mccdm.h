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
#ifndef MODEM_MCCDM_H
#define MODEM_MCCDM_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;

class Modem_MCCDM
{
public:
    Modem_MCCDM();
    cvec modulate(bvec data_packet);
    bvec demodulate(cvec rx_buff, int &ofdm_time_offset_estimate, cvec &received_symbols);
    bool preamble_detection(bvec received_bits,bvec &received_bits2, int &preamble_start);
    bool ack_detection(bvec received_bits,bvec &received_bits2, int &preamble_start);
    bvec charvec2bvec(vector<char> input);
    int nb_bits;
    int Nfft;
    int Ncp;
    int Nu;
private:
    mat hadamard_matrix;
    vec scrambling;
    cvec differential_tmp;


};

#endif // MODEM_MCCDM_H

