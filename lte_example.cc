/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/config-store-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("LenaX2HandoverMeasures");
uint16_t offsetMeter = 0;
bool reverseDirection = false;
NodeContainer ueNodes;
uint16_t numberOfEnbs = 2;
double distance = 500.0; // m
double speed = 20;       // m/s
double yForUe = 500.0;   // m

void
NotifyConnectionEstablishedUe (std::string context,
                               uint64_t imsi,
                               uint16_t cellid,
                               uint16_t rnti)
{
  std::ostringstream sstream;
  sstream.str("");
  sstream << "lenaresults/handover_" << offsetMeter << "m.log";
  char const* hoFile = sstream.str().c_str();
  std::ofstream handoverFile(hoFile, std::ios_base::app | std::ios_base::out);
  double ts = Simulator::Now().GetSeconds();
  handoverFile << ts 
      << " "
      << "ConnectionEstablished " 
      << "UE " 
      << imsi  
      << " " 
      << cellid 
      << " " 
      << rnti 
      << "\n";
  handoverFile.close();
}

void
NotifyHandoverStartUe (std::string context,
                       uint64_t imsi,
                       uint16_t cellid,
                       uint16_t rnti,
                       uint16_t targetCellId)
{
  std::ostringstream sstream;
  sstream.str("");
  sstream << "lenaresults/handover_" << offsetMeter << "m.log";
  char const* hoFile = sstream.str().c_str();
  std::ofstream handoverFile(hoFile, std::ios_base::app | std::ios_base::out);
  double ts = Simulator::Now().GetSeconds();
  handoverFile << ts 
      << " "
      << "HandoverStart " 
      << "UE " 
      << imsi  
      << " " 
      << cellid 
      << " " 
      << rnti 
      <<  " " 
      << targetCellId 
      << "\n";
  handoverFile.close();
}

void
NotifyHandoverEndOkUe (std::string context,
                       uint64_t imsi,
                       uint16_t cellid,
                       uint16_t rnti)
{
  std::ostringstream sstream;
  sstream.str("");
  sstream << "lenaresults/handover_" << offsetMeter << "m.log";
  char const* hoFile = sstream.str().c_str();
  std::ofstream handoverFile(hoFile, std::ios_base::app | std::ios_base::out);
  double ts = Simulator::Now().GetSeconds();
  handoverFile << ts 
      << " "
      << "HandoverEndOk " 
      << "UE " 
      << imsi  
      << " " 
      << cellid 
      << " " 
      << rnti 
      << "\n";
  handoverFile.close();
}

void
NotifyConnectionEstablishedEnb (std::string context,
                                uint64_t imsi,
                                uint16_t cellid,
                                uint16_t rnti)
{
  std::ostringstream sstream;
  sstream.str("");
  sstream << "lenaresults/handover_" << offsetMeter << "m.log";
  char const* hoFile = sstream.str().c_str();
  std::ofstream handoverFile(hoFile, std::ios_base::app | std::ios_base::out);
  double ts = Simulator::Now().GetSeconds();
  handoverFile << ts
      << " "
      << "ConnectionEstablished "
      << "eNB "
      << cellid
      << " "
      << imsi
      << " "
      << rnti
      << "\n";
  handoverFile.close();
}

void
NotifyHandoverStartEnb (std::string context,
                        uint64_t imsi,
                        uint16_t cellid,
                        uint16_t rnti,
                        uint16_t targetCellId)
{
  std::ostringstream sstream;
  sstream.str("");
  sstream << "lenaresults/handover_" << offsetMeter << "m.log";
  char const* hoFile = sstream.str().c_str();
  std::ofstream handoverFile(hoFile, std::ios_base::app | std::ios_base::out);
  double ts = Simulator::Now().GetSeconds();
  handoverFile << ts
      << " "
      << "HandoverStart "
      << "eNB "
      << cellid
      << " "
      << imsi
      << " "
      << rnti
      << " "
      << targetCellId
      << "\n";
  handoverFile.close();
}

void
NotifyHandoverEndOkEnb (std::string context,
                        uint64_t imsi,
                        uint16_t cellid,
                        uint16_t rnti)
{
  std::ostringstream sstream;
  sstream.str("");
  sstream << "lenaresults/handover_" << offsetMeter << "m.log";
  char const* hoFile = sstream.str().c_str();
  std::ofstream handoverFile(hoFile, std::ios_base::app | std::ios_base::out);
  double ts = Simulator::Now().GetSeconds();
  handoverFile << ts
      << " "
      << "HandoverEndOk "
      << "eNB "
      << cellid
      << " "
      << imsi
      << " "
      << rnti
      << "\n";
  handoverFile.close();
}

void rxBytes (Ptr<const Packet> pkt, const Address &addr)
{
  uint32_t receivedBytes = pkt->GetSize();
  double timestamp = Simulator::Now().GetSeconds();
  std::ostringstream sstream;
  sstream.str("");
  sstream << "lenaresults/" << offsetMeter << "m.txt";
  char const* data = sstream.str().c_str();
  std::ofstream dataFile(data, std::ios_base::app | std::ios_base::out);
  dataFile << timestamp << " " << receivedBytes << "\n";
}

void printVector (Vector v) {
  std::cout << "Position: " << v.x << ", " << v.y << ", " << v.z << "\n";
}

void CourseChange (std::string context, Ptr<const MobilityModel> model) {
  double ts = Simulator::Now().GetSeconds();
  
  // One waypoint is still in the queue --> add one waypoint after it
  double waypointTS = ts + 2 * ((double)(numberOfEnbs) * distance / speed);
  Ptr<WaypointMobilityModel> mod = ueNodes.Get (0)->GetObject<WaypointMobilityModel> ();
  Vector pos = mod -> GetPosition();
  Waypoint wp = mod -> GetNextWaypoint();
  Vector nextPos = wp.position;
  std::ostringstream sstream;
  sstream << waypointTS << "s";
  std::string waypointString = sstream.str();
  
  if (reverseDirection) {
    mod -> AddWaypoint (Waypoint(Time(waypointString), Vector (0.5*distance, yForUe + offsetMeter, 0)));
    reverseDirection = false;
  } else {
    mod -> AddWaypoint (Waypoint(Time(waypointString), Vector ((numberOfEnbs*distance)+(distance/2), yForUe + offsetMeter, 0)));
    reverseDirection = true;
  }

  sstream.str("");
  sstream << "lenaresults/handover_" << offsetMeter << "m.log";
  char const* hoFile = sstream.str().c_str();
  std::ofstream handoverFile(hoFile, std::ios_base::app | std::ios_base::out);
  handoverFile << ts
      << " "
      << "CourseChanged"
      << " "
      << pos.x
      << " "
      << pos.y
      << " "
      << pos.z
      << "\n";
  handoverFile << ts
      << " "
      << "NextPosition"
      << " "
      << nextPos.x
      << " "
      << nextPos.y
      << " "
      << nextPos.z
      << "\n";

  handoverFile.close();
}

/**
 * Sample simulation script for an automatic X2-based handover based on the RSRQ measures.
 * It instantiates two eNodeB, attaches one UE to the 'source' eNB.
 * The UE moves between both eNBs, it reports measures to the serving eNB and
 * the 'source' (serving) eNB triggers the handover of the UE towards
 * the 'target' eNB when it considers it is a better eNB.
 */
int
main (int argc, char *argv[])
{
  // LogLevel logLevel = (LogLevel)(LOG_PREFIX_ALL | LOG_LEVEL_ALL);

  // LogComponentEnable ("LteHelper", logLevel);
  // LogComponentEnable ("EpcHelper", logLevel);
  // LogComponentEnable ("EpcEnbApplication", logLevel);
  // LogComponentEnable ("EpcX2", logLevel);
  // LogComponentEnable ("EpcSgwPgwApplication", logLevel);

  // LogComponentEnable ("LteEnbRrc", logLevel);
  // LogComponentEnable ("LteEnbNetDevice", logLevel);
  // LogComponentEnable ("LteUeRrc", logLevel);
  // LogComponentEnable ("LteUeNetDevice", logLevel);
  // LogComponentEnable ("A2A4RsrqHandoverAlgorithm", logLevel);
  // LogComponentEnable ("A3RsrpHandoverAlgorithm", logLevel);

  uint16_t numberOfUes = 1;
  uint16_t numBearersPerUe = 0;
  //double simTime = (double)(numberOfEnbs + 1) * distance / speed; // 1500 m / 20 m/s = 75 secs
  double simTime = 500;
  double enbTxPowerDbm = 46.0;

  // change some default attributes so that they are reasonable for
  // this scenario, but do this before processing command line
  // arguments, so that the user is allowed to override these settings
  Config::SetDefault ("ns3::UdpClient::Interval", TimeValue (MilliSeconds (10)));
  Config::SetDefault ("ns3::UdpClient::MaxPackets", UintegerValue (1000000));
  Config::SetDefault ("ns3::LteHelper::UseIdealRrc", BooleanValue (true));

  // Command line arguments
  CommandLine cmd;
  cmd.AddValue ("simTime", "Total duration of the simulation (in seconds)", simTime);
  cmd.AddValue ("speed", "Speed of the UE (default = 20 m/s)", speed);
  cmd.AddValue ("enbTxPowerDbm", "TX power [dBm] used by HeNBs (defalut = 46.0)", enbTxPowerDbm);
  cmd.AddValue ("offsetMeter", "offset from starting position", offsetMeter);

  cmd.Parse (argc, argv);


  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);
  lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");

  lteHelper->SetHandoverAlgorithmType ("ns3::A2A4RsrqHandoverAlgorithm");
  lteHelper->SetHandoverAlgorithmAttribute ("ServingCellThreshold",
                                            UintegerValue (30));
  lteHelper->SetHandoverAlgorithmAttribute ("NeighbourCellOffset",
                                            UintegerValue (1));

  //  lteHelper->SetHandoverAlgorithmType ("ns3::A3RsrpHandoverAlgorithm");
  //  lteHelper->SetHandoverAlgorithmAttribute ("Hysteresis",
  //                                            DoubleValue (3.0));
  //  lteHelper->SetHandoverAlgorithmAttribute ("TimeToTrigger",
  //                                            TimeValue (MilliSeconds (256)));

  Ptr<Node> pgw = epcHelper->GetPgwNode ();

  // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // Create the Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);


  // Routing of the Internet Host (towards the LTE network)
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  // interface 0 is localhost, 1 is the p2p device
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  /*
   * Network topology:
   *
   *      |     + --------------------------------------------------------->
   *      |     UE
   *      |
   *      |               d                   d                   d
   *    y |     |-------------------x-------------------x-------------------
   *      |     |                 eNodeB              eNodeB
   *      |   d |
   *      |     |
   *      |     |                                             d = distance
   *            o (0, 0, 0)                                   y = yForUe
   */

  
  NodeContainer enbNodes;
  enbNodes.Create (numberOfEnbs);
  ueNodes.Create (numberOfUes);

  // Install Mobility Model in eNB
  Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator> ();
  for (uint16_t i = 0; i < numberOfEnbs; i++)
    {
      Vector enbPosition (distance * (i + 1), distance, 0);
      enbPositionAlloc->Add (enbPosition);
    }
  MobilityHelper enbMobility;
  enbMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  enbMobility.SetPositionAllocator (enbPositionAlloc);
  enbMobility.Install (enbNodes);

  // Install Mobility Model in UE
//  MobilityHelper ueMobility;
//  ueMobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
//  ueMobility.Install (ueNodes);
//  ueNodes.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, yForUe + offsetMeter, 0));
//  std::cout << "Starting position: (0, " << yForUe+offsetMeter << " , 0)\n";
//  ueNodes.Get (0)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (speed, 0, 0));

  MobilityHelper ueMobility;
  ueMobility.SetMobilityModel ("ns3::WaypointMobilityModel");
  ueMobility.Install (ueNodes);
  std::cout << "Starting position: (0, " << yForUe+offsetMeter << " , 0)\n";
  Ptr<WaypointMobilityModel> mod = ueNodes.Get (0)->GetObject<WaypointMobilityModel> ();
  
  // Add the initial waypoint
  mod -> AddWaypoint (Waypoint(Time("0s"), Vector (0.5*distance, yForUe + offsetMeter, 0)));
  
  // Add two following waypoints
  double waypointTS = (double)(numberOfEnbs) * distance / speed;
  std::ostringstream sstream;
  sstream << waypointTS << "s";
  std::string waypointString = sstream.str();
  mod -> AddWaypoint (Waypoint(Time(waypointString), Vector ((numberOfEnbs*distance)+(distance/2), yForUe + offsetMeter, 0)));
  std::ostringstream sstream1;
  sstream1 << 2*waypointTS << "s";
  std::string waypointString1 = sstream1.str();
  mod -> AddWaypoint (Waypoint(Time(waypointString1), Vector (0.5*distance, yForUe + offsetMeter, 0)));

  // Install LTE Devices in eNB and UEs
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (enbTxPowerDbm));
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);
  
  // Install the IP stack on the UEs
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIfaces;
  ueIpIfaces = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));
  // Assign IP address to UEs, and install applications
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }


  // Attach all UEs to the first eNodeB
  for (uint16_t i = 0; i < numberOfUes; i++)
    {
      lteHelper->Attach (ueLteDevs.Get (i), enbLteDevs.Get (0));
    }

  lteHelper->AttachToClosestEnb(ueLteDevs, enbLteDevs);


  NS_LOG_LOGIC ("setting up applications");

  // Install and start applications on UEs and remote host
  uint16_t dlPort = 10000;
  uint16_t ulPort = 20000;

  // randomize a bit start times to avoid simulation artifacts
  // (e.g., buffer overflows due to packet transmissions happening
  // exactly at the same time)
  Ptr<UniformRandomVariable> startTimeSeconds = CreateObject<UniformRandomVariable> ();
  startTimeSeconds->SetAttribute ("Min", DoubleValue (0));
  startTimeSeconds->SetAttribute ("Max", DoubleValue (0.010));



  for (uint32_t u = 0; u < numberOfUes; ++u)
    {
      Ptr<Node> ue = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ue->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
      
      for (uint32_t b = 0; b < numBearersPerUe; ++b)
        {
          ++dlPort;
          ++ulPort;

          ApplicationContainer clientApps;
          ApplicationContainer serverApps;

          NS_LOG_LOGIC ("installing UDP DL app for UE " << u);
          UdpClientHelper dlClientHelper (ueIpIfaces.GetAddress (u), dlPort);
          clientApps.Add (dlClientHelper.Install (remoteHost));
          PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory",
                                               InetSocketAddress (Ipv4Address::GetAny (), dlPort));
          serverApps.Add (dlPacketSinkHelper.Install (ue));

          NS_LOG_LOGIC ("installing UDP UL app for UE " << u);
          UdpClientHelper ulClientHelper (remoteHostAddr, ulPort);
          clientApps.Add (ulClientHelper.Install (ue));
          PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory",
                                               InetSocketAddress (Ipv4Address::GetAny (), ulPort));
          serverApps.Add (ulPacketSinkHelper.Install (remoteHost));

          Ptr<EpcTft> tft = Create<EpcTft> ();
          EpcTft::PacketFilter dlpf;
          dlpf.localPortStart = dlPort;
          dlpf.localPortEnd = dlPort;
          tft->Add (dlpf);
          EpcTft::PacketFilter ulpf;
          ulpf.remotePortStart = ulPort;
          ulpf.remotePortEnd = ulPort;
          tft->Add (ulpf);
          EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
          lteHelper->ActivateDedicatedEpsBearer (ueLteDevs.Get (u), bearer, tft);

          Time startTime = Seconds (startTimeSeconds->GetValue ());
          serverApps.Start (startTime);
          clientApps.Start (startTime);

        } // end for b
    }

  // Install and start applications on UEs and remote host
  ApplicationContainer onoffApps;
  for (uint16_t i = 0; i < numberOfUes; i++) {
    OnOffHelper onoff ("ns3::TcpSocketFactory", InetSocketAddress (ueIpIfaces.GetAddress (i), 50000));
    //onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=30]")); 
    //onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    //onoff.SetAttribute ("DataRate", DataRateValue (DataRate ("1000Mb/s")));
    //onoff.SetAttribute ("PacketSize", UintegerValue (1000000));
    onoff.SetConstantRate (DataRate ("1000Mb/s"));
    onoffApps = onoff.Install (remoteHostContainer.Get (0));
  }
  onoffApps.Start (Seconds (1.0));
  onoffApps.Stop (Seconds (simTime));

  
  ApplicationContainer sinkApps;
  for (uint16_t i = 0; i < numberOfUes; i++) {
    PacketSinkHelper sink ("ns3::TcpSocketFactory", InetSocketAddress (ueIpIfaces.GetAddress (i), 50000));
    sinkApps = sink.Install (ueNodes.Get (i));
  }
  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (Seconds (simTime));


  // Add X2 inteface
  lteHelper->AddX2Interface (enbNodes);

  // X2-based Handover
  //lteHelper->HandoverRequest (Seconds (0.100), ueLteDevs.Get (0), enbLteDevs.Get (0), enbLteDevs.Get (1));

  // Uncomment to enable PCAP tracing
  // p2ph.EnablePcapAll("lena-x2-handover-measures");

  lteHelper->EnablePhyTraces ();
  lteHelper->EnableMacTraces ();
  lteHelper->EnableRlcTraces ();
  lteHelper->EnablePdcpTraces ();
  Ptr<RadioBearerStatsCalculator> rlcStats = lteHelper->GetRlcStats ();
  rlcStats->SetAttribute ("EpochDuration", TimeValue (Seconds (1.0)));
  Ptr<RadioBearerStatsCalculator> pdcpStats = lteHelper->GetPdcpStats ();
  pdcpStats->SetAttribute ("EpochDuration", TimeValue (Seconds (1.0)));

  // connect custom trace sinks for RRC connection establishment and handover notification
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionEstablished",
                   MakeCallback (&NotifyConnectionEstablishedEnb));
  Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionEstablished",
                   MakeCallback (&NotifyConnectionEstablishedUe));
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverStart",
                   MakeCallback (&NotifyHandoverStartEnb));
  Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/HandoverStart",
                   MakeCallback (&NotifyHandoverStartUe));
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverEndOk",
                   MakeCallback (&NotifyHandoverEndOkEnb));
  Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/HandoverEndOk",
                   MakeCallback (&NotifyHandoverEndOkUe));
  
  Config::Connect ("/NodeList/*/$ns3::WaypointMobilityModel/CourseChange",
                   MakeCallback (&CourseChange)); 

  Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::PacketSink/Rx", MakeCallback (rxBytes));


  Simulator::Stop (Seconds (simTime));
  Simulator::Run ();

  // GtkConfigStore config;
  // config.ConfigureAttributes ();

  Simulator::Destroy ();
  return 0;

}
