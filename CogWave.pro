#-------------------------------------------------
#
# Project created by QtCreator 2012-12-11T15:48:28
#
#-------------------------------------------------

QT       += core gui

TARGET = CogWave
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Multimedia/audio_rx.cpp \
    Multimedia/audio_tx.cpp \
    Multimedia/video_rx.cpp \
    Multimedia/video_tx.cpp \
    Multimedia/text_tx.cpp \
    Multimedia/text_rx.cpp \
    Multimedia/plot.cpp \
    BlindOFDM/blindofdm_uhddevice.cpp \
    BlindOFDM/blindofdm_tdd_mode_tx.cpp \
    BlindOFDM/blindofdm_tdd_mode_rx.cpp \
    BlindOFDM/blindofdm_sensing.cpp \
    BlindOFDM/blindofdm_multimediaread.cpp \
    BlindOFDM/blindofdm_framing.cpp \
    BlindOFDM/blindofdm_fec.cpp \
    BlindOFDM/blindofdm_modem.cpp \
    DADS/dads_ber_test.cpp \
    DADS/dads_framing.cpp \
    DADS/dads_fdd_mode_rx.cpp \
    DADS/dads_fdd_mode_tx.cpp \
    DADS/dads_multimediaread.cpp \
    DADS/dads_fec.cpp \
    DADS/dads_modem.cpp \
    DADS/dads_uhddevice.cpp


HEADERS  += mainwindow.h \
    Multimedia/audio_rx.h \
    Multimedia/audio_tx.h \
    Multimedia/video_rx.h \
    Multimedia/video_tx.h \
    Multimedia/text_tx.h \
    Multimedia/text_rx.h \
    Multimedia/plot.h \
    BlindOFDM/blindofdm_uhddevice.h \
    BlindOFDM/blindofdm_tdd_mode_tx.h \
    BlindOFDM/blindofdm_tdd_mode_rx.h \
    BlindOFDM/blindofdm_sensing.h \
    BlindOFDM/blindofdm_multimediaread.h \
    BlindOFDM/blindofdm_framing.h \
    BlindOFDM/blindofdm_fec.h \
    BlindOFDM/blindofdm_modem.h \
    DADS/dads_ber_test.h \
    DADS/dads_framing.h \
    DADS/dads_fdd_mode_rx.h \
    DADS/dads_fdd_mode_tx.h \
    DADS/dads_multimediaread.h \
    DADS/dads_fec.h \
    DADS/dads_modem.h \
    DADS/dads_uhddevice.h \
    waveform_rx.h \
    waveform_tx.h

FORMS    += mainwindow.ui

# Added by V. Le Nir for QWT Plot integration
INCLUDEPATH += /usr/include/qwt-qt4
LIBS += -lqwt-qt4
#INCLUDEPATH += /usr/include/qwt
#LIBS += -lqwt

# Added by V. Le Nir for IT++ integration
LIBS += `/usr/bin/itpp-config --static --libs`
LIBS += `/usr/local/bin/itpp-config --static --libs`

# Added by V. Le Nir for UHD integration
INCLUDEPATH += /usr/local/include/uhd
INCLUDEPATH += /usr/include/uhd
INCLUDEPATH += /opt/uhd/include/
#LIBS += -luhd
LIBS += -L/opt/uhd/lib64 -luhd

# Added by V. Le Nir for Gstreamer integration
INCLUDEPATH += /usr/include/gstreamer-0.10
INCLUDEPATH += /usr/include/glib-2.0
INCLUDEPATH += /usr/lib/x86_64-linux-gnu/glib-2.0/include/
INCLUDEPATH += /usr/lib/i386-linux-gnu/glib-2.0/include/
INCLUDEPATH += /usr/lib/glib-2.0/include/
INCLUDEPATH += /usr/lib64/glib-2.0/include/
INCLUDEPATH += /usr/include/libxml2
LIBS += -lgstreamer-0.10 -lgstinterfaces-0.10
LIBS += -lgobject-2.0 -lglib-2.0
















































