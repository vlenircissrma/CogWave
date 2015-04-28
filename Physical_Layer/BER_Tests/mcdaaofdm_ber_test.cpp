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
#include "mcdaaofdm_ber_test.h"

MCDAAOFDM_BER_Test::MCDAAOFDM_BER_Test()
{

    modem=new Modem_MCDAAOFDM();
    int L=1;
    int quasi_static=modem->Nfft+modem->Ncp;

    cmat fading;
    cvec channel1,channel2,channel3,channel4,channel5,channel6,channel7,channel8,channel9,channel10,channel11,channel12,channel13,channel14,channel15,channel16;
    bvec transmitted_bits;
    bvec received_bits;
    cvec modulated_ofdm;
    cvec transmitted_symbols;
    cvec received_ofdm;
    double norm_fading;
    BERC berc;
    AWGN_Channel channel;

    vec EbN0dB = linspace(0, 40, 41);
    vec EbN0 = pow(10, EbN0dB / 10);
    double Eb = 1.0;
    vec N0 = Eb * pow(EbN0, -1.0);
    int NumOfBits = 1000000;
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
            modulated_ofdm=sqrt(modem->Nfft+modem->Ncp)/sqrt(modem->Nfft)*modem->modulate_mask_qpsk(transmitted_bits,0);

            transmitted_symbols.set_length(modulated_ofdm.length()+L+1);
            transmitted_symbols.zeros();

            fading.set_size(L,modulated_ofdm.length());
            fading.zeros();

            channel1.set_length(modulated_ofdm.length());
/*          channel2.set_length(modulated_ofdm.length());
            channel3.set_length(modulated_ofdm.length());
            channel4.set_length(modulated_ofdm.length());
            channel5.set_length(modulated_ofdm.length());
            channel6.set_length(modulated_ofdm.length());
            channel7.set_length(modulated_ofdm.length());
            channel8.set_length(modulated_ofdm.length());
            channel9.set_length(modulated_ofdm.length());
            channel10.set_length(modulated_ofdm.length());
            channel11.set_length(modulated_ofdm.length());
            channel12.set_length(modulated_ofdm.length());
            channel13.set_length(modulated_ofdm.length());
            channel14.set_length(modulated_ofdm.length());
            channel15.set_length(modulated_ofdm.length());
            channel16.set_length(modulated_ofdm.length());*/


            for(int k=0;k<modulated_ofdm.length()/quasi_static;k++){


                channel1.replace_mid(k*quasi_static,ones_c(quasi_static));
                //complex<double> random_complex= randn_c();
                //double canal=sqrt(real(random_complex*conj(random_complex)));
              //channel1.replace_mid(k*quasi_static,canal*ones_c(quasi_static));
              //channel1.replace_mid(k*quasi_static,randn_c()*ones(quasi_static));
 /*               channel2.replace_mid(k*quasi_static,randn_c()*ones(quasi_static));
                channel3.replace_mid(k*quasi_static,randn_c()*ones(quasi_static));
                channel4.replace_mid(k*quasi_static,randn_c()*ones(quasi_static));
                channel5.replace_mid(k*quasi_static,randn_c()*ones(quasi_static));
                channel6.replace_mid(k*quasi_static,randn_c()*ones(quasi_static));
                channel7.replace_mid(k*quasi_static,randn_c()*ones(quasi_static));
                channel8.replace_mid(k*quasi_static,randn_c()*ones(quasi_static));
                channel9.replace_mid(k*quasi_static,randn_c()*ones(quasi_static));
                channel10.replace_mid(k*quasi_static,randn_c()*ones(quasi_static));
                channel11.replace_mid(k*quasi_static,randn_c()*ones(quasi_static));
                channel12.replace_mid(k*quasi_static,randn_c()*ones(quasi_static));
                channel13.replace_mid(k*quasi_static,randn_c()*ones(quasi_static));
                channel14.replace_mid(k*quasi_static,randn_c()*ones(quasi_static));
                channel15.replace_mid(k*quasi_static,randn_c()*ones(quasi_static));
                channel16.replace_mid(k*quasi_static,randn_c()*ones(quasi_static));*/

            }


            norm_fading=1./sqrt(inv_dB(0)*norm(channel1)*norm(channel1)/modulated_ofdm.length()/*+inv_dB(0)*norm(channel2)*norm(channel2)/modulated_ofdm.length()+inv_dB(0)*norm(channel3)*norm(channel3)/modulated_ofdm.length()+inv_dB(0)*norm(channel4)*norm(channel4)/modulated_ofdm.length()+inv_dB(0)*norm(channel5)*norm(channel5)/modulated_ofdm.length()+inv_dB(0)*norm(channel6)*norm(channel6)/modulated_ofdm.length()+inv_dB(0)*norm(channel7)*norm(channel7)/modulated_ofdm.length()+inv_dB(0)*norm(channel8)*norm(channel8)/modulated_ofdm.length()+inv_dB(0)*norm(channel9)*norm(channel9)/modulated_ofdm.length()+inv_dB(0)*norm(channel10)*norm(channel10)/modulated_ofdm.length()+inv_dB(0)*norm(channel11)*norm(channel11)/modulated_ofdm.length()+inv_dB(0)*norm(channel12)*norm(channel12)/modulated_ofdm.length()+inv_dB(0)*norm(channel13)*norm(channel13)/modulated_ofdm.length()+inv_dB(0)*norm(channel14)*norm(channel14)/modulated_ofdm.length()+inv_dB(0)*norm(channel15)*norm(channel15)/modulated_ofdm.length()+inv_dB(0)*norm(channel16)*norm(channel16)/modulated_ofdm.length()*/);
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
                transmitted_symbols+=concat(zeros_c(k),elem_mult(to_cvec(modulated_ofdm),fading.get_row(k)),zeros_c(L+1-k));
            }
            received_ofdm = channel(transmitted_symbols);

            //received_chips = sum_chips;
            cvec constellation;
            vec estimated_channel;
            double metric;
            //bool is_ofdm=modem->detection(received_ofdm,metric);
            modem->time_offset_estimate=0;
            modem->frequency_offset_estimate=0;
            cvec demodulated_ofdm_symbols=modem->equalizer_fourth_power(received_ofdm,0,estimated_channel);
            received_bits=modem->demodulate_mask_gray_qpsk(demodulated_ofdm_symbols,0,constellation);
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
