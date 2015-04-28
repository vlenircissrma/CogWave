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
#ifndef CHANNEL_MODELS_H
#define CHANNEL_MODELS_H
#include <itpp/itcomm.h>
using namespace itpp;
using namespace std;

struct timestamp_data{
    double vector_timestamp;
    cvec vector_data;
    int src_address;
    // assignment operator
    timestamp_data& operator=(const timestamp_data& a)
    {
        vector_timestamp=a.vector_timestamp;
        vector_data=a.vector_data;
        src_address=a.src_address;
        return *this;
    }
};


class Channel_Models
{
public:
    Channel_Models(double rate);
    cvec OTA_Transmission(cvec tx_buff,string channel_model, int src_address, int my_address);
    void calculate_distance_matrix(cvec positions);
    mat distance;
private:
    double pathlossexponent;
    double tx_power;
    double frequency;
    double speed;
    double los_power;
    double frequency_offset;

    vec COST_207_RA_profile;
    vec COST_207_TU_profile;
    vec COST_207_BU_profile;
    vec COST_207_HT_profile;

    vec ITU_VA_profile;
    vec ITU_VB_profile;
    vec ITU_PA_profile;
    vec ITU_PB_profile;

    vec Indoor_RA_profile;
    vec Indoor_RB_profile;
    vec Indoor_RC_profile;
    vec Indoor_OA_profile;
    vec Indoor_OB_profile;
    vec Indoor_OC_profile;
    vec Indoor_CA_profile;
    vec Indoor_CB_profile;
    vec Indoor_CC_profile;


    vec SUI1_profile;
    vec SUI2_profile;
    vec SUI3_profile;
    vec SUI4_profile;
    vec SUI5_profile;
    vec SUI6_profile;
};

#endif // CHANNEL_MODELS_H
