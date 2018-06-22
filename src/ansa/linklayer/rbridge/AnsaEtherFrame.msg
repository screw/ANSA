import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import inet.linklayer.common.EtherType;
import inet.linklayer.common.EthernetFcsMode;
import inet.linklayer.common.Ieee802Ctrl;
import inet.linklayer.common.MacAddress;
import inet.linklayer.ethernet.EtherFrame;

cplusplus {{
#include "inet/linklayer/ethernet/Ethernet.h"
//#include "inet/linklayer/common/MACAddress.h"
//#include "inet/linklayer/common/Ieee802Ctrl_m.h" // for enums
#include "inet/linklayer/ethernet/EtherFrame_m.h"
}}

namespace inet;

//class noncobject MacAddress;

//class noncobject MessageId;

enum EtherType;

enum SapCode;

//message EthernetIIFrame;


// ANSA ehter frame with VLAN
//TODO Find what class is used for Ethernet frame
packet AnsaEtherFrame extends cPacket //EthernetIIFrame
{
       MacAddress dest;
    MacAddress src;
     byteLength = ETHER_MAC_FRAME_BYTES;
    int etherType @enum(EtherType);
    int vlan;
    
    string displayString = "b=,,,kind,kind,0";
}

