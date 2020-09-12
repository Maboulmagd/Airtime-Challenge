# Airtime Challenge
Airtime Media Software Engineer (C++) Online Assessment (OA)

Instructions:
http://challenge.airtime.com:2324/instructions

Notes:
The code was implemented and tested in Microsoft Visual Studio Enterprise 2019 on my Windows 10 platform.
To run the source code, open the Airtime directory, and open the Airtime.sln file (assuming you have MSVC installed on your computer). 
After that, feel free to run the solution in either (debug/release) mode in x86, NOT in x64 (as that is what I tested it with).

If you are unable to run the solution for any reason, you should still be able to look at the source code by examining the few independent files (.h/.cpp) within the Airtime directory. Also note that you will need to provide a valid directory/file path name (look at line 129 in the Airtime.cpp file), to have it execute successfully.

It might also be important to note that I tested the sound in Audacity by importing the raw data file with these settings:

Encoding: Signed 8-bit PCM
Byte order: Default endianness.
Channels: 1 Channel (Mono)
Start offset: 0 bytes
Amount to import: 100 %
Sample rate: 16000 Hz

It seems as though there is some unintelligible sound at the beginning and at the end, however in the middle there seems to be a verification code of some sort that is much clearer to the ears. 
