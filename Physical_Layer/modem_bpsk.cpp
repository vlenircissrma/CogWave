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
#include "modem_bpsk.h"

Modem_BPSK::Modem_BPSK()
{
    nb_bits=63040;
    OF=4;
    roll_off=0.5;
    int filter_length=6;
    rrc.set_pulse_shape(roll_off,filter_length,OF);

}


vec Modem_BPSK::generate_rrc_taps(double gain,double sampling_freq,double symbol_rate,double alpha,int ntaps)
{
    ntaps |= 1;	// ensure that ntaps is odd

    double spb = sampling_freq/symbol_rate; // samples per bit/symbol
    vec taps(ntaps);
    double scale = 0;
    for(int i = 0; i < ntaps; i++) {
        double x1,x2,x3,num,den;
        double xindx = i - ntaps/2;
        x1 = M_PI * xindx/spb;
        x2 = 4 * alpha * xindx / spb;
        x3 = x2*x2 - 1;

        if(fabs(x3) >= 0.000001) {  // Avoid Rounding errors...
            if(i != ntaps/2)
                num = cos((1+alpha)*x1) + sin((1-alpha)*x1)/(4*alpha*xindx/spb);
            else
                num = cos((1+alpha)*x1) + (1-alpha) * M_PI / (4*alpha);
            den = x3 * M_PI;
        }
        else {
            if(alpha==1) {
                taps[i] = -1;
                continue;
            }
            x3 = (1-alpha)*x1;
            x2 = (1+alpha)*x1;
            num = (sin(x2)*(1+alpha)*M_PI
            -   cos(x3)*((1-alpha)*M_PI*spb)/(4*alpha*xindx)
            + sin(x3)*spb*spb/(4*alpha*xindx*xindx));
            den = -32 * M_PI * alpha * alpha * xindx/spb;
        }
        taps[i] = 4 * alpha * num / den;
        scale += taps[i];
    }
    for(int i = 0; i < ntaps; i++)
    taps[i] = taps[i] * gain / scale;

  return taps;
}



cvec Modem_BPSK::modulate(bvec data_packet)
{

    //Generic Modulation (Good for small filter length)
    BPSK bpsk;
    vec mapped_transmited_symbols=bpsk.modulate_bits(data_packet);
    vec filtered_transmitted_symbols=rrc.shape_symbols(mapped_transmited_symbols);
    return to_cvec(filtered_transmitted_symbols);


    /*//PFB Modulation (Faster for long filter length)
    int nfilts=32;
    vec rrc_response=generate_rrc_taps(nfilts,nfilts,1.0,roll_off,11*OF*nfilts);
    int rrc_response_size=rrc_response.size();
    std::vector<float> taps(rrc_response_size);
    for(int i=0;i<rrc_response_size;i++)
        taps[i]=rrc_response[i];

    //CREATE TOP BLOCK
    tb = gr::make_top_block("modem_bpsk");

    //INJECTOR MODULATOR
    injector_modulator=make_injector_complex();

    //PFB ARB RESAMPLER
    gr::filter::pfb_arb_resampler_ccf::sptr pfb_arb;
    pfb_arb=gr::filter::pfb_arb_resampler_ccf::make(OF,taps,32);

    //SNIFFER MODULATOR
    sniffer_modulator=make_sniffer_complex();
    sniffer_modulator->set_buffer_size(nb_bits*OF);

    //CONNECT TRANSMITTER
    tb->connect(injector_modulator,0,pfb_arb,0);
    tb->connect(pfb_arb,0,sniffer_modulator,0);

    //START TOP BLOCK
    tb->start();

    BPSK bpsk;
    vec mapped_transmitted_symbols=bpsk.modulate_bits(data_packet);
    injector_modulator->set_samples(to_cvec(mapped_transmitted_symbols));
    usleep(100000);
    cvec outvec=sniffer_modulator->get_samples();

    return outvec/sqrt(OF);*/


}




bvec Modem_BPSK::demodulate(cvec rx_buff, cvec &out)
{

    /*//For BER Testing
    //Root Raised Cosine
    cvec filtered_received_symbols=to_cvec(rrc.shape_samples(real(rx_buff)),rrc.shape_samples(imag(rx_buff)));
    out.set_size(filtered_received_symbols.size()/OF);
    int i=0;
    int o=0;
    while((o<filtered_received_symbols.size()/OF)&&(i<filtered_received_symbols.size())){
        out[o]=filtered_received_symbols[i];
        i+=OF;
        o++;
    }*/

    //CREATE TOP BLOCK
    tb = gr::make_top_block("modem_bpsk");

    //INJECTOR DEMODULATOR
    injector_demodulator=make_injector_complex();



    //FIRST RECEIVER: RRC + MPSK RECEIVER
    unsigned int M=2;
    float theta=0.0;
    float loop_bw=pi/100;
    float fmin=-0.5;
    float fmax=0.5;
    float mu=0.5;
    float gain_mu=0.01;
    float omega=OF;
    float gain_omega=0.25*gain_mu*gain_mu;
    float omega_rel=0.005;
    gr::digital::mpsk_receiver_cc::sptr bpsk_receiver;
    bpsk_receiver=gr::digital::mpsk_receiver_cc::make(M,theta,loop_bw,fmin,fmax,mu,gain_mu,omega,gain_omega,omega_rel);

    /*//SECOND RECEIVER: FREQUENCY LOCKED LOOP + PFB CLOCK SYNC + COSTAS LOOP

    //FREQUENCY LOCKED LOOP
    float loop_bw=2*pi/100;
    gr::digital::fll_band_edge_cc::sptr bpsk_fll;
    bpsk_fll=gr::digital::fll_band_edge_cc::make(OF,roll_off,55,loop_bw);


    //PFB CLOCK SYNC
    int nfilts=32;
    vec rrc_response=generate_rrc_taps(nfilts,nfilts,1.0,roll_off,11*OF*nfilts);
    int rrc_response_size=rrc_response.size();
    std::vector<float> taps(rrc_response_size);
    for(int i=0;i<rrc_response_size;i++)
        taps[i]=rrc_response[i];
    gr::digital::pfb_clock_sync_ccf::sptr bpsk_pfb;
    bpsk_pfb=gr::digital::pfb_clock_sync_ccf::make(OF,loop_bw,taps);

    //COSTAS LOOP
    gr::digital::costas_loop_cc::sptr bpsk_costas;
    bpsk_costas=gr::digital::costas_loop_cc::make(loop_bw,2);*/

    //SNIFFER DEMODULATOR
    sniffer_demodulator=make_sniffer_complex();
    sniffer_demodulator->set_buffer_size(nb_bits);

    //CONNECT FIRST RECEIVER
    tb->connect(injector_demodulator,0,bpsk_receiver,0);
    tb->connect(bpsk_receiver,0,sniffer_demodulator,0);

    /*//CONNECT SECOND RECEIVER
    tb->connect(injector_demodulator,0,bpsk_fll,0);
    tb->connect(bpsk_fll,0,bpsk_pfb,0);
    tb->connect(bpsk_pfb,0,bpsk_costas,0);
    tb->connect(bpsk_costas,0,sniffer_demodulator,0);*/

    //START TOP BLOCK
    tb->start();

    //FIRST RECEIVER = RRC + MPSK RECEIVER
    cvec filtered_received_symbols=to_cvec(rrc.shape_samples(real(rx_buff)),rrc.shape_samples(imag(rx_buff)));
    double normalization=1./filtered_received_symbols.size()*real(sum(elem_mult(filtered_received_symbols,conj(filtered_received_symbols))));
    filtered_received_symbols=1./sqrt(normalization)*filtered_received_symbols;
    injector_demodulator->set_samples(filtered_received_symbols);
    usleep(100000);
    out=sniffer_demodulator->get_samples();

    /*//SECOND RECEIVER = FLL + PFB CLOCK SYNC + COSTAS LOOP
    double normalization=1./rx_buff.size()*real(sum(elem_mult(rx_buff,conj(rx_buff))));
    rx_buff=1./sqrt(normalization)*rx_buff;
    injector_demodulator->set_samples(rx_buff);
    usleep(100000);
    out=sniffer_demodulator->get_samples();*/

    //STOP TOP BLOCK
    tb->stop();

    //BPSK Decision
    BPSK bpsk;
    bvec received_bits=bpsk.demodulate_bits(real(out));

    return received_bits;


}


bool Modem_BPSK::preamble_detection(bvec received_bits,bvec &received_bits2,int &preamble_start){

    BPSK bpsk;
    vector<char> preamble(3);
    bvec received_bits_inverted;
    received_bits_inverted=received_bits+ones_b(received_bits.size());

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
            received_bits2.set_size(received_bits.size()-i);
            received_bits2=received_bits.get(i,received_bits.size()-1);
            preamble_start=i;
        }
        if((preamble_bits==received_bits_inverted.get(i,i+24-1))&&(preamble_bits==received_bits_inverted.get(i+120,i+144-1))){
            preamble_ok=true;
            received_bits2.set_size(received_bits.size()-i);
            received_bits2=received_bits_inverted.get(i,received_bits.size()-1);
            preamble_start=i;
        }
        i++;
    }

    return preamble_ok;

}

bool Modem_BPSK::ack_detection(bvec received_bits,bvec &received_bits2,int &preamble_start){

    vector<char> preamble(3);
    bvec received_bits_inverted;
    received_bits_inverted=received_bits+ones_b(received_bits.size());

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
            received_bits2.set_size(received_bits.size()-i);
            received_bits2=received_bits.get(i,received_bits.size()-1);
            preamble_start=i;
        }
        if((preamble_bits==received_bits_inverted.get(i,i+24-1))&&(preamble_bits==received_bits_inverted.get(i+120,i+144-1))){
            preamble_ok=true;
            received_bits2.set_size(received_bits.size()-i);
            received_bits2=received_bits_inverted.get(i,received_bits.size()-1);
            preamble_start=i;
        }
        i++;
    }

    return preamble_ok;

}

bvec Modem_BPSK::charvec2bvec(vector<char> input){
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
