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
#ifndef BLINDOFDM_FEC_H
#define BLINDOFDM_FEC_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;

class BlindOFDM_FEC
{
public:
    BlindOFDM_FEC();
    bvec encode_packet(bvec diff_data_packet);
    bvec decode_packet(bvec received_bits);

    int rate;
private:
    Convolutional_Code code;
};

#endif // BLINDOFDM_FEC_H

