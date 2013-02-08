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

#include "text_tx.h"

Text_TX::Text_TX(){

file.setFileName("text_inputpipe");
out.setDevice(&file);


}

void Text_TX::init_text(QString text){

myText=text;

}

void Text_TX::run(){


        if(!file.isOpen()){
            file.open(QIODevice::WriteOnly|QIODevice::Text);
            cout << "text_inputpipe has been opened for writing" << endl;
        }
        else{
            cout << "text_inputpipe is already opened for writing" << endl;
        }

        out << "C" << "!!T" << myText << "!!T";
        out.flush();


}
