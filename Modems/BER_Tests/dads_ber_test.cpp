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
#include "dads_ber_test.h"

DADS_BER_Test::DADS_BER_Test()
{
    modem=new Modem_DADS();
    int L=1;
    int FH=1;
    int SF=modem->SF;
    int delay=modem->delay;
    int qf=1;

    cmat fading;
    cvec channel1,channel2,channel3,channel4,channel5,channel6,channel7,channel8,channel9,channel10,channel11,channel12,channel13,channel14,channel15,channel16;
    bvec transmitted_bits;
    bvec received_bits;
    cvec sum_chips;
    cvec transmitted_symbols;
    cvec received_chips;
    double norm_fading;
    BERC berc;
    AWGN_Channel channel;

    vec EbN0dB = linspace(0, 40, 41);
    vec EbN0 = pow(10, EbN0dB / 10);
    double Eb = 1.0;
    vec N0 = Eb * pow(EbN0, -1.0);
    int NumOfBits = 64000;
    int MaxIterations = 10;
    int MaxNrOfErrors = 200;
    int MinNrOfErrors = 5;
    vec ber;
    ber.set_size(EbN0dB.size(), false);
    ber.clear();
    RNG_randomize();



    for (int i=0;i<EbN0dB.length();i++){

        cout << endl << "Simulating point nr " << i + 1 << endl;
        berc.clear();
        channel.set_noise(N0(i));

        for (int j=0;j<MaxIterations;j++) {

            transmitted_bits = randb(NumOfBits);
            sum_chips=modem->modulate(transmitted_bits);


            transmitted_symbols.set_length(sum_chips.length()+L+1);
            transmitted_symbols.zeros();

            fading.set_size(L,sum_chips.length());
            fading.zeros();

            channel1.set_length(sum_chips.length());
/*          channel2.set_length(sum_chips.length());
            channel3.set_length(sum_chips.length());
            channel4.set_length(sum_chips.length());
            channel5.set_length(sum_chips.length());
            channel6.set_length(sum_chips.length());
            channel7.set_length(sum_chips.length());
            channel8.set_length(sum_chips.length());
            channel9.set_length(sum_chips.length());
            channel10.set_length(sum_chips.length());
            channel11.set_length(sum_chips.length());
            channel12.set_length(sum_chips.length());
            channel13.set_length(sum_chips.length());
            channel14.set_length(sum_chips.length());
            channel15.set_length(sum_chips.length());
            channel16.set_length(sum_chips.length());*/


            for(int k=0;k<sum_chips.length()/SF*FH;k++){


                channel1.replace_mid(k*SF/FH+delay,ones_c(SF/FH));

/*              channel1.replace_mid(k*SF/FH+delay,randn_c()*ones(SF/FH));
                channel2.replace_mid(k*SF/FH+delay,randn_c()*ones(SF/FH));
                channel3.replace_mid(k*SF/FH+delay,randn_c()*ones(SF/FH));
                channel4.replace_mid(k*SF/FH+delay,randn_c()*ones(SF/FH));
                channel5.replace_mid(k*SF/FH+delay,randn_c()*ones(SF/FH));
                channel6.replace_mid(k*SF/FH+delay,randn_c()*ones(SF/FH));
                channel7.replace_mid(k*SF/FH+delay,randn_c()*ones(SF/FH));
                channel8.replace_mid(k*SF/FH+delay,randn_c()*ones(SF/FH));
                channel9.replace_mid(k*SF/FH+delay,randn_c()*ones(SF/FH));
                channel10.replace_mid(k*SF/FH+delay,randn_c()*ones(SF/FH));
                channel11.replace_mid(k*SF/FH+delay,randn_c()*ones(SF/FH));
                channel12.replace_mid(k*SF/FH+delay,randn_c()*ones(SF/FH));
                channel13.replace_mid(k*SF/FH+delay,randn_c()*ones(SF/FH));
                channel14.replace_mid(k*SF/FH+delay,randn_c()*ones(SF/FH));
                channel15.replace_mid(k*SF/FH+delay,randn_c()*ones(SF/FH));
                channel16.replace_mid(k*SF/FH+delay,randn_c()*ones(SF/FH));*/

            }


            for(int k=0;k<sum_chips.length()/SF;k++){
                for(int l=1;l<qf;l++){
                    channel1.replace_mid((k+l)*SF+delay,channel1.mid(k*SF+delay,SF));
/*                  channel2.replace_mid((k+l)*SF+delay,channel2.mid(k*SF+delay,SF));
                    channel3.replace_mid((k+l)*SF+delay,channel3.mid(k*SF+delay,SF));
                    channel4.replace_mid((k+l)*SF+delay,channel4.mid(k*SF+delay,SF));
                    channel5.replace_mid((k+l)*SF+delay,channel5.mid(k*SF+delay,SF));
                    channel6.replace_mid((k+l)*SF+delay,channel6.mid(k*SF+delay,SF));
                    channel7.replace_mid((k+l)*SF+delay,channel7.mid(k*SF+delay,SF));
                    channel8.replace_mid((k+l)*SF+delay,channel8.mid(k*SF+delay,SF));
                    channel9.replace_mid((k+l)*SF+delay,channel9.mid(k*SF+delay,SF));
                    channel10.replace_mid((k+l)*SF+delay,channel10.mid(k*SF+delay,SF));
                    channel11.replace_mid((k+l)*SF+delay,channel11.mid(k*SF+delay,SF));
                    channel12.replace_mid((k+l)*SF+delay,channel12.mid(k*SF+delay,SF));
                    channel13.replace_mid((k+l)*SF+delay,channel13.mid(k*SF+delay,SF));
                    channel14.replace_mid((k+l)*SF+delay,channel14.mid(k*SF+delay,SF));
                    channel15.replace_mid((k+l)*SF+delay,channel15.mid(k*SF+delay,SF));
                    channel16.replace_mid((k+l)*SF+delay,channel16.mid(k*SF+delay,SF));*/
                }
                for(int l=1;l<qf;l++){k++;}
            }




            norm_fading=1./sqrt(inv_dB(0)*norm(channel1)*norm(channel1)/sum_chips.length()/*+inv_dB(0)*norm(channel2)*norm(channel2)/sum_chips.length()+inv_dB(0)*norm(channel3)*norm(channel3)/sum_chips.length()+inv_dB(0)*norm(channel4)*norm(channel4)/sum_chips.length()+inv_dB(0)*norm(channel5)*norm(channel5)/sum_chips.length()+inv_dB(0)*norm(channel6)*norm(channel6)/sum_chips.length()+inv_dB(0)*norm(channel7)*norm(channel7)/sum_chips.length()+inv_dB(0)*norm(channel8)*norm(channel8)/sum_chips.length()+inv_dB(0)*norm(channel9)*norm(channel9)/sum_chips.length()+inv_dB(0)*norm(channel10)*norm(channel10)/sum_chips.length()+inv_dB(0)*norm(channel11)*norm(channel11)/sum_chips.length()+inv_dB(0)*norm(channel12)*norm(channel12)/sum_chips.length()+inv_dB(0)*norm(channel13)*norm(channel13)/sum_chips.length()+inv_dB(0)*norm(channel14)*norm(channel14)/sum_chips.length()+inv_dB(0)*norm(channel15)*norm(channel15)/sum_chips.length()+inv_dB(0)*norm(channel16)*norm(channel16)/sum_chips.length()*/);
            fading.set_row(0,norm_fading*channel1);
/*          fading.set_row(1,norm_fading*channel2);
            fading.set_row(2,norm_fading*channel3);
            fading.set_row(3,norm_fading*channel4);
            fading.set_row(4,norm_fading*channel5);
            fading.set_row(5,norm_fading*channel6);
            fading.set_row(6,norm_fading*channel7);
            fading.set_row(7,norm_fading*channel8);
            fading.set_row(8,norm_fading*channel9);
            fading.set_row(9,norm_fading*channel10);
            fading.set_row(10,norm_fading*channel11);
            fading.set_row(11,norm_fading*channel12);
            fading.set_row(12,norm_fading*channel13);
            fading.set_row(13,norm_fading*channel14);
            fading.set_row(14,norm_fading*channel15);
            fading.set_row(15,norm_fading*channel16);*/

            for (int k=0;k<L;k++){
                transmitted_symbols+=concat(zeros_c(k),elem_mult(to_cvec(sum_chips),fading.get_row(k)),zeros_c(L+1-k));
            }
            received_chips = channel(/*transmitted_symbols*/sum_chips);

            //received_chips = sum_chips;
            int time_offset;
            vec constellation;
            received_bits=modem->demodulate(received_chips,1,time_offset,constellation);
            berc.count(transmitted_bits, received_bits);
            ber(i) = berc.get_errorrate();

            cout << "   Iteration " << j + 1 << ": ber = " << berc.get_errorrate() << endl;
            if (berc.get_errors() > MaxNrOfErrors) {
                cout << "Breaking on point " << i + 1 << " with " << berc.get_errors() << " errors." << endl;
                break;
            }

        }

        if (berc.get_errors() < MinNrOfErrors) {
            cout << "Exiting Simulation on point " << i + 1 << endl;
            break;
        }

    }

    //Print results:
    cout << endl;
    cout << "EbN0dB = " << EbN0dB << endl;
    cout << "ber = " << ber << endl;

}



















