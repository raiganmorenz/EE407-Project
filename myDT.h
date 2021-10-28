#ifndef myDT_h
#define myDT_h

#include <map>
#include <vector>
#include "ns3/ipv4.h"
#include "ns3/nstime.h"
#include "ns3/output-stream-wrapper.h"

using namespace ns3;

typedef std::pair<double, double> Position;

class BeaconInfo{
public:
    //GETTERS
    uint16_t getHops() const {return hops;}
    Position getPos() const {return pos;}
    Time getTime() const {return updatedAt;}
    //SETTERS
    void setHops(uint16_t h) {hops = h;} 
    void setPos(Position p) {pos = p;}
    void setTime(Time t) {updatedAt = t;}

private:
    uint16_t hops;
    Position pos;
    Time updatedAt;
};

std::ostream& operator<< (std::ostream& os, BeaconInfo const&);
std::ostream& operator<< (std::ostream& os, Packet const&);

class DT{
public:
    DT(){}
    //GETTERS
    size_t getSize() const {return table.size();}
    uint16_t getHopsTo(Ipv4Address beacon) const;
    std::pair<double, double> getBeaconPos(Ipv4Address beacon) const;
    Time lastUpdated(Ipv4Address beacon) const;
    std::vector<Ipv4Address> allBeacons() const;

    void Print(Ptr<OutputStreamWrapper> os) const;

    void addBeacon(Ipv4Address beacon, uint16_t h, double xPos, double yPos);

private:
    std::map<Ipv4Address, BeaconInfo> table;
};

class myHeader: public Header{
public:
    //GETTERS
    double getxPos() {return xPos;}
    double getyPos() {return yPos;}
    uint16_t getHopCount() {return hopCount;}
    uint16_t getSeqNo() {return seqNo;}
    Ipv4Address getBeaconID() {return beaconID;}
    //SETTERS
    void setHopCount(uint16_t count) {hopCount = count;}
    void setxPos(double p) {xPos = p;}
    void setyPos(double p) {yPos = p;}
    void setSeqNo(uint16_t sn) {seqNo = sn;}
    void setBeaconID(Ipv4Address a) {beaconID = a;}
    //SERIALIZATION
    static TypeId GetTypeId (void);
    TypeId GetInstanceTypeId () const;
    virtual void Serialize (Buffer::Iterator start) const;
    virtual uint32_t Deserialize (Buffer::Iterator start);
    virtual uint32_t GetSerializedSize () const;

    virtual void Print(std::ostream& os) const{
        os << "Beacon: " << beaconID << " ,hopCount: " << hopCount << ", (" << xPos << ", "<< yPos<< ")\n"; 
    }

private:
    double xPos;
    double yPos;
    uint16_t seqNo;
    uint16_t hopCount;
    Ipv4Address beaconID;
};

#endif //myDT_h