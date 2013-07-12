#include "dads_fec.h"

DADS_FEC::DADS_FEC()
{

    rate=1;

    if(rate==3){
        ivec generator(3);
        generator(0)=0133;
        generator(1)=0165;
        generator(2)=0171;
        code.set_generator_polynomials(generator, 7);
    }
    if(rate==2){
        ivec generator(2);
        generator(0)=0023;
        generator(1)=0035;
        code.set_generator_polynomials(generator, 5);
    }

}


bvec DADS_FEC::encode_packet(bvec diff_data_packet){


    if(rate>1){
        bvec encoded_data_packet;
        code.encode(diff_data_packet, encoded_data_packet);
        return encoded_data_packet;
    }
    else{
        return diff_data_packet;
    }
}




bvec DADS_FEC::decode_packet(bvec received_bits){

    if(rate>1){
        BPSK bpsk;
        vec received_modulated=bpsk.modulate_bits(received_bits);
        return code.decode(received_modulated);
    }
    else{
        return received_bits;
    }


}
