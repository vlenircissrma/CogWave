#ifndef SNIFFER_CHAR_H
#define SNIFFER__CHAR_H

#include <gnuradio/sync_block.h>
#include <boost/thread/mutex.hpp>
#include <boost/circular_buffer.hpp>
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;

class sniffer_char;

typedef boost::shared_ptr<sniffer_char> sniffer_char_sptr;

sniffer_char_sptr make_sniffer_char(int buffsize=48000);

class sniffer_char : public gr::sync_block
{
    friend sniffer_char_sptr make_sniffer_char(int buffsize);

public:
    sniffer_char(int buffsize);
    ~sniffer_char();

    int work(int noutput_items,
             gr_vector_const_void_star &input_items,
             gr_vector_void_star &output_items);

    vector<unsigned char> get_samples();
    void set_buffer_size(int newsize);

private:

    boost::mutex d_mutex;
    boost::circular_buffer<unsigned char> d_buffer;
    long count;

};


#endif /* SNIFFER_CHAR_H */

