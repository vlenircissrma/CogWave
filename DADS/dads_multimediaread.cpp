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

#include "dads_multimediaread.h"
#include "fstream"



union conversion {
     int i;
     double d;
     float f;
     long unsigned u;
     char c;
};



DADS_Multimediaread::DADS_Multimediaread(char* filename,bool is_readsome2){

    file_name=filename;
    is_readsome=is_readsome2;
    int status;
    status=mkfifo(filename,0666);

    if(status==0){
        cout << "The file " << file_name << " has been created" << endl;
    }
    if (status==-1){
           cout << "The file" << file_name << " already exists"<< endl;
    }

    stopped=false;
    nb_read=0;
}


void DADS_Multimediaread::run(){


    if(!infile.is_open()){
         infile.open(file_name, ifstream::binary);
        cout << file_name << " has been opened for reading"<< endl;
    }
    else{
        cout << file_name << " is already opened for reading"<< endl;
    }
    buff.resize(buff_size);
    vector<char> buff2(buff_size);

    if(is_readsome==false){
        nb_read=buff_size;
        while(!stopped){
            infile.read(&buff2.front(), buff2.size());
            buff=buff2;
            //cout << "##################  INPUT STREAM HAS BEEN READ #####################" << endl;
        }
    }
    else{
        int nb_read2=0;
        while(!stopped){
            while(nb_read2==0){
                nb_read2=infile.readsome(&buff2.front(), buff2.size());
            }
            nb_read=nb_read2;
            buff=buff2;
            nb_read2=0;
            //cout << "##################  INPUT STREAM HAS BEEN READ #####################" << endl;
        }


    }



}


void DADS_Multimediaread::file_close(){

    stopped=true;

}

