#ifndef INJECTOR_COMPLEX_H
#define INJECTOR_COMPLEX_H

#include <gnuradio/sync_block.h>
#include <boost/thread/mutex.hpp>
#include <boost/circular_buffer.hpp>
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;

class injector_complex;

typedef boost::shared_ptr<injector_complex> injector_complex_sptr;

injector_complex_sptr make_injector_complex();

class injector_complex : public gr::sync_block
{
    friend injector_complex_sptr make_injector_complex();

public:
    injector_complex();
    ~injector_complex();

    int work(int noutput_items,
             gr_vector_const_void_star &input_items,
             gr_vector_void_star &output_items);

    void set_samples(cvec in);

private:

    boost::mutex d_mutex;
    vector<gr_complex> d_buffer;
    long count;

};


#endif /* INJECTOR_COMPLEX_H */
