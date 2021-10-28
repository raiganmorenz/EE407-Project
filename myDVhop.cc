/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "myDVhop.h"
#include "myDT.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/random-variable-stream.h"
#include "ns3/inet-socket-address.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/wifi-net-device.h"
#include "ns3/adhoc-wifi-mac.h"
#include "ns3/string.h"
#include "ns3/pointer.h"

using namespace ns3;

Protocol::Protocol(){
    isBeacon = false;
    xPos = 15;
    yPos = 450;
    seqNo = 0;
  }

Ptr<Ipv4Route> Protocol::routput(Ptr<Packet> p, const Ipv4Header &header, Ptr<NetDevice> oif){
  int32_t ifIndex = ipv4->GetInterfaceForDevice(oif);
  if(!p || socketAddresses.empty() || (ifIndex < 0)){
    Ptr<Ipv4Route> route;
    return route;
  }

  Ipv4InterfaceAddress iface = ipv4->GetAddress(ifIndex, 0);
  Ipv4Address dest = header.GetDestination();
  Ptr<Ipv4Route> route = Create<Ipv4Route>();

  route->SetDestination(dest);
  route->SetGateway(iface.GetBroadcast());
  route->SetSource(iface.GetLocal());
  route->SetOutputDevice(oif);
  return route;
}

bool Protocol::rinput(Ptr<const Packet> p, const Ipv4Header &header, Ptr<const NetDevice> idev, UnicastForwardCallback ufcb, MulticastForwardCallback mfcb, LocalDeliverCallback ldcb, ErrorCallback errcb){
  if(socketAddresses.empty()){
    return false;
  }

  NS_ASSERT(ipv4 != 0);
  NS_ASSERT(p != 0);
  int32_t iif = ipv4->GetInterfaceForDevice(idev);
  NS_ASSERT(iif >- 0);

  Ipv4Address dest = header.GetDestination();
  // Ipv4Address source = header.GetSource();

  //Broadcast local delivery or forwarding
  for(std::map<Ptr<Socket>, Ipv4InterfaceAddress>::const_iterator j = socketAddresses.begin(); j != socketAddresses.end(); ++j){
    Ipv4InterfaceAddress iface = j->second;
    if(ipv4->GetInterfaceForAddress(iface.GetLocal()) == iif){
      if(dest == iface.GetBroadcast() || dest.IsBroadcast()){
        Ptr<Packet> packet = p->Copy();
        if(!ldcb.IsNull()){
          ldcb(packet, header, iif);
        }
        else{
          errcb(packet, header, Socket::ERROR_NOROUTETOHOST);
        }
        return true;
      }
    }
  }

  if(ipv4->IsDestinationAddress(dest, iif)){
    if(ldcb.IsNull() == false){
      ldcb(p, header, iif);
    }
    else{
      errcb(p, header, Socket::ERROR_NOROUTETOHOST);
    }
    return true;
  }
  return false;
}

void Protocol::SetIpv4(Ptr<Ipv4> localipv4){
  NS_ASSERT(localipv4 != 0);
  NS_ASSERT(ipv4 != 0);

  ipv4 = localipv4;
}

void Protocol::NotifyInterfaceUp(uint32_t interface){
  Ptr<Ipv4L3Protocol> l3 = ipv4->GetObject<Ipv4L3Protocol>();
  Ipv4InterfaceAddress iface = l3->GetAddress(interface, 0);
  if (iface.GetLocal () == Ipv4Address ("127.0.0.1")){
    return;
  }
  Ptr<Socket> socket = Socket::CreateSocket(GetObject<Node>(), UdpSocketFactory::GetTypeId());
  NS_ASSERT(socket != 0);
  socket->SetRecvCallback(MakeCallback(&Protocol::RecDVhop, this));
  socket->BindToNetDevice(l3->GetNetDevice(interface));
  socket->Bind(InetSocketAddress(Ipv4Address::GetAny(), DVHOP_PORT));
  socket->SetAllowBroadcast(true);
  socket->SetAttribute("IpTtl", UintegerValue(1));
  socketAddresses.insert(std::make_pair(socket, iface));
}

void Protocol::NotifyInterfaceDown(uint32_t interface){
  Ptr<Socket> socket = FindSocket(ipv4->GetAddress(interface, 0));
  NS_ASSERT(socket);
  socket->Close();
  socketAddresses.erase (socket);
  if (socketAddresses.empty()){
      return;
    }
}

void Protocol::NotifyAddAddress(uint32_t interface, Ipv4InterfaceAddress address){
  Ptr<Ipv4L3Protocol> l3 = ipv4->GetObject<Ipv4L3Protocol>();
  if(!l3->IsUp(interface)){
    return;
  }
  if(l3->GetNAddresses(interface) == 1){
    Ipv4InterfaceAddress iface = l3->GetAddress(interface, 0);
    Ptr<Socket> socket = FindSocket(iface);
    if(!socket){
      if(iface.GetLocal() == Ipv4Address("127.0.0.1")){
        return;
      }
      Ptr<Socket> socket = Socket::CreateSocket(GetObject<Node>(), UdpSocketFactory::GetTypeId());
      NS_ASSERT(socket != 0);
      socket->SetRecvCallback(MakeCallback(&Protocol::RecDVhop,this));
      socket->BindToNetDevice(l3->GetNetDevice(interface));
      socket->Bind(InetSocketAddress(Ipv4Address::GetAny(), DVHOP_PORT));
      socket->SetAllowBroadcast(true);
      socketAddresses.insert(std::make_pair (socket, iface));
    }
  }
}

void Protocol::NotifyRemoveAddress(uint32_t interface, Ipv4InterfaceAddress address){
  Ptr<Socket> socket = FindSocket(address);
  if(socket){
    socketAddresses.erase(socket);
    Ptr<Ipv4L3Protocol> l3 = ipv4->GetObject<Ipv4L3Protocol>();
    if(l3->GetNAddresses(interface)){
      Ipv4InterfaceAddress iface = l3->GetAddress(interface, 0);
      Ptr<Socket> socket = Socket::CreateSocket(GetObject<Node> (), UdpSocketFactory::GetTypeId());
      NS_ASSERT(socket != 0);
      socket->SetRecvCallback(MakeCallback(&Protocol::RecDVhop, this));
      socket->Bind(InetSocketAddress(Ipv4Address::GetAny(), DVHOP_PORT));
      socket->SetAllowBroadcast(true);
      socketAddresses.insert(std::make_pair(socket, iface));
    }
    if(socketAddresses.empty()){
      return;
    }
  }
}

void Protocol::PrintDistances(Ptr<OutputStreamWrapper> stream, Ptr<Node> node) const{
  *stream->GetStream() << "----------------- Node " << node->GetId() << "-----------------" << "\n";
  disTable.Print(stream);
}

void Protocol::SendTo(Ptr<Socket> socket, Ptr<Packet> packet, Ipv4Address dest){
  socket->SendTo(packet, 0, InetSocketAddress(dest, DVHOP_PORT));
}

void Protocol::RecDVhop(Ptr<Socket> socket){
  Address sourceAddress;
  Ptr<Packet> packet = socket->RecvFrom(sourceAddress);

  //InetSocketAddress inetSourceAddr = InetSocketAddress::ConvertFrom(sourceAddress);
  // Ipv4Address sender = inetSourceAddr.GetIpv4();
  // Ipv4Address receiver = socketAddresses[socket].GetLocal();

  myHeader myheader;
  packet->RemoveHeader(myheader);
  updateHops(myheader.getBeaconID(), myheader.getHopCount() + 1, myheader.getxPos(), myheader.getyPos());
}

Ptr<Socket> Protocol::FindSocket(Ipv4InterfaceAddress addr) const{
  for(std::map<Ptr<Socket>, Ipv4InterfaceAddress>::const_iterator j = socketAddresses.begin(); j != socketAddresses.end(); ++j){
    Ptr<Socket> socket = j->first;
    Ipv4InterfaceAddress iface = j->second;
    if(iface == addr){
      return socket;
    }
  }
  Ptr<Socket> socket;
  return socket;
}

void Protocol::updateHops(Ipv4Address beacon, uint16_t newHops, double x, double y){
  uint16_t oldHops = disTable.getHopsTo(beacon);
  if(ipv4->GetInterfaceForAddress(beacon) >= 0){
    return;
  }
  if(oldHops > newHops || oldHops == 0){
    disTable.addBeacon(beacon, newHops, x, y);
  }
}