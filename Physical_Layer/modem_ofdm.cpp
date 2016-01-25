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
#include "modem_ofdm.h"

union int_to_char {
     long unsigned u;
     unsigned char c;
};

Modem_OFDM::Modem_OFDM()
{

    packet_len=96*6;
    nb_bits=packet_len*8;
    fft_len=64;
    cp_len=16;
    ivec occupied_carriers1 = "-26 -25 -24 -23 -22 -20 -19 -18 -17 -16 -15 -14 -13 -12 -11 -10 -9 -8 -6 -5 -4 -3 -2 -1 1 2 3 4 5 6 8 9 10 11 12 13 14 15 16 17 18 19 20 22 23 24 25 26";
    occupied_carriers.resize(1);
    occupied_carriers[0].resize(occupied_carriers1.size());
    for(int i=0;i<occupied_carriers[0].size();i++)
        occupied_carriers[0][i]=occupied_carriers1[i];
    ivec pilot_carriers1= "-21 -7 7 21";
    pilot_carriers.resize(1);
    pilot_carriers[0].resize(pilot_carriers1.size());
    for(int i=0;i<pilot_carriers[0].size();i++)
        pilot_carriers[0][i]=pilot_carriers1[i];
    ivec pilot_symbols1= "1 1 1 -1";
    pilot_symbols.resize(1);
    pilot_symbols[0].resize(pilot_symbols1.size());
    for(int i=0;i<pilot_symbols[0].size();i++)
        pilot_symbols[0][i]=pilot_symbols1[i];

    vec bpsk1(2);
    bpsk1[0]=sqrt(2);
    bpsk1[1]=-sqrt(2);
    vec bpsk2(2);
    bpsk2[0]=1;
    bpsk2[1]=-1;
    std::vector<int> active_carriers;
    active_carriers.clear();
    for (int i=0;i<occupied_carriers[0].size();i++){
        if(occupied_carriers[0][i]<0)
            active_carriers.push_back(occupied_carriers[0][i]+fft_len);
        else
            active_carriers.push_back(occupied_carriers[0][i]);
    }
    for (int i=0;i<pilot_carriers[0].size();i++){
        if(pilot_carriers[0][i]<0)
            active_carriers.push_back(pilot_carriers[0][i]+fft_len);
        else
            active_carriers.push_back(pilot_carriers[0][i]);
    }
    cvec sync_word1(fft_len);
    cvec sync_word2(fft_len);
    sync_word1.zeros();
    sync_word2.zeros();

    LFSR lfsr;
    bvec polynomial("1 0 0 0 0 0 0 0 0 0 0 1 0 1 1 0 1");
    lfsr.set_connections(polynomial);
    bvec state("0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1");
    lfsr.set_state(state);
    bvec first_sync=lfsr.shift(active_carriers.size());
    bvec second_sync=lfsr.shift(active_carriers.size());

    for(int i=0;i<active_carriers.size();i++){
        if((active_carriers[i]%2!=0)&&(find(active_carriers.begin(),active_carriers.end(),active_carriers[i])!=active_carriers.end())){
            sync_word1[active_carriers[i]]=bpsk1[first_sync[i]];
        }
        if(find(active_carriers.begin(),active_carriers.end(),active_carriers[i])!=active_carriers.end())
            sync_word2[active_carriers[i]]=bpsk2[second_sync[i]];
    }


    cvec fftshift_sync_word1=concat(sync_word1.get(sync_word1.size()/2,sync_word1.size()-1),sync_word1.get(0,sync_word1.size()/2-1));
    cvec fftshift_sync_word2=concat(sync_word2.get(sync_word2.size()/2,sync_word2.size()-1),sync_word2.get(0,sync_word2.size()/2-1));


    sync_words.resize(2);
    sync_words[0].resize(fft_len);
    sync_words[1].resize(fft_len);
    for(int i=0;i<fft_len;i++){
        sync_words[0][i]=fftshift_sync_word1[i];
        sync_words[1][i]=fftshift_sync_word2[i];
    }

    //PAYLOAD HEADER
    header_mod=gr::digital::constellation_bpsk::make();
    payload_mod=gr::digital::constellation_qpsk::make();
    Number_of_received_symbols=3*(fft_len+cp_len)+ceil(double(nb_bits)/payload_mod->bits_per_symbol()/occupied_carriers[0].size())*(fft_len+cp_len);
    //PACKET HEADER OFDM
    packet_header_ofdm=gr::digital::packet_header_ofdm::make(occupied_carriers,1,"packet_len","frame_len","packet_num",header_mod->bits_per_symbol(),payload_mod->bits_per_symbol(),false);

    //CREATE TOP BLOCK
    tb = gr::make_top_block("modem_ofdm");

}

cvec Modem_OFDM::modulate(bvec data_packet)
{



    //INJECTOR MODULATOR
    injector_char_sptr injector_modulator;
    injector_modulator=make_injector_char();

    //OFDM TAGGED STREAM
    gr::blocks::stream_to_tagged_stream::sptr tagged_stream;
    tagged_stream=gr::blocks::stream_to_tagged_stream::make(sizeof(char),1,packet_len,"packet_len");

    //CRC mod
    gr::digital::crc32_bb::sptr crc_mod;
    crc_mod=gr::digital::crc32_bb::make(false,"packet_len");


    //PACKET HEADER GENERATOR
    gr::digital::packet_headergenerator_bb::sptr packet_header_gen;
    packet_header_gen=gr::digital::packet_headergenerator_bb::make(packet_header_ofdm->base(),"packet_len");


    //REPACK BITS
    gr::blocks::repack_bits_bb::sptr payload_repack_mod;
    payload_repack_mod=gr::blocks::repack_bits_bb::make(8,payload_mod->bits_per_symbol(),"packet_len");

    //CHUNKS TO SYMBOLS
    gr::digital::chunks_to_symbols_bc::sptr header_complex;
    header_complex=gr::digital::chunks_to_symbols_bc::make(header_mod->points());
    gr::digital::chunks_to_symbols_bc::sptr payload_complex;
    payload_complex=gr::digital::chunks_to_symbols_bc::make(payload_mod->points());


    //TAGGED STREAM MUX
    gr::blocks::tagged_stream_mux::sptr tagged_stream_mux;
    tagged_stream_mux=gr::blocks::tagged_stream_mux::make(sizeof(gr_complex),"packet_len",1);

    //OFDM CARRIER ALLOCATOR
    gr::digital::ofdm_carrier_allocator_cvc::sptr carrier_allocator;
    carrier_allocator = gr::digital::ofdm_carrier_allocator_cvc::make(fft_len,occupied_carriers,pilot_carriers,pilot_symbols,sync_words,"packet_len");

    //OFDM IFFT
    gr::fft::fft_vcc::sptr ifft_ofdm;
    std::vector<float> window;
    window.clear();
    ifft_ofdm=gr::fft::fft_vcc::make(fft_len,false,window,true);

    //OFDM CYCLIC PREFIXER
    gr::digital::ofdm_cyclic_prefixer::sptr cyclic_prefixer;
    cyclic_prefixer=gr::digital::ofdm_cyclic_prefixer::make(fft_len,fft_len+cp_len,0,"packet_len");

    //SNIFFER MODULATOR
    sniffer_complex_sptr sniffer_modulator;
    sniffer_modulator=make_sniffer_complex();
    sniffer_modulator->set_buffer_size(Number_of_received_symbols);



    //CONNECT MODULATOR
    tb->connect(injector_modulator,0,tagged_stream,0);
    //tb->connect(tagged_stream,0,crc_mod,0);
    //tb->connect(crc_mod,0,packet_header_gen,0);
    //tb->connect(crc_mod,0,payload_repack_mod,0);
    tb->connect(tagged_stream,0,packet_header_gen,0);
    tb->connect(tagged_stream,0,payload_repack_mod,0);
    tb->connect(packet_header_gen,0,header_complex,0);
    tb->connect(payload_repack_mod,0,payload_complex,0);
    tb->connect(header_complex,0,tagged_stream_mux,0);
    tb->connect(payload_complex,0,tagged_stream_mux,1);
    tb->connect(tagged_stream_mux,0,carrier_allocator,0);
    tb->connect(carrier_allocator,0,ifft_ofdm,0);
    tb->connect(ifft_ofdm,0,cyclic_prefixer,0);
    tb->connect(cyclic_prefixer,0,sniffer_modulator,0);

    //START FLOWGRAPH
    tb->start();

    vector<unsigned char> data_packet_char=bvec2charvec(data_packet);
    injector_modulator->set_samples(data_packet_char);
    usleep(100000);
    cvec out=sniffer_modulator->get_samples();

    //STOP FLOWGRAPH
    tb->stop();
    tb->wait();
    tb->disconnect_all();

    double norm_factor=1./out.size()*sum(real(elem_mult(out,conj(out))));
    out=1./sqrt(norm_factor)*out;
    return out;
}

bvec Modem_OFDM::demodulate(cvec rx_buff, cvec &out)
{




    //INJECTOR DEMODULATOR
    injector_complex_sptr injector_demodulator;
    injector_demodulator=make_injector_complex();

    //OFDM SYNC DETECT
    gr::digital::ofdm_sync_sc_cfb::sptr sync_detect;
    sync_detect=gr::digital::ofdm_sync_sc_cfb::make(fft_len,cp_len);
    gr::blocks::delay::sptr delay;
    delay=gr::blocks::delay::make(sizeof(gr_complex),fft_len+cp_len);
    gr::analog::frequency_modulator_fc::sptr oscillator;
    oscillator=gr::analog::frequency_modulator_fc::make(-2.0/fft_len);
    gr::blocks::multiply_cc::sptr mixer;
    mixer=gr::blocks::multiply_cc::make();

    //HEADER PAYLOAD DEMUX
    gr::digital::header_payload_demux::sptr header_payload_demux;
    header_payload_demux=gr::digital::header_payload_demux::make(3,fft_len,cp_len,"frame_len","",true);


    //OFDM FFT
    std::vector<float> window;
    window.clear();
    gr::fft::fft_vcc::sptr header_fft;
    header_fft=gr::fft::fft_vcc::make(fft_len,true,window,true);
    gr::fft::fft_vcc::sptr payload_fft;
    payload_fft=gr::fft::fft_vcc::make(fft_len,true,window,true);


    //OFDM CHANNEL ESTIMATION
    gr::digital::ofdm_chanest_vcvc::sptr header_chanest;
    header_chanest=gr::digital::ofdm_chanest_vcvc::make(sync_words[0],sync_words[1],1);


    //OFDM FRAME EQUALIZER
    gr::digital::constellation_qpsk::sptr constellation;
    constellation=gr::digital::constellation_qpsk::make();


    gr::digital::ofdm_equalizer_simpledfe::sptr header_equalizer;
    header_equalizer=gr::digital::ofdm_equalizer_simpledfe::make(fft_len,header_mod->base(),occupied_carriers,pilot_carriers,pilot_symbols,0);
    gr::digital::ofdm_equalizer_simpledfe::sptr payload_equalizer;
    payload_equalizer=gr::digital::ofdm_equalizer_simpledfe::make(fft_len,payload_mod->base(),occupied_carriers,pilot_carriers,pilot_symbols,1,0.1);

    gr::digital::ofdm_frame_equalizer_vcvc::sptr header_frame;
    header_frame=gr::digital::ofdm_frame_equalizer_vcvc::make(header_equalizer->base(),cp_len,"frame_len",true,1);
    gr::digital::ofdm_frame_equalizer_vcvc::sptr payload_frame;
    payload_frame=gr::digital::ofdm_frame_equalizer_vcvc::make(payload_equalizer->base(),cp_len,"frame_len");

    //OFDM SERIALIZER
    gr::digital::ofdm_serializer_vcc::sptr header_serializer;
    header_serializer=gr::digital::ofdm_serializer_vcc::make(fft_len,occupied_carriers,"frame_len");

    gr::digital::ofdm_serializer_vcc::sptr payload_serializer;
    payload_serializer=gr::digital::ofdm_serializer_vcc::make(fft_len,occupied_carriers,"frame_len","packet_len",1);


    //CONSTELLATION DECODER
    gr::digital::constellation_decoder_cb::sptr header_decoder;
    header_decoder=gr::digital::constellation_decoder_cb::make(header_mod->base());
    gr::digital::constellation_decoder_cb::sptr payload_decoder;
    payload_decoder=gr::digital::constellation_decoder_cb::make(payload_mod->base());

    //PACKET HEADER PARSER
    gr::digital::packet_headerparser_b::sptr header_parser;
    header_parser=gr::digital::packet_headerparser_b::make(packet_header_ofdm->formatter());

    //REPACK BITS
    gr::blocks::repack_bits_bb::sptr payload_repack_demod;
    payload_repack_demod=gr::blocks::repack_bits_bb::make(payload_mod->bits_per_symbol(),8,"packet_len",true);

    //CRC demod
    gr::digital::crc32_bb::sptr crc_demod;
    crc_demod=gr::digital::crc32_bb::make(true,"packet_len");

    //SNIFFER DEMODULATOR COMPLEX
    sniffer_complex_sptr sniffer_demodulator_complex;
    sniffer_demodulator_complex=make_sniffer_complex();
    sniffer_demodulator_complex->set_buffer_size(nb_bits/payload_mod->bits_per_symbol());

    //SNIFFER DEMODULATOR CHAR
    sniffer_char_sptr sniffer_demodulator_char;
    sniffer_demodulator_char=make_sniffer_char();
    sniffer_demodulator_char->set_buffer_size(nb_bits/8);

    gr::blocks::null_sink::sptr null_sink;
    null_sink=gr::blocks::null_sink::make(sizeof(gr_complex));

    //gr::blocks::file_sink::sptr file_sink;
    //file_sink=gr::blocks::file_sink::make(sizeof(gr_complex)*fft_len,"channel-estimate.dat");
    //CONNECT DEMODULATOR
    tb->connect(injector_demodulator,0,sync_detect,0);
    tb->connect(injector_demodulator,0,delay,0);
    tb->connect(delay,0,mixer,0);
    tb->connect(sync_detect,0,oscillator,0);
    tb->connect(oscillator,0,mixer,1);
    //tb->connect(oscillator,0,null_sink,0);
    tb->connect(mixer,0,header_payload_demux,0);
    //tb->connect(delay,0,header_payload_demux,0);
    tb->connect(sync_detect,1,header_payload_demux,1);
    tb->connect(header_payload_demux,0,header_fft,0);
    tb->connect(header_payload_demux,1,payload_fft,0);
    tb->connect(header_fft,0,header_chanest,0);
    tb->connect(header_chanest,0,header_frame,0);
    tb->connect(header_frame,0,header_serializer,0);
    tb->connect(header_serializer,0,header_decoder,0);
    tb->connect(header_decoder,0,header_parser,0);
    tb->msg_connect(header_parser,"header_data",header_payload_demux,"header_data");
    tb->connect(payload_fft,0,payload_frame,0);
    tb->connect(payload_frame,0,payload_serializer,0);
    tb->connect(payload_serializer,0,payload_decoder,0);
    tb->connect(payload_decoder,0,payload_repack_demod,0);
    //tb->connect(payload_repack_demod,0,crc_demod,0);
    //tb->connect(crc_demod,0,sniffer_demodulator_char,0);
    tb->connect(payload_repack_demod,0,sniffer_demodulator_char,0);
    tb->connect(payload_serializer,0,sniffer_demodulator_complex,0);
    //START FLOWGRAPH
    tb->start();


    rx_buff=concat(rx_buff,zeros_c(fft_len+cp_len));
    injector_demodulator->set_samples(rx_buff);
    usleep(100000);
    out=sniffer_demodulator_complex->get_samples();

    vector<unsigned char> received_char=sniffer_demodulator_char->get_samples();

    //STOP FLOWGRAPH
    tb->stop();
    tb->wait();
    tb->disconnect_all();
    tb->msg_disconnect(header_parser,"header_data",header_payload_demux,"header_data");

    bvec received_bits=unsignedcharvec2bvec(received_char);


    return received_bits;
}


bvec Modem_OFDM::unsignedcharvec2bvec(vector<unsigned char> input){
    int bitsize=sizeof(char)*8;
    bvec output(input.size()*bitsize);
    unsigned char bintemp;
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

vector<unsigned char> Modem_OFDM::bvec2charvec(bvec input){

    int bitsize=sizeof(char)*8;
    bvec bin_vector(bitsize);
    int_to_char character;
    vector<unsigned char> output(input.size()/bitsize);

         for(int i=0;i<input.size()/bitsize;i++){
            bin_vector=input.mid(i*bitsize,bitsize);
            unsigned long tempbin=0;
            for(int j=0;j<bitsize;j++){
                tempbin += (unsigned long)pow2(bitsize-j-1)*int(bin_vector[j]);
            }
            character.u=tempbin;
            output[i]=character.c;
        }
return output;
}

bool Modem_OFDM::preamble_detection(bvec received_bits,bvec &received_bits2,int &preamble_start){

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
            received_bits2.set_size(received_bits.size()-i);
            received_bits2=received_bits.get(i,received_bits.size()-1);
            preamble_start=i;
        }
        i++;
    }

    return preamble_ok;

}

bool Modem_OFDM::ack_detection(bvec received_bits,bvec &received_bits2,int &preamble_start){

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
            received_bits2.set_size(received_bits.size()-i);
            received_bits2=received_bits.get(i,received_bits.size()-1);
            preamble_start=i;
        }
        i++;
    }

    return preamble_ok;

}

bvec Modem_OFDM::charvec2bvec(vector<char> input){
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
