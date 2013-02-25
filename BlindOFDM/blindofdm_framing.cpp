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

#include "blindofdm_framing.h"
#include "fstream"

union conversion {
     int i;
     double d;
     float f;
     long unsigned u;
     char c;
     char doublechar[8];
};


BlindOFDM_Framing::BlindOFDM_Framing(int packet_unit,int nb_packets){

    min_packet_size=packet_unit;
    packet_size=nb_packets*min_packet_size;

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



    fec = new BlindOFDM_FEC();
    file_video = new BlindOFDM_Multimediaread((char*)"video_inputpipe",false);
    file_audio = new BlindOFDM_Multimediaread((char*)"audio_inputpipe",false);
    file_text = new BlindOFDM_Multimediaread((char*)"text_inputpipe",true);
    buff_video.resize(12000);
    buff_audio.resize(4000);
    buff_text.resize(100);

}

void BlindOFDM_Framing::restart_video(){

    file_video->stopped=true;
    delete file_video;
    file_video = new BlindOFDM_Multimediaread((char*)"video_inputpipe",false);

}
void BlindOFDM_Framing::restart_audio(){

    file_audio->stopped=true;
    delete file_audio;
    file_audio = new BlindOFDM_Multimediaread((char*)"audio_inputpipe",false);

}


void BlindOFDM_Framing::file_close(){


    file_video->stopped=true;
    delete file_video;
    file_video = new BlindOFDM_Multimediaread((char*)"video_inputpipe",false);
    file_audio->stopped=true;
    delete file_audio;
    file_audio = new BlindOFDM_Multimediaread((char*)"audio_inputpipe",false);


    if(video_outfile.is_open())
        video_outfile.close();
    if(audio_outfile.is_open())
        audio_outfile.close();
    if(text_outfile.is_open())
        text_outfile.close();

}

bvec BlindOFDM_Framing::encode_frame(int my_adress,int dest_adress,int best_group, double sync_time, int &nb_read)
{

    bvec data_packet(packet_size);
    bvec diff_data_packet(packet_size);
    data_packet.zeros();
    diff_data_packet.zeros();


    for(int i=0;i<packet_size/min_packet_size;i++){
        //Repeat with the scrambler
        //data_packet.replace_mid(i*min_packet_size,scrambler);
        //Random data each time
        //if((i/3)*3==i)
        //    data_packet.replace_mid(i*min_packet_size,ones_b(min_packet_size));
        //else
            data_packet.replace_mid(i*min_packet_size,randb(min_packet_size));
    }

    vector<char> information(17);
    information[0]='!';
    information[1]='!';
    information[2]='H';
    information[3]='0' + my_adress;
    information[4]='0' + dest_adress;
    information[5]='0' + best_group;
    conversion timechar;
    timechar.d=sync_time;
    information[6]=timechar.doublechar[0];
    information[7]=timechar.doublechar[1];
    information[8]=timechar.doublechar[2];
    information[9]=timechar.doublechar[3];
    information[10]=timechar.doublechar[4];
    information[11]=timechar.doublechar[5];
    information[12]=timechar.doublechar[6];
    information[13]=timechar.doublechar[7];
    information[14]='!';
    information[15]='!';
    information[16]='H';

    //cout << "Number_of_bits"  << Number_of_bits << endl;

   /*if(!infile.is_open()){
         infile.open((char*)"inputpipe", ifstream::binary);
        //cout << " inputpipe has been opened for reading"<< endl;
    }
    else{
        //cout << " inputpipe is already opened for reading"<< endl;
    }
    //vector<char> buff=bvec2charvec(randb(packet_size-21*8));
    vector<char> buff((packet_size-21*8)/8);
    //usleep(700000);
    nb_read=0;
    while(nb_read==0){
        nb_read=infile.readsome(&buff.front(), buff.size());
    }*/

    //infile.read(&buff.front(), buff.size());
    //int nb_read=buff.size();
    //infile.close();


    if(!file_video->isRunning()){

        file_video->buff_size=buff_video.size();
        file_video->start();

    }
    buff_video=file_video->buff;
    int nb_read_video=file_video->nb_read;

    if(!file_audio->isRunning()){

        file_audio->buff_size=buff_audio.size();
        file_audio->start();

    }
    buff_audio=file_audio->buff;
    int nb_read_audio=file_audio->nb_read;
    /*if(!audio_infile.is_open()){
             audio_infile.open((char*)"audio_inputpipe", ifstream::binary);
            //cout << " inputpipe has been opened for reading"<< endl;
        }
        else{
            //cout << " inputpipe is already opened for reading"<< endl;
        }
    //int nb_read_audio=buff_audio.size();
    while(audio_infile.rdbuf()->in_avail()<buff_audio.size()/2)
        0;
    int nb_read_audio=audio_infile.readsome(&buff_audio.front(), buff_audio.size());
    //audio_infile.read(&buff_audio.front(), buff_audio.size());*/

    if(!file_text->isRunning()){

        file_text->buff_size=buff_text.size();
        file_text->start();

    }
    buff_text=file_text->buff;
    int nb_read_text=file_text->nb_read;


    nb_read=nb_read_text+nb_read_audio+nb_read_video;

    bvec transmitted_bits_jpeg;
    bvec transmitted_bits_text;
    bvec transmitted_bits_mp3;

    unsigned int i=0;
    unsigned int j=0;
    int soi=-1;
    int eoi=-1;
    while((soi==-1)&&(eoi==-1)&&(i<buff_video.size())){
        if((buff_video[i]==(char)0xFF)&&(buff_video[i+1]==(char)0xD8)){

        //cout << "start of jpeg found " << i << endl;
        soi=i;
        j=soi+2;
        while((eoi==-1)&&(j<buff_video.size())){
            if((buff_video[j]==(char)0xFF)&&(buff_video[j+1]==(char)0xD9)){

            //cout << "end of jpeg found " << j << endl;
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

        //cout << "start of mp3 found " << i << endl;
        som=i;
        j=buff_audio.size()-1;
        while((eom==-1)&&(j>som+5)){
            if((buff_audio[j]==(char)0xFF)&&(buff_audio[j+1]==(char)0xF3)){

            //cout << "end of mp3 found " << j << endl;
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

        //cout << "start of text found " << i << endl;
        sot=i;
        j=sot+3;
        while((eot==-1)&&(j<buff_text.size())){
            if((buff_text[j]=='!')&&(buff_text[j+1]=='!')&&(buff_text[j+2]=='T')){

            //cout << "end of text found " << j << endl;
            eot=j;

            }
        j++;

        }
     }
    i++;

    }


   //cout << "Number of characters read " << nb_read << endl;

    if((soi!=-1)&&(eoi!=-1)){
        vector<char> jpeg(eoi-soi+2);
        for(int k=0;k<eoi-soi+2;k++)
            jpeg[k]=buff_video[soi+k];
        transmitted_bits_jpeg=charvec2bvec(jpeg);
        //cout << "jpeg " << jpeg.size() << endl;
        //cout << "JPEG SIZE " << transmitted_bits_jpeg.size() << endl;



    }else{
      transmitted_bits_jpeg.set_size(0);
    }
    if((som!=-1)&&(eom!=-1)){
        vector<char> mp3(eom-som);
        for(int k=0;k<eom-som;k++)
            mp3[k]=buff_audio[som+k];
        transmitted_bits_mp3=charvec2bvec(mp3);
         //cout << "mp3 " << mp3.size() << endl;
        //cout << "MP3 SIZE " << transmitted_bits_mp3.size() << endl;



        //Test audio
        /* if(last_mp3!=mp3){
        if(!audio_outfile.is_open()){
            audio_outfile.open("audio_outputpipe", ifstream::binary);
            //cout << "audio_outputpipe has been opened for writing"<< endl;

        }
        audio_outfile.write(&mp3.front(), mp3.size());
        audio_outfile.flush();

        }
        last_mp3=mp3;*/





    }else{
      transmitted_bits_mp3.set_size(0);
    }
    if((sot!=-1)&&(eot!=-1)){
        vector<char> text(eot-sot+3);
        for(int k=0;k<eot-sot+3;k++)
            text[k]=buff_text[sot+k];
        transmitted_bits_text=charvec2bvec(text);
        //cout << "text " << text.size() << endl;
        //cout << "TEXT SIZE " << transmitted_bits_text.size() << endl;

    }else{
      transmitted_bits_text.set_size(0);
    }
    bvec merge_transmitted_bits;
    if(((soi!=-1)&&(eoi!=-1))||((sot!=-1)&&(eot!=-1))||((som!=-1)&&(eom!=-1))){
        merge_transmitted_bits.set_size(transmitted_bits_text.size()+transmitted_bits_mp3.size()+transmitted_bits_jpeg.size());
        merge_transmitted_bits.replace_mid(0,transmitted_bits_text);
        merge_transmitted_bits.replace_mid(transmitted_bits_text.size(),transmitted_bits_mp3);
        merge_transmitted_bits.replace_mid(transmitted_bits_text.size()+transmitted_bits_mp3.size(),transmitted_bits_jpeg);
        //cout << " SIZE MERGE TRANSMITTED BITS " << merge_transmitted_bits.size() << endl;
        RNG_reset(0);
        bvec scrambling=randb(merge_transmitted_bits.size());
        //CRC
        CRC_Code crc(string("CRC-32"));
        merge_transmitted_bits = crc.encode(merge_transmitted_bits+scrambling);
        //cout << " SIZE CRC TRANSMITTED BITS " << merge_transmitted_bits.size() << endl;
        //FEC
        merge_transmitted_bits=fec->encode_packet(merge_transmitted_bits);
        //cout << " SIZE FEC TRANSMITTED BITS " << merge_transmitted_bits.size() << endl;
    }
    else{
        merge_transmitted_bits.set_size(0);
    }


    //cout << "My adress " << int(information[3]-'0') << endl;
    //cout << "Destination adress " << int(information[4]-'0') << endl;
    //cout << "Best group from my adress " << int(information[5]-'0') << endl;

    vector<char> delimiter(3);
    delimiter[0]='!';
    delimiter[1]='!';
    delimiter[2]='D';





    bvec information_bits=charvec2bvec(information);
    bvec delimiter_bits=charvec2bvec(delimiter);
    if((information_bits.size()+delimiter_bits.size()+merge_transmitted_bits.size()<data_packet.size())&&(merge_transmitted_bits.size()!=0)){
        data_packet.replace_mid(512*2,information_bits);
        data_packet.replace_mid(512*2+17*8,merge_transmitted_bits);
        data_packet.replace_mid(512*2+17*8+merge_transmitted_bits.size(),delimiter_bits);

    }
    else{
        //if(merge_transmitted_bits.size()==0)
        //    cout << "WARNING !!! NO TEXT NOR VIDEO NOR AUDIO FOUND !!!" << endl;
        if(information_bits.size()+delimiter_bits.size()+merge_transmitted_bits.size()>data_packet.size())
            cout << "WARNING !!! FRAME LENGTH TOO SMALL !!!" << endl;
    }

    return data_packet;




}


bool BlindOFDM_Framing::decode_frame(bvec received_bits, int my_adress,int &src_adress,int &best_group, int &start_frame, double &sync_time, int num_subchannels)
{
    bool packet_ok=false;
    bool end_delimiter=false;


    vector<char> information=bvec2charvec(received_bits);

    conversion timedouble;
    //string t(information.begin(),information.end());
    //cout << t << endl;
    int i=0;
    while((i<int(information.size())-17)&&(packet_ok==false)){
        if((information[i]=='!')&&(information[i+1]=='!')&&(information[i+2]=='H')&&(information[i+14]=='!')&&(information[i+15]=='!')&&(information[i+16]=='H')){
            packet_ok=true;
            if(('0'+ my_adress)==information[i+4]){
                src_adress=int(information[i+3]-'0');
                best_group=int(information[i+5]-'0');
                if((best_group<0)||(best_group>=num_subchannels))
                        best_group=0;
                //cout << "My adress " << int(information[i+4]-'0') << endl;
                //cout << "Source adress " << int(information[i+3]-'0') << endl;
                //cout << "Best group from source adress " << int(information[i+5]-'0') << endl;
                start_frame=i-64*2;
                timedouble.doublechar[0]=information[i+6];
                timedouble.doublechar[1]=information[i+7];
                timedouble.doublechar[2]=information[i+8];
                timedouble.doublechar[3]=information[i+9];
                timedouble.doublechar[4]=information[i+10];
                timedouble.doublechar[5]=information[i+11];
                timedouble.doublechar[6]=information[i+12];
                timedouble.doublechar[7]=information[i+13];
                sync_time=timedouble.d;

                int j=i+18;
                while((j<int(information.size())-3)&&(end_delimiter==false)){
                    if((information[j]=='!')&&(information[j+1]=='!')&&(information[j+2]=='D')){
                        end_delimiter=true;
                        //cout << "END DELIMITER FOUND !!!!!!!!!" << endl;
                        vector<char> buff(j-(i+17));
                        for(int k=0;k<j-(i+17);k++)
                            buff[k]=information[i+17+k];


                        bvec merge_received_bits=charvec2bvec(buff);


                        //cout << "SIZE MERGE RECEIVED BITS " << merge_received_bits.size() << endl;
                        //FEC
                        bvec encoded_bits=fec->decode_packet(merge_received_bits);
                        //cout << "SIZE FEC DECODED BITS " << encoded_bits.size() << endl;
                        //CRC
                        CRC_Code crc(string("CRC-32"));
                        bvec decoded_bits;
                        bool error = crc.decode(encoded_bits,decoded_bits);
                        //cout << "SIZE CRC DECODED BITS " << decoded_bits.size() << endl;

                        if(error==false)
                            cout << "CRC KO" << endl;
                        else{
                            cout << "CRC OK" << endl;
                            RNG_reset(0);
                            bvec scrambling=randb(decoded_bits.size());
                            buff=bvec2charvec(decoded_bits+scrambling);


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
                                while((eom==-1)&&(l>som+5)){
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
                                //outfile.close();
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
                                    //outfile.close();
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

                    }
                j++;
                }
                if(end_delimiter==false)
                    cout << "EOF NOT FOUND" << endl;
            }
        }
    i++;
    }

    return packet_ok;

}

bvec BlindOFDM_Framing::charvec2bvec(vector<char> input){
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

vector<char> BlindOFDM_Framing::bvec2charvec(bvec input){

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
