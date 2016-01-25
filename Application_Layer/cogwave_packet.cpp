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

#include "cogwave_packet.h"
#include "fstream"

union conversion {
     int i;
     double d;
     float f;
     long unsigned u;
     char c;
     char doublechar[8];
     char intchar[4];

};


CogWave_Packet::CogWave_Packet(int nb_bits){

    is_ber_count=false;
    packet_size=nb_bits;
    is_ip=false;

    int status;
    status=mkfifo("video_inputpipe",0666);

    if(status==0){
           cout << "The file video_inputpipe has been created" << endl;
    }
    if (status==-1){
           cout << "The file video_inputpipe already exists"<< endl;
    }
    status=mkfifo("audio_inputpipe",0666);

    if(status==0){
           cout << "The file audio_inputpipe has been created" << endl;
    }
    if (status==-1){
           cout << "The file audio_inputpipe already exists"<< endl;
    }
    status=mkfifo("text_inputpipe",0666);

    if(status==0){
           cout << "The file text_inputpipe has been created" << endl;
    }
    if (status==-1){
           cout << "The file text_inputpipe already exists"<< endl;
    }

    status=mkfifo("video_outputpipe",0666);

    if(status==0){
           cout << "The file video_outputpipe has been created" << endl;
    }
    if (status==-1){
           cout << "The file video_outputpipe already exists"<< endl;
    }

    status=mkfifo("audio_outputpipe",0666);

    if(status==0){
           cout << "The file audio_outputpipe has been created" << endl;
    }
    if (status==-1){
           cout << "The file audio_outputpipe already exists"<< endl;
    }

    status=mkfifo("text_outputpipe",0666);

    if(status==0){
           cout << "The file text_outputpipe has been created" << endl;
    }
    if (status==-1){
           cout << "The file text_outputpipe already exists"<< endl;
    }

    fec = new FEC();

    fd_input_video = open("video_inputpipe", O_RDONLY | O_NONBLOCK);
    fd_input_audio = open("audio_inputpipe", O_RDONLY | O_NONBLOCK);
    fd_input_text = open("text_inputpipe", O_RDONLY | O_NONBLOCK);

    packetnotx=0;
    packetnorx=0;
    previous_packetnorx=-1;


    bvec polynomial("1 0 0 0 0 0 0 0 0 0 0 1 0 1 1 0 1");
    lfsr.set_connections(polynomial);
    bvec state("0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1");
    //bvec state=randb(16);
    lfsr.set_state(state);
    scrambling=lfsr.shift(nb_bits);

    total_ber_size=0;
    total_nb_errors=1;
    error_rate=0.5;

}

void CogWave_Packet::restart_video(){


    close(fd_input_video);
    fd_input_video = open("video_inputpipe", O_RDONLY | O_NONBLOCK);

}
void CogWave_Packet::restart_audio(){

    close(fd_input_audio);
    fd_input_audio = open("audio_inputpipe", O_RDONLY | O_NONBLOCK);
}


void CogWave_Packet::file_close(){



    if(video_outfile.is_open())
        video_outfile.close();
    if(audio_outfile.is_open())
        audio_outfile.close();
    if(text_outfile.is_open())
        text_outfile.close();

}

bvec CogWave_Packet::encode_packet(int src_address,int dst_address, int &nb_read)
{


    bvec data_packet(packet_size);
    data_packet=randb(packet_size);
    nb_read=0;

    //cout << packet_size << endl;
    vector<char> information(18);
    information[0]='!';
    information[1]='!';
    information[2]='H';
    conversion src_address_char;
    src_address_char.i=src_address;
    information[3]=src_address_char.intchar[0];
    information[4]=src_address_char.intchar[1];
    information[5]=src_address_char.intchar[2];
    information[6]=src_address_char.intchar[3];
    conversion dst_address_char;
    dst_address_char.i=dst_address;
    information[7]=dst_address_char.intchar[0];
    information[8]=dst_address_char.intchar[1];
    information[9]=dst_address_char.intchar[2];
    information[10]=dst_address_char.intchar[3];
    conversion packetnochar;
    packetnochar.i=packetnotx;
    information[11]=packetnochar.intchar[0];
    information[12]=packetnochar.intchar[1];
    information[13]=packetnochar.intchar[2];
    information[14]=packetnochar.intchar[3];
    information[15]='!';
    information[16]='!';
    information[17]='H';


    vector<char> buff_video;
    vector<char> buff_audio;
    vector<char> buff_text;

    buff_video.resize(12000);
    int nb_read_video=read(fd_input_video, &buff_video.front(), buff_video.size());
    if(nb_read_video>0)
        buff_video.resize(nb_read_video);
    else
        buff_video.resize(0);


    buff_audio.resize(4000);
    int nb_read_audio=read(fd_input_audio, &buff_audio.front(), buff_audio.size());
    if(nb_read_audio>0)
        buff_audio.resize(nb_read_audio);
    else
        buff_audio.resize(0);

    buff_text.resize(100);
    int nb_read_text=read(fd_input_text, &buff_text.front(), buff_text.size());
    if(nb_read_text>0)
        buff_text.resize(nb_read_text);
    else
        buff_text.resize(0);


    bvec transmitted_bits_jpeg;
    bvec transmitted_bits_text;
    bvec transmitted_bits_mp3;

    unsigned int i=0;
    unsigned int j=0;
    int soi=-1;
    int eoi=-1;
    while((soi==-1)&&(eoi==-1)&&(i<buff_video.size())){
        if((buff_video[i]==(char)0xFF)&&(buff_video[i+1]==(char)0xD8)){

        soi=i;
        j=soi+2;
        while((eoi==-1)&&(j<buff_video.size())){
            if((buff_video[j]==(char)0xFF)&&(buff_video[j+1]==(char)0xD9)){

            eoi=j;

            }
        j++;

        }
     }
    i++;

    }


    i=0;
    j=0;
    int som=-1;
    int eom=-1;
    while((som==-1)&&(eom==-1)&&(i<buff_audio.size())){
        if((buff_audio[i]==(char)0xFF)&&(buff_audio[i+1]==(char)0xF3)){
        som=i;
        j=buff_audio.size()-1;
        while((eom==-1)&&((int)j>som+5)){
            if((buff_audio[j]==(char)0xFF)&&(buff_audio[j+1]==(char)0xF3)){
            eom=j;

            }
        j--;

        }
     }
    i++;

    }

    i=0;
    j=0;
    int sot=-1;
    int eot=-1;
    while((sot==-1)&&(eot==-1)&&(i<buff_text.size())){
        if((buff_text[i]=='!')&&(buff_text[i+1]=='!')&&(buff_text[i+2]=='T')){
        sot=i;
        j=sot+3;
        while((eot==-1)&&(j<buff_text.size())){
            if((buff_text[j]=='!')&&(buff_text[j+1]=='!')&&(buff_text[j+2]=='T')){
            eot=j;

            }
        j++;

        }
     }
    i++;

    }



    if((soi!=-1)&&(eoi!=-1)){
        vector<char> jpeg(eoi-soi+2);
        for(int k=0;k<eoi-soi+2;k++)
            jpeg[k]=buff_video[soi+k];
        transmitted_bits_jpeg=charvec2bvec(jpeg);

        nb_read=nb_read+jpeg.size();


    }else{
      transmitted_bits_jpeg.set_size(0);
    }
    if((som!=-1)&&(eom!=-1)){
        vector<char> mp3(eom-som);
        for(int k=0;k<eom-som;k++)
            mp3[k]=buff_audio[som+k];
        transmitted_bits_mp3=charvec2bvec(mp3);

        nb_read=nb_read+mp3.size();



    }else{
      transmitted_bits_mp3.set_size(0);
    }
    if((sot!=-1)&&(eot!=-1)){
        vector<char> text(eot-sot+3);
        for(int k=0;k<eot-sot+3;k++)
            text[k]=buff_text[sot+k];
        transmitted_bits_text=charvec2bvec(text);

        nb_read=nb_read+text.size();

    }else{
      transmitted_bits_text.set_size(0);
    }
    bvec merge_transmitted_bits;
    merge_transmitted_bits.set_size(0);
    if(((soi!=-1)&&(eoi!=-1))||((sot!=-1)&&(eot!=-1))||((som!=-1)&&(eom!=-1))){
        merge_transmitted_bits.set_size(transmitted_bits_text.size()+transmitted_bits_mp3.size()+transmitted_bits_jpeg.size());
        merge_transmitted_bits.replace_mid(0,transmitted_bits_text);
        merge_transmitted_bits.replace_mid(transmitted_bits_text.size(),transmitted_bits_mp3);
        merge_transmitted_bits.replace_mid(transmitted_bits_text.size()+transmitted_bits_mp3.size(),transmitted_bits_jpeg);
        //CRC
        CRC_Code crc(string("CRC-32"));
        merge_transmitted_bits = crc.encode(merge_transmitted_bits);
        //FEC
        merge_transmitted_bits=fec->encode_packet(merge_transmitted_bits);

    }
    if(is_ip){

        vector<char> buff((packet_size-512)/8/fec->rate);
        nb_read=read(ptr, &buff.front(), buff.size());
        if(nb_read>0){
            vector<char> buff2(buff.begin(),buff.begin()+nb_read);
             merge_transmitted_bits=charvec2bvec(buff2);
             //CRC
             CRC_Code crc(string("CRC-32"));
             merge_transmitted_bits = crc.encode(merge_transmitted_bits);
             //FEC
             merge_transmitted_bits=fec->encode_packet(merge_transmitted_bits);
        }
        else{
            merge_transmitted_bits.set_size(0);
        }
    }

    vector<char> delimiter(3);
    delimiter[0]='!';
    delimiter[1]='!';
    delimiter[2]='D';


    if(is_ber_count==true){
        merge_transmitted_bits.set_size(packet_size-512);
        merge_transmitted_bits.zeros();
        scrambling=lfsr.shift(merge_transmitted_bits.size());
        nb_read=1;
    }

    bvec information_bits=charvec2bvec(information);
    bvec delimiter_bits=charvec2bvec(delimiter);
    if((information_bits.size()+delimiter_bits.size()+merge_transmitted_bits.size()<=data_packet.size())&&(merge_transmitted_bits.size()!=0)){
        data_packet.replace_mid(0,information_bits);
        data_packet.replace_mid(18*8,merge_transmitted_bits+scrambling.get(0,merge_transmitted_bits.size()-1));
        data_packet.replace_mid(18*8+merge_transmitted_bits.size(),delimiter_bits);
        packetnotx=packetnotx+1;
    }
    else{
        if(information_bits.size()+delimiter_bits.size()+merge_transmitted_bits.size()>data_packet.size()){
            cout << "WARNING !!! SLOT LENGTH TOO SMALL !!!" << endl;
        }

    }

    return data_packet;



}

bvec CogWave_Packet::encode_ack(int packetnorx,int src_address,int dst_address)
{

    int ack_size=packet_size;
    bvec data_packet(ack_size);
    data_packet=randb(ack_size);

    vector<char> information(18);
    information[0]='!';
    information[1]='!';
    information[2]='A';
    conversion src_address_char;
    src_address_char.i=src_address;
    information[3]=src_address_char.intchar[0];
    information[4]=src_address_char.intchar[1];
    information[5]=src_address_char.intchar[2];
    information[6]=src_address_char.intchar[3];
    conversion dst_address_char;
    dst_address_char.i=dst_address;
    information[7]=dst_address_char.intchar[0];
    information[8]=dst_address_char.intchar[1];
    information[9]=dst_address_char.intchar[2];
    information[10]=dst_address_char.intchar[3];
    conversion packetnochar;
    packetnochar.i=packetnorx;
    information[11]=packetnochar.intchar[0];
    information[12]=packetnochar.intchar[1];
    information[13]=packetnochar.intchar[2];
    information[14]=packetnochar.intchar[3];
    information[15]='!';
    information[16]='!';
    information[17]='A';

    bvec information_bits=charvec2bvec(information);

    if(information_bits.size()<data_packet.size()){
        data_packet.replace_mid(0,information_bits);
    }
    else{
        if(information_bits.size()>data_packet.size()){
            cout << "WARNING !!! SLOT LENGTH TOO SMALL !!!" << endl;
        }
    }

    return data_packet;


}

bool CogWave_Packet::decode_packet(bvec received_bits, int my_address, bool &same_packet)
{

    bool packet_ok=false;
    bool end_delimiter=false;

    vector<char> information=bvec2charvec(received_bits);
    conversion packetnoint;
    conversion src_address_int;
    conversion dst_address_int;
    int src_address;
    int dst_address;

    //string t(information.begin(),information.end());
    //cout << t << endl;

    int i=0;
    while((i<=int(information.size())-18)&&(packet_ok==false)){
        if((information[i]=='!')&&(information[i+1]=='!')&&(information[i+2]=='H')&&(information[i+15]=='!')&&(information[i+16]=='!')&&(information[i+17]=='H')){
            //cout << "PREAMBLE FOUND " << i << endl;
            src_address_int.intchar[0]=information[i+3];
            src_address_int.intchar[1]=information[i+4];
            src_address_int.intchar[2]=information[i+5];
            src_address_int.intchar[3]=information[i+6];
            src_address=src_address_int.i;
            dst_address_int.intchar[0]=information[i+7];
            dst_address_int.intchar[1]=information[i+8];
            dst_address_int.intchar[2]=information[i+9];
            dst_address_int.intchar[3]=information[i+10];
            dst_address=dst_address_int.i;
            //cout << "this is the destination address " << dst_address <<endl;
            //cout << "this is my adress " << my_address << endl;
            if(my_address==dst_address){
                //cout << "My adress " << int(information[i+4]-'0') << " " << my_adress << endl;
                //cout << "Source adress " << int(information[i+3]-'0') << " " << src_adress << endl;
                //cout << "Best group from source adress " << int(information[i+5]-'0') << endl;
                packetnoint.intchar[0]=information[i+11];
                packetnoint.intchar[1]=information[i+12];
                packetnoint.intchar[2]=information[i+13];
                packetnoint.intchar[3]=information[i+14];
                packetnorx=packetnoint.i;


                int j=i+18;
                while((j<=int(information.size())-3)&&(end_delimiter==false)){
                    if((information[j]=='!')&&(information[j+1]=='!')&&(information[j+2]=='D')){
                        end_delimiter=true;
                        //cout << "END DELIMITER FOUND " << j << endl;
                        vector<char> buff(j-(i+18));
                        for(int k=0;k<j-(i+18);k++)
                            buff[k]=information[i+18+k];


                        bvec merge_received_bits=charvec2bvec(buff);
                        //cout << "SIZE MERGE RECEIVED BITS " << merge_received_bits.size() << endl;
                        //FEC
                        bvec encoded_bits=fec->decode_packet(merge_received_bits+scrambling.get(0,merge_received_bits.size()-1));
                        //cout << "SIZE FEC DECODED BITS " << encoded_bits.size() << endl;
                        bool error;
                        bvec decoded_bits;
                        if(is_ber_count==false){
                        //CRC
                        CRC_Code crc(string("CRC-32"));
                        error = crc.decode(encoded_bits,decoded_bits);
                        //cout << "SIZE CRC DECODED BITS " << decoded_bits.size() << endl;
                        }
                        else{
                            error=true;
                            decoded_bits=encoded_bits;
                        }
                        if(error==false){
                            cout << my_address << " : CRC KO - "<< src_address << "->" << dst_address;
                        }
                        else{

                          packet_ok=true;
                          if(packetnorx!=previous_packetnorx){
                                cout << my_address << " : CRC OK - " << src_address << "->" << dst_address << " - PACKET NUMBER " << packetnorx;
                                previous_packetnorx=packetnorx;

                            buff=bvec2charvec(decoded_bits);

                            //cout << "Number of chars to be written " << buff.size() << endl;
                            //for(unsigned int i=0;i<buff.size();i++)
                                //cout << buff[i];

                            unsigned int k=0;
                            unsigned int l=0;
                            int soi=-1;
                            int eoi=-1;
                            while((soi==-1)&&(eoi==-1)&&(k<buff.size())){
                                if((buff[k]==(char)0xFF)&&(buff[k+1]==(char)0xD8)){

                                //cout << "start of jpeg found " << k << endl;
                                soi=k;
                                l=soi+2;
                                while((eoi==-1)&&(l<buff.size())){
                                    if((buff[l]==(char)0xFF)&&(buff[l+1]==(char)0xD9)){

                                    //cout << "end of jpeg found " << l << endl;
                                    eoi=l;

                                    }
                                l++;

                                }
                            }
                            k++;

                            }

                            k=0;
                            l=0;
                            int som=-1;
                            int eom=-1;
                            while((som==-1)&&(eom==-1)&&(k<buff.size())){
                                if((buff[k]==(char)0xFF)&&(buff[k+1]==(char)0xF3)){


                                //cout << "start of mp3 found " << k << endl;
                                som=k;
                                l=buff.size()-1;
                                while((eom==-1)&&((int)l>som+5)){
                                    if((buff[l]==(char)0xFF)&&(buff[l+1]==(char)0xF3)){

                                    //cout << "end of mp3 found " << l << endl;
                                    eom=l;

                                    }
                                l--;

                                }
                            }
                            k++;

                            }

                            k=0;
                            l=0;
                            int sot=-1;
                            int eot=-1;
                            while((sot==-1)&&(eot==-1)&&(k<buff.size())){
                                if((buff[k]=='!')&&(buff[k+1]=='!')&&(buff[k+2]=='T')){

                                //cout << "start of text found " << k << endl;
                                sot=k;
                                l=sot+3;
                                while((eot==-1)&&(l<buff.size())){
                                    if((buff[l]=='!')&&(buff[l+1]=='!')&&(buff[l+2]=='T')){

                                    //cout << "end of text found " << l << endl;
                                    eot=l;

                                    }
                                l++;

                                }
                            }
                            k++;

                            }



                            if(((soi!=-1)&&(eoi!=-1))||((sot!=-1)&&(eot!=-1))||((som!=-1)&&(eom!=-1))){


                                if((soi!=-1)&&(eoi!=-1)){

                                    vector<char> jpeg(eoi-soi+2);
                                    for(int k=0;k<eoi-soi+2;k++)
                                        jpeg[k]=buff[soi+k];


                                    if(last_jpeg!=jpeg){
                                        if(!video_outfile.is_open()){
                                            video_outfile.open("video_outputpipe", ifstream::binary);
                                            //cout << "video_outputpipe has been opened for writing"<< endl;
                                        }

                                    video_outfile.write(&jpeg.front(), jpeg.size());
                                    video_outfile.flush();
                                    //cout << "The video data has been written " << jpeg.size() << endl;
                                    }
                                    last_jpeg=jpeg;


                                }
                                if((som!=-1)&&(eom!=-1)){

                                    vector<char> mp3(eom-som);
                                    for(int k=0;k<eom-som;k++)
                                        mp3[k]=buff[som+k];

                                    if(last_mp3!=mp3){
                                        if(!audio_outfile.is_open()){
                                            audio_outfile.open("audio_outputpipe", ifstream::binary);
                                            //cout << "audio_outputpipe has been opened for writing"<< endl;
                                        }
                                        audio_outfile.write(&mp3.front(), mp3.size());
                                        audio_outfile.flush();
                                        //cout << "The audio data has been written" << endl;

                                    }
                                   last_mp3=mp3;

                                }

                                if((sot!=-1)&&(eot!=-1)){

                                    vector<char> text(eot-sot+3);
                                    for(int k=0;k<eot-sot+3;k++)
                                        text[k]=buff[sot+k];

                                    if(!text_outfile.is_open()){

                                        text_outfile.open("text_outputpipe", ifstream::binary);
                                        //cout << "text_outputpipe has been opened for writing"<< endl;

                                    }

                                    text_outfile.write(&text.front(), text.size());
                                    //cout << "The text data has been written" << endl;
                                    text_outfile.close();

                                }
                            }
                            if(is_ip){
                                //Write to an external application (OmNeT++,ns-3...)
                                write(ptr,&buff.front(), buff.size());
                            }

                           }
                          else{

                              cout << my_address << " : SAME PACKET - " << src_address << "->" << dst_address << " - PACKET NUMBER " << packetnorx;
                              same_packet=true;
                          }
                        }

                    }

                j++;
                }
                if(end_delimiter==false){
                    cout << my_address << " : EOF NOT FOUND - "<<  src_address << "->" << dst_address<< " - PACKET NUMBER " << packetnorx;
                }
            }
            else{
                cout << my_address << " : WRONG ADDRESS - "<<  src_address << "->" << dst_address << " - PACKET NUMBER " << packetnorx;
            }
        }

    i++;
    }

    return packet_ok;

}

bool CogWave_Packet::is_preamble(bvec received_bits)
{
    bool preamble_ok=false;
    vector<char> information=bvec2charvec(received_bits.get(0,23));
    if((information[0]=='!')&&(information[1]=='!')&&(information[2]=='H')){
        preamble_ok=true;
    }
    return preamble_ok;

}

bool CogWave_Packet::decode_ack(bvec received_bits,int &packet_number, int my_address)
{
    bool ack_ok=false;
    int src_address;
    int dst_address;
    vector<char> information=bvec2charvec(received_bits);
    conversion src_address_int;
    src_address_int.intchar[0]=information[3];
    src_address_int.intchar[1]=information[4];
    src_address_int.intchar[2]=information[5];
    src_address_int.intchar[3]=information[6];
    src_address=src_address_int.i;
    conversion dst_address_int;
    dst_address_int.intchar[0]=information[7];
    dst_address_int.intchar[1]=information[8];
    dst_address_int.intchar[2]=information[9];
    dst_address_int.intchar[3]=information[10];
    dst_address=dst_address_int.i;

    if(my_address==dst_address){

        conversion packetnoint;
        packetnoint.intchar[0]=information[11];
        packetnoint.intchar[1]=information[12];
        packetnoint.intchar[2]=information[13];
        packetnoint.intchar[3]=information[14];

        packet_number=packetnoint.i;
        ack_ok=true;
    }
    else{
        //cout << my_address << " : WRONG ACK ADDRESS - "<< src_address << "->" << dst_address << endl;
    }
    return ack_ok;

}


bvec CogWave_Packet::charvec2bvec(vector<char> input){
    int bitsize=sizeof(char)*8;
    bvec output(input.size()*bitsize);
    char bintemp;
    bvec temp(bitsize);
    for(int i=0;i<int(input.size());i++){
            bintemp=input[i];
            for(int j=bitsize-1;j>=0;j--){
                temp[j] = bin(bintemp & 1);
                bintemp = (bintemp >> 1);
            }
            output.replace_mid(i*bitsize,temp);
       }
return output;
}

vector<char> CogWave_Packet::bvec2charvec(bvec input){

    int bitsize=sizeof(char)*8;
    bvec bin_vector(bitsize);
    conversion character;
    vector<char> output(input.size()/bitsize);

         for(int i=0;i<input.size()/bitsize;i++){
            bin_vector=input.mid(i*bitsize,bitsize);
            unsigned long tempbin=0;
            for(int j=0;j<bitsize;j++){
                tempbin += (unsigned long)pow2(bitsize-j-1)*int(bin_vector[j]);
            }
            character.u=tempbin;
            output[i]=character.c;
        }
return output;
}

double CogWave_Packet::ber_count(bvec input){


    if(input.size()>160){
        int ber_size=packet_size-512;
        if(input.size()<144+ber_size)
            ber_size=input.size()-144;

        LFSR lfsr2;
        bvec polynomial("1 0 0 0 0 0 0 0 0 0 0 1 0 1 1 0 1");
        lfsr2.set_connections(polynomial);
        bvec state=input.get(144,159);
        lfsr2.set_state(reverse(state));
        scrambling=concat(state,lfsr2.shift(ber_size));
        int nb_errors=sum(to_ivec(scrambling.get(0,ber_size-1)+input.get(144,144+ber_size-1)));
        total_ber_size=total_ber_size+ber_size;
        total_nb_errors=total_nb_errors+nb_errors;
        error_rate=((double) total_nb_errors)/total_ber_size;

    }
    return error_rate;

}

