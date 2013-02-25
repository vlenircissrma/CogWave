CogWave
=======

Open-source software platform for cognitive radio waveforms


CogWave is a free and open-source software platform aiming at developing cognitive radio waveforms. The CogWave application allows the exchange of video, audio and text between two USRPs through a graphical user interface (GUI) developed in Qt4/Gstreamer and cognitive radio waveforms developed in IT++. The USRP hardware driver (UHD) C++ application programming interface (API) allows to receive and transmit IQ samples. Combining CogWave with USRP gives a rapid prototyping platform for physical layer design and algorithm validation through a real-time video, audio and text transmission.

For more information, please visit our website http://www.sic.rma.ac.be/~vlenir/CogWave

To see CogWave in action, follow this link http://www.youtube.com/watch?v=ZAv1RK8x3jU

INSTALLATION (Ubuntu 12.04 LTS)
===============================

1) IT++

sudo apt-get install libitpp-dev libblas-dev liblapack-dev libfftw3-dev

2) Qtcreator

sudo apt-get install qtcreator

3) Qwt

sudo apt-get install libqwt5-qt4-dev

4) Gstreamer

sudo apt-get install libgstreamer-plugins-base0.10-dev libgstreamer0.10-dev

5) UHD

http://code.ettus.com/redmine/ettus/projects/uhd/wiki/UHD_Linux

6) Open CogWave.pro in QtCreator and launch the Application !



