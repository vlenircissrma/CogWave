#ifndef INJECTOR_CHAR_H
#define INJECTOR_CHAR_H

#include <gnuradio/sync_block.h>
#include <boost/thread/mutex.hpp>
#include <boost/circular_buffer.hpp>
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;

class injector_char;

typedef boost::shared_ptr<injector_char> injector_char_sptr;

injector_char_sptr make_injector_char();

class injector_char : public gr::sync_block
{
    friend injector_char_sptr make_injector_char();

public:
    injector_char();
    ~injector_char();

    int work(int noutput_items,
             gr_vector_const_void_star &input_items,
             gr_vector_void_star &output_items);

    void set_samples(vector<unsigned char> in);

private:

    boost::mutex d_mutex;
    vector<char> d_buffer;
    long count;

};


#endif /* INJECTOR_CHAR_H */
