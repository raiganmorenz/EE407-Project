# EE407-Project
Release V1.0
--------------------------------------------
How to use:
Based on Pixki's DV-hop NS3.20 code

After installing NS3.30, do the following.
$ cd $NS_HOME/src/
$ git clone https://github.com/pixki/dvhop.git
$ cd $NS_HOME
$ ./waf configure --enable-examples
$ ./waf --run dvhop-example

Place dvhop-example.cc, dvhop-critical.cc, and wscript into ns3/src/dvhop/examples

Place all other files in ns3/src/dvhop/model
(Exception: dvhop.c++ is example code used for trilateration)

Run from ns3 home directory using: ./waf --run (program)

In current release:
Nodes are placed in a grid, user controls percentage of beacon nodes.
Position is calculated per node and show in the terminal output.
Trilateration is implemented, but not working as intended.
Critical condition will be added in a future update.
