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

using namespace ns3;
using namespace std;

Ptr<myBufferApp> sinkApp;
Ptr<MyApp> sourceApp;
Ptr<myVideoBuffer> vidBuffer;
Ptr<myFlowManager> flowManager;

void rxBytes (string context, Ptr<const Packet> pkt, const Address &addr)
{
  uint32_t receivedBytes = pkt->GetSize();
  double timestamp = Simulator::Now().GetSeconds();
  //cout << "Bytes: " << receivedBytes << "\t Timestamp: " << timestamp <<"\n";

  sourceApp->updateSendRest(-receivedBytes);
  sinkApp->mySinkRx(receivedBytes, timestamp);
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

  uint16_t numberOfNodes = 1;
  double simTime = -1.0;
  double distance = 0.0;
  double interPacketInterval = 100;
  double delay = 0.0;
  double loss = 0.0;
  string datarate = "1000";
  string pathlogfiles = "lenaresults/";
  string framelist = "scratch/framelist.txt";

  // Command line arguments
  CommandLine cmd;
  cmd.AddValue("numberOfNodes", "Number of eNodeBs + UE pairs", numberOfNodes);
  cmd.AddValue("simTime", "Total duration of the simulation [s])", simTime);
  cmd.AddValue("distance", "Distance between eNBs [m]", distance);
  cmd.AddValue("interPacketInterval", "Inter packet interval [ms])", interPacketInterval);
  cmd.AddValue("delay", "Delay for p2ph [ms])", delay);
  cmd.AddValue("datarate", "DataRate for p2ph [Mb/s])", datarate);
  cmd.AddValue("pathlogfiles", "Destination for logfiles", pathlogfiles);
  cmd.AddValue("pathframelist", "Destination to framelist", framelist);
  cmd.AddValue("loss", "Loss of link", loss);
  cmd.Parse(argc, argv);
  ns3::PacketMetadata::Enable ();
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);

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

  NodeContainer ueNodes;
  NodeContainer enbNodes;
  enbNodes.Create(numberOfNodes);
  ueNodes.Create(numberOfNodes);

  // Install Mobility Model
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  for (uint16_t i = 0; i < numberOfNodes; i++)
    {
      positionAlloc->Add (Vector(distance * i, 0, 0));
    }
  MobilityHelper mobility;
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator(positionAlloc);
  mobility.Install(enbNodes);
  mobility.Install(ueNodes);

  // Install LTE Devices to the nodes
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

  // Install the IP stack on the UEs
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));
  // Assign IP address to UEs, and install applications
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  // Attach one UE per eNodeB
  for (uint16_t i = 0; i < numberOfNodes; i++)
      {
        lteHelper->Attach (ueLteDevs.Get(i), enbLteDevs.Get(i));
        // side effect: the default EPS bearer will be activated
      }

  
  Address sinkAddress;
  ApplicationContainer sinkApps;
  for (uint16_t i = 0; i < numberOfNodes; i++) {
    uint16_t sinkPort = 8080;
    sinkAddress = (InetSocketAddress (ueIpIface.GetAddress (i), sinkPort));
    PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
    ApplicationContainer sinkApps = packetSinkHelper.Install (ueNodes.Get (i));
  }
  sinkApps.Start (Seconds (0.));

  // generate output folder and logfile
  time_t cputime = time(NULL);
  
  double frate = 24;
  sstream << "/" << delay << "ms_" << datarate << "mbit_" << loss << "pct_" << "results_" << cputime;
  string logpath = sstream.str();
  sstream.str("");
  sstream << "mkdir -p " << logpath;
  char const* mkdir_cmd = sstream.str().c_str();
  system(mkdir_cmd);

  flowManager = CreateObject<myFlowManager> ();
  flowManager->Setup (2.5, 10, 5, 0.5, logpath);
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

  Config::Connect ("/NodeList/3/ApplicationList/0/$ns3::PacketSink/Rx", MakeCallback (rxBytes));

  //lteHelper->EnableTraces ();
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
