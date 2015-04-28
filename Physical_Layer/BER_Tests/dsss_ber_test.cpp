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
#include "dsss_ber_test.h"
#include <QVector>

DSSS_BER_Test::DSSS_BER_Test()
{
    modem=new Modem_DSSS();
    int L=1;
    int OF=modem->OF;
    int SF=modem->SF;

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

    vec EbN0dB = linspace(15, 15, 1);
    vec EbN0 = pow(10, EbN0dB / 10);
    double Eb = 1.0;
    vec N0 = Eb * pow(EbN0, -1.0);
    int NumOfBits = modem->nb_bits;
    int MaxIterations = 1;
    int MaxNrOfErrors = 200;
    int MinNrOfErrors = 0;
    vec ber;
    ber.set_size(EbN0dB.size(), false);
    ber.clear();
    RNG_randomize();
    vec mse1;
    mse1.set_size(EbN0dB.size(), false);
    mse1.clear();
    vec mse2;
    mse2.set_size(EbN0dB.size(), false);
    mse2.clear();
    vec mse3;
    mse3.set_size(EbN0dB.size(), false);
    mse3.clear();
    vec mse4;
    mse4.set_size(EbN0dB.size(), false);
    mse4.clear();
    vec mse5;
    mse5.set_size(EbN0dB.size(), false);
    mse5.clear();
    vec mse6;
    mse6.set_size(EbN0dB.size(), false);
    mse6.clear();
    vec mse7;
    mse7.set_size(EbN0dB.size(), false);
    mse7.clear();
    vec mse8;
    mse8.set_size(EbN0dB.size(), false);
    mse8.clear();
    vec mse9;
    mse9.set_size(EbN0dB.size(), false);
    mse9.clear();
    vec mse10;
    mse10.set_size(EbN0dB.size(), false);
    mse10.clear();
    vec mse11;
    mse11.set_size(EbN0dB.size(), false);
    mse11.clear();
    vec mse12;
    mse12.set_size(EbN0dB.size(), false);
    mse12.clear();

    transmitted_bits.set_size(NumOfBits);
    transmitted_bits.zeros();

    for (int i=0;i<EbN0dB.length();i++){
    //for (int i=0;i<11;i++){
        cout << endl << "Simulating point nr " << i + 1 << endl;
        berc.clear();
        channel.set_noise(N0(i));
        modem->phi_true=2*pi*1.0e-2;
        //modem->phi_true=2*pi*(-0.25+0.05*i);
        //channel.set_noise(N0(0));

        for (int j=0;j<MaxIterations;j++) {


            transmitted_bits = randb(NumOfBits);
            //transmitted_bits = to_bvec(ones(NumOfBits));
            sum_chips=modem->modulate(transmitted_bits);

            transmitted_symbols.set_length(sum_chips.length()+L+1);
            transmitted_symbols.zeros();

            fading.set_size(L,sum_chips.length());
            fading.zeros();

            channel1.set_length(sum_chips.length());

            std::complex<double> x=randn_c();
            for(int k=0;k<sum_chips.length()/(OF*SF);k++){

                channel1.replace_mid(k*(OF*SF),/*x**/ones_c(OF*SF));

            }
            //FIR_Fading_Generator tap1(1.0e-2,100);
            //tap1.generate(sum_chips.length(),channel1);

            norm_fading=1./sqrt(inv_dB(0)*norm(channel1)*norm(channel1)/sum_chips.length());
            fading.set_row(0,norm_fading*channel1);


            for (int k=0;k<L;k++){
                transmitted_symbols+=concat(zeros_c(k),elem_mult(to_cvec(sum_chips),fading.get_row(k)),zeros_c(L+1-k));
            }
            modem->mse1=0;
            modem->mse2=0;
            modem->mse3=0;
            modem->mse4=0;
            modem->mse5=0;
            modem->mse6=0;
            modem->mse7=0;
            modem->mse8=0;
            modem->mse9=0;
            modem->mse10=0;
            modem->mse11=0;
            modem->mse12=0;
            int monte_carlo=1;
            for (int k=0;k<monte_carlo;k++){

                received_chips = channel(transmitted_symbols);
                //double frequency_offset=-0.25+0.05*i;
                double frequency_offset=1.0e-2;
                cvec fo_received_chips(received_chips.size());
                for(int k=0;k<received_chips.size();k++)
                    fo_received_chips(k)=received_chips(k)*exp(std::complex<double>(0,2*pi*k*frequency_offset));
                int time_offset=0;
                if(time_offset==0)
                    received_chips=fo_received_chips.get(0,fo_received_chips.size()-1);
                else
                    received_chips=concat(fo_received_chips.get(fo_received_chips.size()-1-(time_offset-1),fo_received_chips.size()-1),fo_received_chips.get(0,fo_received_chips.size()-2-(time_offset-1)));
                cvec constellation;
                int time_offset_estimate=0;
                received_bits=modem->demodulate(received_chips,time_offset_estimate,constellation);

            }
            mse1(i)=modem->mse1/monte_carlo;
            mse2(i)=modem->mse2/monte_carlo;
            mse3(i)=modem->mse3/monte_carlo;
            mse4(i)=modem->mse4/monte_carlo;
            mse5(i)=modem->mse5/monte_carlo;
            mse6(i)=modem->mse6/monte_carlo;
            mse7(i)=modem->mse7/monte_carlo;
            mse8(i)=modem->mse8/monte_carlo;
            mse9(i)=modem->mse9/monte_carlo;
            mse10(i)=modem->mse10/monte_carlo;
            mse11(i)=modem->mse11/monte_carlo;
            mse12(i)=modem->mse12/monte_carlo;
            berc.count(transmitted_bits, received_bits);
            cout << transmitted_bits << endl;
            cout << received_bits << endl;
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
    cout << "SNR = " << EbN0dB - 10*log10(modem->SF) << endl;
    /*cout << "mse1 = " << mse1 << endl;
    cout << "mse2 = " << mse2 << endl;
    cout << "mse3 = " << mse3 << endl;
    cout << "mse4 = " << mse4 << endl;
    cout << "mse5 = " << mse5 << endl;
    cout << "mse6 = " << mse6 << endl;
    cout << "mse7 = " << mse7 << endl;
    cout << "mse8 = " << mse8 << endl;
    cout << "mse9 = " << mse9 << endl;
    cout << "mse10 = " << mse10 << endl;
    cout << "mse11 = " << mse11 << endl;
    cout << "mse12 = " << mse12 << endl;*/
}
