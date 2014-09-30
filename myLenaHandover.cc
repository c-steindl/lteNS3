/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Author: Jaume Nin <jaume.nin@cttc.cat>
 */

#include "ns3/lte-helper.h"
#include "ns3/epc-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/config-store.h"
#include "ns3/nstime.h"
#include "myFlowManager.h"
#include "myApp.h"
#include "myBufferApp.h"
//#include "ns3/gtk-config-store.h"
#include "string"
 #include <fstream>
#include <ios>

using namespace ns3;
using namespace std;

Ptr<myBufferApp> sinkApp;
Ptr<MyApp> sourceApp;
Ptr<myVideoBuffer> vidBuffer;
Ptr<myFlowManager> flowManager;
string nameHandover = "/handover.log";
string logpath = "";
bool reverseDirection = false;
NodeContainer ueNodes;
uint16_t numberOfEnbs = 2;
double distanceEnb = 500.0; // m
double speed = 20;       // m/s
double yForUe = 0.0;   // m

void rxBytes (Ptr<const Packet> pkt, const Address &addr)
{
  uint32_t receivedBytes = pkt->GetSize();
  double timestamp = Simulator::Now().GetSeconds();
  //cout << "Bytes: " << receivedBytes << "\t Timestamp: " << timestamp <<"\n";

  sourceApp->updateSendRest(-receivedBytes);
  sinkApp->mySinkRx(receivedBytes, timestamp);
}

void
NotifyConnectionEstablishedUe (string context,
                               uint64_t imsi,
                               uint16_t cellid,
                               uint16_t rnti)
{
  ostringstream sstream;
  sstream.str("");
  sstream << logpath << nameHandover;
  char const* hoFile = sstream.str().c_str();
  ofstream handoverFile(hoFile, ios_base::app | ios_base::out);
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
NotifyHandoverStartUe (string context,
                       uint64_t imsi,
                       uint16_t cellid,
                       uint16_t rnti,
                       uint16_t targetCellId)
{
  ostringstream sstream;
  sstream.str("");
  sstream << logpath << nameHandover;
  char const* hoFile = sstream.str().c_str();
  ofstream handoverFile(hoFile, ios_base::app | ios_base::out);
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
NotifyHandoverEndOkUe (string context,
                       uint64_t imsi,
                       uint16_t cellid,
                       uint16_t rnti)
{
  ostringstream sstream;
  sstream.str("");
  sstream << logpath << nameHandover;
  char const* hoFile = sstream.str().c_str();
  ofstream handoverFile(hoFile, ios_base::app | ios_base::out);
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
NotifyConnectionEstablishedEnb (string context,
                                uint64_t imsi,
                                uint16_t cellid,
                                uint16_t rnti)
{
  ostringstream sstream;
  sstream.str("");
  sstream << logpath << nameHandover;
  char const* hoFile = sstream.str().c_str();
  ofstream handoverFile(hoFile, ios_base::app | ios_base::out);
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
NotifyHandoverStartEnb (string context,
                        uint64_t imsi,
                        uint16_t cellid,
                        uint16_t rnti,
                        uint16_t targetCellId)
{
  ostringstream sstream;
  sstream.str("");
  sstream << logpath << nameHandover;
  char const* hoFile = sstream.str().c_str();
  ofstream handoverFile(hoFile, ios_base::app | ios_base::out);
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
NotifyHandoverEndOkEnb (string context,
                        uint64_t imsi,
                        uint16_t cellid,
                        uint16_t rnti)
{
  ostringstream sstream;
  sstream.str("");
  sstream << logpath << nameHandover;
  char const* hoFile = sstream.str().c_str();
  ofstream handoverFile(hoFile, ios_base::app | ios_base::out);
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

void
CourseChange (string context, Ptr<const MobilityModel> model) {
  double ts = Simulator::Now().GetSeconds();
  
  // One waypoint is still in the queue --> add one waypoint after it
  double waypointTS = ts + 2 * ((double)(numberOfEnbs) * distanceEnb / speed);
  Ptr<WaypointMobilityModel> mod = ueNodes.Get (0)->GetObject<WaypointMobilityModel> ();
  Vector pos = mod -> GetPosition();
  Waypoint wp = mod -> GetNextWaypoint();
  Vector nextPos = wp.position;
  ostringstream sstream;
  sstream << waypointTS << "s";
  string waypointString = sstream.str();
  
  if (!reverseDirection) {
    mod -> AddWaypoint (Waypoint(Time(waypointString), Vector (0.5*distanceEnb, distanceEnb + yForUe, 0)));
    reverseDirection = true;
  } else {
    mod -> AddWaypoint (Waypoint(Time(waypointString), Vector ((numberOfEnbs*distanceEnb)+(distanceEnb/2), distanceEnb + yForUe, 0)));
    reverseDirection = false;
  }

  sstream.str("");
  sstream << logpath << nameHandover;
  char const* hoFile = sstream.str().c_str();
  ofstream handoverFile(hoFile, ios_base::app | ios_base::out);
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

void
addInitWaypoints (uint32_t i) {
  cout << "Starting position: (0, " << distanceEnb + yForUe << " , 0)\n";
  Ptr<WaypointMobilityModel> mod = ueNodes.Get (i)->GetObject<WaypointMobilityModel> ();
  
  // Add the initial waypoint
  mod -> AddWaypoint (Waypoint(Time("0s"), Vector (0.5*distanceEnb, distanceEnb + yForUe, 0)));
  
  // Add two following waypoints
  double waypointTS = (double)(numberOfEnbs) * distanceEnb / speed;
  ostringstream sstream;
  sstream << waypointTS << "s";
  string waypointString = sstream.str();
  mod -> AddWaypoint (Waypoint(Time(waypointString), Vector ((numberOfEnbs*distanceEnb)+(distanceEnb/2), distanceEnb + yForUe, 0)));
}


/**
 * Sample simulation script for LTE+EPC. It instantiates several eNodeB,
 * attaches one UE per eNodeB starts a flow for each UE to  and from a remote host.
 * It also  starts yet another flow between each UE pair.
 */


NS_LOG_COMPONENT_DEFINE ("myLena");
int
main (int argc, char *argv[])
{

  Config::SetDefault ("ns3::LteHelper::UseIdealRrc", BooleanValue (true));

  uint16_t numberOfUes = 1;
  double simTime = -1.0;
  double enbTxPowerDbm = 46.0;
  double interPacketInterval = 100;
  double delay = 0.0;
  double loss = 0.0;
  double distance = 500;
  string datarate = "1000";
  string framelist = "scratch/framelist.txt";
  string pathlogfiles = "lenaresults/";

  // Command line arguments
  CommandLine cmd;
  cmd.AddValue("numberOfUes", "Number of UEs", numberOfUes);
  cmd.AddValue("numberOfEnbs", "Number of eNodeBs", numberOfEnbs);
  cmd.AddValue("simTime", "Total duration of the simulation [s])", simTime);
  cmd.AddValue("distance", "Distance between eNBs [m]", distance);
  cmd.AddValue("interPacketInterval", "Inter packet interval [ms])", interPacketInterval);
  cmd.AddValue("delay", "Delay for p2ph [ms])", delay);
  cmd.AddValue("datarate", "DataRate for p2ph [Mb/s])", datarate);
  cmd.AddValue("pathlogfiles", "Destination for logfiles", pathlogfiles);
  cmd.AddValue("pathframelist", "Destination to framelist", framelist);
  cmd.AddValue("loss", "Loss of link", loss);
  cmd.AddValue("speed", "Speed of UE [m/s]", speed);
  cmd.AddValue("offset", "Vertical distance between UE and eNodeBs [m]", yForUe);
  cmd.Parse(argc, argv);
  ns3::PacketMetadata::Enable ();

  distanceEnb = distance;

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);
  lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");
  lteHelper->SetHandoverAlgorithmType ("ns3::A2A4RsrqHandoverAlgorithm");
  lteHelper->SetHandoverAlgorithmAttribute ("ServingCellThreshold",
                                            UintegerValue (30));
  lteHelper->SetHandoverAlgorithmAttribute ("NeighbourCellOffset",
                                            UintegerValue (1));

  string datarate_value = "";
  datarate_value = datarate + "Mb/s";
  
  ostringstream sstream;
  sstream << pathlogfiles;
  ostringstream pcapstream;

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults();

  // parse again so you can override default values from the command line
  cmd.Parse(argc, argv);

  Ptr<Node> pgw = epcHelper->GetPgwNode ();

   // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // Create the Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate (datarate_value)));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (delay)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  // interface 0 is localhost, 1 is the p2p device
  //Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  NodeContainer enbNodes;
  enbNodes.Create(numberOfEnbs);
  ueNodes.Create(numberOfUes);

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
  MobilityHelper ueMobility;
  ueMobility.SetMobilityModel ("ns3::WaypointMobilityModel");
  ueMobility.Install (ueNodes);

  // Install LTE Devices to the nodes
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (enbTxPowerDbm));
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

  // Install the IP stack on the UEs
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));
  // Assign IP address to UEs, and install applications
  

  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      addInitWaypoints(u);
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

  
  Address sinkAddress;
  ApplicationContainer sinkApps;
  for (uint16_t i = 0; i < numberOfUes; i++) {
    uint16_t sinkPort = 8080;
    sinkAddress = (InetSocketAddress (ueIpIface.GetAddress (i), sinkPort));
    PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
    ApplicationContainer sinkApps = packetSinkHelper.Install (ueNodes.Get (i));
  }
  sinkApps.Start (Seconds (0.));

  // generate output folder and logfile
  time_t cputime = time(NULL);
  
  double frate = 24;
  sstream << delay << "ms_" << datarate << "mbit_" << loss << "pct_" << "results_" << cputime;
  logpath = sstream.str();
  sstream.str("");
  sstream << "mkdir -p " << logpath;
  char const* mkdir_cmd = sstream.str().c_str();
  system(mkdir_cmd);

  flowManager = CreateObject<myFlowManager> ();
  flowManager->Setup (2.5, 10000, 5, 0.5, logpath);
  flowManager->SetStartTime (Seconds (0.));

  Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (remoteHostContainer.Get (0), TcpSocketFactory::GetTypeId ());
  sourceApp = CreateObject<MyApp> ();
  sourceApp->Setup (ns3TcpSocket, sinkAddress, 2000, flowManager, frate);
  remoteHostContainer.Get (0)->AddApplication (sourceApp);
  sourceApp->SetStartTime (Seconds (1.));

  vidBuffer = CreateObject<myVideoBuffer> ();
  vidBuffer->Setup (sourceApp, flowManager, logpath);
  vidBuffer->SetStartTime (Seconds (0.));

  sinkApp = CreateObject<myBufferApp> ();
  sinkApp->Setup (sourceApp, 1000, frate, vidBuffer, flowManager, framelist);
  ueNodes.Get (0)->AddApplication (sinkApp);
  sinkApp->SetStartTime (Seconds (0.));

  // Callback for receiving data
  Config::ConnectWithoutContext ("/NodeList/3/ApplicationList/0/$ns3::PacketSink/Rx", MakeCallback (rxBytes));

  // Callbacks for handover
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

  // Callback for mobility model
  Config::Connect ("/NodeList/*/$ns3::WaypointMobilityModel/CourseChange",
                   MakeCallback (&CourseChange)); 

  lteHelper->EnableTraces ();
  lteHelper->AddX2Interface (enbNodes);
  // Uncomment to enable PCAP tracing
  pcapstream << "/" << "Lena_" << delay << "ms_" << datarate << "mbit_" << loss << "pct_" << "results_" << cputime;
  //p2ph.EnablePcapAll(logpath + pcapstream.str());
  //AsciiTraceHelper ascii;
  //p2ph.EnableAsciiAll (ascii.CreateFileStream (logpath + pcapstream.str() + ".tr"));
  
  if (simTime >= 0) {  
    sinkApps.Stop (Seconds (simTime));
    flowManager->SetStopTime (Seconds (simTime));
    sourceApp->SetStopTime (Seconds (simTime));
    vidBuffer->SetStopTime (Seconds (simTime));
    sinkApp->SetStopTime (Seconds (simTime));
    Simulator::Stop(Seconds(simTime));
  }

  Simulator::Run();

  /*GtkConfigStore config;
  config.ConfigureAttributes();*/

  Simulator::Destroy();

  /*Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (sinkApps.Get (0));
  double mbps = (sink1->GetTotalRx () * 8)/((simTime-1)*1e6);
  cout << "Mbit/s: " << mbps << endl;
  cout << "Total Bytes Received: " << sink1->GetTotalRx () << endl;
  */

  /*vidBuffer->~myVideoBuffer();
  sstream.str("");
  sstream << "Rscript scratch/lena_analyse.r " << logpath;
  char const* cmd_R = sstream.str().c_str();
  system(cmd_R);*/

  return 0;

}
