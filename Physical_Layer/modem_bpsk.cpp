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

#define protected public
#define private public
#include "pfb_arb_resampler_fff_impl.h"
#include "mpsk_receiver_cc_impl.h"
#include "pfb_clock_sync_ccf_impl.h"
#include "fll_band_edge_cc_impl.h"
#include "clock_recovery_mm_ff_impl.h"
#include <gnuradio/math.h>
#include "constellation_receiver_cb_impl.h"
#undef protected
#undef private

Modem_BPSK::Modem_BPSK()
{
    nb_bits=63040;
    OF=4;
    float roll_off=0.5;
    int filter_length=6;
    rrc.set_pulse_shape(roll_off,filter_length,OF);

    //CREATE TOP BLOCK
    tb = gr::make_top_block("modem_bpsk");


    //INJECTOR MODULATOR
    injector_modulator=make_injector_complex();

    //PFB ARB RESAMPLER
    int nfilts=32;
    vec rrc_response=generate_rrc_taps(nfilts,nfilts,1.0,roll_off,11*OF*nfilts);
    int rrc_response_size=rrc_response.size();
    std::vector<float> taps(rrc_response_size);
    for(int i=0;i<rrc_response_size;i++)
        taps[i]=rrc_response[i];
    gr::filter::pfb_arb_resampler_ccf::sptr pfb_arb;
    pfb_arb=gr::filter::pfb_arb_resampler_ccf::make(OF,taps,32);

    //SNIFFER MODULATOR
    sniffer_modulator=make_sniffer_complex();
    sniffer_modulator->set_buffer_size(nb_bits*OF);

    //CONNECT MODULATOR
    tb->connect(injector_modulator,0,pfb_arb,0);
    tb->connect(pfb_arb,0,sniffer_modulator,0);

    //START TOP BLOCK
    tb->start();

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

    /*//Generic Modulation (Good for small filter length)
    BPSK bpsk;
    vec mapped_transmited_symbols=bpsk.modulate_bits(data_packet);
    vec filtered_transmitted_symbols=rrc.shape_symbols(mapped_transmited_symbols);
    return to_cvec(filtered_transmitted_symbols);*/


    //PFB Modulation (Faster for long filter length)
    int nfilts=32;
    float roll_off=0.5;
    vec rrc_response=generate_rrc_taps(nfilts,nfilts,1.0,roll_off,11*OF*nfilts);
    int rrc_response_size=rrc_response.size();
    std::vector<float> taps(rrc_response_size);
    for(int i=0;i<rrc_response_size;i++)
        taps[i]=rrc_response[i];
    gr::filter::pfb_arb_resampler_fff_impl d_arb_pfb(OF,taps,32);


    BPSK bpsk;
    vec mapped_transmited_symbols=bpsk.modulate_bits(data_packet);
    int ninput_items=mapped_transmited_symbols.size();
    int noutput_items=mapped_transmited_symbols.size()*OF;
    float in[ninput_items];
    for (int i=0;i<ninput_items;i++)
        in[i]=mapped_transmited_symbols[i];
    float out[noutput_items];
    vec outvec;
    outvec.set_size(noutput_items);
    outvec.zeros();

    int nitems_read;
    int nitems = floorf((float)noutput_items / d_arb_pfb.relative_rate());
    d_arb_pfb.d_resamp->filter(out, in, nitems, nitems_read);

    for(int i=0;i<noutput_items;i++){
        outvec[i]=out[i];
    }
    return to_cvec(outvec/sqrt(OF));


    /*//PFB Modulation by injector sniffer
    BPSK bpsk;
    vec mapped_transmitted_symbols=bpsk.modulate_bits(data_packet);
    injector_modulator->set_samples(to_cvec(mapped_transmitted_symbols));
    usleep(100000);
    cvec outvec=sniffer_modulator->get_samples();

    return outvec/sqrt(OF);*/


}


cvec Modem_BPSK::costas_mm(cvec received_samples){

    //M&M+Costas loop MPSK Receiver
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
    gr::digital::mpsk_receiver_cc_impl d_clock_bpsk_mm(M,theta,loop_bw,fmin,fmax,mu,gain_mu,omega,gain_omega,omega_rel);

    int ninput_items=received_samples.size();
    int noutput_items=ninput_items/OF;
    cvec out;
    out.set_size(noutput_items);

    int i=0;
    int o=0;
    d_clock_bpsk_mm.set_mu(mu);
    d_clock_bpsk_mm.set_omega(omega);
    while((o<noutput_items)&&(i<ninput_items)){
        while((d_clock_bpsk_mm.d_mu>1)&&(i<ninput_items)){
            gr_complex tmp=std::complex<float>(received_samples[i]);
            d_clock_bpsk_mm.mm_sampler(tmp);   // puts symbols into a buffer and adjusts d_mu
            i++;
        }

        if(i<ninput_items){
            gr_complex interp_sample=d_clock_bpsk_mm.d_interp->interpolate(&d_clock_bpsk_mm.d_dl[d_clock_bpsk_mm.d_dl_idx], d_clock_bpsk_mm.d_mu);
            d_clock_bpsk_mm.mm_error_tracking(interp_sample);     // corrects M&M sample time
            d_clock_bpsk_mm.phase_error_tracking(interp_sample);  // corrects phase and frequency offsets
            out[o++] = interp_sample;
        }
    }
    return out;

}

cvec Modem_BPSK::costas_pfb(cvec received_samples){


    //FLL+PFB+Costas loop Generic receiver
    float loop_bw=2*pi/100;
    float fmin=-0.25;
    float fmax=0.25;
    int nfilts=32;
    float roll_off=0.5;
    vec rrc_response2=generate_rrc_taps(nfilts,nfilts*OF,1.0,roll_off,11*OF*nfilts);
    int rrc_response_size2=rrc_response2.size();
    std::vector<float> taps2(rrc_response_size2);
    for(int i=0;i<rrc_response_size2;i++)
        taps2[i]=rrc_response2[i];

    gr::digital::fll_band_edge_cc_impl d_bpsk_fll(OF,roll_off,55,loop_bw);
    gr::digital::pfb_clock_sync_ccf_impl d_clock_bpsk_pfb(OF,loop_bw,taps2);

    gr::digital::constellation_bpsk::sptr d_bpsk;
    d_bpsk = gr::digital::constellation_bpsk::make();
    gr::digital::constellation_receiver_cb_impl d_constellation_bpsk_pfb(d_bpsk,loop_bw,fmin,fmax);


    //Frequency Locked Loop
    int noutput_items=received_samples.size();
    float error;
    gr_complex nco_out;
    gr_complex out_upper, out_lower;
    gr_complex tmp2[noutput_items];

    for(int i = 0; i < noutput_items; i++) {
      nco_out = exp(std::complex<double>(0,d_bpsk_fll.d_phase));
      tmp2[i] = received_samples[i] * (std::complex<double>)nco_out;

      // Perform the dot product of the output with the filters
      out_upper = d_bpsk_fll.d_filter_lower->filter(tmp2[i]);
      out_lower = d_bpsk_fll.d_filter_upper->filter(tmp2[i]);

      error = norm(out_lower) - norm(out_upper);

      d_bpsk_fll.advance_loop(error);
      d_bpsk_fll.phase_wrap();
      d_bpsk_fll.frequency_limit();
    }


    for(int i = 0; i < noutput_items; i++)
        received_samples[i]=tmp2[i];

    //PFB receiver
    int rx_buff_size=received_samples.size();

    // We need this many to process one output
    int nrequired = rx_buff_size - d_clock_bpsk_pfb.d_taps_per_filter - d_clock_bpsk_pfb.d_osps;
    int noutput_items2=rx_buff_size/OF;
    cvec out;
    out.set_size(noutput_items2);

    int i = 0;
    int count = 0;
    float error_r, error_i;
    gr_complex in[rx_buff_size];
    for(int i=0;i<rx_buff_size;i++)
        in[i]=received_samples[i];

    // produce output as long as we can and there are enough input samples
    while((i < noutput_items2) && (count < nrequired)) {
        while(d_clock_bpsk_pfb.d_out_idx < d_clock_bpsk_pfb.d_osps) {

          d_clock_bpsk_pfb.d_filtnum = (int)floor(d_clock_bpsk_pfb.d_k);

        // Keep the current filter number in [0, d_nfilters]
        // If we've run beyond the last filter, wrap around and go to next sample
        // If we've go below 0, wrap around and go to previous sample
        while(d_clock_bpsk_pfb.d_filtnum >= d_clock_bpsk_pfb.d_nfilters) {
            d_clock_bpsk_pfb.d_k -= d_clock_bpsk_pfb.d_nfilters;
            d_clock_bpsk_pfb.d_filtnum -= d_clock_bpsk_pfb.d_nfilters;
            count += 1;
        }

        while(d_clock_bpsk_pfb.d_filtnum < 0) {
            d_clock_bpsk_pfb.d_k += d_clock_bpsk_pfb.d_nfilters;
            d_clock_bpsk_pfb.d_filtnum += d_clock_bpsk_pfb.d_nfilters;
            count -= 1;
        }

        out[i+d_clock_bpsk_pfb.d_out_idx] = d_clock_bpsk_pfb.d_filters[d_clock_bpsk_pfb.d_filtnum]->filter(&in[count+d_clock_bpsk_pfb.d_out_idx]);
        d_clock_bpsk_pfb.d_k = d_clock_bpsk_pfb.d_k + d_clock_bpsk_pfb.d_rate_i + d_clock_bpsk_pfb.d_rate_f; // update phase
        d_clock_bpsk_pfb.d_out_idx++;

      }

      // reset here; if we didn't complete a full osps samples last time,
      // the early return would take care of it.
      d_clock_bpsk_pfb.d_out_idx = 0;

      // Update the phase and rate estimates for this symbol

      gr_complex diff = d_clock_bpsk_pfb.d_diff_filters[d_clock_bpsk_pfb.d_filtnum]->filter(&in[count]);
        error_r = out[i].real() * diff.real();
        error_i = out[i].imag() * diff.imag();
        d_clock_bpsk_pfb.d_error = (error_i + error_r) / 2.0;       // average error from I&Q channel

      // Run the control loop to update the current phase (k) and
      // tracking rate estimates based on the error value
      d_clock_bpsk_pfb.d_rate_f = d_clock_bpsk_pfb.d_rate_f + d_clock_bpsk_pfb.d_beta*d_clock_bpsk_pfb.d_error;
      d_clock_bpsk_pfb.d_k = d_clock_bpsk_pfb.d_k + d_clock_bpsk_pfb.d_alpha*d_clock_bpsk_pfb.d_error;
      // Keep our rate within a good range
      d_clock_bpsk_pfb.d_rate_f = gr::branchless_clip(d_clock_bpsk_pfb.d_rate_f, d_clock_bpsk_pfb.d_max_dev);

      i+=d_clock_bpsk_pfb.d_osps;
      count += (int)floor(d_clock_bpsk_pfb.d_sps);

    }

    cvec in2=out;
    i=0;
    float phase_error;
    unsigned int sym_value;
    gr_complex sample, nco;

    //Digital Constellation receiver
    while(i < noutput_items2) {

      sample = in2[i];

      nco = exp(std::complex<double>(0,d_constellation_bpsk_pfb.d_phase));   // get the NCO value for derotating the current sample
      sample = nco*sample;      // get the downconverted symbol

      sym_value = d_constellation_bpsk_pfb.d_constellation->decision_maker_pe(&sample, &phase_error);
      d_constellation_bpsk_pfb.phase_error_tracking(phase_error);  // corrects phase and frequency offsets

      out[i] = sample;
      i++;
    }

    return out;


}

bvec Modem_BPSK::demodulate(cvec rx_buff, cvec &out)
{

    //First Receiver = RRC + M&M + Costas
    //Root Raised Cosine
    cvec filtered_received_symbols=to_cvec(rrc.shape_samples(real(rx_buff)),rrc.shape_samples(imag(rx_buff)));
    //Normalize
    double normalization=sqrt(1./filtered_received_symbols.size())*itpp::norm(filtered_received_symbols);
    filtered_received_symbols=1./normalization*filtered_received_symbols;
    //M&M timing synchronization and Costas loop frequency synchronization
    out=costas_mm(filtered_received_symbols);

    /*//Second Receiver = FLL + PFB + Costas
    //Normalize
    double normalization=sqrt(1./rx_buff.size())*itpp::norm(rx_buff);
    rx_buff=1./normalization*rx_buff;
    //PFB timing synchronization and Costas loop frequency synchronization
    out=costas_pfb(rx_buff);*/


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
