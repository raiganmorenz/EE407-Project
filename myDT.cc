#include "myDT.h"
#include "ns3/simulator.h"
#include "ns3/address-utils.h"
#include <algorithm>
#include <utility>

using namespace std;

std::ostream& operator<< (std::ostream &os, BeaconInfo const& h){
      std::pair<float,float> pos = h.getPos();
      os << h.getHops() << "\t(" << pos.first << ","<< pos.second << ")\t"<< h.getTime() <<"\n";
      return os;
    }

//GETTERS
uint16_t DT::getHopsTo(Ipv4Address beacon) const{
    std::map<Ipv4Address, BeaconInfo>::const_iterator it = table.find(beacon);
      if( it != table.end ()){
          return ((BeaconInfo)it->second).getHops();
        }
      else {return 0;}
}

std::pair<double, double> DT::getBeaconPos(Ipv4Address beacon) const
    {
      std::map<Ipv4Address, BeaconInfo>::const_iterator it = table.find(beacon);
      if( it != table.end ()){
          return ((BeaconInfo)it->second).getPos();
        }
      else {return std::make_pair<double,double>(-1.0,-1.0);}
    }

void DT::addBeacon(Ipv4Address beacon, uint16_t h, double xPos, double yPos)
    {
      std::map<Ipv4Address, BeaconInfo>::iterator it = table.find(beacon);
      BeaconInfo info;
      if( it != table.end ()){
          info.setPos(it->second.getPos());
          info.setHops(h);
          info.setTime(Simulator::Now());
          it->second = info;
        }
      else
        {
            Position temp;
            temp.first = xPos;
            temp.second = yPos;
            info.setHops(h);
            info.setPos(temp);
            info.setTime(Simulator::Now());
            std::pair<Ipv4Address, BeaconInfo> temp2;
            temp2.first = beacon;
            temp2.second = info;
            table.insert(temp2);
        }
    }

Time DT::lastUpdated(Ipv4Address beacon) const
    {
      std::map<Ipv4Address, BeaconInfo>::const_iterator it = table.find(beacon);
      if( it != table.end ())
        {
          return ((BeaconInfo)it->second).getTime();
        }

      else return Time::Max();
    }

std::vector<Ipv4Address> DT::allBeacons() const
    {
      std::vector<Ipv4Address> theBeacons;
      for(std::map<Ipv4Address, BeaconInfo>::const_iterator j = table.begin(); j != table.end (); ++j)
        {
          theBeacons.push_back (j->first);
        }
      return theBeacons;
    }

void DT::Print(Ptr<OutputStreamWrapper> os) const
    {
      *os->GetStream () << table.size() << " entries\n";
      for(std::map<Ipv4Address,BeaconInfo>::const_iterator j = table.begin(); j != table.end(); ++j)
        {
          //                    BeaconAddr           BeaconInfo
          *os->GetStream () <<  j->first << "\t" << j->second;
        }
    }

//SERIALIZATION
TypeId myHeader::GetTypeId(){
    static TypeId tid = TypeId("ns3::myHeader").SetParent<Header>().AddConstructor<myHeader>();
    return tid;
}

TypeId myHeader::GetInstanceTypeId() const{
    return GetTypeId();
}

uint32_t myHeader::GetSerializedSize() const{
    return 24;
}

void myHeader::Serialize(Buffer::Iterator start) const{
      double x = xPos;
      uint64_t dst;
      char *const p = reinterpret_cast<char*>(&x);
      std::copy(p, p+sizeof(uint64_t), reinterpret_cast<char*>(&dst));
      start.WriteHtonU64 (dst);

      double y = yPos;
      char* const p2 = reinterpret_cast<char*>(&y);
      std::copy(p2, p2+sizeof(uint64_t), reinterpret_cast<char*>(&dst));
      start.WriteHtonU64 (dst);

      start.WriteU16 (seqNo);
      start.WriteU16 (hopCount);
      WriteTo(start, beaconID);
}

uint32_t myHeader::Deserialize(Buffer::Iterator start){
      Buffer::Iterator i = start;

      uint64_t midX = i.ReadNtohU64 ();
      char *const p = reinterpret_cast<char*>(&midX);
      std::copy(p, p + sizeof(double), reinterpret_cast<char*>(&xPos));

      uint64_t midY = i.ReadNtohU64 ();
      char* const p2 = reinterpret_cast<char*>(&midY);
      std::copy(p2, p2 + sizeof(double), reinterpret_cast<char*>(&yPos));


      std::cout << "Deserializing coordinates (" << xPos <<","<< yPos << ")" << std::endl;

      seqNo = i.ReadU16 ();
      hopCount = i.ReadU16 ();
      ReadFrom (i, beaconID);

      //Validate the readed bytes match the serialized size
      uint32_t dist = i.GetDistanceFrom (start);
      NS_ASSERT (dist == GetSerializedSize () );
      return dist;
}