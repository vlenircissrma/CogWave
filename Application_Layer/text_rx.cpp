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

#include "text_rx.h"

Text_RX::Text_RX(){



file2.setFileName("text_outputpipe");
in.setDevice(&file2);

stopped=false;

last_text="";

}


void Text_RX::close(){

    stopped=true;

}
void Text_RX::run(){

        if(!file2.isOpen()){

            file2.open(QIODevice::ReadOnly|QIODevice::Text);
            cout << "text_outputpipe has been opened for reading" << endl;
        }
        else{
            cout << "text_outputpipe is already opened for reading" << endl;
        }
        QString line = in.readLine();
        while(!stopped){
            int first_index=line.indexOf("!!T");
            int last_index=line.indexOf("!!T",first_index+3);
            if((first_index<last_index)&&(last_text!=line.mid(first_index+3,last_index-(first_index+3)))){
                    emit edited_text(line.mid(first_index+3,last_index-(first_index+3)));
                    last_text=line.mid(first_index+3,last_index-(first_index+3));
            }
            line = in.readLine();
        }
        stopped=false;
        file2.close();

}
