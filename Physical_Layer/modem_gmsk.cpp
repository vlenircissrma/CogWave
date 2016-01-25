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
#include "modem_gmsk.h"

Modem_GMSK::Modem_GMSK()
{
    nb_bits=63040;
    OF=4;
    h_index=0.5;
    unsigned L=4;
    double bt=0.5;
    vec gaussian_response=generate_gaussian_taps(OF,L,bt);
    int gaussian_response_size=gaussian_response.size()+(OF-1);
    vec gaussian_rectangular_response(gaussian_response_size);
    vec mask(gaussian_response_size);
    for(int i=0;i<gaussian_response_size;i++){
        mask.zeros();
        if(i>gaussian_response_size-OF){
            mask.set_subvector(i,ones(OF-(i-(gaussian_response_size-OF))));
        }
        else
           mask.set_subvector(i,ones(OF));
        gaussian_rectangular_response[i]=sum(elem_mult(mask,concat(zeros(OF-1),gaussian_response)))/OF;
    }
    shaper.set_coeffs(gaussian_rectangular_response);
    upsampled_shaper.set_pulse_shape(gaussian_rectangular_response,OF);

    //CREATE TOP BLOCK
    tb = gr::make_top_block("modem_gmsk");
}


vec Modem_GMSK::generate_gaussian_taps(unsigned samples_per_sym, unsigned L, double bt){

    //Gaussian taps from gnuradio-core/src/lib/general/gr_cpm.cc
    /*double Ls=(double)L*samples_per_sym;
    vec taps(L*samples_per_sym);
    taps.zeros();
    // alpha = sqrt(2/ln(2)) * pi * BT
    double alpha=5.336446256636997*bt;
    double k;
    for(unsigned i=0;i<samples_per_sym*L;i++){
        k=i-Ls/2; // Causal to acausal
        taps[i]=(erf(alpha*(k/samples_per_sym+0.5))-erf(alpha*(k/samples_per_sym-0.5)))*0.5/samples_per_sym;
    }
    return taps;*/

    //Gaussian taps from Gnuradio gr-filter/lib/firdes.cc
    vec taps(L*samples_per_sym);
    double scale = 0;
    double dt = 1.0/samples_per_sym;
    double s = 1.0/(sqrt(log(2.0)) / (2*M_PI*bt));
    double t0 = -0.5 * L*samples_per_sym;
    double ts;
    for(int i=0;i<L*samples_per_sym;i++) {
        t0++;
        ts = s*dt*t0;
        taps[i] = exp(-0.5*ts*ts);
        scale += taps[i];
    }
    for(int i=0;i<L*samples_per_sym;i++)
        taps[i] = taps[i] / scale;

    return taps;
}

cvec Modem_GMSK::modulate(bvec data_packet)
{
    BPSK bpsk;
    double sensitivity=(double(M_PI)*h_index);

    vec mapped_transmited_symbols=bpsk.modulate_bits(data_packet+bin(1));
    vec filtered_transmitted_symbols;
    if(OF==1)
        filtered_transmitted_symbols=shaper(mapped_transmited_symbols);
    else
        filtered_transmitted_symbols=upsampled_shaper.shape_symbols(mapped_transmited_symbols);

    cvec fm_transmitted_symbols;
    int fm_size=filtered_transmitted_symbols.size();
    fm_transmitted_symbols.set_size(fm_size);
    double phase=0;
    for(int i=0;i<fm_size;i++){
        phase=phase+sensitivity*filtered_transmitted_symbols[i];
        while(phase>pi)
          phase-=2*pi;
        while(phase<-pi)
          phase+=2*pi;

        //FM modulation
        fm_transmitted_symbols[i]=exp(std::complex<double>(0,phase));
    }

    return to_cvec(fm_transmitted_symbols);
}


bvec Modem_GMSK::demodulate(cvec rx_buff, vec &out)
{
    int rx_buff_size=rx_buff.size(); 
    //Differential Phase Detector
    double sensitivity = (double(M_PI)*h_index);
    std::complex<double> product;
    vec fm_received_symbols;
    fm_received_symbols.set_size(rx_buff_size);
    fm_received_symbols.zeros();
    for(int i=1;i<rx_buff_size;i++){
        product=rx_buff[i]*conj(rx_buff[i-1]);
        fm_received_symbols[i-1]=1.0/sensitivity*std::arg(product);
    }

    //For BER Testing
    /*fm_received_symbols=upsampled_shaper.shape_samples(fm_received_symbols);
    out.set_size(rx_buff_size/OF);
    int i=0;
    int o=0;
    while((o<rx_buff_size/OF)&&(i<rx_buff_size)){
        out[o]=fm_received_symbols[i];
        i+=4;
        o++;
    }*/


    //FIRST SYNC: M&M
    float mu=0.5;
    float gain_mu=0.0;
    float omega=OF;
    float gain_omega=0.25*gain_mu*gain_mu;
    float omega_rel=0.005;
    gr::digital::clock_recovery_mm_ff::sptr clock_mm;
    clock_mm=gr::digital::clock_recovery_mm_ff::make(omega,gain_omega,mu,gain_mu,omega_rel);

    /*//SECOND SYNC : PFB SYNC
    vec gmsk_response=upsampled_shaper.get_pulse_shape();
    int gmsk_response_size=gmsk_response.size();
    std::vector<float> taps(gmsk_response_size);
    for(int i=0;i<gmsk_response_size;i++){
        taps[i]=gmsk_response[i];
    }
    gr::digital::pfb_clock_sync_fff::sptr clock_pfb;
    clock_pfb=gr::digital::pfb_clock_sync_fff::make(OF,1.0,taps);*/

    //INJECTOR DEMODULATOR
    injector_float_sptr injector_demodulator;
    injector_demodulator=make_injector_float();

    //SNIFFER DEMODULATOR
    sniffer_float_sptr sniffer_demodulator;
    sniffer_demodulator=make_sniffer_float();
    sniffer_demodulator->set_buffer_size(nb_bits);

    //CONNECT FIRST SYNC
    tb->connect(injector_demodulator,0,clock_mm,0);
    tb->connect(clock_mm,0,sniffer_demodulator,0);

    /*//CONNECT SECOND SYNC
    tb->connect(injector_demodulator,0,clock_pfb,0);
    tb->connect(clock_pfb,0,sniffer_demodulator,0);*/

    //START TOP BLOCK
    tb->start();

    //FIRST SYNC: M&M
    fm_received_symbols=upsampled_shaper.shape_samples(fm_received_symbols);
    injector_demodulator->set_samples(fm_received_symbols);
    usleep(100000);
    out=sniffer_demodulator->get_samples();

    /*//SECOND SYNC : PFB SYNC
    injector_demodulator->set_samples(fm_received_symbols);
    usleep(100000);
    out=sniffer_demodulator->get_samples();*/

    //STOP FLOWGRAPH
    tb->stop();
    tb->wait();
    tb->disconnect_all();

    double normalization=sqrt(1./out.size())*itpp::norm(out);
    if(normalization!=0.0)
        out=1./normalization*out;
    BPSK bpsk;
    bvec received_bits= bpsk.demodulate_bits(out);

    return received_bits;


}


bool Modem_GMSK::preamble_detection(bvec received_bits,bvec &received_bits2,int &preamble_start){

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
    while((i<=int(received_bits.size())-144)&&(preamble_ok==false)){
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

bool Modem_GMSK::ack_detection(bvec received_bits,bvec &received_bits2,int &preamble_start){

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
    while((i<=int(received_bits.size())-144)&&(preamble_ok==false)){
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

bvec Modem_GMSK::charvec2bvec(vector<char> input){
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
