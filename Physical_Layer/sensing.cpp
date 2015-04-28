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

#include "sensing.h"

Sensing::Sensing(){


}

vec Sensing::spectrum_block(cvec rx_buff, int Nfft)
{

        vec spectrum_sensed;
        spectrum_sensed.set_size(Nfft);
        spectrum_sensed.zeros();


        for(int i=0;i<rx_buff.size()/Nfft;i++)
             spectrum_sensed=spectrum_sensed+1./Nfft*real(elem_mult(fft(rx_buff.get(i*Nfft,(i+1)*Nfft-1)),conj(fft(rx_buff.get(i*Nfft,(i+1)*Nfft-1)))));

        spectrum_sensed=1./(rx_buff.size()/Nfft)*spectrum_sensed;

    return spectrum_sensed;
}


int Sensing::best_group_mask(vec spectrum_sensed,int num_subchannels){


    vec integrate_sensing(num_subchannels);
    integrate_sensing.zeros();
    int best_group;
    int Nfft=spectrum_sensed.size();

    for(int i=0;i<num_subchannels;i++){
        ivec mask(Nfft);
        mask.zeros();
        if(num_subchannels==1)
            mask.replace_mid(Nfft/32,concat(ones_i(Nfft/2-3*Nfft/32),zeros_i(2*Nfft/16),ones_i(Nfft/2-3*Nfft/32)));
        else{
            if(i<num_subchannels/2)
                mask.replace_mid(Nfft/32+i*(Nfft-3*Nfft/16)/num_subchannels,ones_i((Nfft-3*Nfft/16)/num_subchannels));
            if(i>=num_subchannels/2)
                mask.replace_mid(Nfft/32+2*Nfft/16+i*(Nfft-3*Nfft/16)/num_subchannels,ones_i((Nfft-3*Nfft/16)/num_subchannels));
        }

        for(int j=0;j<Nfft;j++){
            if(mask[j]==1){
                integrate_sensing[i]=integrate_sensing[i]+spectrum_sensed[j];
            }

        }

    }

    best_group=min_index(integrate_sensing);

    return best_group;
}
