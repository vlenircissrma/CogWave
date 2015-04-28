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
#ifndef DATA_LINK_LAYER_RX_H
#define DATA_LINK_LAYER_RX_H

#include <QThread>
#include "Channel/hackrfdevice.h"
#include "Channel/uhddevice.h"
#include "Channel/virtualdevice.h"
#include "Channel/simulatordevice.h"

class Data_Link_Layer_RX: public QThread
{
public:
#if HACKRF_ENABLED==1
    HackRFDevice *device;
#endif
#if UHD_ENABLED==1
    UHDDevice *device;
#endif
#if VIRTUAL_ENABLED==1
    VirtualDevice *device;
#endif
#if SIMULATOR_ENABLED==1
    SimulatorDevice *device;
#endif
};

#endif // DATA_LINK_LAYER_RX_H

