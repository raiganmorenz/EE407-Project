#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-routing-protocol.h"
#include "ns3/wifi-module.h"
#include "ns3/wifi-helper.h"
#include "ns3/wifi-mac-helper.h"
#include "ns3/netanim-module.h"
#include "myDVhop.h"
#include <iostream>
#include <cmath>

using namespace ns3;

class DVrunner{
public:
    DVrunner();

    void Run();

private:
    uint32_t size; //node count
    double step; //step size
    double totalTime; //in seconds
    bool pcap; //PCAP tracing if true   

    NodeContainer nodes;
    NetDeviceContainer devices;
    Ipv4InterfaceContainer interfaces;

    void CreateNodes();
    void CreateDevices();
    void InstallInternetStack();
    void InstallApplications();
    void CreateBeacons();
};

int main(int argc, char **argv){
    DVrunner test;
    test.Run();
    return 0;
}

//------------------------------------------
DVrunner::DVrunner() :
    size(10),
    step(100),
    totalTime(10),
    pcap(true)
{}

void DVrunner::Run(){
    CreateNodes();
    CreateDevices();
    InstallInternetStack();
    CreateBeacons();

    Simulator::Stop(Seconds(totalTime));
    Simulator::Run();
    Simulator::Destroy();
}

void DVrunner::CreateNodes(){
    nodes.Create(size);

    MobilityHelper mobility;
     mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (step),
                                 "DeltaY", DoubleValue (0),
                                 "GridWidth", UintegerValue (size),
                                 "LayoutType", StringValue ("RowFirst"));
     mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
     mobility.Install (nodes);  
}

void DVrunner::CreateBeacons(){
    Ptr<Ipv4RoutingProtocol> proto = nodes.Get(0)->GetObject<Ipv4>()->GetRoutingProtocol();
    Ptr<Protocol> dvhop = DynamicCast<Protocol>(proto);
    dvhop->setBeacon(true);
    dvhop->setPos(125, 4500);

    proto = nodes.Get(4)->GetObject<Ipv4>()->GetRoutingProtocol();
    dvhop = DynamicCast<Protocol>(proto);
    dvhop->setBeacon(true);
    dvhop->setPos(6000, 500);

    proto = nodes.Get(9)->GetObject<Ipv4>()->GetRoutingProtocol();
    dvhop = DynamicCast<Protocol>(proto);
    dvhop->setBeacon(true);
    dvhop->setPos(125, 9500);
}

void DVrunner::CreateDevices(){
    WifiMacHelper wifiMac = WifiMacHelper();
    YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
    YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
    wifiPhy.SetChannel (wifiChannel.Create());
    WifiHelper wifi = WifiHelper();
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue ("OfdmRate6Mbps"), "RtsCtsThreshold", UintegerValue (0));
    devices = wifi.Install (wifiPhy, wifiMac, nodes);

  if (pcap)
    {
      wifiPhy.EnablePcapAll (std::string ("aodv"));
    }
}

void DVrunner::InstallInternetStack(){
    InternetStackHelper stack;
    stack.Install(nodes);
    Ipv4AddressHelper address;
    address.SetBase("10.0.0.0", "255.0.0.0");
    interfaces = address.Assign(devices);
    Ptr<OutputStreamWrapper> distStream = Create<OutputStreamWrapper>("dvhop.distances", std::ios::out);

}