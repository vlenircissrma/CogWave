#include <math.h>
#include <gnuradio/io_signature.h>
#include "sniffer_char.h"



sniffer_char_sptr make_sniffer_char(int buffsize)
{
    return gnuradio::get_initial_sptr(new sniffer_char(buffsize));
}

sniffer_char::sniffer_char(int buffsize)
    : gr::sync_block ("sniffer_char",
          gr::io_signature::make(1, 1, sizeof(char)),
          gr::io_signature::make(0, 0, 0))
{

    d_buffer.set_capacity(buffsize);
    count=0;

}

sniffer_char::~sniffer_char()
{

}

int sniffer_char::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
{
    int i;
    const unsigned char *in = (const unsigned char *)input_items[0];

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

void sniffer_char::set_buffer_size(int newsize)
{
    boost::mutex::scoped_lock lock(d_mutex);
    d_buffer.set_capacity(newsize);
}

vector<unsigned char> sniffer_char::get_samples()
{

    boost::mutex::scoped_lock lock(d_mutex);

    int num = d_buffer.size();
    //cout << num << endl;
    //cout << d_buffer.capacity() << endl;
    vector<unsigned char> out;
    out.resize(num);
    for(int i=0;i<num;i++)
        out[i]=d_buffer[i];
    d_buffer.clear();
    count=0;

    return out;
}

