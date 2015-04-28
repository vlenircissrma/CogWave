#ifndef SNIFFER_COMPLEX_H
#define SNIFFER_COMPLEX_H

#include <gnuradio/sync_block.h>
#include <boost/thread/mutex.hpp>
#include <boost/circular_buffer.hpp>
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;

class sniffer_complex;

typedef boost::shared_ptr<sniffer_complex> sniffer_complex_sptr;

sniffer_complex_sptr make_sniffer_complex(int buffsize=48000);

class sniffer_complex : public gr::sync_block
{
    friend sniffer_complex_sptr make_sniffer_complex(int buffsize);

public:
    sniffer_complex(int buffsize);
    ~sniffer_complex();

    int work(int noutput_items,
             gr_vector_const_void_star &input_items,
             gr_vector_void_star &output_items);

    cvec get_samples();
    void set_buffer_size(int newsize);

private:

    boost::mutex d_mutex;
    boost::circular_buffer<gr_complex> d_buffer;
    long count;

};


#endif /* SNIFFER_COMPLEX_H */

