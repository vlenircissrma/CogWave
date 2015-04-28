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
#ifndef MODEM_DSSS_H
#define MODEM_DSSS_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
#include <QVector>
using namespace std;
using namespace itpp;

class Modem_DSSS
{
public:
    Modem_DSSS();
    cvec modulate(bvec data_packet);
    bvec demodulate(cvec rx_buff,int &time_offset_estimate, cvec &constellation);
    bool preamble_detection(bvec received_bits,bvec &received_bits2, int &preamble_start);
    bool ack_detection(bvec received_bits,bvec &received_bits2, int &preamble_start);
    bvec charvec2bvec(vector<char> input);
    int SF;
    int nb_bits;
    int OF;
    double mse1;
    double mse2;
    double mse3;
    double mse4;
    double mse5;
    double mse6;
    double mse7;
    double mse8;
    double mse9;
    double mse10;
    double mse11;
    double mse12;
    double phi_true;

private :
    Spread_1d spread;
    vec vcode;



};

#endif // MODEM_DSSS_H
