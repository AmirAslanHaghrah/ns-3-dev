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
#include <ns3/buildings-module.h>
#include "ns3/lte-spectrum-value-helper.h"
#include "ns3/channel-condition-model.h"
#include "ns3/three-gpp-propagation-loss-model.h"
#include "ns3/three-gpp-spectrum-propagation-loss-model.h"

#include <time.h>
#include <sys/stat.h>
#include <iostream>
#include <string>

#ifdef _DEBUG
    #undef _DEBUG
    #include <python.h>
    #define _DEBUG
#else
    #include <Python.h>    
#endif

PyObject* pName, * pModule, * pDict, * pClass, * pInstance;
int handoverType = 0;

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FuzzyType2Handover");

std::ofstream osConnectionEstablishedEnb;
std::ofstream osHandoverStartEnb;
std::ofstream osHandoverEndOkEnb;
std::ofstream osConnectionEstablishedUe;
std::ofstream osHandoverStartUe;
std::ofstream osHandoverEndOkUe;
std::ofstream osRadioLinkFailure;

void
NotifyConnectionEstablishedUe (std::string context,
                               uint64_t imsi,
                               uint16_t cellid,
                               uint16_t rnti)
{
  std::cout << "Time:" << Simulator::Now()
            // << context
            << " UE IMSI " << imsi
            << ": connected to CellId " << cellid
            << " with RNTI " << rnti
            << std::endl;

  osConnectionEstablishedUe << Simulator::Now() << "," << imsi << "," << cellid << "," << rnti << "," << context << std::endl;
}

void
NotifyHandoverStartUe (std::string context,
                       uint64_t imsi,
                       uint16_t cellid,
                       uint16_t rnti,
                       uint16_t targetCellId)
{
  std::cout << "Time:" << Simulator::Now()
            // << context
            << " UE IMSI " << imsi
            << ": previously connected to CellId " << cellid
            << " with RNTI " << rnti
            << ", doing handover to CellId " << targetCellId
            << std::endl;

  osHandoverStartUe << Simulator::Now() << "," << imsi << "," << cellid << "," << rnti << "," << targetCellId << "," << context << std::endl;
}

void
NotifyHandoverEndOkUe (std::string context,
                       uint64_t imsi,
                       uint16_t cellid,
                       uint16_t rnti)
{
  std::cout << "Time:" << Simulator::Now()
            // << context
            << " UE IMSI " << imsi
            << ": successful handover to CellId " << cellid
            << " with RNTI " << rnti
            << std::endl;
  
  osHandoverEndOkUe << Simulator::Now() << "," << imsi << "," << cellid << "," << rnti << "," << context << std::endl;
}

void
NotifyConnectionEstablishedEnb (std::string context,
                                uint64_t imsi,
                                uint16_t cellid,
                                uint16_t rnti)
{
  std::cout << "Time:" << Simulator::Now()
            // << context
            << " eNB CellId " << cellid
            << ": successful connection of UE with IMSI " << imsi
            << " RNTI " << rnti
            << std::endl;
  if (handoverType == 3 || handoverType == 4)
  {
    PyObject_CallMethod ( pInstance, "UpdateServingCellId", "(diii)", 
                          Simulator::Now ().GetSeconds(), 
                          imsi, 
                          cellid, 
                          rnti);
  }
  osConnectionEstablishedEnb << Simulator::Now() << "," << cellid << "," << imsi << "," << rnti << "," << context  << std::endl;
}

void
NotifyHandoverStartEnb (std::string context,
                        uint64_t imsi,
                        uint16_t cellid,
                        uint16_t rnti,
                        uint16_t targetCellId)
{
  std::cout << "Time:" << Simulator::Now()
            // << context
            << " eNB CellId " << cellid
            << ": start handover of UE with IMSI " << imsi
            << " RNTI " << rnti
            << " to CellId " << targetCellId
            << std::endl;

  osHandoverStartEnb << Simulator::Now() << "," << cellid << "," << imsi << "," << rnti << "," << targetCellId << "," << context << std::endl;
}

void
NotifyHandoverEndOkEnb (std::string context,
                        uint64_t imsi,
                        uint16_t cellid,
                        uint16_t rnti)
{
  std::cout << "Time:" << Simulator::Now()
            // << context
            << " eNB CellId " << cellid
            << ": completed handover of UE with IMSI " << imsi
            << " RNTI " << rnti
            << std::endl;

  if (handoverType == 3 || handoverType == 4)
  {
    PyObject_CallMethod ( pInstance, "UpdateServingCellId", "(diii)", 
                          Simulator::Now ().GetSeconds(), 
                          imsi, 
                          cellid, 
                          rnti);
  }
  osHandoverEndOkEnb << Simulator::Now() << "," << cellid << "," << imsi << "," << rnti << "," << context << std::endl;
}


void
NotifyRadioLinkFailure (std::string context,
                        uint64_t imsi,
                        uint16_t cellid,
                        uint16_t rnti)
{     
  
  std::cout << "Time:" << Simulator::Now()
            // << context
            << " UE IMSI " << imsi
            << ": radio link fail to CellId " << cellid
            << " with RNTI " << rnti            
            << std::endl;  

  osRadioLinkFailure << Simulator::Now() << "," << imsi << "," << cellid << "," << rnti << "," << context << std::endl;
}

void 
SaveUePosition (NodeContainer nodes,
                std::ostream *os, 
                double interval)
{
    for (NodeContainer::Iterator node = nodes.Begin (); node != nodes.End (); ++node)
    {
        uint32_t nodeId = (*node)->GetId();
        uint64_t nodeImsi = (Ptr<NetDevice>((*node)->GetDevice(0)))->GetObject<LteUeNetDevice> ()->GetImsi ();
        Ptr<MobilityModel> mobModel = (*node)->GetObject<MobilityModel>();
        Vector3D pos = mobModel->GetPosition();
        Vector3D vel = mobModel->GetVelocity();

        // Prints position and velocities
        *os << Simulator::Now().GetSeconds() << ","
            << nodeId << ","
            << nodeImsi << ","
            << pos.x << ":" << pos.y << ":" << pos.z << "," 
            << vel.x << ":" << vel.y << ":" << vel.z << "\n";
    }
    Simulator::Schedule(Seconds(interval), &SaveUePosition, nodes, os, interval);
}

void 
SaveEnbInitialPosition (NodeContainer nodes, 
                        std::string logFile)
{
    // open log file for output
    std::ofstream os;
    os.open (logFile.c_str ());

    // Prints position and velocities
    os << "<TIME>," << "<ID>," << "<CELLID>," <<"<POSITION>," << "<VELOCITY>" << "\n";
    for (NodeContainer::Iterator node = nodes.Begin (); node != nodes.End (); ++node)
    {              
        uint64_t nodeId = (*node)->GetId();
        uint64_t nodeCellId = (Ptr<NetDevice>((*node)->GetDevice(0)))->GetObject<LteEnbNetDevice> ()->GetCellId ();
        Ptr<MobilityModel> mobModel = (*node)->GetObject<MobilityModel>();
        Vector3D pos = mobModel->GetPosition();
        Vector3D vel = mobModel->GetVelocity();

        // Prints position and velocities
        os  << Simulator::Now().GetSeconds() << ","
            << nodeId << ","            
            << nodeCellId << "," 
            << pos.x << ":" << pos.y << ":" << pos.z << "," 
            << vel.x << ":" << vel.y << ":" << vel.z << "\n";
    }
}

void 
SaveUeInitialPosition ( NodeContainer nodes, 
                        std::string logFile)
{
    // open log file for output
    std::ofstream os;
    os.open (logFile.c_str ());

    // Prints position and velocities
    os << "<TIME>," << "<ID>," << "<IMSI>," <<"<POSITION>," << "<VELOCITY>" << "\n";
    for (NodeContainer::Iterator node = nodes.Begin (); node != nodes.End (); ++node)
    {              
        uint64_t nodeId = (*node)->GetId();        
        uint64_t nodeImsi = (Ptr<NetDevice>((*node)->GetDevice(0)))->GetObject<LteUeNetDevice> ()->GetImsi ();
        Ptr<MobilityModel> mobModel = (*node)->GetObject<MobilityModel>();
        Vector3D pos = mobModel->GetPosition();
        Vector3D vel = mobModel->GetVelocity();

        // Prints position and velocities
        os  << Simulator::Now().GetSeconds() << ","
            << nodeId << ","
            << nodeImsi << ","
            << pos.x << ":" << pos.y << ":" << pos.z << "," 
            << vel.x << ":" << vel.y << ":" << vel.z << "\n";
    }
}



int
main (int argc, char *argv[])
{  
  CommandLine cmd;
  cmd.Parse (argc, argv);

  srand (time(NULL));
  // uint64_t seed = rand () % 10000;
  int seed;
  seed = std::atoi(argv[1]);
  std::cout << "Simulation random seed: " << seed << std::endl;
  RngSeedManager::SetSeed (seed + 1);  
  
      handoverType = std::atoi(argv[2]);
  int speed = std::atoi(argv[3]);
  int speedMin = speed - 5;
  int speedMax = speed + 5;
 
  if (handoverType == 3 || handoverType == 4)
  {
    setenv("PYTHONPATH", "/mnt/data/Ph.D./ns-3/ns-3-dev/src/lte/model/fuzzy/", 1);

    Py_Initialize();

    if (handoverType == 3)
    {
      pName = PyUnicode_FromString("type1fuzzy");
    }
    else if (handoverType == 4)
    {
      pName = PyUnicode_FromString("type2fuzzy");
    }
    pModule = PyImport_Import(pName);
    pDict = PyModule_GetDict(pModule);

    // Build the name of a callable class 
    pClass = PyDict_GetItemString(pDict, "FuzzyHandover");

    // Create an instance of the class
    if (PyCallable_Check(pClass)) {
        pInstance = PyObject_CallObject(pClass, NULL);
    }
  }

  char path[256] = "output/";
  char* speedStr = new char[std::to_string(speed).length() + 1];
  strcpy(speedStr, std::to_string(speed).c_str());
  std::strcat(path, speedStr);  
  if (-1 == mkdir(path , S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
    {
      if (errno == EEXIST) 
        {
          // alredy exists
          std::cout << "Directory is alredy exists" << std::endl;
        }
      else
        {
          // something else
          std::cout << "cannot create directory, error:" << strerror(errno) << std::endl;
          throw std::runtime_error( strerror(errno) );
        }
    }
  
  std::strcat(path, "/");
  char* handoverTypeStr = new char[std::to_string(handoverType).length() + 1];
  strcpy(handoverTypeStr, std::to_string(handoverType).c_str());
  std::strcat(path, handoverTypeStr);
  if (-1 == mkdir(path , S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
    {
      if (errno == EEXIST) 
        {
          // alredy exists
          std::cout << "Directory is alredy exists" << std::endl;
        }
      else
        {
          // something else
          std::cout << "cannot create directory, error:" << strerror(errno) << std::endl;
          throw std::runtime_error( strerror(errno) );
        }
    }

  std::strcat(path, "/");
  char* seedStr = new char[std::to_string(seed).length() + 1];
  strcpy(seedStr, std::to_string(seed).c_str());
  std::strcat(path, seedStr);
  std::cout << path << std::endl;   
  if (-1 == mkdir(path , S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
  {
      if (errno == EEXIST) 
        {
          // alredy exists
          std::cout << "Directory is alredy exists" << std::endl;
        }
      else
        {
          // something else
          std::cout << "cannot create directory, error:" << strerror(errno) << std::endl;
          throw std::runtime_error( strerror(errno) );
        }
  }

  uint16_t numberOfUes = 1;  
  uint16_t numberOfScenbs = 100;
  uint16_t numBearersPerUe = 1;
  double simTime = 100;
  double scenbTxPowerDbm = 20.0;

  bool generateRem = false;

  Config::SetDefault ("ns3::PhyTxStatsCalculator::DlTxOutputFilename", StringValue ("output/DlTxPhyStats.txt"));
  Config::SetDefault ("ns3::PhyTxStatsCalculator::UlTxOutputFilename", StringValue ("output/UlTxPhyStats.txt"));

  Config::SetDefault ("ns3::PhyStatsCalculator::DlRsrpSinrFilename", StringValue ("output/DlRsrpSinrStats.txt"));
  Config::SetDefault ("ns3::PhyStatsCalculator::UlSinrFilename", StringValue ("output/UlSinrStats.txt"));
  Config::SetDefault ("ns3::PhyStatsCalculator::UlInterferenceFilename", StringValue ("output/UlInterferenceStats.txt"));

  Config::SetDefault ("ns3::MacStatsCalculator::DlOutputFilename", StringValue ("output/DlMacStats.txt"));
  Config::SetDefault ("ns3::MacStatsCalculator::UlOutputFilename", StringValue ("output/UlMacStats.txt"));

  Config::SetDefault ("ns3::RadioBearerStatsCalculator::DlRlcOutputFilename", StringValue ("output/DlRlcStats.txt"));
  Config::SetDefault ("ns3::RadioBearerStatsCalculator::UlRlcOutputFilename", StringValue ("output/UlRlcStats.txt"));
  Config::SetDefault ("ns3::RadioBearerStatsCalculator::DlPdcpOutputFilename", StringValue ("output/DlPdcpStats.txt"));
  Config::SetDefault ("ns3::RadioBearerStatsCalculator::UlPdcpOutputFilename", StringValue ("output/UlPdcpStats.txt"));

  Config::SetDefault ("ns3::UdpClient::Interval", TimeValue (MilliSeconds (10)));
  Config::SetDefault ("ns3::UdpClient::MaxPackets", UintegerValue (1000000));
  
  // change some default attributes so that they are reasonable for
  // this scenario, but do this before processing command line
  // arguments, so that the user is allowed to override these settings 
  Config::SetDefault ("ns3::UdpClient::Interval", TimeValue (MilliSeconds (1)));
  Config::SetDefault ("ns3::UdpClient::MaxPackets", UintegerValue (1000000));
  Config::SetDefault ("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue (10 * 1024));
  

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);

  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::HybridBuildingsPropagationLossModel"));
  // lteHelper->SetPathlossModelAttribute ("ShadowSigmaExtWalls", DoubleValue (0.0));
  // lteHelper->SetPathlossModelAttribute ("ShadowSigmaOutdoor", DoubleValue (7.0));
  // lteHelper->SetPathlossModelAttribute ("ShadowSigmaIndoor", DoubleValue (0.0));
  // lteHelper->SetPathlossModelAttribute ("InternalWallLoss", DoubleValue (0.0));	    
  // lteHelper->SetPathlossModelAttribute ("Los2NlosThr", DoubleValue (2e2));

  lteHelper->SetSpectrumChannelType ("ns3::MultiModelSpectrumChannel");


  // lteHelper->SetAttribute ("FadingModel", StringValue ("ns3::TraceFadingLossModel"));  
  // std::ifstream ifTraceFile;
  // ifTraceFile.open ("../../src/lte/model/fading-traces/fading_trace_EVA_60kmph.fad", std::ifstream::in);
  // if (ifTraceFile.good ())
  //   {
  //     // script launched by test.py
  //     lteHelper->SetFadingModelAttribute ("TraceFilename", StringValue ("../../src/lte/model/fading-traces/fading_trace_EVA_60kmph.fad"));
  //   }
  // else
  //   {
  //     // script launched as an example
  //     lteHelper->SetFadingModelAttribute ("TraceFilename", StringValue ("src/lte/model/fading-traces/fading_trace_EVA_60kmph.fad"));
  //   }


  Config::SetDefault ("ns3::LteHelper::UseIdealRrc", BooleanValue (true));
  Config::SetDefault ("ns3::LteSpectrumPhy::CtrlErrorModelEnabled", BooleanValue (false));
  Config::SetDefault ("ns3::LteSpectrumPhy::DataErrorModelEnabled", BooleanValue (false));  

  Config::SetDefault ("ns3::LteUeRrc::T310", TimeValue (MilliSeconds (0)));
  Config::SetDefault ("ns3::LteUeRrc::N310", UintegerValue (1));
  Config::SetDefault ("ns3::LteUeRrc::N311", UintegerValue (1));
  // Config::SetDefault ("ns3::LteUePhy::Qout", DoubleValue (-5.0));
  // Config::SetDefault ("ns3::LteUePhy::Qin", DoubleValue (-3.8));


  // ---- MAC Scheduler ----
  lteHelper->SetSchedulerType ("ns3::PfFfMacScheduler");

  // ---- Handover Algorithm ----
  if (handoverType == 0)
  {
    lteHelper->SetHandoverAlgorithmType ("ns3::NoOpHandoverAlgorithm"); // disable automatic handover
  }
  else if (handoverType == 1)
    {
      lteHelper->SetHandoverAlgorithmType ("ns3::A3RsrqHandoverAlgorithm");
      lteHelper->SetHandoverAlgorithmAttribute ("Hysteresis",
                                                DoubleValue (1.0));
      lteHelper->SetHandoverAlgorithmAttribute ("TimeToTrigger",
                                                TimeValue (MilliSeconds (64)));
    }
  else if (handoverType == 2)
    {
      lteHelper->SetHandoverAlgorithmType ("ns3::A2A4RsrqHandoverAlgorithm");
      lteHelper->SetHandoverAlgorithmAttribute ("ServingCellThreshold",
                                                UintegerValue (30));
      lteHelper->SetHandoverAlgorithmAttribute ("NeighbourCellOffset",
                                                UintegerValue (1));
    }
  else if (handoverType == 3)
    {
      lteHelper->SetHandoverAlgorithmType ("ns3::Type1FuzzyHandoverAlgorithm");
    }
  else if (handoverType == 4)
    {
      lteHelper->SetHandoverAlgorithmType ("ns3::Type2FuzzyHandoverAlgorithm");
    }

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

  NodeContainer scenbNodes;
  NodeContainer ueNodes;

  scenbNodes.Create (numberOfScenbs);
  ueNodes.Create (numberOfUes);

  MobilityHelper scenbMobility;
  Ptr<ListPositionAllocator> scenbPositionAlloc = CreateObject<ListPositionAllocator> ();

  // Small Cell eNBs
  double distance = 100.0;
  Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
  x->SetAttribute ("Min", DoubleValue (20));
  x->SetAttribute ("Max", DoubleValue (80));

  for (uint16_t i = 0; i < 10; i++)
    {
      for (uint16_t j = 0; j < 10; j++)
        {          
          Vector scenbPosition (distance * i + x->GetInteger() , distance * j + x->GetInteger(), 30.0);
          scenbPositionAlloc->Add (scenbPosition);
        }
    }

  scenbMobility.SetPositionAllocator (scenbPositionAlloc);
  scenbMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  scenbMobility.Install (scenbNodes);
  BuildingsHelper::Install (scenbNodes);

  // // Install Mobility Model in SCeNB
  // MobilityHelper scenbmobility;
  // scenbmobility.SetPositionAllocator ("ns3::RandomRectanglePositionAllocator",
  //                                     "X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1000.0]"),
  //                                     "Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1000.0]"),
  //                                     "Z", StringValue ("30.0"));
  // scenbmobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  // scenbmobility.Install (scenbNodes);


  ObjectFactory pos;
  pos.SetTypeId ("ns3::RandomRectanglePositionAllocator");

  pos.Set ("X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1000.0]"));
  pos.Set ("Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1000.0]"));
  pos.Set ("Z", StringValue ("1.5"));
  Ptr<PositionAllocator> uePositionAlloc = pos.Create ()->GetObject<PositionAllocator> ();
  
  // auto _p1 = strcat("ns3::UniformRandomVariable[Min=", speedMin);
  // auto _p2 = strcat(_p1, "|Max=");
  // auto _p3 = strcat(_p2, speedMax);
  // auto _p4 = strcat(_p3, "]");
  
  //"ns3::UniformRandomVariable[Min=70|Max=80]"

  std::stringstream ssSpeed;
  ssSpeed << "ns3::UniformRandomVariable[Min=" << speedMin << "|Max=" << speedMax << "]";

  MobilityHelper uemobility;
  uemobility.SetPositionAllocator (uePositionAlloc);
  uemobility.SetMobilityModel ("ns3::RandomWaypointMobilityModel",
                                "Speed", StringValue(ssSpeed.str()),
                                "Pause", StringValue("ns3::ConstantRandomVariable[Constant=0.00]"),
                                "PositionAllocator", PointerValue(uePositionAlloc));

  uemobility.Install (ueNodes);
  BuildingsHelper::Install (ueNodes);

  // Install LTE Devices in eNB and UEs
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (scenbTxPowerDbm));  
  NetDeviceContainer scenbLteDevs = lteHelper->InstallEnbDevice (scenbNodes);

  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

  // Install the IP stack on the UEs
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIfaces;
  ueIpIfaces = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));

  lteHelper->AttachToClosestEnb(ueLteDevs, scenbLteDevs);


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

  // Add X2 interface
  lteHelper->AddX2Interface (scenbNodes);
  
 
  // lteHelper->EnablePdcpTraces ();
  // Ptr<RadioBearerStatsCalculator> pdcpStats = lteHelper->GetPdcpStats ();
  // pdcpStats->SetAttribute ("EpochDuration", TimeValue (Seconds (1.0)));

  // connect custom trace sinks for RRC connection establishment and handover notification
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionEstablished", MakeCallback (&NotifyConnectionEstablishedEnb));
  Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionEstablished", MakeCallback (&NotifyConnectionEstablishedUe));
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverStart", MakeCallback (&NotifyHandoverStartEnb));
  Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/HandoverStart", MakeCallback (&NotifyHandoverStartUe));
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverEndOk", MakeCallback (&NotifyHandoverEndOkEnb));
  Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/HandoverEndOk", MakeCallback (&NotifyHandoverEndOkUe)); 
  Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/RadioLinkFailure", MakeCallback (&NotifyRadioLinkFailure));

  char *_path = new char[256];
  strcpy(_path, path);  
  Simulator::Schedule(Seconds(0.00), &SaveEnbInitialPosition, scenbNodes, strcat(_path, "/init-scenb-mob.log"));
  _path = new char[256];
  strcpy(_path, path);  
  Simulator::Schedule(Seconds(0.00), &SaveUeInitialPosition, ueNodes, strcat(_path, "/init-ue-mob.log"));

  _path = new char[256];
  strcpy(_path, path);
  std::string logFile = strcat(_path, "/ue-mob.log");
  // open log file for output
  std::ofstream os;
  os.open (logFile.c_str ());
  // Prints position and velocities
  os << "<TIME>," << "<ID>," << "<IMSI>," <<"<POSITION>," << "<VELOCITY>" << std::endl;
  Simulator::Schedule(Seconds(0.00), &SaveUePosition, ueNodes, &os, 0.01);

  _path = new char[256];
  strcpy(_path, path);
  std::string connectionEstablishedEnbLogFile = strcat(_path, "/connection-established-enb.log");
  _path = new char[256];
  strcpy(_path, path);
  std::string connectionEstablishedUeLogFile = strcat(_path, "/connection-established-ue.log");
  _path = new char[256];
  strcpy(_path, path);
  std::string handoverStartEnbLogFile = strcat(_path, "/handover-start-enb.log");
  _path = new char[256];
  strcpy(_path, path);
  std::string handoverStartUeLogFile = strcat(_path, "/handover-start-ue.log");
  _path = new char[256];
  strcpy(_path, path);
  std::string handoverEndOkEnbLogFile = strcat(_path, "/handover-end-ok-enb.log");
  _path = new char[256];
  strcpy(_path, path);
  std::string handoverEndOkUeLogFile = strcat(_path, "/handover-end-ok-ue.log");
  _path = new char[256];
  strcpy(_path, path);
  std::string radioLinkFailureLogFile = strcat(_path, "/radio-link-failure.log");

  osConnectionEstablishedEnb.open(connectionEstablishedEnbLogFile.c_str ());
  osConnectionEstablishedUe.open(connectionEstablishedUeLogFile.c_str ());
  osHandoverStartEnb.open(handoverStartEnbLogFile.c_str ());
  osHandoverStartUe.open(handoverStartUeLogFile.c_str ());
  osHandoverEndOkEnb.open(handoverEndOkEnbLogFile.c_str ());
  osHandoverEndOkUe.open(handoverEndOkUeLogFile.c_str ());
  osRadioLinkFailure.open(radioLinkFailureLogFile.c_str ());

  osConnectionEstablishedEnb << "<TIME>," << "<CELLID>," << "<IMSI>," << "<RNTI>," << "<CONTEXT>" <<  std::endl;
  osConnectionEstablishedUe << "<TIME>,"  << "<IMSI>," << "<CELLID>," << "<RNTI>," << "<CONTEXT>" << std::endl;
  osHandoverStartEnb << "<TIME>," << "<CELLID>," << "<IMSI>," << "<RNTI>," << "<TARGETCELLID>," << "<CONTEXT>" << std::endl;
  osHandoverStartUe << "<TIME>," << "<IMSI>," << "<CELLID>," << "<RNTI>," << "<TARGETCELLID>," << "<CONTEXT>" << std::endl;
  osHandoverEndOkEnb << "<TIME>,"  << "<CELLID>," << "<IMSI>," << "<RNTI>," << "<CONTEXT>" << std::endl;
  osHandoverEndOkUe << "<TIME>," << "<IMSI>," << "<CELLID>," << "<RNTI>," << "<CONTEXT>" << std::endl;
  osRadioLinkFailure << "<TIME>," << "<IMSI>," << "<CELLID>," << "<RNTI>," << "<CONTEXT>" << std::endl;

  _path = new char[256];
  strcpy(_path, path);
  AsciiTraceHelper ascii;
  MobilityHelper::EnableAsciiAll (ascii.CreateFileStream (strcat(_path, "/mobility-trace.mob")));

  Ptr<LteSpectrumPhy> enbDlSpectrumPhy = scenbLteDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetPhy ()->GetDownlinkSpectrumPhy ()->GetObject<LteSpectrumPhy> ();
  Ptr<SpectrumChannel> dlChannel = enbDlSpectrumPhy->GetChannel ();
  uint32_t dlChannelId = dlChannel->GetId ();
  std::cout << "DL ChannelId: " << dlChannelId << std::endl;

  Ptr<RadioEnvironmentMapHelper> remHelper;
  if (generateRem)
    {
      _path = new char[256];
      strcpy(_path, path);
      remHelper = CreateObject<RadioEnvironmentMapHelper> ();
      remHelper->SetAttribute ("ChannelPath", StringValue ("/ChannelList/3"));
      remHelper->SetAttribute ("OutputFile", StringValue (strcat(_path, "/fuzzyRem.rem")));
			remHelper->SetAttribute ("XMin", DoubleValue (0.0));
      remHelper->SetAttribute ("XMax", DoubleValue (+1000.0));
      remHelper->SetAttribute ("YMin", DoubleValue (0.0));
      remHelper->SetAttribute ("YMax", DoubleValue (+1000.0));

			remHelper->SetAttribute ("XRes", UintegerValue (200));
      remHelper->SetAttribute ("YRes", UintegerValue (200));

      remHelper->SetAttribute ("Z", DoubleValue (1.5));

      remHelper->Install ();
      // simulation will stop right after the REM has been generated
    }
  else
    {
      Simulator::Stop (Seconds (simTime));
    }

  Simulator::Run ();
  Simulator::Destroy ();
  if (handoverType == 3 || handoverType == 4)
  {
    // Clean up
    Py_DECREF(pName);
    Py_Finalize();
  }
  return 0;
}
