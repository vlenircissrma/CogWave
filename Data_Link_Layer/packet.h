/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////                            Author: Vincent Le Nir                                         ///////
///////                                 Royal Military Academy                                    ///////
///////           Department Communications, Information Systems & Sensors (CISS)                 ///////
///////                             30, Avenue de la Renaissance                                  ///////
///////                                B-1000 Brussels BELGIUM                                    ///////
///////                                   Tel: +3227426624                                        ///////
///////                                 email:vincent.lenir@rma.ac.be                             ///////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PACKET_H
#define PACKET_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <QString>
#include "Physical_Layer/fec.h"

class Packet
{
public:
    Packet(int nb_bits);
    bvec encode_packet(int src_address,int dst_address, int &nb_read, int fd_ext);
    bool decode_packet(bvec received_bits, int my_adress, bool &same_packet, int fd_ext);
    bvec encode_ack(int packetnorx,int my_adress,int dest_adress);
    bool decode_ack(bvec received_bits,int &packet_number, int my_address);
    bvec charvec2bvec(vector<char> input);
    vector<char> bvec2charvec(bvec input);
    double ber_count(bvec input);
    void file_close();
    void restart_video();
    void restart_audio();
    bool is_ber_count;
    long int total_ber_size;
    long int total_nb_errors;
    int packetnorx;
    int packetnotx;
private:

    int packet_size;

    FEC *fec;

    int fd_input_video, fd_input_audio, fd_input_text;
    int fd_output_video, fd_output_audio, fd_output_text;

    ofstream video_outfile;
    ofstream audio_outfile;
    ofstream text_outfile;

    vector<char> last_mp3;
    vector<char> last_jpeg;


    int previous_packetnorx;
    bvec scrambling;
    LFSR lfsr;

    double error_rate;
};
#endif // PACKET_H

