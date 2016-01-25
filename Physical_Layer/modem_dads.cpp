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
#include "modem_dads.h"

Modem_DADS::Modem_DADS()
{
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


}



cvec Modem_DADS::modulate(bvec data_packet)
{
    BPSK bpsk;
    vec mapped_transmited_symbols=bpsk.modulate_bits(data_packet);


    vec reference;
    reference.set_size(mapped_transmited_symbols.size());
    reference=ones(mapped_transmited_symbols.size());

    vec transmitted_chips;
    transmitted_chips=spread.spread(mapped_transmited_symbols);



    vec sum_chips;
    sum_chips.set_size(transmitted_chips.size()+SF);
    sum_chips.zeros();
    sum_chips.set_subvector(0,1./sqrt(2)*concat(spread.spread(reference),zeros(delay))+1./sqrt(2)*concat(zeros(delay),transmitted_chips));

    return to_cvec(sum_chips);
}



bvec Modem_DADS::demodulate(cvec rx_buff, int OF, int &time_offset, vec &received_symbols)
{
    BPSK bpsk;

    int SF_OF=SF*OF;
    int SF_pseudo_OF=SF_pseudo*OF;
    int delay_OF=delay*OF;
    cvec temp_received_chips;
    cvec noise_received_chips;
    temp_received_chips.set_size(SF_pseudo_OF);
    temp_received_chips.zeros();
    noise_received_chips.set_size(rx_buff.size());
    /*for(int k=0;k<rx_buff.size()/SF_OF;k++){
           temp_received_chips=temp_received_chips+rx_buff.get(k*SF_OF,(k+1)*SF_OF-1);
    }
    for(int l=0;l<rx_buff.size()/SF_OF;l++){
        noise_received_chips.replace_mid(l*SF_OF,temp_received_chips/(rx_buff.size()/SF_OF));
    }

    cout << temp_received_chips/(rx_buff.size()/SF_OF) << endl;*/

    for(int l=0;l<rx_buff.size()/SF_OF;l++){
        temp_received_chips.zeros();
            for(int m=0;m<SF/SF_pseudo;m++){
                temp_received_chips=temp_received_chips+rx_buff.get(l*SF_OF+m*SF_pseudo_OF,l*SF_OF+(m+1)*SF_pseudo_OF-1)/(SF/SF_pseudo);
            }
            for(int m=0;m<SF/SF_pseudo;m++){
                    noise_received_chips.replace_mid(l*SF_OF+m*SF_pseudo_OF,temp_received_chips);
            }
    }
    //cout << temp_received_chips << endl;

    cvec shifted_received_chips;
    shifted_received_chips.set_size(rx_buff.size()+delay_OF);
    shifted_received_chips=elem_mult(concat(rx_buff,zeros_c(delay_OF)),conj(concat(zeros_c(delay_OF),noise_received_chips)));

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
    time_offset_estimate=max_index(metric2);
    //cout << time_offset_estimate << endl;
    time_offset=time_offset_estimate;
    received_symbols.set_size(shifted_received_chips.size()/SF_OF);

    for(int k=0;k<shifted_received_chips.size()/SF_OF;k++){
            //received_symbols(k) = sum(real(shifted_received_chips.get(k*SF+2*delay,(k+1)*SF+delay-1)));
            received_symbols(k) = metric(k,time_offset_estimate);
    }

    double normalization=sqrt(1./shifted_received_chips.size()/SF_OF)*itpp::norm(received_symbols);
    received_symbols=1./(SF_OF*normalization)*received_symbols;
    bvec received_bits = bpsk.demodulate_bits(received_symbols);

    return received_bits;
}

bool Modem_DADS::preamble_detection(bvec received_bits,bvec &received_bits2,int &preamble_start){

    vector<char> preamble(3);

    preamble[0]='!';
    preamble[1]='!';
    preamble[2]='H';

    bvec preamble_bits=charvec2bvec(preamble);

    bool preamble_ok=false;
    int i=0;
    preamble_start=0;
    while((i<=int(received_bits.size())-144)&&(preamble_ok==false)){
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

bool Modem_DADS::ack_detection(bvec received_bits,bvec &received_bits2,int &preamble_start){

    vector<char> preamble(3);

    preamble[0]='!';
    preamble[1]='!';
    preamble[2]='A';

    bvec preamble_bits=charvec2bvec(preamble);

    bool preamble_ok=false;
    int i=0;
    preamble_start=0;
    while((i<=int(received_bits.size())-144)&&(preamble_ok==false)){
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

bvec Modem_DADS::charvec2bvec(vector<char> input){
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
