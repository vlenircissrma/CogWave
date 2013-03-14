CogWave
=======

Open-source software platform for cognitive radio waveforms


CogWave is a free and open-source software platform aiming at developing cognitive radio waveforms. The CogWave application allows the exchange of video, audio and text between two USRPs through a graphical user interface (GUI) developed in Qt4/Gstreamer and cognitive radio waveforms developed in IT++. The USRP hardware driver (UHD) C++ application programming interface (API) allows to receive and transmit IQ samples. Combining CogWave with USRP gives a rapid prototyping platform for physical layer design and algorithm validation through a real-time video, audio and text transmission.

For more information, visit our website http://www.sic.rma.ac.be/~vlenir/CogWave

To download the code, go to GitHub https://github.com/vlenircissrma/CogWave

To see CogWave in action, follow these links:

http://www.youtube.com/watch?v=ZAv1RK8x3jU

http://www.youtube.com/watch?v=s2TQcjxNOmM



To get help and to report bugs, post your message to the google group https://groups.google.com/forum/#!forum/cogwave or send an email to the diffusion list cogwave@googlegroups.com


![Setup Screenshot](https://raw.github.com/vlenircissrma/CogWave/master/Screenshots/setup.jpg)
![Features Screenshot](https://raw.github.com/vlenircissrma/CogWave/master/Screenshots/features.jpg)
![Applications Screenshot](https://raw.github.com/vlenircissrma/CogWave/master/Screenshots/applications.jpg)

INSTALLATION (Ubuntu 12.04 LTS)
===============================

1) IT++

sudo apt-get install libitpp-dev libblas-dev liblapack-dev libfftw3-dev

2) Qtcreator

sudo apt-get install qtcreator

3) Qwt

sudo apt-get install libqwt5-qt4-dev

4) Gstreamer

sudo apt-get install libgstreamer-plugins-base0.10-dev libgstreamer0.10-dev gstreamer0.10-plugins-ugly

5) UHD

http://code.ettus.com/redmine/ettus/projects/uhd/wiki/UHD_Linux

6) Open CogWave.pro in QtCreator and launch the Application !



