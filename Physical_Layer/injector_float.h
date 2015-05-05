#ifndef INJECTOR_FLOAT_H
#define INJECTOR_FLOAT_H

#include <gnuradio/sync_block.h>
#include <boost/thread/mutex.hpp>
#include <boost/circular_buffer.hpp>
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;

class injector_float;

typedef boost::shared_ptr<injector_float> injector_float_sptr;

injector_float_sptr make_injector_float();

class injector_float : public gr::sync_block
{
    friend injector_float_sptr make_injector_float();

public:
    injector_float();
    ~injector_float();

    int work(int noutput_items,
             gr_vector_const_void_star &input_items,
             gr_vector_void_star &output_items);

    void set_samples(vec in);

private:

    boost::mutex d_mutex;
    vector<float> d_buffer;
    long count;

};


#endif /* INJECTOR_FLOAT_H */
