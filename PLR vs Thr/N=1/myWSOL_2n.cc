#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/ipv4-global-routing-helper.h"

// For flowmonitor
#include "ns3/flow-monitor-module.h"
#include "ns3/openflow-module.h"
#include "ns3/log.h"
#include "ns3/olsr-helper.h"

// For gnuplot and Gnuplot2Ddatabase
#include "ns3/stats-module.h"
#include "ns3/random-variable-stream.h"

#include "ns3/netanim-module.h"
#include <iostream>
#include <fstream>
#include <vector>

// Default Network Topology
//
//        11.2   192.168.11.1
//        STA           AP
//         *            *
//   y^    |            | 
//    |    U------------S
//    |   0.0         5.0
//    ------------------------------------------->x

double nSamplingPeriod = 0.1;   // 抽样间隔，根据总的Simulation时间做相应的调整

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("WSOL_2n");

void
ThroughputMonitor (FlowMonitorHelper* fmhelper, Ptr<FlowMonitor> monitor, 
  Gnuplot2dDataset dataset)
{
  
  double throu   = 0.0;
  monitor->CheckForLostPackets ();
  std::map<FlowId, FlowMonitor::FlowStats> flowStats = monitor->GetFlowStats ();
  /* since fmhelper is a pointer, we should use it as a pointer.
   * `fmhelper->GetClassifier ()` instead of `fmhelper.GetClassifier ()`
   */
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier ());
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = flowStats.begin (); i != flowStats.end (); ++i)
  {
  /* 
   * `Ipv4FlowClassifier`
   * Classifies packets by looking at their IP and TCP/UDP headers. 
   * FiveTuple五元组是：(source-ip, destination-ip, protocol, source-port, destination-port)
  */

  /* 每个flow是根据包的五元组(协议，源IP/端口，目的IP/端口)来区分的 */
  Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
  // `192.168.0.11`是client(Node #14)的IP,
  // `192.168.0.7` 是client(Node #10)的IP
    if ((t.sourceAddress=="192.168.11.2" && t.destinationAddress == "192.168.11.1"))
    {
        // UDP_PROT_NUMBER = 17
      if (17 == unsigned(t.protocol))
      {
      	//bytes*8=bits, bits/s=bps, 1bps/1024/1024=Mbps, 1bps=10e6 bpus, 10e6/1024/1024=0.9537 => throu (Mbps)
        throu   = i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetMicroSeconds() - i->second.timeFirstTxPacket.GetMicroSeconds()) * 0.9537 ;
        dataset.Add  (Simulator::Now().GetSeconds(), throu);
      }
      else
      {
        std::cout << "This is not UDP traffic" << std::endl;
      }
    }

  }
  /* check throughput every nSamplingPeriod second(每隔nSamplingPeriod调用1次Simulation)
   * 表示每隔nSamplingPeriod时间
   */
  Simulator::Schedule (Seconds(nSamplingPeriod), &ThroughputMonitor, fmhelper, monitor, 
    dataset);

}

void
DelayMonitor (FlowMonitorHelper* fmhelper, Ptr<FlowMonitor> monitor, 
  Gnuplot2dDataset dataset1)
{
  
  double delay   = 0.0;
  double rxpackets = 0.0;
  double mean_delay = 0.0;
  monitor->CheckForLostPackets ();
  std::map<FlowId, FlowMonitor::FlowStats> flowStats = monitor->GetFlowStats ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier ());
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = flowStats.begin (); i != flowStats.end (); ++i)
    {

    Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
    if ((t.sourceAddress=="192.168.11.2" && t.destinationAddress == "192.168.11.1"))
      {
          // UDP_PROT_NUMBER = 17
          if (17 == unsigned(t.protocol))
          {
          	//lastDelay: The last measured delay of a packet.
            delay   = i->second.delaySum.GetMilliSeconds ();	//MilliSeconds, ms
            rxpackets = i->second.rxPackets;
            mean_delay = delay / rxpackets;
            dataset1.Add (Simulator::Now().GetSeconds(), mean_delay);
          }
          else
          {
            std::cout << "This is not UDP traffic" << std::endl;
          }
      }

    }
  Simulator::Schedule (Seconds(nSamplingPeriod), &DelayMonitor, fmhelper, monitor, 
    dataset1);
}

void
LostPacketsMonitor (FlowMonitorHelper* fmhelper, Ptr<FlowMonitor> monitor, 
  Gnuplot2dDataset dataset2)
{
  
  double packets = 0.0;
  double rxpackets = 0.0;
  double packet_loss_ratio = 0.0;
  monitor->CheckForLostPackets ();
  std::map<FlowId, FlowMonitor::FlowStats> flowStats = monitor->GetFlowStats ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier ());
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = flowStats.begin (); i != flowStats.end (); ++i)
    {

    Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
    if ((t.sourceAddress=="192.168.11.2" && t.destinationAddress == "192.168.11.1"))
      {
          // UDP_PROT_NUMBER = 17
          if (17 == unsigned(t.protocol))
          {
            packets = i->second.lostPackets;
            rxpackets = i -> second.rxPackets;
            packet_loss_ratio = packets / (rxpackets + packets);
            dataset2.Add (Simulator::Now().GetSeconds(), packet_loss_ratio);
          }
          else
          {
            std::cout << "This is not UDP traffic" << std::endl;
          }
      }

    }
  Simulator::Schedule (Seconds(nSamplingPeriod), &LostPacketsMonitor, fmhelper, monitor, 
    dataset2);

}

void
JitterMonitor (FlowMonitorHelper* fmhelper, Ptr<FlowMonitor> monitor, 
  Gnuplot2dDataset dataset3)
{
  
  double jitter  = 0.0;
  double rxpackets = 0.0;
  double mean_jitter = 0.0;
  monitor->CheckForLostPackets ();
  std::map<FlowId, FlowMonitor::FlowStats> flowStats = monitor->GetFlowStats ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier ());
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = flowStats.begin (); i != flowStats.end (); ++i)
    {

    Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
    if ((t.sourceAddress=="192.168.11.2" && t.destinationAddress == "192.168.11.1"))
      {
          // UDP_PROT_NUMBER = 17
          if (17 == unsigned(t.protocol))
          {
            jitter  = i->second.jitterSum.GetMilliSeconds ();
            rxpackets = i -> second.rxPackets;
            mean_jitter = jitter / rxpackets;
            dataset3.Add (Simulator::Now().GetSeconds(), mean_jitter);
          }
          else
          {
            std::cout << "This is not UDP traffic" << std::endl;
          }
      }

    }
  Simulator::Schedule (Seconds(nSamplingPeriod), &JitterMonitor, fmhelper, monitor, 
    dataset3);

}

int 
main (int argc, char *argv[])
{
	bool verbose = true;
	bool tracing = true;
  std::string time_interval = "0.1";

	CommandLine cmd;
	cmd.AddValue ("verbose", "Tell applications to log if true", verbose);
  cmd.AddValue ("time_interval", "Tell Client the send Interval", time_interval);
	// cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

	cmd.Parse (argc,argv);

	if (verbose)
	{
		LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);
		LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);
	}
//--------------------Nodes Define-------------------------------------
	Ptr<Node> nU = CreateObject<Node> ();		// Node0
	// Ptr<Node> nR0 = CreateObject<Node> ();	// Node1
	// Ptr<Node> nR1 = CreateObject<Node> ();	// Node2
	Ptr<Node> nS = CreateObject<Node> ();		// Node3

	NodeContainer nodes = NodeContainer(nU, nS);
	// Sta-AP links
	// NodeContainer nSnR1 = NodeContainer (nS, nR1);
	// NodeContainer nR1nR0 = NodeContainer (nR1, nR0);
	// NodeContainer nR0nU = NodeContainer (nR0, nU);
	NodeContainer nSnU = NodeContainer (nS, nU);
	// Sta and AP nodes
	// NodeContainer nSTA = NodeContainer (nU, nR0, nR1);
	// NodeContainer nAP = NodeContainer (nR0, nR1, nS);

//----------------------PHY Define----------------------------------------
	int mcs = 9;
	uint16_t channelWidth = 80;
	bool Short_GI = true;
	bool udp = true;
	uint32_t payloadSize; //1500 byte IP packet
	if (udp)
		payloadSize = 1472; //bytes
	else
	{
		payloadSize = 1448; //for TCP
		Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));
	}
	
	std::cout << "mcs: " << mcs << "\tChannelWidth: " << channelWidth << " MHz\tShort GI: " << Short_GI << "\t" << std::endl;

	YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
	YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
	phy.SetChannel (channel.Create ());

	phy.Set ("ShortGuardEnabled", BooleanValue (Short_GI));


//--------------------MAC and Device Define-------------------------------
	WifiHelper wifi;
	// wifi.SetRemoteStationManager ("ns3::AarfWifiManager");
	wifi.SetStandard (WIFI_PHY_STANDARD_80211ac);

	std::ostringstream oss;
	oss << "VhtMcs" << mcs;
	wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode", StringValue (oss.str ()),
	                              "ControlMode", StringValue (oss.str ()));
	WifiMacHelper mac;
	Ssid ssid = Ssid ("openwrt11");
	mac.SetType ("ns3::StaWifiMac",
							 "Ssid", SsidValue (ssid),
							 "ActiveProbing", BooleanValue (false));

	NetDeviceContainer dSdU;
	dSdU = wifi.Install (phy, mac, nS);
	// WifiMacHelper apmac;
	// Ssid ssid_11 = Ssid ("openwrt11");
	mac.SetType ("ns3::ApWifiMac",
							 "Ssid", SsidValue (ssid));


	dSdU.Add (wifi.Install (phy, mac, nU));	

	// dSdR1.Add (wifi.Install (phy, stamac, nR1));

	// Ssid ssid_35 = Ssid ("openwrt35");
	// apmac.SetType ("ns3::ApWifiMac",
	// 						 "Ssid", SsidValue (ssid_35));

	// NetDeviceContainer dR1dR0;
	// dR1dR0 = wifi.Install (phy, apmac, nR1);
	// dR1dR0.Add (wifi.Install (phy, stamac, nR0));

	// Ssid ssid_25 = Ssid ("openwrt25");
	// apmac.SetType ("ns3::ApWifiMac",
	// 						 "Ssid", SsidValue (ssid_25));
	
	// NetDeviceContainer dU;
	// dU = wifi.Install (phy, stamac, nU);
	Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/ChannelWidth", UintegerValue (channelWidth));
	
	InternetStackHelper stack;
	stack.Install (nodes);

//----------------------Mobility model(Must for wifi)---------------------------------------
	MobilityHelper mobility;
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
	positionAlloc->Add (Vector (0.0, 0.0, 0.0));
	positionAlloc->Add (Vector (8.0, 0.0, 0.0));
	mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);
  // When InitialPositionIsWaypoint is false (default), the position
  // set by the position allocator is ignored.  The first waypoint set will
  // set the initial position (with velocity 0 until first waypoint time)
  // mobility.SetMobilityModel ("ns3::WaypointMobilityModel",
  //                            "InitialPositionIsWaypoint", BooleanValue (false));
  // When InitialPositionIsWaypoint is true, the position allocator creates
  // the first waypoint, and movement occurs between this origin and the
  // initial waypoint below at 5 seconds
 //  mobility.SetMobilityModel ("ns3::WaypointMobilityModel",
 //                             "InitialPositionIsWaypoint", BooleanValue (true));
	// mobility.Install (nodes);

 //  Ptr<WaypointMobilityModel> mob = nodes.Get (1)->GetObject<WaypointMobilityModel> ();
 //  Waypoint wpt (Seconds (2.0), Vector (1.0, 0, 0));
 //  Waypoint wpt2 (Seconds (10.0), Vector (2.0, 0, 0));
 //  Waypoint wpt3 (Seconds (20.0), Vector (5.0, 0, 0));
 //  Waypoint wpt4 (Seconds (30.0), Vector (8.0, 0, 0));
 //  Waypoint wpt5 (Seconds (40.0), Vector (9.0, 0, 0));
 //  Waypoint wpt6 (Seconds (50.0), Vector (10.0, 0, 0));

 //  mob->AddWaypoint (wpt);
 //  mob->AddWaypoint (wpt2);
 //  mob->AddWaypoint (wpt3);
 //  mob->AddWaypoint (wpt4);
 //  mob->AddWaypoint (wpt5);
 //  mob->AddWaypoint (wpt6);

//-----------------------Interface and IP Define----------------------------------
	Ipv4AddressHelper address;

	address.SetBase ("192.168.11.0", "255.255.255.0");
	Ipv4InterfaceContainer iSiU = address.Assign (dSdU);

	// address.SetBase ("192.168.35.0", "255.255.255.0");
	// Ipv4InterfaceContainer iR1iR0 = address.Assign (dR1dR0);

	// address.SetBase ("192.168.25.0", "255.255.255.0");
	// Ipv4InterfaceContainer iR0iU = address.Assign (dR0dU);

// 	Ptr<Ipv4> ipv4S = nS->GetObject<Ipv4> ();
// 	// Ptr<Ipv4> ipv4R1 = nR1->GetObject<Ipv4> ();
// 	// Ptr<Ipv4> ipv4R0 = nR0->GetObject<Ipv4> ();
// 	Ptr<Ipv4> ipv4U = nU->GetObject<Ipv4> ();

// //----------------------Route Table Define-------------------------------------
// 	Ipv4StaticRoutingHelper ipv4RoutingHelper;
// 	Ptr<Ipv4StaticRouting> staticRoutingS = ipv4RoutingHelper.GetStaticRouting (ipv4S);
// 	// Ptr<Ipv4StaticRouting> staticRoutingR1 = ipv4RoutingHelper.GetStaticRouting (ipv4R1);
// 	// Ptr<Ipv4StaticRouting> staticRoutingR0 = ipv4RoutingHelper.GetStaticRouting (ipv4R0);
// 	Ptr<Ipv4StaticRouting> staticRoutingU = ipv4RoutingHelper.GetStaticRouting (ipv4U);

//   staticRoutingS->AddNetworkRouteTo (Ipv4Address ("192.168.11.0"), Ipv4Mask ("255.255.255.0"), 1);
//   // staticRoutingS->AddNetworkRouteTo (Ipv4Address ("192.168.35.0"), Ipv4Mask ("255.255.255.0"), Ipv4Address ("192.168.11.2"), 1);
//   // staticRoutingR1->AddNetworkRouteTo (Ipv4Address ("192.168.25.0"), Ipv4Mask ("255.255.255.0"), Ipv4Address ("192.168.35.2"), 2);
//   // staticRoutingR1->AddNetworkRouteTo (Ipv4Address ("192.168.11.0"), Ipv4Mask ("255.255.255.0"), 1);
//   // staticRoutingR0->AddNetworkRouteTo (Ipv4Address ("192.168.11.0"), Ipv4Mask ("255.255.255.0"), Ipv4Address ("192.168.35.1"), 1);
//   // staticRoutingR0->AddNetworkRouteTo (Ipv4Address ("192.168.35.0"), Ipv4Mask ("255.255.255.0"), 1);
//   staticRoutingU->AddNetworkRouteTo (Ipv4Address ("192.168.11.0"), Ipv4Mask ("255.255.255.0"), 1);
//   // staticRoutingU->AddNetworkRouteTo (Ipv4Address ("192.168.35.0"), Ipv4Mask ("255.255.255.0"), Ipv4Address ("192.168.25.1"), 1);

//   // Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  

//-----------------------APP Define----------------------------------------------
	UdpServerHelper Server (9);
	ApplicationContainer serverApps = Server.Install (nS);
	serverApps.Start (Seconds (1.0));
	serverApps.Stop (Seconds (20));

	UdpClientHelper Client (iSiU.GetAddress (0), 9);
	Client.SetAttribute ("MaxPackets", UintegerValue (4294967295u));
	Client.SetAttribute ("Interval", TimeValue (Time (time_interval)));
	Client.SetAttribute ("PacketSize", UintegerValue (payloadSize));
	ApplicationContainer clientApps = Client.Install (nU);
	clientApps.Start (Seconds (2.0));
	clientApps.Stop (Seconds (5.0));
	
	//-----------------------Data Analyse-------------------------------------------
	FlowMonitorHelper flowmon;
	Ptr<FlowMonitor> monitor = flowmon.InstallAll();
	
	Simulator::Stop (Seconds (20));

// GNUplot parameters
	std::string base = "WSOL_2n";
  //Throughput
  std::string throu = base + "ThroughputVSTime";
  std::string graphicsFileName        = throu + ".png";
  std::string plotFileName            = throu + ".plt";
  std::string plotTitle               = "Throughput vs Time";
  std::string dataTitle               = "Throughput";
  Gnuplot gnuplot (graphicsFileName);
  gnuplot.SetTitle (plotTitle);
  gnuplot.SetTerminal ("png");
  gnuplot.SetLegend ("Time (s)", "Throughput (Mbps)");
  Gnuplot2dDataset dataset;
  dataset.SetTitle (dataTitle);
  dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);
  //Delay
  std::string delay = base + "DelayVSTime";
  std::string graphicsFileName1        = delay + ".png";
  std::string plotFileName1            = delay + ".plt";
  std::string plotTitle1               = "Mean Delay vs Time";
  std::string dataTitle1               = "Mean Delay";
  Gnuplot gnuplot1 (graphicsFileName1);
  gnuplot1.SetTitle (plotTitle1);
  gnuplot1.SetTerminal ("png");
  gnuplot1.SetLegend ("Time (s)", "Mean Delay (ms)");
  Gnuplot2dDataset dataset1;
  dataset1.SetTitle (dataTitle1);
  dataset1.SetStyle (Gnuplot2dDataset::LINES_POINTS);
  //LostPackets
  std::string lost = base + "LostPacketsVSTime";
  std::string graphicsFileName2        = lost + ".png";
  std::string plotFileName2            = lost + ".plt";
  std::string plotTitle2               = "LostPackets Ratio vs Time";
  std::string dataTitle2               = "LostPackets Ratio";
  Gnuplot gnuplot2 (graphicsFileName2);
  gnuplot2.SetTitle (plotTitle2);
  gnuplot2.SetTerminal ("png");
  gnuplot2.SetLegend ("Time (s)", "LostPackets Ratio (%)");
  Gnuplot2dDataset dataset2;
  dataset2.SetTitle (dataTitle2);
  dataset2.SetStyle (Gnuplot2dDataset::LINES_POINTS);
  //Jitter
  std::string jitter = base + "JitterVSTime";
  std::string graphicsFileName3        = jitter + ".png";
  std::string plotFileName3            = jitter + ".plt";
  std::string plotTitle3               = "Jitter vs Time";
  std::string dataTitle3               = "Jitter";
  Gnuplot gnuplot3 (graphicsFileName3);
  gnuplot3.SetTitle (plotTitle3);
  gnuplot3.SetTerminal ("png");
  gnuplot3.SetLegend ("Time (s)", "Jitter (ms)");
  Gnuplot2dDataset dataset3;
  dataset3.SetTitle (dataTitle3);
  dataset3.SetStyle (Gnuplot2dDataset::LINES_POINTS);

/*-----------------------------------------------------*/
  // 测吞吐量, 延时, 丢包, 抖动, 最后打印出这些参数
  ThroughputMonitor (&flowmon, monitor, dataset);
  DelayMonitor      (&flowmon, monitor, dataset1);
  LostPacketsMonitor(&flowmon, monitor, dataset2);
  JitterMonitor     (&flowmon, monitor, dataset3);

	if (tracing == false)
	{
		// pointToPoint.EnablePcapAll ("WSOL");
		// phy.EnablePcap ("WSOL", dSdR1.Get (0));
		phy.EnablePcapAll ("WSOL");	//<name>-<node>-<device>.pcap, <node>是在node create时确定的。
		// wifi.EnablePcap ("WSOL", staDevices.Get (0), true);
	}

	Simulator::Run ();

	//Throughput
  gnuplot.AddDataset (dataset);
  std::ofstream plotFile (plotFileName.c_str());
  gnuplot.GenerateOutput (plotFile);
  plotFile.close ();
  //Delay
  gnuplot1.AddDataset (dataset1);
  std::ofstream plotFile1 (plotFileName1.c_str());
  gnuplot1.GenerateOutput (plotFile1);
  plotFile1.close ();
  //LostPackets
  gnuplot2.AddDataset (dataset2);
  std::ofstream plotFile2 (plotFileName2.c_str());
  gnuplot2.GenerateOutput (plotFile2);
  plotFile2.close ();
  //Jitter
  gnuplot3.AddDataset (dataset3);
  std::ofstream plotFile3 (plotFileName3.c_str());
  gnuplot3.GenerateOutput (plotFile3);
  plotFile3.close ();

  monitor->SerializeToXmlFile("WSOL_2n.xml", true, true);
  /* the SerializeToXmlFile () function 2nd and 3rd parameters 
   * are used respectively to activate/deactivate the histograms and the per-probe detailed stats.
   */

	Simulator::Destroy ();
	return 0;
}
