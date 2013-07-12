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

#ifndef DADS_FRAMING_H
#define DADS_FRAMING_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <QString>
#include "dads_fec.h"
#include "dads_multimediaread.h"

class DADS_Framing
{
public:
    DADS_Framing(int nb_bits);
    bvec encode_frame(int my_adress,int dest_adress, int &nb_read);
    bool decode_frame(bvec received_bits, int my_adress,int &src_adress);
    bvec charvec2bvec(vector<char> input);
    vector<char> bvec2charvec(bvec input);
    void file_close();
    void restart_video();
    void restart_audio();

    DADS_Multimediaread *file_video;
    DADS_Multimediaread *file_audio;
    DADS_Multimediaread *file_text;

private:

    int frame_size;

    DADS_FEC *fec;


    ifstream video_infile;
    ifstream audio_infile;
    ifstream text_infile;
    ofstream video_outfile;
    ofstream audio_outfile;
    ofstream text_outfile;

    vector<char> buff_video;
    vector<char> buff_audio;
    vector<char> buff_text;
    vector<char> last_mp3;
    vector<char> last_jpeg;
};
#endif // DADS_FRAMING_H
