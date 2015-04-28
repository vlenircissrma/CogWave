#include <math.h>
#include <gnuradio/io_signature.h>
#include "injector_complex.h"



injector_complex_sptr make_injector_complex()
{
    return gnuradio::get_initial_sptr(new injector_complex());
}

injector_complex::injector_complex()
    : gr::sync_block ("injector_complex",
          gr::io_signature::make(0, 0, 0),
          gr::io_signature::make(1, 1, sizeof(gr_complex)))
{

    d_buffer.resize(0);
    count=0;
}

injector_complex::~injector_complex()
{

}

int injector_complex::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
{
    int i;
    gr_complex *out = (gr_complex *)output_items[0];

    boost::mutex::scoped_lock lock(d_mutex);
    i=0;

    while((count<d_buffer.size())&&(i<noutput_items)){
          out[i]=d_buffer[count];
          i++;
          count++;
    }
    if(count==d_buffer.size()&&(i<noutput_items)){
        noutput_items=i;
        count=0;
        d_buffer.clear();
    }
    return noutput_items;
}


void injector_complex::set_samples(cvec in)
{

    boost::mutex::scoped_lock lock(d_mutex);
    d_buffer.resize(in.size());
    for(int i=0;i<in.size();i++)
        d_buffer[i]=in[i];

}

