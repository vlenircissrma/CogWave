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
#ifndef MODEM_OFDM_H
#define MODEM_OFDM_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include "sniffer_char.h"
#include "injector_char.h"
#include "sniffer_complex.h"
#include "injector_complex.h"
#include <gnuradio/top_block.h>
#include <gnuradio/blocks/stream_to_tagged_stream.h>
#include <gnuradio/digital/ofdm_carrier_allocator_cvc.h>
#include <gnuradio/fft/fft_vcc.h>
#include <gnuradio/digital/ofdm_cyclic_prefixer.h>
#include <gnuradio/digital/ofdm_sync_sc_cfb.h>
#include <gnuradio/blocks/delay.h>
#include <gnuradio/analog/frequency_modulator_fc.h>
#include <gnuradio/blocks/multiply_cc.h>
#include <gnuradio/blocks/stream_to_vector.h>
#include <gnuradio/digital/constellation.h>
#include <gnuradio/digital/ofdm_frame_equalizer_vcvc.h>
#include <gnuradio/digital/ofdm_equalizer_simpledfe.h>
#include <gnuradio/digital/ofdm_serializer_vcc.h>
#include <gnuradio/digital/packet_headergenerator_bb.h>
#include <gnuradio/digital/packet_header_ofdm.h>
#include <gnuradio/blocks/repack_bits_bb.h>
#include <gnuradio/digital/chunks_to_symbols_bc.h>
#include <gnuradio/blocks/tagged_stream_mux.h>
#include <gnuradio/digital/header_payload_demux.h>
#include <gnuradio/digital/ofdm_chanest_vcvc.h>
#include <gnuradio/digital/constellation_decoder_cb.h>
#include <gnuradio/digital/packet_headerparser_b.h>
#include <gnuradio/blocks/null_sink.h>
#include <gnuradio/blocks/file_sink.h>
#include <gnuradio/digital/crc32_bb.h>


class Modem_OFDM
{
public:
    Modem_OFDM();
    cvec modulate(bvec data_packet);
    bvec demodulate(cvec rx_buff, cvec &out);
    cvec ofdm_carrier_allocator(cvec mapped_transmitted_symbols);
    cvec ofdm_ifft(cvec ofdm_allocated);
    cvec cp_ofdm_allocated(cvec ofdm_allocated_ifft);
    bvec unsignedcharvec2bvec(vector<unsigned char> input);
    vector<unsigned char> bvec2charvec(bvec input);
    bvec charvec2bvec(vector<char> input);
    bool preamble_detection(bvec received_bits,bvec &received_bits2,int &preamble_start);
    int fft_len;
    int cp_len;
    int nb_bits;
    int packet_len;
    int Number_of_received_symbols;

private:

    std::vector<std::vector<int> > occupied_carriers;
    std::vector<std::vector<int> > pilot_carriers;
    std::vector<std::vector<gr_complex> > pilot_symbols;
    std::vector<std::vector<gr_complex> > sync_words;

    gr::top_block_sptr tb;
    gr::digital::constellation_bpsk::sptr header_mod;
    gr::digital::constellation_qpsk::sptr payload_mod;
    gr::digital::packet_header_ofdm::sptr packet_header_ofdm;
};

#endif // MODEM_OFDM_H
