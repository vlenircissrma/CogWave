#include <math.h>
#include <gnuradio/io_signature.h>
#include "injector_float.h"



injector_float_sptr make_injector_float()
{
    return gnuradio::get_initial_sptr(new injector_float());
}

injector_float::injector_float()
    : gr::sync_block ("injector_float",
          gr::io_signature::make(0, 0, 0),
          gr::io_signature::make(1, 1, sizeof(float)))
{

    d_buffer.resize(0);
    count=0;
}

injector_float::~injector_float()
{

}

int injector_float::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
{
    int i;
    float *out = (float *)output_items[0];

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


void injector_float::set_samples(vec in)
{

    boost::mutex::scoped_lock lock(d_mutex);
    d_buffer.resize(in.size());
    for(int i=0;i<in.size();i++)
        d_buffer[i]=in[i];

}

