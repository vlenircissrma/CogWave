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
    Modems/modem_ofdma.cpp \
    Modems/modem_dads.cpp \
    Modems/modem_bpsk.cpp \
    Modems/modem_gmsk.cpp \
    Modems/BER_Tests/ofdma_ber_test.cpp \
    Modems/BER_Tests/dads_ber_test.cpp \
    Modems/BER_Tests/bpsk_ber_test.cpp \
    Common/uhddevice.cpp \
    Common/fec.cpp \
    Common/packet.cpp \
    Common/sensing.cpp \
    waveform_tdd_rx.cpp \
    waveform_tdd_tx.cpp \
    waveform_fdd_rx.cpp \
    waveform_fdd_tx.cpp \
    Modems/BER_Tests/gmsk_ber_test.cpp \
    Modems/modem_qpsk.cpp \
    Modems/BER_Tests/qpsk_ber_test.cpp \
    Modems/modem_cpfsk.cpp \
    Modems/BER_Tests/cpfsk_ber_test.cpp \



HEADERS  += mainwindow.h \
    Multimedia/audio_rx.h \
    Multimedia/audio_tx.h \
    Multimedia/video_rx.h \
    Multimedia/video_tx.h \
    Multimedia/text_tx.h \
    Multimedia/text_rx.h \
    Multimedia/plot.h \
    Modems/modem_ofdma.h \
    Modems/modem_dads.h \
    Modems/modem_bpsk.h \
    Modems/modem_gmsk.h \
    Modems/BER_Tests/ofdma_ber_test.h \
    Modems/BER_Tests/dads_ber_test.h \
    Modems/BER_Tests/bpsk_ber_test.h \
    Common/uhddevice.h \
    Common/fec.h \
    Common/packet.h \
    Common/sensing.h \
    Common/waveform_rx.h \
    Common/waveform_tx.h \
    waveform_tdd_rx.h \
    waveform_tdd_tx.h \
    waveform_fdd_rx.h \
    waveform_fdd_tx.h \
    Modems/BER_Tests/gmsk_ber_test.h \
    Modems/modem_qpsk.h \
    Modems/BER_Tests/qpsk_ber_test.h \
    Modems/modem_cpfsk.h \
    Modems/BER_Tests/cpfsk_ber_test.h \


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

# Added by V. Le Nir for Gnuradio integration
CONFIG += link_pkgconfig
PKGCONFIG += gnuradio-digital
INCLUDEPATH += /usr/local/include/gnuradio
LIBS += -lboost_system






































































