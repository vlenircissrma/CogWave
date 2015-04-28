#include <math.h>
#include <gnuradio/io_signature.h>
#include "injector_char.h"



injector_char_sptr make_injector_char()
{
    return gnuradio::get_initial_sptr(new injector_char());
}

injector_char::injector_char()
    : gr::sync_block ("injector_char",
          gr::io_signature::make(0, 0, 0),
          gr::io_signature::make(1, 1, sizeof(char)))
{

    d_buffer.resize(0);
    count=0;
}

injector_char::~injector_char()
{

}

int injector_char::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
{
    int i;
    unsigned char *out = (unsigned char *)output_items[0];

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


void injector_char::set_samples(vector<unsigned char> in)
{

    boost::mutex::scoped_lock lock(d_mutex);
    d_buffer.resize(in.size());
    for(int i=0;i<in.size();i++)
        d_buffer[i]=in[i];

}

