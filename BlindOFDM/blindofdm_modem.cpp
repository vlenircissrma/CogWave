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

#include "blindofdm_modem.h"

BlindOFDM_Modem::BlindOFDM_Modem()
{
    Nfft=512;
    Ncp=128;
    num_subchannels=4;
    Number_of_OFDM_symbols=160;
    Modulation_Order=2;
    time_offset_estimate=0;

}

cvec BlindOFDM_Modem::modulate_mask_bpsk(bvec data_packet, int best_group)
{

    ivec mask(Nfft);
    mask.zeros();
    if(num_subchannels==1)
        mask.replace_mid(Nfft/32,concat(ones_i(Nfft/2-3*Nfft/32),zeros_i(2*Nfft/16),ones_i(Nfft/2-3*Nfft/32)));
    else{
        if(best_group<num_subchannels/2)
            mask.replace_mid(Nfft/32+best_group*(Nfft-3*Nfft/16)/num_subchannels,ones_i((Nfft-3*Nfft/16)/num_subchannels));
        if(best_group>=num_subchannels/2)
            mask.replace_mid(Nfft/32+2*Nfft/16+best_group*(Nfft-3*Nfft/16)/num_subchannels,ones_i((Nfft-3*Nfft/16)/num_subchannels));
    }

    int sum_mask=(Nfft-3*Nfft/16)/num_subchannels;


    BPSK bpsk;
    int packet_size_frequency_domain;
    if((data_packet.size()/sum_mask)*sum_mask!=data_packet.size())
        packet_size_frequency_domain=(data_packet.size()/sum_mask+1)*Nfft;
    else
        packet_size_frequency_domain=(data_packet.size()/sum_mask)*Nfft;
    cvec frequency_packet(packet_size_frequency_domain);
    frequency_packet.zeros();
    int number_blocks_frequency_domain;
    if((packet_size_frequency_domain/Nfft/num_subchannels)*num_subchannels!=packet_size_frequency_domain/Nfft)
        number_blocks_frequency_domain=packet_size_frequency_domain/Nfft/num_subchannels+1;
    else
        number_blocks_frequency_domain=packet_size_frequency_domain/Nfft/num_subchannels;
    int index=0;
    for(int i=0;i<number_blocks_frequency_domain;i++){
      for(int j=0;j<num_subchannels;j++){
        for(int k=0;k<Nfft;k++){
          if(mask[k]==1){
            //BPSK mapping
              if(index<data_packet.size()){
                frequency_packet.replace_mid(i*num_subchannels*Nfft+j*Nfft+k,to_cvec(bpsk.modulate_bits(data_packet.get(index,index))));
                index=index+mask[k];
              }
          }
        }
      }
    }
    //cout << " DATA PACKET SIZE " << data_packet.size() << endl;
    //cout << " PACKET SIZE FREQUENCY DOMAIN " <<  packet_size_frequency_domain << endl;
    //cout << " INDEX " << index << endl;


    int packet_size_time_domain;
    packet_size_time_domain=packet_size_frequency_domain/Nfft*(Nfft+Ncp);
    cvec tx_buff;
    tx_buff.set_length(packet_size_time_domain);
    tx_buff.zeros();
    //cout << " PACKET SIZE TIME DOMAIN " <<  packet_size_time_domain << endl;

    for(int i=0;i<packet_size_frequency_domain/Nfft;i++){
       tx_buff.replace_mid(i*(Nfft+Ncp),concat((ifft(frequency_packet.get(i*Nfft,(i+1)*Nfft-1))*Nfft/sqrt(Nfft+Ncp)).right(Ncp),ifft(frequency_packet.get(i*Nfft,(i+1)*Nfft-1))*Nfft/sqrt(Nfft+Ncp)));
    }

    return tx_buff;
}


cvec BlindOFDM_Modem::modulate_mask_qpsk(bvec data_packet, int best_group)
{

    ivec mask(Nfft);
    mask.zeros();
    if(num_subchannels==1)
        mask.replace_mid(Nfft/32,concat(2*ones_i(Nfft/2-3*Nfft/32),zeros_i(2*Nfft/16),2*ones_i(Nfft/2-3*Nfft/32)));
    else{
        if(best_group<num_subchannels/2)
            mask.replace_mid(Nfft/32+best_group*(Nfft-3*Nfft/16)/num_subchannels,2*ones_i((Nfft-3*Nfft/16)/num_subchannels));
        if(best_group>=num_subchannels/2)
            mask.replace_mid(Nfft/32+2*Nfft/16+best_group*(Nfft-3*Nfft/16)/num_subchannels,2*ones_i((Nfft-3*Nfft/16)/num_subchannels));
    }

    int sum_mask=sum(mask);

    QPSK qpsk;
    int packet_size_frequency_domain;
    if((data_packet.size()/sum_mask)*sum_mask!=data_packet.size())
        packet_size_frequency_domain=(data_packet.size()/sum_mask+1)*Nfft;
    else
        packet_size_frequency_domain=(data_packet.size()/sum_mask)*Nfft;
    cvec frequency_packet(packet_size_frequency_domain);
    frequency_packet.zeros();
    int number_blocks_frequency_domain;
    if((packet_size_frequency_domain/Nfft/num_subchannels)*num_subchannels!=packet_size_frequency_domain/Nfft)
        number_blocks_frequency_domain=packet_size_frequency_domain/Nfft/num_subchannels+1;
    else
        number_blocks_frequency_domain=packet_size_frequency_domain/Nfft/num_subchannels;
    int index=0;
    for(int i=0;i<number_blocks_frequency_domain;i++){
      for(int j=0;j<num_subchannels;j++){
        for(int k=0;k<Nfft;k++){
          if(mask[k]>0){
            //QPSK mapping
              if(index<data_packet.size()){
                  frequency_packet.replace_mid(i*num_subchannels*Nfft+j*Nfft+k,to_cvec(qpsk.modulate_bits(data_packet.get(index,index+mask[k]-1))));
                index=index+mask[k];
              }
          }
        }
      }
    }
    //cout << " DATA PACKET SIZE " << data_packet.size() << endl;
    //cout << " PACKET SIZE FREQUENCY DOMAIN " <<  packet_size_frequency_domain << endl;
    //cout << " INDEX " << index << endl;

    int packet_size_time_domain;
    packet_size_time_domain=packet_size_frequency_domain/Nfft*(Nfft+Ncp);
    cvec tx_buff;
    tx_buff.set_length(packet_size_time_domain);
    tx_buff.zeros();
    //cout << " PACKET SIZE TIME DOMAIN " <<  packet_size_time_domain << endl;

    for(int i=0;i<packet_size_frequency_domain/Nfft;i++){
       tx_buff.replace_mid(i*(Nfft+Ncp),concat((ifft(frequency_packet.get(i*Nfft,(i+1)*Nfft-1))*Nfft/sqrt(Nfft+Ncp)).right(Ncp),ifft(frequency_packet.get(i*Nfft,(i+1)*Nfft-1))*Nfft/sqrt(Nfft+Ncp)));
    }

    return tx_buff;
}

cvec BlindOFDM_Modem::modulate_mask_gray_16qam(bvec data_packet, int best_group)
{

    ivec mask(Nfft);
    mask.zeros();
    if(num_subchannels==1)
        mask.replace_mid(Nfft/32,concat(4*ones_i(Nfft/2-3*Nfft/32),zeros_i(2*Nfft/16),4*ones_i(Nfft/2-3*Nfft/32)));
    else{
        if(best_group<num_subchannels/2)
            mask.replace_mid(Nfft/32+best_group*(Nfft-3*Nfft/16)/num_subchannels,4*ones_i((Nfft-3*Nfft/16)/num_subchannels));
        if(best_group>=num_subchannels/2)
            mask.replace_mid(Nfft/32+2*Nfft/16+best_group*(Nfft-3*Nfft/16)/num_subchannels,4*ones_i((Nfft-3*Nfft/16)/num_subchannels));
    }

    int sum_mask=sum(mask);

    QAM qam;
    qam.set_M(16);

    int packet_size_frequency_domain;
    if((data_packet.size()/sum_mask)*sum_mask!=data_packet.size())
        packet_size_frequency_domain=(data_packet.size()/sum_mask+1)*Nfft;
    else
        packet_size_frequency_domain=(data_packet.size()/sum_mask)*Nfft;
    cvec frequency_packet(packet_size_frequency_domain);
    frequency_packet.zeros();
    int number_blocks_frequency_domain;
    if((packet_size_frequency_domain/Nfft/num_subchannels)*num_subchannels!=packet_size_frequency_domain/Nfft)
        number_blocks_frequency_domain=packet_size_frequency_domain/Nfft/num_subchannels+1;
    else
        number_blocks_frequency_domain=packet_size_frequency_domain/Nfft/num_subchannels;
    int index=0;
    for(int i=0;i<number_blocks_frequency_domain;i++){
      for(int j=0;j<num_subchannels;j++){
        for(int k=0;k<Nfft;k++){
                if(mask[k]>0){
                    //16QAM mapping
                    if(index<data_packet.size()){
                            frequency_packet.replace_mid(i*num_subchannels*Nfft+j*Nfft+k,to_cvec(qam.modulate_bits(data_packet.get(index,index+mask[k]-1)))*exp(std::complex<double>(0,-pi/4)));
                            index=index+mask[k];
                    }
                }
        }
      }
    }
    //cout << frequency_packet << endl;
    //cout << " DATA PACKET SIZE " << data_packet.size() << endl;
    //cout << " PACKET SIZE FREQUENCY DOMAIN " <<  packet_size_frequency_domain << endl;
    //cout << " INDEX " << index << endl;
    int packet_size_time_domain;
    packet_size_time_domain=packet_size_frequency_domain/Nfft*(Nfft+Ncp);
    cvec tx_buff;
    tx_buff.set_length(packet_size_time_domain);
    tx_buff.zeros();
    //cout << " PACKET SIZE TIME DOMAIN " <<  packet_size_time_domain << endl;

    for(int i=0;i<packet_size_frequency_domain/Nfft;i++){
       tx_buff.replace_mid(i*(Nfft+Ncp),concat((ifft(frequency_packet.get(i*Nfft,(i+1)*Nfft-1))*Nfft/sqrt(Nfft+Ncp)).right(Ncp),ifft(frequency_packet.get(i*Nfft,(i+1)*Nfft-1))*Nfft/sqrt(Nfft+Ncp)));
    }

    return tx_buff;
}

cvec BlindOFDM_Modem::modulate_mask_quadrant_16qam(bvec data_packet, int best_group)
{

    ivec mask(Nfft);
    mask.zeros();
    if(num_subchannels==1)
        mask.replace_mid(Nfft/32,concat(4*ones_i(Nfft/2-3*Nfft/32),zeros_i(2*Nfft/16),4*ones_i(Nfft/2-3*Nfft/32)));
    else{
        if(best_group<num_subchannels/2)
            mask.replace_mid(Nfft/32+best_group*(Nfft-3*Nfft/16)/num_subchannels,4*ones_i((Nfft-3*Nfft/16)/num_subchannels));
        if(best_group>=num_subchannels/2)
            mask.replace_mid(Nfft/32+2*Nfft/16+best_group*(Nfft-3*Nfft/16)/num_subchannels,4*ones_i((Nfft-3*Nfft/16)/num_subchannels));
    }

    int sum_mask=sum(mask);

    QAM qam;
    qam.set_M(16);
    ivec new_map = "15 14 9 11 13 12 8 10 6 4 0 1 7 5 2 3";
    qam.set(qam.get_symbols(),new_map);

    int packet_size_frequency_domain;
    if((data_packet.size()/sum_mask)*sum_mask!=data_packet.size())
        packet_size_frequency_domain=(data_packet.size()/sum_mask+1)*Nfft;
    else
        packet_size_frequency_domain=(data_packet.size()/sum_mask)*Nfft;
    cvec frequency_packet(packet_size_frequency_domain);
    frequency_packet.zeros();
    int number_blocks_frequency_domain;
    if((packet_size_frequency_domain/Nfft/num_subchannels)*num_subchannels!=packet_size_frequency_domain/Nfft)
        number_blocks_frequency_domain=packet_size_frequency_domain/Nfft/num_subchannels+1;
    else
        number_blocks_frequency_domain=packet_size_frequency_domain/Nfft/num_subchannels;
    int index=0;
    for(int i=0;i<number_blocks_frequency_domain;i++){
      for(int j=0;j<num_subchannels;j++){
        for(int k=0;k<Nfft;k++){
                if(mask[k]>0){
                    //16QAM mapping
                    if(index<data_packet.size()){
                        frequency_packet.replace_mid(i*num_subchannels*Nfft+j*Nfft+k,to_cvec(qam.modulate_bits(data_packet.get(index,index+mask[k]-1)))*exp(std::complex<double>(0,-pi/4)));
                            index=index+mask[k];
                    }
                }
        }
      }
    }
    //cout << frequency_packet << endl;
    //cout << " DATA PACKET SIZE " << data_packet.size() << endl;
    //cout << " PACKET SIZE FREQUENCY DOMAIN " <<  packet_size_frequency_domain << endl;
    //cout << " INDEX " << index << endl;
    int packet_size_time_domain;
    packet_size_time_domain=packet_size_frequency_domain/Nfft*(Nfft+Ncp);
    cvec tx_buff;
    tx_buff.set_length(packet_size_time_domain);
    tx_buff.zeros();
    //cout << " PACKET SIZE TIME DOMAIN " <<  packet_size_time_domain << endl;

    for(int i=0;i<packet_size_frequency_domain/Nfft;i++){
       tx_buff.replace_mid(i*(Nfft+Ncp),concat((ifft(frequency_packet.get(i*Nfft,(i+1)*Nfft-1))*Nfft/sqrt(Nfft+Ncp)).right(Ncp),ifft(frequency_packet.get(i*Nfft,(i+1)*Nfft-1))*Nfft/sqrt(Nfft+Ncp)));
    }

    return tx_buff;
}


cvec BlindOFDM_Modem::modulate_mask_gray_64qam(bvec data_packet, int best_group)
{

    ivec mask(Nfft);
    mask.zeros();
    if(num_subchannels==1)
        mask.replace_mid(Nfft/32,concat(6*ones_i(Nfft/2-3*Nfft/32),zeros_i(2*Nfft/16),6*ones_i(Nfft/2-3*Nfft/32)));
    else{
        if(best_group<num_subchannels/2)
            mask.replace_mid(Nfft/32+best_group*(Nfft-3*Nfft/16)/num_subchannels,6*ones_i((Nfft-3*Nfft/16)/num_subchannels));
        if(best_group>=num_subchannels/2)
            mask.replace_mid(Nfft/32+2*Nfft/16+best_group*(Nfft-3*Nfft/16)/num_subchannels,6*ones_i((Nfft-3*Nfft/16)/num_subchannels));
    }

    int sum_mask=sum(mask);

    QAM qam;
    qam.set_M(64);


    int packet_size_frequency_domain;
    if((data_packet.size()/sum_mask)*sum_mask!=data_packet.size())
        packet_size_frequency_domain=(data_packet.size()/sum_mask+1)*Nfft;
    else
        packet_size_frequency_domain=(data_packet.size()/sum_mask)*Nfft;
    cvec frequency_packet(packet_size_frequency_domain);
    frequency_packet.zeros();
    int number_blocks_frequency_domain;
    if((packet_size_frequency_domain/Nfft/num_subchannels)*num_subchannels!=packet_size_frequency_domain/Nfft)
        number_blocks_frequency_domain=packet_size_frequency_domain/Nfft/num_subchannels+1;
    else
        number_blocks_frequency_domain=packet_size_frequency_domain/Nfft/num_subchannels;
    int index=0;
    for(int i=0;i<number_blocks_frequency_domain;i++){
      for(int j=0;j<num_subchannels;j++){
        for(int k=0;k<Nfft;k++){
                if(mask[k]>0){
                    //64QAM mapping
                    if(index<data_packet.size()-mask[k]){
                            frequency_packet.replace_mid(i*num_subchannels*Nfft+j*Nfft+k,to_cvec(qam.modulate_bits(data_packet.get(index,index+mask[k]-1)))*exp(std::complex<double>(0,-pi/4)));
                            index=index+mask[k];
                    }
                }
        }
      }
    }
    //cout << frequency_packet << endl;
    //cout << " DATA PACKET SIZE " << data_packet.size() << endl;
    //cout << " PACKET SIZE FREQUENCY DOMAIN " <<  packet_size_frequency_domain << endl;
    //cout << " INDEX " << index << endl;
    int packet_size_time_domain;
    packet_size_time_domain=packet_size_frequency_domain/Nfft*(Nfft+Ncp);
    cvec tx_buff;
    tx_buff.set_length(packet_size_time_domain);
    tx_buff.zeros();
    //cout << " PACKET SIZE TIME DOMAIN " <<  packet_size_time_domain << endl;

    for(int i=0;i<packet_size_frequency_domain/Nfft;i++){
       tx_buff.replace_mid(i*(Nfft+Ncp),concat((ifft(frequency_packet.get(i*Nfft,(i+1)*Nfft-1))*Nfft/sqrt(Nfft+Ncp)).right(Ncp),ifft(frequency_packet.get(i*Nfft,(i+1)*Nfft-1))*Nfft/sqrt(Nfft+Ncp)));
    }

    return tx_buff;
}

cvec BlindOFDM_Modem::modulate_mask_gray_256qam(bvec data_packet, int best_group)
{

    ivec mask(Nfft);
    mask.zeros();
    if(num_subchannels==1)
        mask.replace_mid(Nfft/32,concat(8*ones_i(Nfft/2-3*Nfft/32),zeros_i(2*Nfft/16),8*ones_i(Nfft/2-3*Nfft/32)));
    else{
        if(best_group<num_subchannels/2)
            mask.replace_mid(Nfft/32+best_group*(Nfft-3*Nfft/16)/num_subchannels,8*ones_i((Nfft-3*Nfft/16)/num_subchannels));
        if(best_group>=num_subchannels/2)
            mask.replace_mid(Nfft/32+2*Nfft/16+best_group*(Nfft-3*Nfft/16)/num_subchannels,8*ones_i((Nfft-3*Nfft/16)/num_subchannels));
    }

    int sum_mask=sum(mask);

    QAM qam;
    qam.set_M(256);

    int packet_size_frequency_domain;
    if((data_packet.size()/sum_mask)*sum_mask!=data_packet.size())
        packet_size_frequency_domain=(data_packet.size()/sum_mask+1)*Nfft;
    else
        packet_size_frequency_domain=(data_packet.size()/sum_mask)*Nfft;
    cvec frequency_packet(packet_size_frequency_domain);
    frequency_packet.zeros();
    int number_blocks_frequency_domain;
    if((packet_size_frequency_domain/Nfft/num_subchannels)*num_subchannels!=packet_size_frequency_domain/Nfft)
        number_blocks_frequency_domain=packet_size_frequency_domain/Nfft/num_subchannels+1;
    else
        number_blocks_frequency_domain=packet_size_frequency_domain/Nfft/num_subchannels;
    int index=0;
    for(int i=0;i<number_blocks_frequency_domain;i++){
      for(int j=0;j<num_subchannels;j++){
        for(int k=0;k<Nfft;k++){
                if(mask[k]>0){
                    //256QAM mapping
                    if(index<data_packet.size()){
                            frequency_packet.replace_mid(i*num_subchannels*Nfft+j*Nfft+k,to_cvec(qam.modulate_bits(data_packet.get(index,index+mask[k]-1)))*exp(std::complex<double>(0,-pi/4)));
                            index=index+mask[k];
                    }
                }
        }
      }
    }
    //cout << frequency_packet << endl;
    //cout << " DATA PACKET SIZE " << data_packet.size() << endl;
    //cout << " PACKET SIZE FREQUENCY DOMAIN " <<  packet_size_frequency_domain << endl;
    //cout << " INDEX " << index << endl;
    int packet_size_time_domain;
    packet_size_time_domain=packet_size_frequency_domain/Nfft*(Nfft+Ncp);
    cvec tx_buff;
    tx_buff.set_length(packet_size_time_domain);
    tx_buff.zeros();
    //cout << " PACKET SIZE TIME DOMAIN " <<  packet_size_time_domain << endl;

    for(int i=0;i<packet_size_frequency_domain/Nfft;i++){
       tx_buff.replace_mid(i*(Nfft+Ncp),concat((ifft(frequency_packet.get(i*Nfft,(i+1)*Nfft-1))*Nfft/sqrt(Nfft+Ncp)).right(Ncp),ifft(frequency_packet.get(i*Nfft,(i+1)*Nfft-1))*Nfft/sqrt(Nfft+Ncp)));
    }

    return tx_buff;
}


bool BlindOFDM_Modem::detection(cvec rx_buff)
{

    //cout << "detection 1" << endl;


    int Number_of_received_symbols;
    Number_of_received_symbols=rx_buff.size();
    cvec non_shifted_rx_samples(Number_of_received_symbols-Nfft);
    non_shifted_rx_samples.zeros();
    cvec shifted_rx_samples(Number_of_received_symbols-Nfft);
    shifted_rx_samples.zeros();
    cvec correlation(Number_of_received_symbols-Nfft);
    correlation.zeros();
    vec time_offset(Nfft+Ncp);
    time_offset.zeros();
    cvec time_offset2(Nfft+Ncp);
    time_offset2.zeros();
    vec non_correlated_time_offset(Nfft-3*Ncp);
    non_correlated_time_offset.zeros();
    double total_variance;


    //cout << "detection 2" << endl;



    //rx_buff=randn_c(Number_of_received_symbols);

    non_shifted_rx_samples=rx_buff.get(0,Number_of_received_symbols-Nfft-1);
    shifted_rx_samples=rx_buff.get(Nfft,Number_of_received_symbols-1);

    if((Number_of_received_symbols-Nfft)/(Nfft+Ncp)<1)
        cout << "Not enough samples to determine if there is a cyclic prefix" << endl;

    //cout << "detection 3" << endl;
    time_offset.zeros();
    time_offset2.zeros();
    correlation.zeros();

    correlation=elem_mult(non_shifted_rx_samples,conj(shifted_rx_samples));
    total_variance=variance(rx_buff);

    //cout << "total variance " << total_variance << endl;

    for(int j=0;j<(Number_of_received_symbols-Nfft)/(Nfft+Ncp);j++){
        //Time offset estimation based on sliding window cyclic prefix
        time_offset2[0]=time_offset2[0]+sum(correlation.get(j*(Nfft+Ncp),j*(Nfft+Ncp)+Ncp-1));
    }
    //Modulus Time offset estimation based on sliding window cyclic prefix
    time_offset[0]=1./(Ncp*((Number_of_received_symbols-Nfft)/(Nfft+Ncp)))*sqrt(real(time_offset2[0]*conj(time_offset2[0])))/total_variance;
    //Real Part Time offset estimation based on sliding window cyclic prefix
    //time_offset[0]=1./(Ncp*((Number_of_received_symbols-Nfft)/(Nfft+Ncp)))*real(time_offset2[0])/total_variance;
    for(int i=1;i<Nfft+Ncp;i++){
           time_offset2[i]=time_offset2[i-1];
        for(int j=0;j<(Number_of_received_symbols-Nfft)/(Nfft+Ncp);j++){
           //Time offset estimation based on sliding window cyclic prefix
           time_offset2[i]=time_offset2[i]+correlation.get(j*(Nfft+Ncp)+Ncp-1+i)-correlation.get(j*(Nfft+Ncp)+i-1);

        }
        //Modulus Time offset estimation based on sliding window cyclic prefix
        time_offset[i]=1./(Ncp*((Number_of_received_symbols-Nfft)/(Nfft+Ncp)))*sqrt(real(time_offset2[i]*conj(time_offset2[i])))/total_variance;
        //Real Part Time offset estimation based on sliding window cyclic prefix
        //time_offset[i]=1./(Ncp*((Number_of_received_symbols-Nfft)/(Nfft+Ncp)))*real(time_offset2[i])/total_variance;
    }

    //cout << "detection 4" << endl;
    time_offset_estimate=max_index(time_offset);
    frequency_offset_estimate=-1./(2*pi*Nfft)*arg(time_offset2(time_offset_estimate));
    //cout << "Time offset estimate " << time_offset_estimate << endl;
    //cout << "Time offset FFT estimate " << mod(time_offset_estimate+Ncp,Nfft+Ncp) << endl;
    //cout << "Frequency offset estimate " << frequency_offset_estimate << endl;

    if(mod(time_offset_estimate+2*Ncp,Nfft+Ncp)<mod(time_offset_estimate+Nfft-Ncp-1,Nfft+Ncp)){
       non_correlated_time_offset=time_offset.get(mod(time_offset_estimate+2*Ncp,Nfft+Ncp),mod(time_offset_estimate+Nfft-Ncp-1,Nfft+Ncp));
       //cout << "ici" << endl;
       //cout << non_correlated_time_offset.length() << endl;
       //cout << "start" << mod(time_offset_estimate+2*Ncp,Nfft+Ncp) << endl;
       //cout << "end" << mod(time_offset_estimate+Nfft-Ncp-1,Nfft+Ncp) << endl;
    }
    else{
      non_correlated_time_offset=concat(time_offset.get(mod(time_offset_estimate+2*Ncp,Nfft+Ncp),Nfft+Ncp-1),time_offset.get(0,mod(time_offset_estimate+Nfft-Ncp-1,Nfft+Ncp)));
      //cout << "ou la" << endl;
      //cout << non_correlated_time_offset.length() << endl;
      //cout << "start" << mod(time_offset_estimate+2*Ncp,Nfft+Ncp) << endl;
      //cout << "end" << mod(time_offset_estimate+Nfft-Ncp-1,Nfft+Ncp) << endl;
    }

    double mean_detection=mean(non_correlated_time_offset);
    double variance_detection=variance(non_correlated_time_offset);


    //cout << "mean detection " << mean_detection << endl;
    //cout << "variance detection " << variance_detection << endl;

    //double mean_detection=mean(time_offset);
    //double variance_detection=variance(time_offset) ;

    //cout << "mean detection " << mean_detection << endl;
    //cout << "variance detection " << variance_detection << endl;


    //double pfa=0.1;
    //double threshold=sqrt(2*variance_detection)*erfinv(1-2*pfa)+mean_detection;
    //double threshold=sqrt(1./(Ncp*((Number_of_received_symbols-Nfft)/(Nfft+Ncp))))*erfinv(1-2*pfa);
    if(variance_detection<0)
        variance_detection=0;
    double threshold=sqrt(variance_detection)*40+mean_detection;

    //cout << 1./(2*Ncp*((Number_of_received_symbols-Nfft)/(Nfft+Ncp))) << endl;

    //cout << "Time offset value " << time_offset(time_offset_estimate) << endl;
    //cout << "Threshold "<< threshold << endl;

    if(time_offset(time_offset_estimate)>threshold){
        //cout << "OFDM detected" << endl;
        return true;

    }
    else{
        //cout << "No OFDM detected" << endl;
        return false;

    }

}


bool BlindOFDM_Modem::detection(cvec rx_buff,double &metric)
{

    //cout << "detection 1" << endl;


    int Number_of_received_symbols;
    Number_of_received_symbols=rx_buff.size();
    cvec non_shifted_rx_samples(Number_of_received_symbols-Nfft);
    non_shifted_rx_samples.zeros();
    cvec shifted_rx_samples(Number_of_received_symbols-Nfft);
    shifted_rx_samples.zeros();
    cvec correlation(Number_of_received_symbols-Nfft);
    correlation.zeros();
    vec time_offset(Nfft+Ncp);
    time_offset.zeros();
    cvec time_offset2(Nfft+Ncp);
    time_offset2.zeros();
    vec non_correlated_time_offset(Nfft-3*Ncp);
    non_correlated_time_offset.zeros();
    double total_variance;


    //cout << "detection 2" << endl;



    //rx_buff=randn_c(Number_of_received_symbols);

    non_shifted_rx_samples=rx_buff.get(0,Number_of_received_symbols-Nfft-1);
    shifted_rx_samples=rx_buff.get(Nfft,Number_of_received_symbols-1);

    if((Number_of_received_symbols-Nfft)/(Nfft+Ncp)<1)
        cout << "Not enough samples to determine if there is a cyclic prefix" << endl;

    //cout << "detection 3" << endl;
    time_offset.zeros();
    time_offset2.zeros();
    correlation.zeros();

    correlation=elem_mult(non_shifted_rx_samples,conj(shifted_rx_samples));
    total_variance=variance(rx_buff);

    //cout << "total variance " << total_variance << endl;

    for(int j=0;j<(Number_of_received_symbols-Nfft)/(Nfft+Ncp);j++){
        //Time offset estimation based on sliding window cyclic prefix
        time_offset2[0]=time_offset2[0]+sum(correlation.get(j*(Nfft+Ncp),j*(Nfft+Ncp)+Ncp-1));
    }
    //Modulus Time offset estimation based on sliding window cyclic prefix
    time_offset[0]=1./(Ncp*((Number_of_received_symbols-Nfft)/(Nfft+Ncp)))*sqrt(real(time_offset2[0]*conj(time_offset2[0])))/total_variance;
    //Real Part Time offset estimation based on sliding window cyclic prefix
    //time_offset[0]=1./(Ncp*((Number_of_received_symbols-Nfft)/(Nfft+Ncp)))*real(time_offset2[0])/total_variance;
    for(int i=1;i<Nfft+Ncp;i++){
           time_offset2[i]=time_offset2[i-1];
        for(int j=0;j<(Number_of_received_symbols-Nfft)/(Nfft+Ncp);j++){
           //Time offset estimation based on sliding window cyclic prefix
           time_offset2[i]=time_offset2[i]+correlation.get(j*(Nfft+Ncp)+Ncp-1+i)-correlation.get(j*(Nfft+Ncp)+i-1);

        }
        //Modulus Time offset estimation based on sliding window cyclic prefix
        time_offset[i]=1./(Ncp*((Number_of_received_symbols-Nfft)/(Nfft+Ncp)))*sqrt(real(time_offset2[i]*conj(time_offset2[i])))/total_variance;
        //Real Part Time offset estimation based on sliding window cyclic prefix
        //time_offset[i]=1./(Ncp*((Number_of_received_symbols-Nfft)/(Nfft+Ncp)))*real(time_offset2[i])/total_variance;
    }

    //cout << "detection 4" << endl;
    time_offset_estimate=max_index(time_offset);
    frequency_offset_estimate=-1./(2*pi*Nfft)*arg(time_offset2(time_offset_estimate));



    //cout << "Time offset estimate " << time_offset_estimate << endl;
    //cout << "Time offset FFT estimate " << mod(time_offset_estimate+Ncp,Nfft+Ncp) << endl;
    //cout << "Frequency offset estimate " << frequency_offset_estimate << endl;

    if(mod(time_offset_estimate+2*Ncp,Nfft+Ncp)<mod(time_offset_estimate+Nfft-Ncp-1,Nfft+Ncp)){
       non_correlated_time_offset=time_offset.get(mod(time_offset_estimate+2*Ncp,Nfft+Ncp),mod(time_offset_estimate+Nfft-Ncp-1,Nfft+Ncp));
       //cout << "ici" << endl;
       //cout << non_correlated_time_offset.length() << endl;
       //cout << "start" << mod(time_offset_estimate+2*Ncp,Nfft+Ncp) << endl;
       //cout << "end" << mod(time_offset_estimate+Nfft-Ncp-1,Nfft+Ncp) << endl;
    }
    else{
      non_correlated_time_offset=concat(time_offset.get(mod(time_offset_estimate+2*Ncp,Nfft+Ncp),Nfft+Ncp-1),time_offset.get(0,mod(time_offset_estimate+Nfft-Ncp-1,Nfft+Ncp)));
      //cout << "ou la" << endl;
      //cout << non_correlated_time_offset.length() << endl;
      //cout << "start" << mod(time_offset_estimate+2*Ncp,Nfft+Ncp) << endl;
      //cout << "end" << mod(time_offset_estimate+Nfft-Ncp-1,Nfft+Ncp) << endl;
    }

    double mean_detection=mean(non_correlated_time_offset);
    double variance_detection=variance(non_correlated_time_offset);


    //cout << "mean detection " << mean_detection << endl;
    //cout << "variance detection " << variance_detection << endl;

    //double mean_detection=mean(time_offset);
    //double variance_detection=variance(time_offset) ;

    //cout << "mean detection " << mean_detection << endl;
    //cout << "variance detection " << variance_detection << endl;


    //double pfa=0.1;
    //double threshold=sqrt(2*variance_detection)*erfinv(1-2*pfa)+mean_detection;
    //double threshold=sqrt(1./(Ncp*((Number_of_received_symbols-Nfft)/(Nfft+Ncp))))*erfinv(1-2*pfa);
    if(variance_detection<0)
        variance_detection=0;
    double threshold=sqrt(variance_detection)*40+mean_detection;

    //cout << 1./(2*Ncp*((Number_of_received_symbols-Nfft)/(Nfft+Ncp))) << endl;

    //cout << "Time offset value " << time_offset(time_offset_estimate) << endl;
    //cout << "Threshold "<< threshold << endl;

    metric=time_offset(time_offset_estimate)/threshold;

    if(time_offset(time_offset_estimate)>threshold){
        //cout << "OFDM detected" << endl;
        return true;

    }
    else{
        //cout << "No OFDM detected" << endl;
        return false;

    }

}



cvec BlindOFDM_Modem::equalizer_second_power(cvec rx_buff, int best_group, vec &estimated_channel)
{


    ivec mask(Nfft);
    mask.zeros();
    if(num_subchannels==1)
        mask.replace_mid(Nfft/32,concat(ones_i(Nfft/2-3*Nfft/32),zeros_i(2*Nfft/16),ones_i(Nfft/2-3*Nfft/32)));
    else{
        if(best_group<num_subchannels/2)
            mask.replace_mid(Nfft/32+best_group*(Nfft-3*Nfft/16)/num_subchannels,ones_i((Nfft-3*Nfft/16)/num_subchannels));
        if(best_group>=num_subchannels/2)
            mask.replace_mid(Nfft/32+2*Nfft/16+best_group*(Nfft-3*Nfft/16)/num_subchannels,ones_i((Nfft-3*Nfft/16)/num_subchannels));
    }

    int sum_mask=(Nfft-3*Nfft/16)/num_subchannels;

    int Number_of_received_symbols;
    Number_of_received_symbols=rx_buff.size();
    vec frequency_offset(Number_of_received_symbols);
    frequency_offset.zeros();
    cvec fo_rx_samples(Number_of_received_symbols);
    fo_rx_samples.zeros();
    vec phase_offset(Nfft);
    phase_offset.zeros();
    vec rotation_phase_offset(Nfft);
    rotation_phase_offset.zeros();
    estimated_channel.set_size(Nfft);
    estimated_channel.zeros();
    vec amplitude(Nfft);
    amplitude.zeros();

    for (int i=0;i<Number_of_received_symbols;i++)
        frequency_offset[i]=-2*pi*frequency_offset_estimate*i;
    fo_rx_samples=elem_mult(exp(to_cvec(zeros(Number_of_received_symbols),frequency_offset)),rx_buff);
    cvec to_fo_rx_samples=fo_rx_samples.get(mod(time_offset_estimate+Ncp,Nfft+Ncp),Number_of_received_symbols-1);

    int Number_of_blocks_time_domain=(to_fo_rx_samples.size()/(Nfft+Ncp));
    vec phase_offset_time(Number_of_blocks_time_domain);
    phase_offset_time.zeros();
    vec rotation_phase_offset_time(Number_of_blocks_time_domain);
    rotation_phase_offset_time.zeros();
    vec amplitude_time(Number_of_blocks_time_domain);
    amplitude_time.zeros();



    //Demodulation of the OFDM symbols
    cvec demodulated_ofdm_symbols(Number_of_blocks_time_domain*Nfft);
    demodulated_ofdm_symbols.zeros();
    for (int i=0;i<Number_of_blocks_time_domain;i++)
        demodulated_ofdm_symbols.replace_mid(i*Nfft,(fft(to_fo_rx_samples.get(i*(Nfft+Ncp),(i+1)*(Nfft+Ncp)-Ncp-1))/Nfft*sqrt(Nfft+Ncp)).get(0,Nfft-1));


    cmat matrix_demodulated(Number_of_blocks_time_domain,Nfft);
    matrix_demodulated.zeros();
    for(int i=0;i<Number_of_blocks_time_domain;i++){
        for(int j=0;j<Nfft;j++){
            if(mask[j]==1){
                matrix_demodulated(i,j)=demodulated_ofdm_symbols(i*(Nfft)+j);
            }
        }
    }

    for (int i=0;i<Nfft;i++){
        if(mask[i]==1){
            //BPSK phase offset estimation
            phase_offset[i]=1./2*arg(1./Number_of_blocks_time_domain*sum(elem_mult(matrix_demodulated.get_col(i),matrix_demodulated.get_col(i))));
            amplitude[i]=1./Number_of_blocks_time_domain*real(sum(elem_mult(matrix_demodulated.get_col(i),conj(matrix_demodulated.get_col(i)))));
        }
    }

     bool is_started=false;
     int previous_j=0;
     for(int j=Nfft/2;j<Nfft;j++){
         if(mask[j]==1){
             if(is_started==false)
                 rotation_phase_offset[j]=phase_offset[j];
             else{
                 if(abs(phase_offset[j]-phase_offset[previous_j])>pi/2)
                     rotation_phase_offset[j]=abs(phase_offset[j]-phase_offset[previous_j])-pi;
                 else
                     rotation_phase_offset[j]=phase_offset[j]-phase_offset[previous_j];
             }
             is_started=true;
             previous_j=j;
         }
     }
     for(int j=0;j<Nfft/2;j++){
         if(mask[j]==1){
             if(is_started==false)
                 rotation_phase_offset[j]=phase_offset[j];
             else{
                 if(abs(phase_offset[j]-phase_offset[previous_j])>pi/2)
                     rotation_phase_offset[j]=abs(phase_offset[j]-phase_offset[previous_j])-pi;
                 else
                     rotation_phase_offset[j]=phase_offset[j]-phase_offset[previous_j];
             }
             is_started=true;
             previous_j=j;
         }
     }

     is_started=false;
     previous_j=0;
     for (int j=Nfft/2;j<Nfft;j++)
     {
         if(mask[j]==1){
             if(is_started==false)
                 is_started=true;
             else
                 rotation_phase_offset[j]=rotation_phase_offset[j]+rotation_phase_offset[previous_j];
             previous_j=j;
         }
     }
     for (int j=0;j<Nfft/2;j++)
     {
         if(mask[j]==1){
             if(is_started==false)
                 is_started=true;
             else
                 rotation_phase_offset[j]=rotation_phase_offset[j]+rotation_phase_offset[previous_j];
             previous_j=j;
         }
     }


    // Correction of the rotation phase offset
    cvec phase_rotated_demodulated_ofdm_symbols(Number_of_blocks_time_domain*Nfft);
    phase_rotated_demodulated_ofdm_symbols.zeros();
    for (int i=0;i<Number_of_blocks_time_domain;i++)
        phase_rotated_demodulated_ofdm_symbols.replace_mid(i*(Nfft),elem_mult(demodulated_ofdm_symbols.get(i*(Nfft),(i+1)*Nfft-1),elem_mult(to_cvec(1./sqrt(amplitude)),exp(to_cvec(zeros(Nfft),-rotation_phase_offset)))));


    //Calculate if there is a remaining phase offset
    //Reorganization in a matrix
    for(int i=0;i<Number_of_blocks_time_domain;i++){
        for(int j=0;j<Nfft;j++){
            if(mask[j]==1){
                matrix_demodulated(i,j)=phase_rotated_demodulated_ofdm_symbols(i*Nfft+j);
            }
        }
    }

    for (int i=0;i<Nfft;i++){
        if(mask[i]==1){
       //BPSK phase offset estimation
       phase_offset[i]=1./2*arg(1./Number_of_blocks_time_domain*sum(elem_mult(matrix_demodulated.get_col(i),matrix_demodulated.get_col(i))));
       amplitude[i]=1./Number_of_blocks_time_domain*real(sum(elem_mult(matrix_demodulated.get_col(i),conj(matrix_demodulated.get_col(i)))));
        }
    }
    amplitude.ones();
    cvec phase_demodulated_ofdm_symbols(Number_of_blocks_time_domain*Nfft);
    phase_demodulated_ofdm_symbols.zeros();
    for (int i=0;i<Number_of_blocks_time_domain;i++)
        phase_demodulated_ofdm_symbols.replace_mid(i*Nfft,elem_mult(phase_rotated_demodulated_ofdm_symbols.get(i*Nfft,(i+1)*Nfft-1),elem_mult(to_cvec(1./sqrt(amplitude)),exp(to_cvec(zeros(Nfft),-phase_offset)))));



    matrix_demodulated.zeros();
    for(int i=0;i<Number_of_blocks_time_domain;i++){
        for(int j=0;j<Nfft;j++){
            if(mask[j]==1){
                matrix_demodulated(i,j)=phase_demodulated_ofdm_symbols(i*(Nfft)+j);
            }
        }
    }

    for (int i=0;i<Number_of_blocks_time_domain;i++){
       //BPSK phase offset estimation
       phase_offset_time[i]=1./2*arg(1./sum_mask*sum(elem_mult(matrix_demodulated.get_row(i),matrix_demodulated.get_row(i))));
       amplitude_time[i]=1./sum_mask*real(sum(elem_mult(matrix_demodulated.get_row(i),conj(matrix_demodulated.get_row(i)))));
    }
    amplitude_time.ones();


        is_started=false;
        int previous_i=0;

        for(int i=0;i<Number_of_blocks_time_domain;i++){
            if(is_started==false){
                rotation_phase_offset_time[i]=phase_offset_time[i];
                is_started=true;
            }
            else{
                if(abs(phase_offset_time[i]-phase_offset_time[previous_i])>pi/2)
                    rotation_phase_offset_time[i]=abs(phase_offset_time[i]-phase_offset_time[previous_i])-pi;
                else
                    rotation_phase_offset_time[i]=phase_offset_time[i]-phase_offset_time[previous_i];

                rotation_phase_offset_time[i]=rotation_phase_offset_time[i]+rotation_phase_offset_time[previous_i];
            }
            previous_i=i;
        }

        // Correction of the rotation phase offset in time
        phase_rotated_demodulated_ofdm_symbols.zeros();
        for (int i=0;i<Number_of_blocks_time_domain;i++)
            phase_rotated_demodulated_ofdm_symbols.replace_mid(i*(Nfft),phase_demodulated_ofdm_symbols.get(i*(Nfft),(i+1)*Nfft-1)*1./sqrt(amplitude_time[i])*exp(std::complex<double>(0,-rotation_phase_offset_time[i])));


        matrix_demodulated.zeros();
        for(int i=0;i<Number_of_blocks_time_domain;i++){
            for(int j=0;j<Nfft;j++){
                if(mask[j]==1){
                    matrix_demodulated(i,j)=phase_rotated_demodulated_ofdm_symbols(i*(Nfft)+j);
                }
            }
        }

        for (int i=0;i<Number_of_blocks_time_domain;i++){
           //BPSK phase offset estimation
           phase_offset_time[i]=1./2*arg(1./sum_mask*sum(elem_mult(matrix_demodulated.get_row(i),matrix_demodulated.get_row(i))));
           amplitude_time[i]=1./sum_mask*real(sum(elem_mult(matrix_demodulated.get_row(i),conj(matrix_demodulated.get_row(i)))));
        }
        amplitude_time.ones();
        demodulated_ofdm_symbols.zeros();
        for (int i=0;i<Number_of_blocks_time_domain;i++)
            demodulated_ofdm_symbols.replace_mid(i*(Nfft),phase_rotated_demodulated_ofdm_symbols.get(i*(Nfft),(i+1)*Nfft-1)*1./sqrt(amplitude_time[i])*exp(std::complex<double>(0,-phase_offset_time[i])));


    estimated_channel=amplitude;

    return demodulated_ofdm_symbols;


}

cvec BlindOFDM_Modem::equalizer_fourth_power(cvec rx_buff, int best_group, vec &estimated_channel)
{


    ivec mask(Nfft);
    mask.zeros();
    if(num_subchannels==1)
        mask.replace_mid(Nfft/32,concat(ones_i(Nfft/2-3*Nfft/32),zeros_i(2*Nfft/16),ones_i(Nfft/2-3*Nfft/32)));
    else{
        if(best_group<num_subchannels/2)
            mask.replace_mid(Nfft/32+best_group*(Nfft-3*Nfft/16)/num_subchannels,ones_i((Nfft-3*Nfft/16)/num_subchannels));
        if(best_group>=num_subchannels/2)
            mask.replace_mid(Nfft/32+2*Nfft/16+best_group*(Nfft-3*Nfft/16)/num_subchannels,ones_i((Nfft-3*Nfft/16)/num_subchannels));
    }

    int sum_mask=sum(mask);
    int Number_of_received_symbols;
    Number_of_received_symbols=rx_buff.size();
    vec frequency_offset(Number_of_received_symbols);
    frequency_offset.zeros();
    cvec fo_rx_samples(Number_of_received_symbols);
    fo_rx_samples.zeros();
    vec phase_offset(Nfft);
    phase_offset.zeros();
    vec rotation_phase_offset(Nfft);
    rotation_phase_offset.zeros();
    estimated_channel.set_size(Nfft);
    estimated_channel.zeros();
    vec amplitude(Nfft);
    amplitude.zeros();
    vec rotation_offset(Nfft);
    rotation_offset.zeros();


    // Correction of the frequency offset
    for (int i=0;i<Number_of_received_symbols;i++)
        frequency_offset[i]=-2*pi*frequency_offset_estimate*i;
    fo_rx_samples=elem_mult(exp(to_cvec(zeros(Number_of_received_symbols),frequency_offset)),rx_buff);
    cvec to_fo_rx_samples=fo_rx_samples.get(mod(time_offset_estimate+Ncp,Nfft+Ncp),Number_of_received_symbols-1);

    int Number_of_blocks_time_domain=(to_fo_rx_samples.size()/(Nfft+Ncp));
    vec phase_offset_time(Number_of_blocks_time_domain);
    phase_offset_time.zeros();
    vec rotation_phase_offset_time(Number_of_blocks_time_domain);
    rotation_phase_offset_time.zeros();
    vec amplitude_time(Number_of_blocks_time_domain);
    amplitude_time.zeros();

    //Demodulation of the OFDM symbols
    cvec demodulated_ofdm_symbols(Number_of_blocks_time_domain*Nfft);
    demodulated_ofdm_symbols.zeros();
    for (int i=0;i<Number_of_blocks_time_domain;i++)
        demodulated_ofdm_symbols.replace_mid(i*Nfft,(fft(to_fo_rx_samples.get(i*(Nfft+Ncp),(i+1)*(Nfft+Ncp)-Ncp-1))/Nfft*sqrt(Nfft+Ncp)).get(0,Nfft-1));


    //Reorganization in a matrix
    cmat matrix_demodulated(Number_of_blocks_time_domain,Nfft);
    matrix_demodulated.zeros();
    for(int i=0;i<Number_of_blocks_time_domain;i++){
        for(int j=0;j<Nfft;j++){
            if(mask[j]>0){
                matrix_demodulated(i,j)=demodulated_ofdm_symbols(i*(Nfft)+j);
            }
        }
    }
    //Calculate the phase and amplitude for each tone
    for (int j=0;j<Nfft;j++){
        if(mask[j]>0){
       //QPSK phase offset estimation
       phase_offset[j]=1./4*arg(1./Number_of_blocks_time_domain*sum(elem_mult(elem_mult(matrix_demodulated.get_col(j),matrix_demodulated.get_col(j)),elem_mult(matrix_demodulated.get_col(j),matrix_demodulated.get_col(j)))));
       amplitude[j]=1./Number_of_blocks_time_domain*real(sum(elem_mult(matrix_demodulated.get_col(j),conj(matrix_demodulated.get_col(j)))));
        }
    }

    bool is_started=false;
    int previous_j=0;
    for(int j=Nfft/2;j<Nfft;j++){
        if(mask[j]>0){
            if(is_started==false)
                rotation_phase_offset[j]=phase_offset[j];
            else{
                if(abs(phase_offset[j]-phase_offset[previous_j])>pi/4)
                    rotation_phase_offset[j]=abs(phase_offset[j]-phase_offset[previous_j])-pi/2;
                else
                    rotation_phase_offset[j]=phase_offset[j]-phase_offset[previous_j];
            }
            is_started=true;
            previous_j=j;
        }
    }
    for(int j=0;j<Nfft/2;j++){
        if(mask[j]>0){
            if(is_started==false)
                rotation_phase_offset[j]=phase_offset[j];
            else{
                if(abs(phase_offset[j]-phase_offset[previous_j])>pi/4)
                    rotation_phase_offset[j]=abs(phase_offset[j]-phase_offset[previous_j])-pi/2;
                else
                    rotation_phase_offset[j]=phase_offset[j]-phase_offset[previous_j];
            }
            is_started=true;
            previous_j=j;
        }
    }

    is_started=false;
    previous_j=0;
    for (int j=Nfft/2;j<Nfft;j++)
    {
        if(mask[j]>0){
            if(is_started==false)
                is_started=true;
            else
                rotation_phase_offset[j]=rotation_phase_offset[j]+rotation_phase_offset[previous_j];
            previous_j=j;
        }
    }
    for (int j=0;j<Nfft/2;j++)
    {
        if(mask[j]>0){
            if(is_started==false)
                is_started=true;
            else
                rotation_phase_offset[j]=rotation_phase_offset[j]+rotation_phase_offset[previous_j];
            previous_j=j;
        }
    }

    // Correction of the rotation phase offset
    cvec phase_rotated_demodulated_ofdm_symbols(Number_of_blocks_time_domain*Nfft);
    phase_rotated_demodulated_ofdm_symbols.zeros();
    for (int i=0;i<Number_of_blocks_time_domain;i++)
        phase_rotated_demodulated_ofdm_symbols.replace_mid(i*(Nfft),elem_mult(demodulated_ofdm_symbols.get(i*(Nfft),(i+1)*Nfft-1),elem_mult(to_cvec(1./sqrt(amplitude)),exp(to_cvec(zeros(Nfft),-rotation_phase_offset)))));


    //Calculate if there is a remaining phase offset
    //Reorganization in a matrix
    for(int i=0;i<Number_of_blocks_time_domain;i++){
        for(int j=0;j<Nfft;j++){
            if(mask[j]>0){
                matrix_demodulated(i,j)=phase_rotated_demodulated_ofdm_symbols(i*Nfft+j);
            }
        }
    }
    //Calculate the phase and amplitude for each tone
    for (int j=0;j<Nfft;j++){
        if(mask[j]>0){
            //QPSK phase offset estimation
            phase_offset[j]=1./4*arg(1./Number_of_blocks_time_domain*sum(elem_mult(elem_mult(matrix_demodulated.get_col(j),matrix_demodulated.get_col(j)),elem_mult(matrix_demodulated.get_col(j),matrix_demodulated.get_col(j)))));
            amplitude[j]=1./Number_of_blocks_time_domain*real(sum(elem_mult(matrix_demodulated.get_col(j),conj(matrix_demodulated.get_col(j)))));
        }
    }
    //phase_offset.zeros();
    amplitude.ones();
    //cout << phase_offset << endl;
    //cout << amplitude << endl;

    cvec phase_demodulated_ofdm_symbols(Number_of_blocks_time_domain*Nfft);
    phase_demodulated_ofdm_symbols.zeros();
    for (int i=0;i<Number_of_blocks_time_domain;i++)
         phase_demodulated_ofdm_symbols.replace_mid(i*Nfft,elem_mult(phase_rotated_demodulated_ofdm_symbols.get(i*Nfft,(i+1)*Nfft-1),elem_mult(to_cvec(1./sqrt(amplitude)),exp(to_cvec(zeros(Nfft),-phase_offset)))));


    matrix_demodulated.zeros();
    for(int i=0;i<Number_of_blocks_time_domain;i++){
        for(int j=0;j<Nfft;j++){
            if(mask[j]>0){
                matrix_demodulated(i,j)=phase_demodulated_ofdm_symbols(i*(Nfft)+j);
            }
        }
    }

    for (int i=0;i<Number_of_blocks_time_domain;i++){
       //QPSK phase offset estimation
        phase_offset_time[i]=1./4*arg(1./sum_mask*sum(elem_mult(elem_mult(matrix_demodulated.get_row(i),matrix_demodulated.get_row(i)),elem_mult(matrix_demodulated.get_row(i),matrix_demodulated.get_row(i)))));
        amplitude_time[i]=8./sum_mask*real(sum(elem_mult(matrix_demodulated.get_row(i),conj(matrix_demodulated.get_row(i)))));
    }
    //phase_offset_time.zeros();
    amplitude_time.ones();
    //cout << amplitude_time << endl;
    //cout << phase_offset_time << endl;

        is_started=false;
        int previous_i=0;

        for(int i=0;i<Number_of_blocks_time_domain;i++){
            if(is_started==false){
                rotation_phase_offset_time[i]=phase_offset_time[i];
                is_started=true;
            }
            else{
                if(abs(phase_offset_time[i]-phase_offset_time[previous_i])>pi/4)
                    rotation_phase_offset_time[i]=abs(phase_offset_time[i]-phase_offset_time[previous_i])-pi/2;
                else
                    rotation_phase_offset_time[i]=phase_offset_time[i]-phase_offset_time[previous_i];

                rotation_phase_offset_time[i]=rotation_phase_offset_time[i]+rotation_phase_offset_time[previous_i];
            }
            previous_i=i;
        }

        //cout << rotation_phase_offset_time << endl;
        // Correction of the rotation phase offset in time
        phase_rotated_demodulated_ofdm_symbols.zeros();
        for (int i=0;i<Number_of_blocks_time_domain;i++)
            phase_rotated_demodulated_ofdm_symbols.replace_mid(i*(Nfft),phase_demodulated_ofdm_symbols.get(i*(Nfft),(i+1)*Nfft-1)*1./sqrt(amplitude_time[i])*exp(std::complex<double>(0,-rotation_phase_offset_time[i])));

        matrix_demodulated.zeros();
        for(int i=0;i<Number_of_blocks_time_domain;i++){
            for(int j=0;j<Nfft;j++){
                if(mask[j]>0){
                    matrix_demodulated(i,j)=phase_rotated_demodulated_ofdm_symbols(i*(Nfft)+j);
                }
            }
        }

        for (int i=0;i<Number_of_blocks_time_domain;i++){
           //QPSK phase offset estimation
            phase_offset_time[i]=1./4*arg(1./sum_mask*sum(elem_mult(elem_mult(matrix_demodulated.get_row(i),matrix_demodulated.get_row(i)),elem_mult(matrix_demodulated.get_row(i),matrix_demodulated.get_row(i)))));
            amplitude_time[i]=8./sum_mask*real(sum(elem_mult(matrix_demodulated.get_row(i),conj(matrix_demodulated.get_row(i)))));
        }
        //cout << phase_offset_time << endl;
        //phase_offset_time.zeros();
        amplitude_time.ones();

        demodulated_ofdm_symbols.zeros();
        for (int i=0;i<Number_of_blocks_time_domain;i++)
            demodulated_ofdm_symbols.replace_mid(i*(Nfft),phase_rotated_demodulated_ofdm_symbols.get(i*(Nfft),(i+1)*Nfft-1)*1./sqrt(amplitude_time[i])*exp(std::complex<double>(0,-phase_offset_time[i])));

        estimated_channel=amplitude;
        return demodulated_ofdm_symbols;

}




bvec BlindOFDM_Modem::demodulate_mask_gray_256qam(cvec demodulated_ofdm_symbols, int best_group, cvec &constellation)
{


    bvec received_bits;
    ivec mask(Nfft);
    mask.zeros();
    if(num_subchannels==1)
        mask.replace_mid(Nfft/32,concat(8*ones_i(Nfft/2-3*Nfft/32),zeros_i(2*Nfft/16),8*ones_i(Nfft/2-3*Nfft/32)));
    else{
        if(best_group<num_subchannels/2)
            mask.replace_mid(Nfft/32+best_group*(Nfft-3*Nfft/16)/num_subchannels,8*ones_i((Nfft-3*Nfft/16)/num_subchannels));
        if(best_group>=num_subchannels/2)
            mask.replace_mid(Nfft/32+2*Nfft/16+best_group*(Nfft-3*Nfft/16)/num_subchannels,8*ones_i((Nfft-3*Nfft/16)/num_subchannels));
    }

    int sum_mask=sum(mask);
    QAM qam;
    qam.set_M(256);
    int Number_of_blocks_time_domain=(demodulated_ofdm_symbols.size()/Nfft);
    received_bits.set_size(Number_of_blocks_time_domain*sum_mask);
    received_bits.zeros();
    constellation.set_size(Number_of_blocks_time_domain*sum_mask/8);
    constellation.zeros();
    int index=0;
    int index2=0;
    for (int i=0;i<Number_of_blocks_time_domain;i++){
        for(int j=0;j<Nfft;j++){
            if(mask[j]>0){
                received_bits.replace_mid(index,qam.demodulate_bits(demodulated_ofdm_symbols.get(i*Nfft+j,i*Nfft+j)*exp(std::complex<double>(0,+pi/4))));
                constellation.replace_mid(index2,demodulated_ofdm_symbols.get(i*Nfft+j,i*Nfft+j)*exp(std::complex<double>(0,+pi/4)));
                index=index+mask[j];
                index2=index2+1;
            }
        }
    }

    return received_bits;

}

bvec BlindOFDM_Modem::demodulate_mask_gray_64qam(cvec demodulated_ofdm_symbols, int best_group, cvec &constellation)
{


    bvec received_bits;
    ivec mask(Nfft);
    mask.zeros();
    if(num_subchannels==1)
        mask.replace_mid(Nfft/32,concat(6*ones_i(Nfft/2-3*Nfft/32),zeros_i(2*Nfft/16),6*ones_i(Nfft/2-3*Nfft/32)));
    else{
        if(best_group<num_subchannels/2)
            mask.replace_mid(Nfft/32+best_group*(Nfft-3*Nfft/16)/num_subchannels,6*ones_i((Nfft-3*Nfft/16)/num_subchannels));
        if(best_group>=num_subchannels/2)
            mask.replace_mid(Nfft/32+2*Nfft/16+best_group*(Nfft-3*Nfft/16)/num_subchannels,6*ones_i((Nfft-3*Nfft/16)/num_subchannels));
    }

    int sum_mask=sum(mask);
    QAM qam;
    qam.set_M(64);
    int Number_of_blocks_time_domain=(demodulated_ofdm_symbols.size()/Nfft);
    received_bits.set_size(Number_of_blocks_time_domain*sum_mask);
    received_bits.zeros();
    constellation.set_size(Number_of_blocks_time_domain*sum_mask/6);
    constellation.zeros();
    int index=0;
    int index2=0;
    for (int i=0;i<Number_of_blocks_time_domain;i++){
        for(int j=0;j<Nfft;j++){
            if(mask[j]>0){
                received_bits.replace_mid(index,qam.demodulate_bits(demodulated_ofdm_symbols.get(i*Nfft+j,i*Nfft+j)*exp(std::complex<double>(0,+pi/4))));
                constellation.replace_mid(index2,demodulated_ofdm_symbols.get(i*Nfft+j,i*Nfft+j)*exp(std::complex<double>(0,+pi/4)));
                index=index+mask[j];
                index2=index2+1;
            }
        }
    }

    return received_bits;

}

bvec BlindOFDM_Modem::demodulate_mask_gray_16qam(cvec demodulated_ofdm_symbols, int best_group, cvec &constellation)
{


    bvec received_bits;
    ivec mask(Nfft);
    mask.zeros();
    if(num_subchannels==1)
        mask.replace_mid(Nfft/32,concat(4*ones_i(Nfft/2-3*Nfft/32),zeros_i(2*Nfft/16),4*ones_i(Nfft/2-3*Nfft/32)));
    else{
        if(best_group<num_subchannels/2)
            mask.replace_mid(Nfft/32+best_group*(Nfft-3*Nfft/16)/num_subchannels,4*ones_i((Nfft-3*Nfft/16)/num_subchannels));
        if(best_group>=num_subchannels/2)
            mask.replace_mid(Nfft/32+2*Nfft/16+best_group*(Nfft-3*Nfft/16)/num_subchannels,4*ones_i((Nfft-3*Nfft/16)/num_subchannels));
    }

    int sum_mask=sum(mask);
    QAM qam;
    qam.set_M(16);
    int Number_of_blocks_time_domain=(demodulated_ofdm_symbols.size()/Nfft);
    received_bits.set_size(Number_of_blocks_time_domain*sum_mask);
    received_bits.zeros();
    constellation.set_size(Number_of_blocks_time_domain*sum_mask/4);
    constellation.zeros();
    int index=0;
    int index2=0;
    for (int i=0;i<Number_of_blocks_time_domain;i++){
        for(int j=0;j<Nfft;j++){
            if(mask[j]>0){
                received_bits.replace_mid(index,qam.demodulate_bits(demodulated_ofdm_symbols.get(i*Nfft+j,i*Nfft+j)*exp(std::complex<double>(0,+pi/4))));
                constellation.replace_mid(index2,demodulated_ofdm_symbols.get(i*Nfft+j,i*Nfft+j)*exp(std::complex<double>(0,+pi/4)));
                index=index+mask[j];
                index2=index2+1;
            }
        }
    }

    return received_bits;

}

bvec BlindOFDM_Modem::demodulate_mask_quadrant_16qam(cvec demodulated_ofdm_symbols, int best_group, cvec &constellation)
{


    bvec received_bits;
    ivec mask(Nfft);
    mask.zeros();
    if(num_subchannels==1)
        mask.replace_mid(Nfft/32,concat(4*ones_i(Nfft/2-3*Nfft/32),zeros_i(2*Nfft/16),4*ones_i(Nfft/2-3*Nfft/32)));
    else{
        if(best_group<num_subchannels/2)
            mask.replace_mid(Nfft/32+best_group*(Nfft-3*Nfft/16)/num_subchannels,4*ones_i((Nfft-3*Nfft/16)/num_subchannels));
        if(best_group>=num_subchannels/2)
            mask.replace_mid(Nfft/32+2*Nfft/16+best_group*(Nfft-3*Nfft/16)/num_subchannels,4*ones_i((Nfft-3*Nfft/16)/num_subchannels));
    }

    int sum_mask=sum(mask);
    QAM qam;
    qam.set_M(16);
    ivec new_map = "15 14 9 11 13 12 8 10 6 4 0 1 7 5 2 3";
    qam.set(qam.get_symbols(),new_map);
    int Number_of_blocks_time_domain=(demodulated_ofdm_symbols.size()/Nfft);
    received_bits.set_size(Number_of_blocks_time_domain*sum_mask);
    received_bits.zeros();
    constellation.set_size(Number_of_blocks_time_domain*sum_mask/4);
    constellation.zeros();
    int index=0;
    int index2=0;
    for (int i=0;i<Number_of_blocks_time_domain;i++){
        for(int j=0;j<Nfft;j++){
            if(mask[j]>0){
                received_bits.replace_mid(index,qam.demodulate_bits(demodulated_ofdm_symbols.get(i*Nfft+j,i*Nfft+j)*exp(std::complex<double>(0,+pi/4))));
                constellation.replace_mid(index2,demodulated_ofdm_symbols.get(i*Nfft+j,i*Nfft+j)*exp(std::complex<double>(0,+pi/4)));
                index=index+mask[j];
                index2=index2+1;
            }
        }
    }

    return received_bits;

}

bvec BlindOFDM_Modem::demodulate_mask_gray_qpsk(cvec demodulated_ofdm_symbols, int best_group, cvec &constellation)
{


    bvec received_bits;
    ivec mask(Nfft);
    mask.zeros();
    if(num_subchannels==1)
        mask.replace_mid(Nfft/32,concat(2*ones_i(Nfft/2-3*Nfft/32),zeros_i(2*Nfft/16),2*ones_i(Nfft/2-3*Nfft/32)));
    else{
        if(best_group<num_subchannels/2)
            mask.replace_mid(Nfft/32+best_group*(Nfft-3*Nfft/16)/num_subchannels,2*ones_i((Nfft-3*Nfft/16)/num_subchannels));
        if(best_group>=num_subchannels/2)
            mask.replace_mid(Nfft/32+2*Nfft/16+best_group*(Nfft-3*Nfft/16)/num_subchannels,2*ones_i((Nfft-3*Nfft/16)/num_subchannels));
    }

    int sum_mask=sum(mask);
    QPSK qpsk;
    int Number_of_blocks_time_domain=(demodulated_ofdm_symbols.size()/Nfft);
    received_bits.set_size(Number_of_blocks_time_domain*sum_mask);
    received_bits.zeros();
    constellation.set_size(Number_of_blocks_time_domain*sum_mask/2);
    constellation.zeros();
    int index=0;
    int index2=0;
    for (int i=0;i<Number_of_blocks_time_domain;i++){
        for(int j=0;j<Nfft;j++){
            if(mask[j]>0){
                received_bits.replace_mid(index,qpsk.demodulate_bits(demodulated_ofdm_symbols.get(i*Nfft+j,i*Nfft+j)));
                constellation.replace_mid(index2,demodulated_ofdm_symbols.get(i*Nfft+j,i*Nfft+j));
                index=index+mask[j];
                index2=index2+1;
            }
        }
    }

    return received_bits;

}

bvec BlindOFDM_Modem::demodulate_mask_gray_bpsk(cvec demodulated_ofdm_symbols, int best_group, cvec &constellation)
{


    bvec received_bits;
    ivec mask(Nfft);
    mask.zeros();
    if(num_subchannels==1)
        mask.replace_mid(Nfft/32,concat(ones_i(Nfft/2-3*Nfft/32),zeros_i(2*Nfft/16),ones_i(Nfft/2-3*Nfft/32)));
    else{
        if(best_group<num_subchannels/2)
            mask.replace_mid(Nfft/32+best_group*(Nfft-3*Nfft/16)/num_subchannels,ones_i((Nfft-3*Nfft/16)/num_subchannels));
        if(best_group>=num_subchannels/2)
            mask.replace_mid(Nfft/32+2*Nfft/16+best_group*(Nfft-3*Nfft/16)/num_subchannels,ones_i((Nfft-3*Nfft/16)/num_subchannels));
    }

    int sum_mask=sum(mask);
    BPSK bpsk;
    int Number_of_blocks_time_domain=(demodulated_ofdm_symbols.size()/Nfft);
    received_bits.set_size(Number_of_blocks_time_domain*sum_mask);
    received_bits.zeros();
    constellation.set_size(Number_of_blocks_time_domain*sum_mask);
    constellation.zeros();
    int index=0;
    int index2=0;
    for (int i=0;i<Number_of_blocks_time_domain;i++){
        for(int j=0;j<Nfft;j++){
            if(mask[j]>0){
                received_bits.replace_mid(index,bpsk.demodulate_bits(real(demodulated_ofdm_symbols.get(i*Nfft+j,i*Nfft+j))));
                constellation.replace_mid(index2,demodulated_ofdm_symbols.get(i*Nfft+j,i*Nfft+j));
                index=index+mask[j];
                index2=index2+1;
            }
        }
    }

    return received_bits;

}

bool BlindOFDM_Modem::preamble_detection_bpsk(bvec received_bits,bvec &received_bits2){

    vector<char> preamble(3);
    bvec received_bits_inverted;


    received_bits_inverted=received_bits+ones_b(received_bits.size());



    preamble[0]='!';
    preamble[1]='!';
    preamble[2]='H';

    bvec preamble_bits=charvec2bvec(preamble);

    bool preamble_ok=false;
    int i=0;
    while((i<int(received_bits.size())-24)&&(preamble_ok==false)){
        if(preamble_bits==received_bits.get(i,i+24-1)){
            preamble_ok=true;
            received_bits2=received_bits;
        }
        if(preamble_bits==received_bits_inverted.get(i,i+24-1)){
            preamble_ok=true;
            received_bits2=received_bits_inverted;
        }
        i++;
    }

    return preamble_ok;

}

bool BlindOFDM_Modem::preamble_detection_qpsk(bvec received_bits,bvec &received_bits2){

    vector<char> preamble(3);
    QPSK qpsk;
    bvec received_bits_rotated1;
    bvec received_bits_rotated2;
    bvec received_bits_rotated3;

    received_bits_rotated1=qpsk.demodulate_bits(qpsk.modulate_bits(received_bits)*exp(std::complex<double>(0,pi/2)));
    received_bits_rotated2=qpsk.demodulate_bits(qpsk.modulate_bits(received_bits)*exp(std::complex<double>(0,pi)));
    received_bits_rotated3=qpsk.demodulate_bits(qpsk.modulate_bits(received_bits)*exp(std::complex<double>(0,3*pi/2)));


    preamble[0]='!';
    preamble[1]='!';
    preamble[2]='H';

    bvec preamble_bits=charvec2bvec(preamble);

    bool preamble_ok=false;
    int i=0;
    while((i<int(received_bits.size())-24)&&(preamble_ok==false)){
        if((preamble_bits==received_bits.get(i,i+24-1))&&(preamble_bits==received_bits.get(i+112,i+136-1))){
            preamble_ok=true;
            received_bits2=received_bits;
        }
        if((preamble_bits==received_bits_rotated1.get(i,i+24-1))&&(preamble_bits==received_bits_rotated1.get(i+112,i+136-1))){
            preamble_ok=true;
            received_bits2=received_bits_rotated1;
        }
        if((preamble_bits==received_bits_rotated2.get(i,i+24-1))&&(preamble_bits==received_bits_rotated2.get(i+112,i+136-1))){
            preamble_ok=true;
            received_bits2=received_bits_rotated2;
        }
        if((preamble_bits==received_bits_rotated3.get(i,i+24-1))&&(preamble_bits==received_bits_rotated3.get(i+112,i+136-1))){
            preamble_ok=true;
            received_bits2=received_bits_rotated3;
        }
        i++;
    }

    return preamble_ok;

}

bool BlindOFDM_Modem::preamble_detection_gray_16qam(bvec received_bits,bvec &received_bits2){

    vector<char> preamble(3);
    QAM qam;
    qam.set_M(16);
    bvec received_bits_rotated1;
    bvec received_bits_rotated2;
    bvec received_bits_rotated3;

    received_bits_rotated1=qam.demodulate_bits(qam.modulate_bits(received_bits)*exp(std::complex<double>(0,pi/2)));
    received_bits_rotated2=qam.demodulate_bits(qam.modulate_bits(received_bits)*exp(std::complex<double>(0,pi)));
    received_bits_rotated3=qam.demodulate_bits(qam.modulate_bits(received_bits)*exp(std::complex<double>(0,3*pi/2)));


    preamble[0]='!';
    preamble[1]='!';
    preamble[2]='H';

    bvec preamble_bits=charvec2bvec(preamble);

    bool preamble_ok=false;
    int i=0;
    while((i<int(received_bits.size())-24)&&(preamble_ok==false)){
        if(preamble_bits==received_bits.get(i,i+24-1)){
            preamble_ok=true;
            received_bits2=received_bits;
        }
        if(preamble_bits==received_bits_rotated1.get(i,i+24-1)){
            preamble_ok=true;
            received_bits2=received_bits_rotated1;
        }
        if(preamble_bits==received_bits_rotated2.get(i,i+24-1)){
            preamble_ok=true;
            received_bits2=received_bits_rotated2;
        }
        if(preamble_bits==received_bits_rotated3.get(i,i+24-1)){
            preamble_ok=true;
            received_bits2=received_bits_rotated3;
        }
        i++;
    }

    return preamble_ok;

}

bool BlindOFDM_Modem::preamble_detection_quadrant_16qam(bvec received_bits,bvec &received_bits2){

    vector<char> preamble(3);
    QAM qam;
    qam.set_M(16);
    ivec new_map = "15 14 9 11 13 12 8 10 6 4 0 1 7 5 2 3";
    qam.set(qam.get_symbols(),new_map);
    bvec received_bits_rotated1;
    bvec received_bits_rotated2;
    bvec received_bits_rotated3;

    received_bits_rotated1=qam.demodulate_bits(qam.modulate_bits(received_bits)*exp(std::complex<double>(0,pi/2)));
    received_bits_rotated2=qam.demodulate_bits(qam.modulate_bits(received_bits)*exp(std::complex<double>(0,pi)));
    received_bits_rotated3=qam.demodulate_bits(qam.modulate_bits(received_bits)*exp(std::complex<double>(0,3*pi/2)));


    preamble[0]='!';
    preamble[1]='!';
    preamble[2]='H';

    bvec preamble_bits=charvec2bvec(preamble);

    bool preamble_ok=false;
    int i=0;
    while((i<int(received_bits.size())-24)&&(preamble_ok==false)){
        if(preamble_bits==received_bits.get(i,i+24-1)){
            preamble_ok=true;
            received_bits2=received_bits;
        }
        if(preamble_bits==received_bits_rotated1.get(i,i+24-1)){
            preamble_ok=true;
            received_bits2=received_bits_rotated1;
        }
        if(preamble_bits==received_bits_rotated2.get(i,i+24-1)){
            preamble_ok=true;
            received_bits2=received_bits_rotated2;
        }
        if(preamble_bits==received_bits_rotated3.get(i,i+24-1)){
            preamble_ok=true;
            received_bits2=received_bits_rotated3;
        }
        i++;
    }

    return preamble_ok;

}

bool BlindOFDM_Modem::preamble_detection_gray_64qam(bvec received_bits,bvec &received_bits2){

    vector<char> preamble(3);
    QAM qam;
    qam.set_M(64);
    bvec received_bits_rotated1;
    bvec received_bits_rotated2;
    bvec received_bits_rotated3;

    received_bits_rotated1=qam.demodulate_bits(qam.modulate_bits(received_bits)*exp(std::complex<double>(0,pi/2)));
    received_bits_rotated2=qam.demodulate_bits(qam.modulate_bits(received_bits)*exp(std::complex<double>(0,pi)));
    received_bits_rotated3=qam.demodulate_bits(qam.modulate_bits(received_bits)*exp(std::complex<double>(0,3*pi/2)));


    preamble[0]='!';
    preamble[1]='!';
    preamble[2]='H';

    bvec preamble_bits=charvec2bvec(preamble);

    bool preamble_ok=false;
    int i=0;
    while((i<int(received_bits.size())-24)&&(preamble_ok==false)){
        if(preamble_bits==received_bits.get(i,i+24-1)){
            preamble_ok=true;
            received_bits2=received_bits;
        }
        if(preamble_bits==received_bits_rotated1.get(i,i+24-1)){
            preamble_ok=true;
            received_bits2=received_bits_rotated1;
        }
        if(preamble_bits==received_bits_rotated2.get(i,i+24-1)){
            preamble_ok=true;
            received_bits2=received_bits_rotated2;
        }
        if(preamble_bits==received_bits_rotated3.get(i,i+24-1)){
            preamble_ok=true;
            received_bits2=received_bits_rotated3;
        }
        i++;
    }

    return preamble_ok;

}

bool BlindOFDM_Modem::preamble_detection_gray_256qam(bvec received_bits,bvec &received_bits2){

    vector<char> preamble(3);
    QAM qam;
    qam.set_M(256);
    bvec received_bits_rotated1;
    bvec received_bits_rotated2;
    bvec received_bits_rotated3;

    received_bits_rotated1=qam.demodulate_bits(qam.modulate_bits(received_bits)*exp(std::complex<double>(0,pi/2)));
    received_bits_rotated2=qam.demodulate_bits(qam.modulate_bits(received_bits)*exp(std::complex<double>(0,pi)));
    received_bits_rotated3=qam.demodulate_bits(qam.modulate_bits(received_bits)*exp(std::complex<double>(0,3*pi/2)));


    preamble[0]='!';
    preamble[1]='!';
    preamble[2]='H';

    bvec preamble_bits=charvec2bvec(preamble);

    bool preamble_ok=false;
    int i=0;
    while((i<int(received_bits.size())-24)&&(preamble_ok==false)){
        if(preamble_bits==received_bits.get(i,i+24-1)){
            preamble_ok=true;
            received_bits2=received_bits;
        }
        if(preamble_bits==received_bits_rotated1.get(i,i+24-1)){
            preamble_ok=true;
            received_bits2=received_bits_rotated1;
        }
        if(preamble_bits==received_bits_rotated2.get(i,i+24-1)){
            preamble_ok=true;
            received_bits2=received_bits_rotated2;
        }
        if(preamble_bits==received_bits_rotated3.get(i,i+24-1)){
            preamble_ok=true;
            received_bits2=received_bits_rotated3;
        }
        i++;
    }

    return preamble_ok;

}

bvec BlindOFDM_Modem::bpsk_differential_coding(bvec data_packet){

    bvec diff_data_packet(data_packet.size());
    diff_data_packet.zeros();

    //Differential encoding
    for(int i=0;i<data_packet.size();i++){
        if(i==0)
            diff_data_packet(i)=data_packet(i)+bin(1);
        else
            diff_data_packet(i)=data_packet(i)+diff_data_packet(i-1);
    }
    return diff_data_packet;

}
bvec BlindOFDM_Modem::bpsk_differential_decoding(bvec received_bits){

    //Differential decoding
    bvec diff_received_bits=received_bits;
    for(int i=0;i<received_bits.size();i++){
        if(i==0)
            received_bits(i)=diff_received_bits(i)+bin(1);
        else
            received_bits(i)=diff_received_bits(i)+diff_received_bits(i-1);
    }
    return received_bits;
}

bvec BlindOFDM_Modem::qpsk_differential_coding(bvec data_packet){

    bvec diff_data_packet(data_packet.size());
    diff_data_packet.zeros();

    //Differential encoding
    for(int i=0;i<data_packet.size();i=i+2){
        if(i==0){
            diff_data_packet(i)=(data_packet(i)+data_packet(i+1)+bin(1))*(data_packet(i)+bin(1))+(data_packet(i)+data_packet(i+1))*(data_packet(i+1)+bin(1));
            diff_data_packet(i+1)=(data_packet(i)+data_packet(i+1)+bin(1))*(data_packet(i+1)+bin(1))+(data_packet(i)+data_packet(i+1))*(data_packet(i)+bin(1));
        }
        else{
            diff_data_packet(i)=(data_packet(i)+data_packet(i+1)+bin(1))*(data_packet(i)+diff_data_packet(i-2))+(data_packet(i)+data_packet(i+1))*(data_packet(i+1)+diff_data_packet(i-1));
            diff_data_packet(i+1)=(data_packet(i)+data_packet(i+1)+bin(1))*(data_packet(i+1)+diff_data_packet(i-1))+(data_packet(i)+data_packet(i+1))*(data_packet(i)+diff_data_packet(i-2));
        }
    }
    return diff_data_packet;

}
bvec BlindOFDM_Modem::qpsk_differential_decoding(bvec received_bits){

    //Differential decoding
    bvec diff_received_bits=received_bits;
    for(int i=0;i<received_bits.size();i=i+2){
        if(i==0){
            received_bits(i)=(diff_received_bits(i)+diff_received_bits(i+1)+bin(1))*(diff_received_bits(i)+bin(1))+(diff_received_bits(i)+diff_received_bits(i+1))*(diff_received_bits(i+1)+bin(1));
            received_bits(i+1)=(diff_received_bits(i)+diff_received_bits(i+1)+bin(1))*(diff_received_bits(i+1)+bin(1))+(diff_received_bits(i)+diff_received_bits(i+1))*(diff_received_bits(i)+bin(1));
        }
        else{
            received_bits(i)=(diff_received_bits(i)+diff_received_bits(i+1)+bin(1))*(diff_received_bits(i)+diff_received_bits(i-2))+(diff_received_bits(i)+diff_received_bits(i+1))*(diff_received_bits(i+1)+diff_received_bits(i-1));
            received_bits(i+1)=(diff_received_bits(i)+diff_received_bits(i+1)+bin(1))*(diff_received_bits(i+1)+diff_received_bits(i-1))+(diff_received_bits(i)+diff_received_bits(i+1))*(diff_received_bits(i)+diff_received_bits(i-2));

        }
    }
    return received_bits;

}

bvec BlindOFDM_Modem::qam16_gray_differential_coding(bvec data_packet){

    bvec diff_data_packet(data_packet.size());
    diff_data_packet.zeros();

    //Differential encoding
    for(int i=0;i<data_packet.size();i=i+4){
        if(i==0){
            diff_data_packet(i)=(data_packet(i)+data_packet(i+1)+bin(1))*(data_packet(i)+bin(1))+(data_packet(i)+data_packet(i+1))*(data_packet(i+1)+bin(1));
            diff_data_packet(i+1)=(data_packet(i)+data_packet(i+1)+bin(1))*(data_packet(i+1)+bin(1))+(data_packet(i)+data_packet(i+1))*(data_packet(i)+bin(1));
            diff_data_packet(i+2)=(data_packet(i+2)+data_packet(i+3)+bin(1))*(data_packet(i+2)+bin(1))+(data_packet(i+2)+data_packet(i+3))*(data_packet(i+3)+bin(1));
            diff_data_packet(i+3)=(data_packet(i+2)+data_packet(i+3)+bin(1))*(data_packet(i+3)+bin(1))+(data_packet(i+2)+data_packet(i+3))*(data_packet(i+2)+bin(1));
        }
        else{
            diff_data_packet(i)=(data_packet(i)+data_packet(i+1)+bin(1))*(data_packet(i)+diff_data_packet(i-4))+(data_packet(i)+data_packet(i+1))*(data_packet(i+1)+diff_data_packet(i-3));
            diff_data_packet(i+1)=(data_packet(i)+data_packet(i+1)+bin(1))*(data_packet(i+1)+diff_data_packet(i-3))+(data_packet(i)+data_packet(i+1))*(data_packet(i)+diff_data_packet(i-4));
            diff_data_packet(i+2)=(data_packet(i+2)+data_packet(i+3)+bin(1))*(data_packet(i+2)+diff_data_packet(i-2))+(data_packet(i+2)+data_packet(i+3))*(data_packet(i+3)+diff_data_packet(i-1));
            diff_data_packet(i+3)=(data_packet(i+2)+data_packet(i+3)+bin(1))*(data_packet(i+3)+diff_data_packet(i-1))+(data_packet(i+2)+data_packet(i+3))*(data_packet(i+2)+diff_data_packet(i-2));

        }
    }
    return diff_data_packet;

}
bvec BlindOFDM_Modem::qam16_gray_differential_decoding(bvec received_bits){

    //Differential decoding
    bvec diff_received_bits=received_bits;
    for(int i=0;i<received_bits.size();i=i+4){
        if(i==0){
            received_bits(i)=(diff_received_bits(i)+diff_received_bits(i+1)+bin(1))*(diff_received_bits(i)+bin(1))+(diff_received_bits(i)+diff_received_bits(i+1))*(diff_received_bits(i+1)+bin(1));
            received_bits(i+1)=(diff_received_bits(i)+diff_received_bits(i+1)+bin(1))*(diff_received_bits(i+1)+bin(1))+(diff_received_bits(i)+diff_received_bits(i+1))*(diff_received_bits(i)+bin(1));
            received_bits(i+2)=(diff_received_bits(i+2)+diff_received_bits(i+3)+bin(1))*(diff_received_bits(i+2)+bin(1))+(diff_received_bits(i+2)+diff_received_bits(i+3))*(diff_received_bits(i+3)+bin(1));
            received_bits(i+3)=(diff_received_bits(i+2)+diff_received_bits(i+3)+bin(1))*(diff_received_bits(i+3)+bin(1))+(diff_received_bits(i+2)+diff_received_bits(i+3))*(diff_received_bits(i+2)+bin(1));

        }
        else{
            received_bits(i)=(diff_received_bits(i)+diff_received_bits(i+1)+bin(1))*(diff_received_bits(i)+diff_received_bits(i-4))+(diff_received_bits(i)+diff_received_bits(i+1))*(diff_received_bits(i+1)+diff_received_bits(i-3));
            received_bits(i+1)=(diff_received_bits(i)+diff_received_bits(i+1)+bin(1))*(diff_received_bits(i+1)+diff_received_bits(i-3))+(diff_received_bits(i)+diff_received_bits(i+1))*(diff_received_bits(i)+diff_received_bits(i-4));
            received_bits(i+2)=(diff_received_bits(i+2)+diff_received_bits(i+3)+bin(1))*(diff_received_bits(i+2)+diff_received_bits(i-2))+(diff_received_bits(i+2)+diff_received_bits(i+3))*(diff_received_bits(i+3)+diff_received_bits(i-1));
            received_bits(i+3)=(diff_received_bits(i+2)+diff_received_bits(i+3)+bin(1))*(diff_received_bits(i+3)+diff_received_bits(i-1))+(diff_received_bits(i+2)+diff_received_bits(i+3))*(diff_received_bits(i+2)+diff_received_bits(i-2));

        }
    }
    return received_bits;

}

bvec BlindOFDM_Modem::qam16_quadrant_differential_coding(bvec data_packet){

    bvec diff_data_packet(data_packet.size());
    diff_data_packet.zeros();

    //Differential encoding
    for(int i=0;i<data_packet.size();i=i+4){
        if(i==0){
            diff_data_packet(i)=(data_packet(i)+data_packet(i+1)+bin(1))*(data_packet(i)+bin(1))+(data_packet(i)+data_packet(i+1))*(data_packet(i+1)+bin(1));
            diff_data_packet(i+1)=(data_packet(i)+data_packet(i+1)+bin(1))*(data_packet(i+1)+bin(1))+(data_packet(i)+data_packet(i+1))*(data_packet(i)+bin(1));
            //diff_data_packet(i+2)=(data_packet(i+2)+data_packet(i+3)+bin(1))*(data_packet(i+2)+bin(1))+(data_packet(i+2)+data_packet(i+3))*(data_packet(i+3)+bin(1));
            //diff_data_packet(i+3)=(data_packet(i+2)+data_packet(i+3)+bin(1))*(data_packet(i+3)+bin(1))+(data_packet(i+2)+data_packet(i+3))*(data_packet(i+2)+bin(1));
            diff_data_packet(i+2)=data_packet(i+2);
            diff_data_packet(i+3)=data_packet(i+3);
        }
        else{
            diff_data_packet(i)=(data_packet(i)+data_packet(i+1)+bin(1))*(data_packet(i)+diff_data_packet(i-4))+(data_packet(i)+data_packet(i+1))*(data_packet(i+1)+diff_data_packet(i-3));
            diff_data_packet(i+1)=(data_packet(i)+data_packet(i+1)+bin(1))*(data_packet(i+1)+diff_data_packet(i-3))+(data_packet(i)+data_packet(i+1))*(data_packet(i)+diff_data_packet(i-4));
            //diff_data_packet(i+2)=(data_packet(i+2)+data_packet(i+3)+bin(1))*(data_packet(i+2)+diff_data_packet(i-2))+(data_packet(i+2)+data_packet(i+3))*(data_packet(i+3)+diff_data_packet(i-1));
            //diff_data_packet(i+3)=(data_packet(i+2)+data_packet(i+3)+bin(1))*(data_packet(i+3)+diff_data_packet(i-1))+(data_packet(i+2)+data_packet(i+3))*(data_packet(i+2)+diff_data_packet(i-2));
            diff_data_packet(i+2)=data_packet(i+2);
            diff_data_packet(i+3)=data_packet(i+3);
        }
    }
    return diff_data_packet;

}
bvec BlindOFDM_Modem::qam16_quadrant_differential_decoding(bvec received_bits){

    //Differential decoding
    bvec diff_received_bits=received_bits;
    for(int i=0;i<received_bits.size();i=i+4){
        if(i==0){
            received_bits(i)=(diff_received_bits(i)+diff_received_bits(i+1)+bin(1))*(diff_received_bits(i)+bin(1))+(diff_received_bits(i)+diff_received_bits(i+1))*(diff_received_bits(i+1)+bin(1));
            received_bits(i+1)=(diff_received_bits(i)+diff_received_bits(i+1)+bin(1))*(diff_received_bits(i+1)+bin(1))+(diff_received_bits(i)+diff_received_bits(i+1))*(diff_received_bits(i)+bin(1));
            //received_bits(i+2)=(diff_received_bits(i+2)+diff_received_bits(i+3)+bin(1))*(diff_received_bits(i+2)+bin(1))+(diff_received_bits(i+2)+diff_received_bits(i+3))*(diff_received_bits(i+3)+bin(1));
            //received_bits(i+3)=(diff_received_bits(i+2)+diff_received_bits(i+3)+bin(1))*(diff_received_bits(i+3)+bin(1))+(diff_received_bits(i+2)+diff_received_bits(i+3))*(diff_received_bits(i+2)+bin(1));
            received_bits(i+2)=diff_received_bits(i+2);
            received_bits(i+3)=diff_received_bits(i+3);
        }
        else{
            received_bits(i)=(diff_received_bits(i)+diff_received_bits(i+1)+bin(1))*(diff_received_bits(i)+diff_received_bits(i-4))+(diff_received_bits(i)+diff_received_bits(i+1))*(diff_received_bits(i+1)+diff_received_bits(i-3));
            received_bits(i+1)=(diff_received_bits(i)+diff_received_bits(i+1)+bin(1))*(diff_received_bits(i+1)+diff_received_bits(i-3))+(diff_received_bits(i)+diff_received_bits(i+1))*(diff_received_bits(i)+diff_received_bits(i-4));
            //received_bits(i+2)=(diff_received_bits(i+2)+diff_received_bits(i+3)+bin(1))*(diff_received_bits(i+2)+diff_received_bits(i-2))+(diff_received_bits(i+2)+diff_received_bits(i+3))*(diff_received_bits(i+3)+diff_received_bits(i-1));
            //received_bits(i+3)=(diff_received_bits(i+2)+diff_received_bits(i+3)+bin(1))*(diff_received_bits(i+3)+diff_received_bits(i-1))+(diff_received_bits(i+2)+diff_received_bits(i+3))*(diff_received_bits(i+2)+diff_received_bits(i-2));
            received_bits(i+2)=diff_received_bits(i+2);
            received_bits(i+3)=diff_received_bits(i+3);
        }
    }
    return received_bits;

}
bvec BlindOFDM_Modem::charvec2bvec(vector<char> input){
    int bitsize=sizeof(char)*8;
    bvec output(input.size()*bitsize);
    char bintemp;
    bvec temp(bitsize);
    for(int i=0;i<int(input.size());i++){
            bintemp=input[i];
            for(int j=bitsize-1;j>=0;j--){
                temp[j] = bin(bintemp & 1);
                bintemp = (bintemp >> 1);
            }
            output.replace_mid(i*bitsize,temp);
       }
return output;
}

