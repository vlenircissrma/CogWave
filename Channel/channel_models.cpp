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
#include "channel_models.h"

Channel_Models::Channel_Models(double rate)
{
    pathlossexponent=2.0;
    tx_power=23;//dBm
    frequency=400;//MegaHertz
    los_power=inv_dB(100);//dB
    speed=0;//km/h

    frequency_offset=speed*1000/3600*frequency/3.0e8/rate;
    double delay_interval=1./rate;
    double channel_power;
    int tap_number;
    double channel_norm;

    //COST_207_RA
    vec tmp_delay="0 0.1e-6 0.2e-6 0.3e-6 0.4e-6 0.5e-6";
    vec tmp_power="0 -4 -8 -12 -16 -20";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    COST_207_RA_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    COST_207_RA_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            COST_207_RA_profile[i]=COST_207_RA_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        COST_207_RA_profile[i]=sqrt(COST_207_RA_profile[i]);
    }
    //cout << COST_207_RA_profile << endl;

    //COST_207_TU
    tmp_delay="0 0.1e-6 0.2e-6 0.3e-6 0.4e-6 0.5e-6";
    tmp_power="-3 0 -2 -6 -8 -10";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    COST_207_TU_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    COST_207_TU_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            COST_207_TU_profile[i]=COST_207_TU_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        COST_207_TU_profile[i]=sqrt(COST_207_TU_profile[i]);
    }
    //cout << COST_207_TU_profile << endl;

    //COST_207_BU
    tmp_delay="0 0.4e-6 1.0e-6 1.6e-6 5.0e-6 6.6e-6";
    tmp_power="-3 0 -3 -5 -2 -4";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    COST_207_BU_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    COST_207_BU_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            COST_207_BU_profile[i]=COST_207_BU_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        COST_207_BU_profile[i]=sqrt(COST_207_BU_profile[i]);
    }
    //cout << COST_207_BU_profile << endl;

    //COST_207_HT
    tmp_delay="0 0.2e-6 0.4e-6 0.6e-6 15.0e-6 17.2e-6";
    tmp_power="0 -2 -4 -7 -6 -12";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    COST_207_HT_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    COST_207_HT_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            COST_207_HT_profile[i]=COST_207_HT_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        COST_207_HT_profile[i]=sqrt(COST_207_HT_profile[i]);
    }
    //cout << COST_207_HT_profile << endl;

    //ITU_VA
    tmp_delay="0 0.31e-6 0.71e-6 1.09e-6 1.73e-6 2.51e-6";
    tmp_power="0 -1 -9 -10 -15 -20";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    ITU_VA_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    ITU_VA_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            ITU_VA_profile[i]=ITU_VA_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        ITU_VA_profile[i]=sqrt(ITU_VA_profile[i]);
    }
    //cout << ITU_VA_profile << endl;

    //ITU_VB
    tmp_delay="0 0.3e-6 8.9e-6 12.9e-6 17.1e-6 20.0e-6";
    tmp_power="-2.5 0 12.8 -10 -25.2 -16";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    ITU_VB_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    ITU_VB_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            ITU_VB_profile[i]=ITU_VB_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        ITU_VB_profile[i]=sqrt(ITU_VB_profile[i]);
    }
    //cout << ITU_VB_profile << endl;

    //ITU_PA
    tmp_delay="0 0.11e-6 0.19e-6 0.41e-6";
    tmp_power="0 -9.7 -19.2 -22.8";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    ITU_PA_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    ITU_PA_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            ITU_PA_profile[i]=ITU_PA_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        ITU_PA_profile[i]=sqrt(ITU_PA_profile[i]);
    }
    //cout << ITU_PA_profile << endl;

    //ITU_PB
    tmp_delay="0 0.2e-6 0.8e-6 1.2e-6 2.3e-6 3.7e-6";
    tmp_power="0 -0.9 -4.9 -8 -7.8 -23.9";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    ITU_PB_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    ITU_PB_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            ITU_PB_profile[i]=ITU_PB_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        ITU_PB_profile[i]=sqrt(ITU_PB_profile[i]);
    }
    //cout << ITU_PB_profile << endl;

    //ITU_RA
    tmp_delay="0 0.05e-6 0.1e-6";
    tmp_power="0 -9.4 -18.9";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    Indoor_RA_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    Indoor_RA_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            Indoor_RA_profile[i]=Indoor_RA_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        Indoor_RA_profile[i]=sqrt(Indoor_RA_profile[i]);
    }
    //cout << Indoor_RA_profile << endl;

    //ITU_RB
    tmp_delay="0 0.05e-6 0.1e-6 0.15e-6 0.2e-6 0.25e-6 0.3e-6 0.35e-6";
    tmp_power="0 -2.9 -5.8 -8.7 -11.6 -14.5 -17.4 -20.3";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    Indoor_RB_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    Indoor_RB_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            Indoor_RB_profile[i]=Indoor_RB_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        Indoor_RB_profile[i]=sqrt(Indoor_RB_profile[i]);
    }
    //cout <<  Indoor_RB_profile << endl;

    //ITU_RC
    tmp_delay="0 0.05e-6 0.15e-6 0.225e-6 0.4e-6 0.525e-6 0.75e-6";
    tmp_power="-4.6 0 -4.3 -6.5 -3 -15.2 -21.7";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    Indoor_RC_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    Indoor_RC_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            Indoor_RC_profile[i]=Indoor_RC_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        Indoor_RC_profile[i]=sqrt(Indoor_RC_profile[i]);
    }
    //cout << Indoor_RC_profile << endl;

    //ITU_OA
    tmp_delay="0 0.05e-6 0.1e-6";
    tmp_power="0 -3.6 -7.2";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    Indoor_OA_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    Indoor_OA_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            Indoor_OA_profile[i]=Indoor_OA_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        Indoor_OA_profile[i]=sqrt(Indoor_OA_profile[i]);
    }
    //cout << Indoor_OA_profile << endl;

    //ITU_OB
    tmp_delay="0 0.05e-6 0.15e-6 0.325e-6 0.55e-6 0.7e-6";
    tmp_power="0 -1.6 -4.7 -10.1 -17.1 -21.7";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    Indoor_OB_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    Indoor_OB_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            Indoor_OB_profile[i]=Indoor_OB_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        Indoor_OB_profile[i]=sqrt(Indoor_OB_profile[i]);
    }
    //cout << Indoor_OB_profile << endl;

    //ITU_OC
    tmp_delay="0 0.1e-6 0.15e-6 0.5e-6 0.55e-6 1.125e-6 1.65e-6 2.375e-6";
    tmp_power="0 -0.9 -1.4 -2.6 -5 -1.2 -10 -21.7";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    Indoor_OC_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    Indoor_OC_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            Indoor_OC_profile[i]=Indoor_OC_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        Indoor_OC_profile[i]=sqrt(Indoor_OC_profile[i]);
    }
    //cout << Indoor_OC_profile << endl;

    //ITU_CA
    tmp_delay="0 0.05e-6 0.1e-6 0.15e-6 0.2e-6";
    tmp_power="0 -2.9 -5.8 -8.7 -11.6";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    Indoor_CA_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    Indoor_CA_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            Indoor_CA_profile[i]=Indoor_CA_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        Indoor_CA_profile[i]=sqrt(Indoor_CA_profile[i]);
    }
    //cout << Indoor_CA_profile << endl;

    //ITU_CB
    tmp_delay="0 0.05e-6 0.15e-6 0.225e-6 0.4e-6 0.525e-6 0.75e-6";
    tmp_power="-4.6 0 -4.3 -6.5 -3 -15.2 -21.7";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    Indoor_CB_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    Indoor_CB_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            Indoor_CB_profile[i]=Indoor_CB_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        Indoor_CB_profile[i]=sqrt(Indoor_CB_profile[i]);
    }
    //cout << Indoor_CB_profile << endl;

    //ITU_CC
    tmp_delay="0 0.05e-6 0.25e-6 0.3e-6 0.55e-6 0.8e-6 2.05e-6 2.675e-6";
    tmp_power="0 -0.4 -6 -2.5 -4.5 -1.2 -17 -10";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    Indoor_CC_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    Indoor_CC_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            Indoor_CC_profile[i]=Indoor_CC_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        Indoor_CC_profile[i]=sqrt(Indoor_CC_profile[i]);
    }
    //cout << Indoor_CC_profile << endl;

    //SUI1
    tmp_delay="0 0.4e-6 0.9e-6";
    tmp_power="0 -15 -20";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    SUI1_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    SUI1_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            SUI1_profile[i]=SUI1_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        SUI1_profile[i]=sqrt(SUI1_profile[i]);
    }
    //cout << SUI1_profile << endl;

    //SUI2
    tmp_delay="0 0.4e-6 1.1e-6";
    tmp_power="0 -12 -15";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    SUI2_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    SUI2_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            SUI2_profile[i]=SUI2_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        SUI2_profile[i]=sqrt(SUI2_profile[i]);
    }
    //cout << SUI2_profile << endl;

    //SUI3
    tmp_delay="0 0.4e-6 0.9e-6";
    tmp_power="0 -5 -10";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    SUI3_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    SUI3_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            SUI3_profile[i]=SUI3_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        SUI3_profile[i]=sqrt(SUI3_profile[i]);
    }
    //cout << SUI3_profile << endl;

    //SUI4
    tmp_delay="0 1.5e-6 4.0e-6";
    tmp_power="0 -4 -8";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    SUI4_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    SUI4_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            SUI4_profile[i]=SUI4_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        SUI4_profile[i]=sqrt(SUI4_profile[i]);
    }
    //cout << SUI4_profile << endl;

    //SUI5
    tmp_delay="0 4.0e-6 10.0e-6";
    tmp_power="0 -5 -10";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    SUI5_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    SUI5_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            SUI5_profile[i]=SUI5_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        SUI5_profile[i]=sqrt(SUI5_profile[i]);
    }
    //cout << SUI5_profile << endl;

    //SUI6
    tmp_delay="0 14.0e-6 20.0e-6";
    tmp_power="0 -10 -14";
    channel_power=0;
    for(int i=0;i<tmp_power.size();i++){
        channel_power=channel_power+inv_dB(tmp_power[i]);
    }
    channel_norm=1./channel_power;
    SUI6_profile.set_size(ceil(tmp_delay[tmp_delay.size()-1]/delay_interval));
    SUI6_profile.zeros();
    tap_number=0;
    for(int i=0;i<ceil(tmp_delay[tmp_delay.size()-1]/delay_interval);i++){
        while((i*delay_interval<=tmp_delay[tap_number])&&(tmp_delay[tap_number]<=(i+1)*delay_interval)&&(tap_number<tmp_delay.size())){
            SUI6_profile[i]=SUI6_profile[i]+channel_norm*inv_dB(tmp_power[tap_number]);
            tap_number=tap_number+1;
        }
        SUI6_profile[i]=sqrt(SUI6_profile[i]);
    }
    //cout << SUI6_profile << endl;


}

void Channel_Models::calculate_distance_matrix(cvec positions){
    distance.set_size(positions.size(),positions.size());
    for(int i=0;i<positions.size();i++){
        for(int j=0;j<positions.size();j++){
            if(i==j)
                distance.set(i,j,0.001);
            else
                distance.set(i,j,sqrt((real(positions(i))-real(positions(j)))*(real(positions(i))-real(positions(j)))+(imag(positions(i))-imag(positions(j)))*(imag(positions(i))-imag(positions(j)))));
        }


    }
    cout << distance << endl;
}

cvec Channel_Models::OTA_Transmission(cvec tx_buff,string channel_model,int src_address, int my_address){

cvec rx_buff;
double Pr_dbm=tx_power-(32.45+20*log10(frequency)+10*pathlossexponent*log10(distance.get(src_address,my_address)));
double Pr=pow(10,Pr_dbm/10-3);

    if(channel_model=="PathLossOnly"){
        rx_buff=sqrt(Pr)*tx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="COST_207_RA"){
        for (int k=0;k<COST_207_RA_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,COST_207_RA_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(COST_207_RA_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="COST_207_TU"){
        for (int k=0;k<COST_207_TU_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,COST_207_TU_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(COST_207_TU_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="COST_207_BU"){
        for (int k=0;k<COST_207_BU_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,COST_207_BU_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(COST_207_BU_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="COST_207_HT"){
        for (int k=0;k<COST_207_HT_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,COST_207_HT_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(COST_207_HT_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="ITU_VA"){
        for (int k=0;k<ITU_VA_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,ITU_VA_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(ITU_VA_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="ITU_VB"){
        for (int k=0;k<COST_207_TU_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,ITU_VB_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(ITU_VB_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="ITU_PA"){
        for (int k=0;k<ITU_PA_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,ITU_PA_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(ITU_PA_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="ITU_PB"){
        for (int k=0;k<ITU_PB_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,ITU_PB_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(ITU_PB_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="Indoor_RA"){
        for (int k=0;k<Indoor_RA_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,Indoor_RA_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(Indoor_RA_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="Indoor_RB"){
        for (int k=0;k<Indoor_RB_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,Indoor_RB_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(Indoor_RB_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="Indoor_RC"){
        for (int k=0;k<Indoor_RC_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,Indoor_RC_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(Indoor_RC_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="Indoor_OA"){
        for (int k=0;k<Indoor_OA_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,Indoor_OA_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(Indoor_OA_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="Indoor_OB"){
        for (int k=0;k<Indoor_OB_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,Indoor_OB_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(Indoor_OB_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="Indoor_OC"){
        for (int k=0;k<Indoor_OC_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,Indoor_OC_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(Indoor_OC_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="Indoor_CA"){
        for (int k=0;k<Indoor_CA_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,Indoor_CA_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(Indoor_CA_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="Indoor_CB"){
        for (int k=0;k<Indoor_CB_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,Indoor_CB_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(Indoor_CB_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="Indoor_CC"){
        for (int k=0;k<Indoor_CC_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,Indoor_CC_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(Indoor_CC_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="SUI1"){
        for (int k=0;k<SUI1_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,SUI1_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(SUI1_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="SUI2"){
        for (int k=0;k<SUI2_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,SUI2_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(SUI2_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="SUI3"){
        for (int k=0;k<SUI3_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,SUI3_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(SUI3_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="SUI4"){
        for (int k=0;k<SUI4_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,SUI4_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(SUI4_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="SUI5"){
        for (int k=0;k<SUI5_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,SUI5_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(SUI5_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
    if(channel_model=="SUI6"){
        for (int k=0;k<SUI6_profile.size();k++){
            rx_buff+=concat(zeros_c(k),elem_mult(tx_buff,SUI6_profile[k]*(sqrt(los_power/(1+los_power))*ones_c(tx_buff.size())+sqrt(1/(1+los_power))*randn_c()*ones(tx_buff.size()))),zeros_c(SUI6_profile.size()-1-k));
        }
        rx_buff=sqrt(Pr)*rx_buff;
        for(int k=0;k<rx_buff.size();k++)
            rx_buff(k)=rx_buff(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
        return rx_buff;
    }
}
