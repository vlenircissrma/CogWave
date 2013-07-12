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

#ifndef DADS_MULTIMEDIAREAD_H
#define DADS_MULTIMEDIAREAD_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <QString>
#include <QThread>

class DADS_Multimediaread: public QThread
{
Q_OBJECT
public:
     DADS_Multimediaread(char* filename,bool is_readsome2);
     void file_close();
     volatile bool stopped;
     int buff_size;
     vector<char> buff;
     int nb_read;
     char* file_name;
     bool is_readsome;

protected:
    void run();

private:
     ifstream infile;





};


#endif // DADS_MULTIMEDIAREAD_H
