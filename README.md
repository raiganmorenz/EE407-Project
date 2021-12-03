# EE407-Project

Based on Pixki's DV-hop NS3.20 code

After installing NS3.30, do the following.
$ cd $NS_HOME/src/
$ git clone https://github.com/pixki/dvhop.git
$ cd $NS_HOME
$ ./waf configure --enable-examples
$ ./waf --run dvhop-example

Place dvhop-example.cc, dvhop-critical.cc, and wscript into ns3/src/dvhop/examples

Place all other files in ns3/src/dvhop/model

Run from ns3 home directory using: ./waf --run (program)
