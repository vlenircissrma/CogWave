CogWave
=======

CogWave: Open-source Software Framework for Cognitive Radio Waveform Design


CogWave is an open-source software framework aiming at developing cognitive radio (CR) waveforms.
For the physical layer, the CogWave framework provides many modulation schemes, such as the multichannel DAA-OFDM, the DADS modulation scheme with a short spreading sequence, and other modulation schemes ported from the GNU Radio framework (OFDM, BPSK, QPSK, GMSK,
CPFSK etc.). The CogWave framework is able to reconfigure the modulation scheme during run-time (e.g. switching from DADS to multichannel DAA-OFDM in the presence of a jammer) and allows precise timing control for burst transmissions (FDD and TDD). The CogWave framework can be interfaced with SDR hardware (USRP, HackRF) for testbed evaluation.
For the data link layer, the CogWave provides several MAC protocols (Point-to-Point-TDD, Point-to-Point-FDD, Aloha, non-persistent CSMA, 1-persistent CSMA, p-persistent CSMA, TDMA, OFDMA). The CogWave framework can be interfaced with external applications by named pipes (FIFO) or virtual network interfaces at data link layer (TAP) or network layer (TUN).
For instance, the CogWave framework can be combined with networks simulators (e.g. OMNeT++, ns-3) and network emulators (e.g. CORE) to obtain a CRN software framework for high fidelity simulation of CRN. Realistic channel models (AWGN channel, log-distance path loss, fading channel, frequency-
selective fading) have been implemented for simulating a high number of nodes.


For more information, visit our website http://www.sic.rma.ac.be/~vlenir/CogWave

To download the code, go to GitHub https://github.com/vlenircissrma/CogWave

To see CogWave in action, follow this link http://www.youtube.com/watch?v=mETzIbV3Edg


To get help and to report bugs, post your message to the google group https://groups.google.com/forum/#!forum/cogwave or send an email to the diffusion list cogwave@googlegroups.com


![Setup Screenshot](https://raw.github.com/vlenircissrma/CogWave/master/Screenshots/setup.jpg)
![Features Screenshot](https://raw.github.com/vlenircissrma/CogWave/master/Screenshots/features.jpg)
![Applications Screenshot](https://raw.github.com/vlenircissrma/CogWave/master/Screenshots/applications.jpg)
![High-Fidelity Simulation Screenshot](https://raw.github.com/vlenircissrma/CogWave/master/Screenshots/combination.jpg)

INSTALLATION (Ubuntu 14.04 LTS)
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

https://github.com/EttusResearch/uhd

6) Gnu Radio > 3.7

https://github.com/gnuradio/gnuradio

7) Open CogWave.pro (modify directories according to your installation path) in QtCreator and launch the Application !



