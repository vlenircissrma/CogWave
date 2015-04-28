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
#include "modem_mcdads.h"

Modem_MCDADS::Modem_MCDADS()
{
    Nfft=512;
    Ncp=128;
    delay=2;
    SF = 4;
    SF_pseudo=4;
    vec pseudo(SF_pseudo);
    nb_bits=63040;
    //Random Code Generation
    RNG_randomize();
    vec code = to_vec(randb(SF));
    for(int i=0;i<SF;i++){
     if(code(i)==0)
           code(i)=-1;
    }
    if(SF_pseudo==4){
     pseudo(0)=-1;
     pseudo(1)=1;
     pseudo(2)=-1;
     pseudo(3)=-1;

    }
    if(SF_pseudo==16){
     pseudo(0)=1;
     pseudo(1)=1;
     pseudo(2)=-1;
     pseudo(3)=-1;
     pseudo(4)=-1;
     pseudo(5)=1;
     pseudo(6)=1;
     pseudo(7)=-1;
     pseudo(8)=1;
     pseudo(9)=1;
     pseudo(10)=1;
     pseudo(11)=1;
     pseudo(12)=1;
     pseudo(13)=1;
     pseudo(14)=-1;
     pseudo(15)=-1;
    }
    if(SF_pseudo==64){
     pseudo(0)=-1;
     pseudo(1)=-1;
     pseudo(2)=1;
     pseudo(3)=1;
     pseudo(4)=-1;
     pseudo(5)=-1;
     pseudo(6)=1;
     pseudo(7)=-1;
     pseudo(8)=1;
     pseudo(9)=1;
     pseudo(10)=-1;
     pseudo(11)=1;
     pseudo(12)=1;
     pseudo(13)=-1;
     pseudo(14)=-1;
     pseudo(15)=1;
     pseudo(16)=1;
     pseudo(17)=1;
     pseudo(18)=1;
     pseudo(19)=-1;
     pseudo(20)=1;
     pseudo(21)=1;
     pseudo(22)=-1;
     pseudo(23)=1;
     pseudo(24)=-1;
     pseudo(25)=1;
     pseudo(26)=-1;
     pseudo(27)=1;
     pseudo(28)=-1;
     pseudo(29)=-1;
     pseudo(30)=1;
     pseudo(31)=1;
     pseudo(32)=-1;
     pseudo(33)=-1;
     pseudo(34)=1;
     pseudo(35)=-1;
     pseudo(36)=1;
     pseudo(37)=-1;
     pseudo(38)=-1;
     pseudo(39)=-1;
     pseudo(40)=-1;
     pseudo(41)=-1;
     pseudo(42)=1;
     pseudo(43)=-1;
     pseudo(44)=1;
     pseudo(45)=1;
     pseudo(46)=-1;
     pseudo(47)=-1;
     pseudo(48)=-1;
     pseudo(49)=1;
     pseudo(50)=1;
     pseudo(51)=-1;
     pseudo(52)=-1;
     pseudo(53)=1;
     pseudo(54)=1;
     pseudo(55)=1;
     pseudo(56)=-1;
     pseudo(57)=1;
     pseudo(58)=1;
     pseudo(59)=-1;
     pseudo(60)=1;
     pseudo(61)=-1;
     pseudo(62)=-1;
     pseudo(63)=1;
    }
    if(SF_pseudo==256){
   pseudo(0)=1;
   pseudo(1)=-1;
   pseudo(2)=1;
   pseudo(3)=-1;
   pseudo(4)=-1;
   pseudo(5)=-1;
   pseudo(6)=-1;
   pseudo(7)=1;
   pseudo(8)=-1;
   pseudo(9)=-1;
   pseudo(10)=-1;
   pseudo(11)=1;
   pseudo(12)=1;
   pseudo(13)=-1;
   pseudo(14)=1;
   pseudo(15)=-1;
   pseudo(16)=-1;
   pseudo(17)=-1;

   pseudo(18)=-1;
   pseudo(19)=-1;
   pseudo(20)=1;
   pseudo(21)=-1;
   pseudo(22)=1;
   pseudo(23)=1;
   pseudo(24)=1;
   pseudo(25)=1;
   pseudo(26)=1;
   pseudo(27)=-1;
   pseudo(28)=-1;
   pseudo(29)=1;
   pseudo(30)=-1;
   pseudo(31)=-1;
   pseudo(32)=1;
   pseudo(33)=1;
   pseudo(34)=1;
   pseudo(35)=-1;

   pseudo(36)=1;
   pseudo(37)=1;
   pseudo(38)=-1;
   pseudo(39)=-1;
   pseudo(40)=-1;
   pseudo(41)=-1;
   pseudo(42)=1;
   pseudo(43)=1;
   pseudo(44)=1;
   pseudo(45)=-1;
   pseudo(46)=-1;
   pseudo(47)=1;
   pseudo(48)=1;
   pseudo(49)=-1;
   pseudo(50)=-1;
   pseudo(51)=-1;
   pseudo(52)=-1;
   pseudo(53)=-1;

   pseudo(54)=-1;
   pseudo(55)=-1;
   pseudo(56)=-1;
   pseudo(57)=-1;
   pseudo(58)=-1;
   pseudo(59)=-1;
   pseudo(60)=-1;
   pseudo(61)=-1;
   pseudo(62)=1;
   pseudo(63)=1;
   pseudo(64)=1;
   pseudo(65)=-1;
   pseudo(66)=-1;
   pseudo(67)=-1;
   pseudo(68)=-1;
   pseudo(69)=1;
   pseudo(70)=1;
   pseudo(71)=-1;

   pseudo(120)=-1;
   pseudo(73)=1;
   pseudo(74)=-1;
   pseudo(75)=1;
   pseudo(76)=1;
   pseudo(77)=-1;
   pseudo(78)=1;
   pseudo(79)=-1;
   pseudo(80)=-1;
   pseudo(81)=1;
   pseudo(82)=-1;
   pseudo(83)=1;
   pseudo(84)=1;
   pseudo(85)=-1;
   pseudo(86)=-1;
   pseudo(87)=-1;
   pseudo(88)=1;
   pseudo(89)=1;

   pseudo(90)=-1;
   pseudo(91)=1;
   pseudo(92)=1;
   pseudo(93)=-1;
   pseudo(94)=-1;
   pseudo(95)=-1;
   pseudo(144)=1;
   pseudo(97)=1;
   pseudo(98)=-1;
   pseudo(99)=-1;
   pseudo(100)=1;
   pseudo(101)=-1;
   pseudo(102)=-1;
   pseudo(103)=-1;
   pseudo(104)=1;
   pseudo(105)=-1;
   pseudo(106)=1;
   pseudo(107)=-1;

   pseudo(108)=-1;
   pseudo(109)=-1;
   pseudo(110)=1;
   pseudo(111)=-1;
   pseudo(112)=1;
   pseudo(113)=1;
   pseudo(114)=-1;
   pseudo(115)=1;
   pseudo(116)=-1;
   pseudo(117)=-1;
   pseudo(118)=-1;
   pseudo(119)=1;
   pseudo(120)=1;
   pseudo(121)=1;
   pseudo(122)=1;
   pseudo(123)=1;
   pseudo(124)=1;
   pseudo(125)=1;

   pseudo(126)=-1;
   pseudo(127)=1;
   pseudo(128)=1;
   pseudo(129)=1;
   pseudo(130)=1;
   pseudo(131)=1;
   pseudo(132)=-1;
   pseudo(133)=1;
   pseudo(134)=-1;
   pseudo(135)=-1;
   pseudo(136)=1;
   pseudo(137)=1;
   pseudo(138)=-1;
   pseudo(139)=1;
   pseudo(140)=-1;
   pseudo(141)=1;
   pseudo(142)=1;
   pseudo(143)=-1;

   pseudo(144)=1;
   pseudo(145)=-1;
   pseudo(146)=-1;
   pseudo(147)=-1;
   pseudo(148)=-1;
   pseudo(149)=1;
   pseudo(150)=-1;
   pseudo(151)=-1;
   pseudo(152)=-1;
   pseudo(153)=1;
   pseudo(154)=-1;
   pseudo(155)=1;
   pseudo(156)=1;
   pseudo(157)=-1;
   pseudo(158)=-1;
   pseudo(159)=-1;
   pseudo(160)=1;
   pseudo(161)=1;

   pseudo(162)=1;
   pseudo(163)=-1;
   pseudo(164)=-1;
   pseudo(165)=1;
   pseudo(166)=1;
   pseudo(167)=-1;
   pseudo(168)=1;
   pseudo(169)=-1;
   pseudo(170)=-1;
   pseudo(171)=1;
   pseudo(1120)=1;
   pseudo(173)=-1;
   pseudo(174)=-1;
   pseudo(175)=1;
   pseudo(176)=-1;
   pseudo(177)=1;
   pseudo(178)=-1;
   pseudo(179)=1;

   pseudo(180)=-1;
   pseudo(181)=1;
   pseudo(182)=-1;
   pseudo(183)=1;
   pseudo(184)=1;
   pseudo(185)=-1;
   pseudo(186)=-1;
   pseudo(187)=-1;
   pseudo(188)=1;
   pseudo(189)=-1;
   pseudo(190)=-1;
   pseudo(191)=1;
   pseudo(192)=-1;
   pseudo(193)=1;
   pseudo(194)=1;
   pseudo(195)=-1;
   pseudo(1144)=1;
   pseudo(197)=-1;


   pseudo(198)=-1;
   pseudo(199)=-1;
   pseudo(200)=1;
   pseudo(201)=1;
   pseudo(202)=-1;
   pseudo(203)=1;
   pseudo(204)=-1;
   pseudo(205)=-1;
   pseudo(206)=1;
   pseudo(207)=-1;
   pseudo(208)=-1;
   pseudo(209)=1;
   pseudo(210)=1;
   pseudo(211)=-1;
   pseudo(212)=-1;
   pseudo(213)=1;
   pseudo(214)=1;
   pseudo(215)=1;

   pseudo(216)=1;
   pseudo(217)=-1;
   pseudo(218)=1;
   pseudo(219)=-1;
   pseudo(220)=-1;
   pseudo(221)=-1;
   pseudo(222)=1;
   pseudo(223)=1;
   pseudo(224)=1;
   pseudo(225)=1;
   pseudo(226)=1;
   pseudo(227)=-1;
   pseudo(228)=1;
   pseudo(229)=1;
   pseudo(230)=-1;
   pseudo(231)=-1;
   pseudo(232)=1;
   pseudo(233)=1;

   pseudo(234)=1;
   pseudo(235)=1;
   pseudo(236)=-1;
   pseudo(237)=1;
   pseudo(238)=1;
   pseudo(239)=-1;
   pseudo(240)=1;
   pseudo(241)=1;
   pseudo(242)=1;
   pseudo(243)=-1;
   pseudo(244)=-1;
   pseudo(245)=1;
   pseudo(246)=-1;
   pseudo(247)=1;
   pseudo(248)=-1;
   pseudo(249)=-1;
   pseudo(250)=-1;
   pseudo(251)=1;

   pseudo(252)=-1;
   pseudo(253)=-1;
   pseudo(254)=-1;
   pseudo(255)=-1;
  }

    for(int j=0;j<SF/SF_pseudo;j++){
        code.set_subvector(j*SF_pseudo,pseudo);
    }

    spread.set_code(code);


    int group=0;
    num_subchannels=1;
    mask.set_size(Nfft);
    mask.zeros();
    if(num_subchannels==1)
        mask.replace_mid(Nfft/32,concat(ones_i(Nfft/2-3*Nfft/32),zeros_i(2*Nfft/16),ones_i(Nfft/2-3*Nfft/32)));
    else{
        if(group<num_subchannels/2)
            mask.replace_mid(Nfft/32+group*(Nfft-3*Nfft/16)/num_subchannels,ones_i((Nfft-3*Nfft/16)/num_subchannels));
        if(group>=num_subchannels/2)
            mask.replace_mid(Nfft/32+2*Nfft/16+group*(Nfft-3*Nfft/16)/num_subchannels,ones_i((Nfft-3*Nfft/16)/num_subchannels));
    }
    //mask.ones();
    sum_mask=sum(mask);


}



cvec Modem_MCDADS::modulate(bvec data_packet)
{

    bvec diff_data_packet(data_packet.size());
    diff_data_packet.zeros();
    for(int i=0;i<data_packet.size();i++){
        if(i==0)
            diff_data_packet(i)=data_packet(i)+bin(1);
        else
            diff_data_packet(i)=data_packet(i)+diff_data_packet(i-1);
    }
    BPSK bpsk;
    cvec sum_chips;
    sum_chips=to_cvec(bpsk.modulate_bits(diff_data_packet));

    /*BPSK bpsk;
    vec mapped_transmited_symbols=bpsk.modulate_bits(data_packet);


    vec reference;
    reference.set_size(mapped_transmited_symbols.size());
    reference=ones(mapped_transmited_symbols.size());

    vec transmitted_chips;
    transmitted_chips=spread.spread(mapped_transmited_symbols);



    cvec sum_chips;
    sum_chips.set_size(transmitted_chips.size()+SF);
    sum_chips.zeros();
    sum_chips.set_subvector(0,to_cvec(1./sqrt(2)*concat(spread.spread(reference),zeros(delay))+1./sqrt(2)*concat(zeros(delay),transmitted_chips)));*/

    //cvec ofdm_chips=sum_chips;

    int mask_sum_chips_size;
    if((sum_chips.size()/sum_mask)*sum_mask!=sum_chips.size())
        mask_sum_chips_size=(sum_chips.size()/sum_mask+1)*Nfft;
    else
        mask_sum_chips_size=(sum_chips.size()/sum_mask)*Nfft;
    cvec mask_sum_chips(mask_sum_chips_size);
    mask_sum_chips.zeros();
    int number_blocks_mask;
    if((mask_sum_chips_size/Nfft/num_subchannels)*num_subchannels!=mask_sum_chips_size/Nfft)
        number_blocks_mask=mask_sum_chips_size/Nfft/num_subchannels+1;
    else
        number_blocks_mask=mask_sum_chips_size/Nfft/num_subchannels;
    int index=0;
    for(int i=0;i<number_blocks_mask;i++){
      for(int j=0;j<num_subchannels;j++){
        for(int k=0;k<Nfft;k++){
          if(mask[k]>0){
            //mask mapping
              if(index<sum_chips.size()){
                  mask_sum_chips.replace_mid(i*num_subchannels*Nfft+j*Nfft+k,sum_chips.get(index,index+mask[k]-1));
                index=index+mask[k];
              }
          }
        }
      }
    }


    int ofdm_chips_size;
    ofdm_chips_size=mask_sum_chips.size()/Nfft*(Nfft+Ncp);
    cvec ofdm_chips;
    ofdm_chips.set_size(ofdm_chips_size);
    ofdm_chips.zeros();

    for(int i=0;i<mask_sum_chips.size()/Nfft;i++){
       ofdm_chips.replace_mid(i*(Nfft+Ncp),concat((ifft(mask_sum_chips.get(i*Nfft,(i+1)*Nfft-1))*sqrt(Nfft)).right(Ncp),ifft(mask_sum_chips.get(i*Nfft,(i+1)*Nfft-1))*sqrt(Nfft)));
    }

    return ofdm_chips;
}



bvec Modem_MCDADS::demodulate(cvec rx_buff, int OF, int &time_offset1, int &time_offset2, vec &received_symbols)
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

    int ofdm_time_offset_estimate=max_index(ofdm_time_offset);
    double ofdm_frequency_offset_estimate=-1./(2*pi*Nfft)*arg(ofdm_time_offset2(ofdm_time_offset_estimate));

    // Correction of the time and frequency offsets
    for (int i=0;i<Number_of_received_symbols;i++)
        ofdm_frequency_offset[i]=-2*pi*ofdm_frequency_offset_estimate*i;
    cvec fo_rx_samples=elem_mult(exp(to_cvec(zeros(Number_of_received_symbols),ofdm_frequency_offset)),rx_buff);
    cvec to_fo_rx_samples=fo_rx_samples.get(mod(ofdm_time_offset_estimate+Ncp,Nfft+Ncp),Number_of_received_symbols-1);

    cvec demodulated_ofdm_chips((Number_of_received_symbols/(Nfft+Ncp))*Nfft);
    demodulated_ofdm_chips.zeros();
    for (int i=0;i<Number_of_received_symbols/(Nfft+Ncp);i++)
        demodulated_ofdm_chips.replace_mid(i*Nfft,(fft(to_fo_rx_samples.get(i*(Nfft+Ncp),(i+1)*(Nfft+Ncp)-Ncp-1))/sqrt(Nfft)).get(0,Nfft-1));

    //cvec demodulated_ofdm_chips=rx_buff;
    int number_blocks_mask=(demodulated_ofdm_chips.size()/Nfft);
    cvec mask_demodulated_ofdm_chips(number_blocks_mask*sum_mask);
    mask_demodulated_ofdm_chips.zeros();
    int index=0;
    int index2=0;
    for (int i=0;i<number_blocks_mask;i++){
        for(int j=0;j<Nfft;j++){
            if(mask[j]>0){
                mask_demodulated_ofdm_chips.replace_mid(index2,demodulated_ofdm_chips.get(i*Nfft+j,i*Nfft+j));
                index=index+mask[j];
                index2=index2+1;
            }
        }
    }


    /*BPSK bpsk;
    int SF_OF=SF*OF;
    int SF_pseudo_OF=SF_pseudo*OF;
    int delay_OF=delay*OF;
    cvec temp_received_chips;
    cvec noise_received_chips;
    temp_received_chips.set_size(SF_pseudo_OF);
    temp_received_chips.zeros();
    noise_received_chips.set_size(mask_demodulated_ofdm_chips.size());
    for(int l=0;l<mask_demodulated_ofdm_chips.size()/SF_OF;l++){
        temp_received_chips.zeros();
            for(int m=0;m<SF/SF_pseudo;m++){
                temp_received_chips=temp_received_chips+mask_demodulated_ofdm_chips.get(l*SF_OF+m*SF_pseudo_OF,l*SF_OF+(m+1)*SF_pseudo_OF-1)/(SF/SF_pseudo);
            }
            for(int m=0;m<SF/SF_pseudo;m++){
                    noise_received_chips.replace_mid(l*SF_OF+m*SF_pseudo_OF,temp_received_chips);
            }
    }
    cvec shifted_received_chips;
    shifted_received_chips.set_size(mask_demodulated_ofdm_chips.size()+delay_OF);
    shifted_received_chips=elem_mult(concat(mask_demodulated_ofdm_chips,zeros_c(delay_OF)),conj(concat(zeros_c(delay_OF),noise_received_chips)));

    mat metric;
    metric.set_size(shifted_received_chips.size()/SF_OF,SF_OF);
    vec metric2;
    metric2.set_size(SF_OF);
    metric2.zeros();

    for(int k=0;k<shifted_received_chips.size()/SF_OF-1;k++){
           metric(k,0) = sum(real(shifted_received_chips.get(k*SF_OF+2*delay_OF,(k+1)*SF_OF+delay_OF-1)));
           metric2(0)=metric2(0)+sign(metric(k,0))*metric(k,0);
    }

    for(int j=1;j<SF_OF;j++){
        for(int k=0;k<shifted_received_chips.size()/SF_OF-2;k++){
               metric(k,j) = metric(k,j-1)+real(shifted_received_chips.get((k+1)*SF_OF+delay_OF+j-1))-real(shifted_received_chips.get(k*SF_OF+2*delay_OF+j-1));
               metric2(j)=metric2(j)+sign(metric(k,j))*metric(k,j);
        }
    }


    //cout << metric2 << endl;
    int dads_time_offset_estimate=max_index(metric2);
    time_offset1=ofdm_time_offset_estimate;
    time_offset2=dads_time_offset_estimate;
    received_symbols.set_size(shifted_received_chips.size()/SF_OF);

    for(int k=0;k<shifted_received_chips.size()/SF_OF;k++){
            //received_symbols(k) = sum(real(shifted_received_chips.get(k*SF+2*delay,(k+1)*SF+delay-1)));
            received_symbols(k) = metric(k,dads_time_offset_estimate);
    }

    double normalization=sqrt(1./shifted_received_chips.size()/SF_OF)*itpp::norm(received_symbols);
    received_symbols=1./(SF_OF*normalization)*received_symbols;
    bvec received_bits = bpsk.demodulate_bits(received_symbols);*/

    BPSK bpsk;
    bvec received_bits(mask_demodulated_ofdm_chips.size());
    for(int i=0;i<mask_demodulated_ofdm_chips.size();i++){
        if(i==0){
            mask_demodulated_ofdm_chips=conj(mask_demodulated_ofdm_chips[0])*mask_demodulated_ofdm_chips;
            received_bits(i)=bpsk.demodulate_bits(real(mask_demodulated_ofdm_chips.get(0,0)))[0];
        }
        else
            received_bits(i)=bpsk.demodulate_bits(real(elem_mult(mask_demodulated_ofdm_chips.get(i,i),conj(mask_demodulated_ofdm_chips.get(i-1,i-1)))))[0];
    }

    return received_bits;
}

bool Modem_MCDADS::preamble_detection(bvec received_bits,bvec &received_bits2,int &preamble_start){

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

bool Modem_MCDADS::ack_detection(bvec received_bits,bvec &received_bits2,int &preamble_start){

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



bvec Modem_MCDADS::charvec2bvec(vector<char> input){
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
