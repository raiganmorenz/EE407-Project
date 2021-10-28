#ifndef myDVhop_h
#define myDVhop_h

#include "ns3/node.h"
#include "ns3/random-variable-stream.h"
#include "ns3/output-stream-wrapper.h"
#include "ns3/ipv4-routing-protocol.h"
#include "ns3/ipv4-interface.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/nstime.h"
#include "ns3/timer.h"
#include "ns3/packet.h"
#include "ns3/ipv4-header.h"
#include "myDT.h"
#include <map>

using namespace ns3;

class Protocol : public Ipv4RoutingProtocol{
public:
    Protocol();

    Ptr<Ipv4Route> routput(Ptr<Packet> p, const Ipv4Header& header, Ptr<NetDevice> oif);
    bool rinput (Ptr<const Packet> p, const Ipv4Header &header, Ptr<const NetDevice> idev, UnicastForwardCallback ufcb, MulticastForwardCallback mfcb, LocalDeliverCallback ldcb, ErrorCallback errcb);
    void SetIpv4(Ptr<Ipv4> ipv4);
    void NotifyInterfaceUp (uint32_t interface);
    void NotifyInterfaceDown (uint32_t interface);
    void NotifyAddAddress (uint32_t interface, Ipv4InterfaceAddress address);
    void NotifyRemoveAddress (uint32_t interface, Ipv4InterfaceAddress address);

    //GETTERS
    double getxPos() {return xPos;}
    double getyPos() {return yPos;}
    bool IsBeacon() {return isBeacon;}
    //SETTERS
    void setBeacon(bool IB) {isBeacon = IB;}
    void setPos(double x, double y) {xPos = x, yPos = y;}

    void PrintDistances(Ptr<OutputStreamWrapper> stream, Ptr<Node> node) const;
    const uint32_t DVHOP_PORT = 1234;
    Ptr<Ipv4> ipv4;

private:
    //Protocol Operation
    void SendTo(Ptr<Socket> socket, Ptr<Packet> packet, Ipv4Address dest);
    void RecDVhop(Ptr<Socket> socket);
    Ptr<Socket> FindSocket(Ipv4InterfaceAddress iface) const;
//    bool Forwarding(Ptr<const Packet> p, const Ipv4Header& header, UnicastForwardCallback ufcb, ErrorCallback errcb);
    void SendUnicastTo(Ptr<Socket> socket, Ptr<Packet> packet, Ipv4Address dest);

    DT disTable;
    void updateHops(Ipv4Address beacon, uint16_t hops, double x, double y);

    bool isBeacon;
    double xPos;
    double yPos;
    std::map<Ptr<Socket>, Ipv4InterfaceAddress> socketAddresses;
    uint32_t seqNo;
};

#endif //myDVhop_h