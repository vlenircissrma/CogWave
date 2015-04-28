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
#include "modem_mccdm.h"

Modem_MCCDM::Modem_MCCDM()
{
    Nfft=512;
    Nu=1;
    Ncp=128;
    hadamard_matrix=1./sqrt(Nfft)*to_mat(hadamard(Nfft));

    /*LFSR lfsr;
    bvec polynomial("1 0 0 0 0 0 0 0 0 0 0 1 0 1 1 0 1");
    lfsr.set_connections(polynomial);
    bvec state("0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1");
    lfsr.set_state(state);
    bvec tmp_bin(Nfft);
    vec tmp(Nfft);
    for(int i=0;i<Nfft;i++){
        tmp_bin=lfsr.shift(Nfft);
        for(int j=0;j<Nfft;j++){
            if(tmp_bin[j]==bin(0))
                tmp[j]=-1./sqrt(Nu);
            else
                tmp[j]=1./sqrt(Nu);
        }
        hadamard_matrix.set_col(i,tmp);
    }*/

    LFSR lfsr;
    bvec polynomial("1 0 0 0 0 0 0 0 0 0 0 1 0 1 1 0 1");
    lfsr.set_connections(polynomial);
    bvec state("0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1");
    lfsr.set_state(state);
    bvec tmp_bin(Nfft);
    scrambling.set_size(Nfft);
    tmp_bin=lfsr.shift(Nfft);
    for(int j=0;j<Nfft;j++){
        if(tmp_bin[j]==bin(0))
            scrambling[j]=-1;
        else
            scrambling[j]=1;
    }
    differential_tmp.set_size(Nfft);
    nb_bits=63040;
}

cvec Modem_MCCDM::modulate(bvec data_packet)
{

    QPSK qpsk;
    cvec symbols=qpsk.modulate_bits(data_packet);
    int mapped_symbols_size;
    if((symbols.size()/Nu)*Nu!=symbols.size())
        mapped_symbols_size=(symbols.size()/Nu+1)*Nfft;
    else
        mapped_symbols_size=(symbols.size()/Nu)*Nfft;

    cvec mapped_symbols(mapped_symbols_size);
    mapped_symbols.zeros();
    int number_blocks;
    number_blocks=mapped_symbols_size/Nfft;
    int index=0;
    for(int i=0;i<number_blocks;i++){
        mapped_symbols.replace_mid(i*Nfft,to_cvec(symbols.get(index,index+Nu-1)));
        index=index+Nu;
    }

    cvec spread_symbols(mapped_symbols_size);
    cmat tmp(Nfft,1);
    tmp.zeros();
    spread_symbols.zeros();
    for(int i=0;i<number_blocks;i++){
        tmp.set_col(0,mapped_symbols.get(i*Nfft,(i+1)*Nfft-1));
        //differential_tmp=(hadamard_matrix*tmp).get_col(0);
        if(i==0){
            differential_tmp=(hadamard_matrix*tmp).get_col(0);
        }
        else{
            differential_tmp=elem_mult((hadamard_matrix*tmp).get_col(0),exp(to_cvec(zeros(Nfft),arg(differential_tmp))));
        }
        spread_symbols.replace_mid(i*Nfft,elem_mult(differential_tmp,to_cvec(scrambling)));
    }

    int ofdm_chips_size;
    ofdm_chips_size=spread_symbols.size()/Nfft*(Nfft+Ncp);
    cvec ofdm_chips;
    ofdm_chips.set_size(ofdm_chips_size);
    ofdm_chips.zeros();

    for(int i=0;i<spread_symbols.size()/Nfft;i++){
       ofdm_chips.replace_mid(i*(Nfft+Ncp),concat((ifft(spread_symbols.get(i*Nfft,(i+1)*Nfft-1))*sqrt(Nfft)).right(Ncp),ifft(spread_symbols.get(i*Nfft,(i+1)*Nfft-1))*sqrt(Nfft)));
    }


    return ofdm_chips;
}



bvec Modem_MCCDM::demodulate(cvec rx_buff, int &ofdm_time_offset_estimate, cvec &received_symbols)
{

    int Number_of_received_symbols=rx_buff.size();
    cvec correlation(Number_of_received_symbols-Nfft);
    correlation.zeros();
    vec ofdm_time_offset(Nfft+Ncp);
    ofdm_time_offset.zeros();
    cvec ofdm_time_offset2(Nfft+Ncp);
    ofdm_time_offset2.zeros();
    vec ofdm_frequency_offset(Number_of_received_symbols);
    ofdm_frequency_offset.zeros();


    cvec non_shifted_rx_samples=rx_buff.get(0,Number_of_received_symbols-Nfft-1);
    cvec shifted_rx_samples=rx_buff.get(Nfft,Number_of_received_symbols-1);

    ofdm_time_offset.zeros();
    ofdm_time_offset2.zeros();
    correlation.zeros();

    correlation=elem_mult(non_shifted_rx_samples,conj(shifted_rx_samples));
    double total_variance=variance(rx_buff);

    for(int j=0;j<(Number_of_received_symbols-Nfft)/(Nfft+Ncp);j++){
        ofdm_time_offset2[0]=ofdm_time_offset2[0]+sum(correlation.get(j*(Nfft+Ncp),j*(Nfft+Ncp)+Ncp-1));
    }
    ofdm_time_offset[0]=1./(Ncp*((Number_of_received_symbols-Nfft)/(Nfft+Ncp)))*sqrt(real(ofdm_time_offset2[0]*conj(ofdm_time_offset2[0])))/total_variance;
    for(int i=1;i<Nfft+Ncp;i++){
           ofdm_time_offset2[i]=ofdm_time_offset2[i-1];
        for(int j=0;j<(Number_of_received_symbols-Nfft)/(Nfft+Ncp);j++){
           ofdm_time_offset2[i]=ofdm_time_offset2[i]+correlation.get(j*(Nfft+Ncp)+Ncp-1+i)-correlation.get(j*(Nfft+Ncp)+i-1);

        }
        ofdm_time_offset[i]=1./(Ncp*((Number_of_received_symbols-Nfft)/(Nfft+Ncp)))*sqrt(real(ofdm_time_offset2[i]*conj(ofdm_time_offset2[i])))/total_variance;
    }

    ofdm_time_offset_estimate=max_index(ofdm_time_offset);
    double ofdm_frequency_offset_estimate=-1./(2*pi*Nfft)*arg(ofdm_time_offset2(ofdm_time_offset_estimate));
    ofdm_time_offset_estimate=0;
    ofdm_frequency_offset_estimate=0;
    // Correction of the time and frequency offsets
    for (int i=0;i<Number_of_received_symbols;i++)
        ofdm_frequency_offset[i]=-2*pi*ofdm_frequency_offset_estimate*i;
    cvec fo_rx_samples=elem_mult(exp(to_cvec(zeros(Number_of_received_symbols),ofdm_frequency_offset)),rx_buff);
    cvec to_fo_rx_samples=fo_rx_samples.get(mod(ofdm_time_offset_estimate+Ncp,Nfft+Ncp),Number_of_received_symbols-1);

    int Number_of_blocks_time_domain=Number_of_received_symbols/(Nfft+Ncp);
    cvec demodulated_ofdm_chips(Number_of_blocks_time_domain*Nfft);
    demodulated_ofdm_chips.zeros();
    for (int i=0;i<Number_of_blocks_time_domain;i++)
        demodulated_ofdm_chips.replace_mid(i*Nfft,(fft(to_fo_rx_samples.get(i*(Nfft+Ncp),(i+1)*(Nfft+Ncp)-Ncp-1))/sqrt(Nfft)).get(0,Nfft-1));

    int demodulated_ofdm_chips_size=demodulated_ofdm_chips.size();
    cvec despread_symbols(demodulated_ofdm_chips_size);
    cmat tmp(Nfft,1);
    tmp.zeros();
    despread_symbols.zeros();
    int number_blocks;
    number_blocks=demodulated_ofdm_chips_size/Nfft;
    cvec previous_differential_tmp;
    for(int i=0;i<number_blocks;i++){
        tmp.set_col(0,elem_mult(demodulated_ofdm_chips.get(i*Nfft,(i+1)*Nfft-1),to_cvec(scrambling)));
        if(i==0){
            differential_tmp=tmp.get_col(0);
        }
        else{
            differential_tmp=elem_div(elem_mult(tmp.get_col(0),conj(previous_differential_tmp)),to_cvec(abs(previous_differential_tmp)));
        }
        previous_differential_tmp=tmp.get_col(0);
        tmp.set_col(0,differential_tmp);
        despread_symbols.replace_mid(i*Nfft,(transpose(hadamard_matrix)*tmp).get_col(0));
    }
    received_symbols.set_size(demodulated_ofdm_chips_size/Nfft*Nu);
    received_symbols.zeros();
    int index=0;
    for(int i=0;i<number_blocks;i++){
        received_symbols.replace_mid(index,despread_symbols.get(i*Nfft,i*Nfft+Nu-1));
        index=index+Nu;
    }

    QPSK qpsk;
    bvec received_bits;
    received_bits=qpsk.demodulate_bits(received_symbols);


    return received_bits;
}

bool Modem_MCCDM::preamble_detection(bvec received_bits,bvec &received_bits2,int &preamble_start){

    vector<char> preamble(3);

    preamble[0]='!';
    preamble[1]='!';
    preamble[2]='H';

    bvec preamble_bits=charvec2bvec(preamble);

    bool preamble_ok=false;
    int i=0;
    preamble_start=0;
    while((i<int(received_bits.size())-144)&&(preamble_ok==false)){
        if((preamble_bits==received_bits.get(i,i+24-1))&&(preamble_bits==received_bits.get(i+120,i+144-1))){
            preamble_ok=true;
            preamble_start=i;
        }
        i++;
    }


    received_bits2.set_size(received_bits.size()-preamble_start);
    received_bits2=received_bits.get(preamble_start,received_bits.size()-1);


    return preamble_ok;

}

bool Modem_MCCDM::ack_detection(bvec received_bits,bvec &received_bits2,int &preamble_start){

    vector<char> preamble(3);

    preamble[0]='!';
    preamble[1]='!';
    preamble[2]='A';

    bvec preamble_bits=charvec2bvec(preamble);

    bool preamble_ok=false;
    int i=0;
    preamble_start=0;
    while((i<int(received_bits.size())-144)&&(preamble_ok==false)){
        if((preamble_bits==received_bits.get(i,i+24-1))&&(preamble_bits==received_bits.get(i+120,i+144-1))){
            preamble_ok=true;
            preamble_start=i;
        }
        i++;
    }

    received_bits2.set_size(received_bits.size()-preamble_start);
    received_bits2=received_bits.get(preamble_start,received_bits.size()-1);


    return preamble_ok;

}

bvec Modem_MCCDM::charvec2bvec(vector<char> input){
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
