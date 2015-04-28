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
    Application_Layer/audio_rx.cpp \
    Application_Layer/audio_tx.cpp \
    Application_Layer/video_rx.cpp \
    Application_Layer/video_tx.cpp \
    Application_Layer/text_tx.cpp \
    Application_Layer/text_rx.cpp \
    Application_Layer/plot.cpp \
    Data_Link_Layer/packet.cpp \
    Data_Link_Layer/point_to_point_tdd_rx.cpp \
    Data_Link_Layer/point_to_point_tdd_tx.cpp \
    Data_Link_Layer/point_to_point_fdd_rx.cpp \
    Data_Link_Layer/point_to_point_fdd_tx.cpp \
    Data_Link_Layer/aloha_rx.cpp \
    Data_Link_Layer/aloha_tx.cpp \
    Data_Link_Layer/csma_rx.cpp \
    Data_Link_Layer/csma_tx.cpp \
    Data_Link_Layer/tdma_tdd_rx.cpp \
    Data_Link_Layer/tdma_tdd_tx.cpp \
    Data_Link_Layer/ofdma_tdd_rx.cpp \
    Data_Link_Layer/ofdma_tdd_tx.cpp \
    Data_Link_Layer/Performance_Tests/aloha_performance_test.cpp \
    Data_Link_Layer/Performance_Tests/csma_performance_test.cpp \
    Data_Link_Layer/Performance_Tests/ofdma_tdd_performance_test.cpp \
    Data_Link_Layer/Performance_Tests/point_to_point_fdd_performance_test.cpp \
    Data_Link_Layer/Performance_Tests/point_to_point_tdd_performance_test.cpp \
    Data_Link_Layer/Performance_Tests/tdma_tdd_performance_test.cpp \
    Physical_Layer/modem_dads.cpp \
    Physical_Layer/modem_bpsk.cpp \
    Physical_Layer/modem_gmsk.cpp \
    Physical_Layer/fec.cpp \
    Physical_Layer/sensing.cpp \
    Physical_Layer/modem_qpsk.cpp \
    Physical_Layer/modem_cpfsk.cpp \
    Physical_Layer/modem_mcdaaofdm.cpp \
    Physical_Layer/BER_Tests/dads_ber_test.cpp \
    Physical_Layer/BER_Tests/bpsk_ber_test.cpp \
    Physical_Layer/BER_Tests/gmsk_ber_test.cpp \
    Physical_Layer/BER_Tests/qpsk_ber_test.cpp \
    Physical_Layer/BER_Tests/cpfsk_ber_test.cpp \
    Physical_Layer/BER_Tests/mcdaaofdm_ber_test.cpp \
    Channel/uhddevice.cpp \
    Channel/virtualdevice.cpp \
    Channel/channel_models.cpp \
    Channel/simulatordevice.cpp \
    Physical_Layer/modem_dsss.cpp \
    Physical_Layer/BER_Tests/dsss_ber_test.cpp \
    Physical_Layer/modem_mcdads.cpp \
    Physical_Layer/BER_Tests/mcdads_ber_test.cpp \
    Physical_Layer/modem_mccdm.cpp \
    Physical_Layer/BER_Tests/mccdm_ber_test.cpp \
    Physical_Layer/modem_ofdm.cpp \
    Physical_Layer/BER_Tests/ofdm_ber_test.cpp \
    Physical_Layer/sniffer_complex.cpp \
    Physical_Layer/injector_complex.cpp \
    Physical_Layer/sniffer_char.cpp \
    Physical_Layer/injector_char.cpp \
    Channel/hackrfdevice.cpp





HEADERS  += mainwindow.h \
    Application_Layer/audio_rx.h \
    Application_Layer/audio_tx.h \
    Application_Layer/video_rx.h \
    Application_Layer/video_tx.h \
    Application_Layer/text_tx.h \
    Application_Layer/text_rx.h \
    Application_Layer/plot.h \
    Data_Link_Layer/packet.h \
    Data_Link_Layer/data_link_layer_rx.h \
    Data_Link_Layer/data_link_layer_tx.h \
    Data_Link_Layer/point_to_point_tdd_rx.h \
    Data_Link_Layer/point_to_point_tdd_tx.h \
    Data_Link_Layer/point_to_point_fdd_rx.h \
    Data_Link_Layer/point_to_point_fdd_tx.h \
    Data_Link_Layer/aloha_rx.h \
    Data_Link_Layer/aloha_tx.h \
    Data_Link_Layer/csma_rx.h \
    Data_Link_Layer/csma_tx.h \
    Data_Link_Layer/tdma_tdd_rx.h \
    Data_Link_Layer/tdma_tdd_tx.h \
    Data_Link_Layer/ofdma_tdd_rx.h \
    Data_Link_Layer/ofdma_tdd_tx.h \
    Data_Link_Layer/Performance_Tests/aloha_performance_test.h \
    Data_Link_Layer/Performance_Tests/csma_performance_test.h \
    Data_Link_Layer/Performance_Tests/ofdma_tdd_performance_test.h \
    Data_Link_Layer/Performance_Tests/point_to_point_fdd_performance_test.h \
    Data_Link_Layer/Performance_Tests/point_to_point_tdd_performance_test.h \
    Data_Link_Layer/Performance_Tests/tdma_tdd_performance_test.h \
    Physical_Layer/modem_dads.h \
    Physical_Layer/modem_bpsk.h \
    Physical_Layer/modem_gmsk.h \
    Physical_Layer/fec.h \
    Physical_Layer/sensing.h \
    Physical_Layer/modem_qpsk.h \
    Physical_Layer/modem_cpfsk.h \
    Physical_Layer/modem_mcdaaofdm.h \
    Physical_Layer/BER_Tests/dads_ber_test.h \
    Physical_Layer/BER_Tests/bpsk_ber_test.h \
    Physical_Layer/BER_Tests/gmsk_ber_test.h \
    Physical_Layer/BER_Tests/qpsk_ber_test.h \
    Physical_Layer/BER_Tests/cpfsk_ber_test.h \
    Physical_Layer/BER_Tests/mcdaaofdm_ber_test.h \
    Channel/uhddevice.h \
    Channel/virtualdevice.h \
    Channel/channel_models.h \
    Channel/simulatordevice.h \
    Physical_Layer/modem_dsss.h \
    Physical_Layer/BER_Tests/dsss_ber_test.h \
    Physical_Layer/modem_mcdads.h \
    Physical_Layer/BER_Tests/mcdads_ber_test.h \
    Physical_Layer/modem_mccdm.h \
    Physical_Layer/BER_Tests/mccdm_ber_test.h \
    Physical_Layer/modem_ofdm.h \
    Physical_Layer/BER_Tests/ofdm_ber_test.h \
    Physical_Layer/sniffer_complex.h \
    Physical_Layer/injector_complex.h \
    Physical_Layer/sniffer_char.h \
    Physical_Layer/injector_char.h \
    Channel/hackrfdevice.h


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
PREFIX=/home/vlenir/Documents/5_Programmes/GnuRadio/gnuradio
CONFIG += link_pkgconfig
PKGCONFIG += gnuradio-digital gnuradio-filter gnuradio-blocks gnuradio-fft gnuradio-runtime gnuradio-analog
#INCLUDEPATH += /usr/local/include/gnuradio
INCLUDEPATH += $$PREFIX/gr-digital/lib
INCLUDEPATH += $$PREFIX/gr-filter/lib
INCLUDEPATH += $$PREFIX/gr-fft/lib
LIBS += -lboost_system -lfftw3f
LIBS += -lhackrf

LIBS += $$PREFIX/build/gr-digital/lib/CMakeFiles/gnuradio-digital.dir/mpsk_receiver_cc_impl.cc.o
LIBS += $$PREFIX/build/gr-digital/lib/CMakeFiles/gnuradio-digital.dir/fll_band_edge_cc_impl.cc.o
LIBS += $$PREFIX/build/gr-digital/lib/CMakeFiles/gnuradio-digital.dir/constellation_receiver_cb_impl.cc.o
LIBS += $$PREFIX/build/gr-digital/lib/CMakeFiles/gnuradio-digital.dir/pfb_clock_sync_ccf_impl.cc.o
LIBS += $$PREFIX/build/gr-digital/lib/CMakeFiles/gnuradio-digital.dir/pfb_clock_sync_fff_impl.cc.o
LIBS += $$PREFIX/build/gr-digital/lib/CMakeFiles/gnuradio-digital.dir/clock_recovery_mm_ff_impl.cc.o
LIBS += $$PREFIX/build/gr-digital/lib/CMakeFiles/gnuradio-digital.dir/ofdm_carrier_allocator_cvc_impl.cc.o
LIBS += $$PREFIX/build/gr-digital/lib/CMakeFiles/gnuradio-digital.dir/ofdm_serializer_vcc_impl.cc.o
LIBS += $$PREFIX/build/gr-digital/lib/CMakeFiles/gnuradio-digital.dir/ofdm_frame_equalizer_vcvc_impl.cc.o
LIBS += $$PREFIX/build/gr-digital/lib/CMakeFiles/gnuradio-digital.dir/ofdm_sync_sc_cfb_impl.cc.o
LIBS += $$PREFIX/build/gr-digital/lib/CMakeFiles/gnuradio-digital.dir/ofdm_cyclic_prefixer_impl.cc.o







































































