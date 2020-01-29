# Reverb effect based on JUCE framework

An audio application adding a reverberation effect to the WAV file under 10 seconds, allowing user to visualize the audio file,
choose the reverb parameters and bit depth of the resulting signal.

The reverb algorithm used is 12-line feedback delay network, utilizing a Householder matrix as feedback matrix. The application 
works with uncompressed audio files, 44.1 kHz sampling rate, mono or stereo.

The program was built using JUCE framework. 

The project was completed as an engineering thesis at AGH University of Science and Technology in Cracow, Poland.

![app-interface](https://i.imgur.com/9UuLDeH.jpg)
