#include <math.h>
#include <gnuradio/io_signature.h>
#include "sniffer_complex.h"



sniffer_complex_sptr make_sniffer_complex(int buffsize)
{
    return gnuradio::get_initial_sptr(new sniffer_complex(buffsize));
}

sniffer_complex::sniffer_complex(int buffsize)
    : gr::sync_block ("sniffer_complex",
          gr::io_signature::make(1, 1, sizeof(gr_complex)),
          gr::io_signature::make(0, 0, 0))
{

    d_buffer.set_capacity(buffsize);
    count=0;

}

sniffer_complex::~sniffer_complex()
{

}

int sniffer_complex::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
{
    int i;
    const gr_complex *in = (const gr_complex *)input_items[0];

    (void) output_items;

    boost::mutex::scoped_lock lock(d_mutex);
    i=0;
    while((count<d_buffer.capacity())&&(i<noutput_items)){
        d_buffer.push_back(in[i]);
        count++;
        i++;
    }
    return noutput_items;
}

void sniffer_complex::set_buffer_size(int newsize)
{
    boost::mutex::scoped_lock lock(d_mutex);
    d_buffer.set_capacity(newsize);
}

cvec sniffer_complex::get_samples()
{

    boost::mutex::scoped_lock lock(d_mutex);

    int num = d_buffer.size();
    //cout << num << endl;
    //cout << d_buffer.capacity() << endl;
    cvec out;
    out.set_size(num);
    for(int i=0;i<num;i++)
        out[i]=d_buffer[i];
    d_buffer.clear();
    count=0;

    return out;
}

