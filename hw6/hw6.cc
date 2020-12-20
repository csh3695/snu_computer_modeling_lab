#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/internet-module.h"

// Default Network Topology
//
//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -------------- n1   n2   n3   n4
//                   point-to-point  |    |    |    |
//                                   ================
//                                     LAN 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("CM_HW6");

int main (int argc, char *argv[])
{
  bool verbose = true;
  bool tracing = true;

  CommandLine cmd (__FILE__);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);
  cmd.Parse (argc,argv);

  if (verbose){
    LogComponentEnable ("PacketSink", LOG_LEVEL_INFO);
    LogComponentEnable ("BulkSendApplication", LOG_LEVEL_INFO);
  }

  NodeContainer p2pNodes;
  p2pNodes.Create (6);

  PointToPointHelper pointToPoint[6];
  NetDeviceContainer p2pDevices[6];
  
  for(int i=0; i<6; i++){
    pointToPoint[i].SetDeviceAttribute ("DataRate", StringValue (i == 5? "2Mbps":"1Mbps"));
    pointToPoint[i].SetChannelAttribute ("Delay", StringValue ((i < 2)? "50ms": (i < 5)? "100ms": "200ms"));
    NodeContainer tmp;
    switch (i)
    {
    case 0:
      tmp.Add(p2pNodes.Get(0));
      tmp.Add(p2pNodes.Get(3));
      break;
    case 1:
      tmp.Add(p2pNodes.Get(1));
      tmp.Add(p2pNodes.Get(3));
      break;
    case 2:
      tmp.Add(p2pNodes.Get(3));
      tmp.Add(p2pNodes.Get(4));
      break;
    case 3:
      tmp.Add(p2pNodes.Get(1));
      tmp.Add(p2pNodes.Get(4));
      break;
    case 4:
      tmp.Add(p2pNodes.Get(2));
      tmp.Add(p2pNodes.Get(4));
      break;
    case 5:
      tmp.Add(p2pNodes.Get(4));
      tmp.Add(p2pNodes.Get(5));
      break;
    
    default:
      break;
    }
    p2pDevices[i] = pointToPoint[i].Install(tmp);
  }

  InternetStackHelper stack;
  stack.Install (p2pNodes);

  Ipv4InterfaceContainer interfaces[6];

  for(int i=0; i<6; i++){
    Ipv4AddressHelper address;
    std::string prefix("10.1.");
    std::string suffix(".0");
    address.SetBase(prefix.append(std::to_string(i)).append(suffix).c_str(), "255.255.255.0");
    interfaces[i] = address.Assign (p2pDevices[i]);
  }


  uint16_t sinkPort[3] = {6666, 7777, 8888};
  Address sinkAddress[3];
  ApplicationContainer sinkApps[3];

  for(int i=0; i<3; i++){
    sinkAddress[i] = Address(InetSocketAddress((i == 2? interfaces[4].GetAddress(0): interfaces[5].GetAddress(1)), sinkPort[i]));
    PacketSinkHelper packetSinkHelper (i == 0? "ns3::TcpSocketFactory": "ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort[i]));
    sinkApps[i] = packetSinkHelper.Install (p2pNodes.Get(i == 2? 2: 5));
    sinkApps[i].Start (Seconds (10.));
    sinkApps[i].Stop (Seconds (20.));
  }


  OnOffHelper clientHelper ("ns3::UdpSocketFactory", Address ());
  ApplicationContainer clientApps;

  clientHelper.SetAttribute ("PacketSize", UintegerValue (1000));
  clientHelper.SetAttribute ("DataRate", StringValue ("0.7Mbps"));
  clientHelper.SetAttribute ("Remote", AddressValue(sinkAddress[1]));
  clientApps.Add (clientHelper.Install(p2pNodes.Get(1)));
  clientHelper.SetAttribute ("PacketSize", UintegerValue (200));
  clientHelper.SetAttribute ("DataRate", StringValue ("1.0Mbps"));
  clientHelper.SetAttribute ("Remote", AddressValue(sinkAddress[2]));
  clientApps.Add (clientHelper.Install(p2pNodes.Get(5)));


  BulkSendHelper bulkHelper("ns3::TcpSocketFactory", sinkAddress[0]);
  bulkHelper.SetAttribute("MaxBytes", StringValue("0"));
  clientApps.Add(bulkHelper.Install(p2pNodes.Get(0)));


  clientApps.Start (Seconds (10.0));
  clientApps.Stop (Seconds (20.0));

  // Create the animation object and configure for specified output

  std::string animFile = "hw6-animation.xml" ;  // Name of file for animation output
  AnimationInterface anim (animFile);

  
  anim.SetConstantPosition(p2pNodes.Get(0), 0.0, 0.0);
  anim.SetConstantPosition(p2pNodes.Get(1), 0.0, 40.0);
  anim.SetConstantPosition(p2pNodes.Get(2), 30.0, 60.0);
  anim.SetConstantPosition(p2pNodes.Get(3), 30.0, 20.0);
  anim.SetConstantPosition(p2pNodes.Get(4), 60.0, 20.0);
  anim.SetConstantPosition(p2pNodes.Get(5), 90.0, 40.0);

  anim.EnablePacketMetadata (true); // Optional
  anim.EnableIpv4L3ProtocolCounters (Seconds (0), Seconds (20)); // Optional

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  if (tracing == true) {
    for(int i=0; i<6; i++){
      std::string title = "hw6_";
      pointToPoint[i].EnablePcapAll(title.append(std::to_string(i)));
    }
  }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
