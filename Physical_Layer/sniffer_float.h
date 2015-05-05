#ifndef SNIFFER_FLOAT_H
#define SNIFFER_FLOAT_H

#include <gnuradio/sync_block.h>
#include <boost/thread/mutex.hpp>
#include <boost/circular_buffer.hpp>
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;

class sniffer_float;

typedef boost::shared_ptr<sniffer_float> sniffer_float_sptr;

sniffer_float_sptr make_sniffer_float(int buffsize=48000);

class sniffer_float : public gr::sync_block
{
    friend sniffer_float_sptr make_sniffer_float(int buffsize);

public:
    sniffer_float(int buffsize);
    ~sniffer_float();

    int work(int noutput_items,
             gr_vector_const_void_star &input_items,
             gr_vector_void_star &output_items);

    vec get_samples();
    void set_buffer_size(int newsize);

private:

    boost::mutex d_mutex;
    boost::circular_buffer<float> d_buffer;
    long count;

};


#endif /* SNIFFER_FLOAT_H */

