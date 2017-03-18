/* 
Trabalho de Teleinformatica e Redes

- Felipe Lira
- Ingrid Santana Lopes        14/0083065      
- Marcos Paulo Cayres Rosa    14/0027131
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"


// Number of wifi or csma nodes can be increased up to 250
//
//
//  Wifi 10.1.1.0                                                              Wifi 10.1.5.0
//                     AP                                                    AP
//  *    *   *    *     *                                                    *   *    *   *     *
//  |   |    |    |     |    10.1.2.0                           10.1.4.0     |   |    |   |    |
// n0   n1   n2   n3    n4 -------------- n5   n6   n7   n8 ---------------- n9  n10  n11  n12 n13
//                         point-to-point  |    |    |    |  point-to-point     
//                                         ================
//                                          LAN 10.1.3.0
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Trabalho Final de Teleinformatica e Redes 1");

int 
main (int argc, char *argv[]){
  /*definição dos valores do verbose, nCsma, nWifi e tracing com especial atenção a 
  nCsma e nWifi que definirão a quantidade de nodes que existiram em cada rede ethernet
   e wifi respectivamente*/
  bool verbose = true;
  uint32_t nCsma = 4;
  uint32_t nWifi = 4;
  bool tracing = true;

  /*Checagem se o número de nodes de csma e wifi escolhidos são válidos*/
  if (nWifi > 250 || nCsma > 250){
      std::cout << "Too many wifi or csma nodes, no more than 250 each." << std::endl;
      return 1;
  }

  /* determina se os componentes  UdpEchoClientApplication e UdpEchoServerApplication estão ativos*/
  if (verbose){
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
  }

  /* criação dos dois pares de nodes point to point para formar a conecção*/
  NodeContainer p2pNodes1;
  p2pNodes1.Create (2);
  NodeContainer p2pNodes2;
  p2pNodes2.Create (2);

/* cria um transmissor de 5 Mbps usando o helper tendo delay de 2ms nos canais criados*/

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute (".;", StringValue ("2ms"));

  /* ativando o tracing*/
  AsciiTraceHelper ascii;
  pointToPoint.EnableAsciiAll(ascii.CreateFileStream("trabf.tr"));
  pointToPoint.EnablePcapAll("trabf");

  NetDeviceContainer p2pDevices1;
  p2pDevices1 = pointToPoint.Install (p2pNodes1);

  NetDeviceContainer p2pDevices2;
  p2pDevices2 = pointToPoint.Install (p2pNodes2);

  /* conterá todos os nós para a rede ethernet definida no tipo de topoligia de bus network (CSMA)*/
  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes1.Get (1));
  csmaNodes.Add (p2pNodes2.Get (1));
  csmaNodes.Create (nCsma);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);

  /*criação dos nós que farão parte da rede wifi*/
  NodeContainer wifiStaNodes1;
  wifiStaNodes1.Create (nWifi);
  NodeContainer wifiApNode1 = p2pNodes1.Get (0);

  NodeContainer wifiStaNodes2;
  wifiStaNodes2.Create (nWifi);
  NodeContainer wifiApNode2 = p2pNodes2.Get (0);

  /* construções dos canais de intercomunicação entre os nós wifi*/
  YansWifiChannelHelper channel1 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy1 = YansWifiPhyHelper::Default ();
  phy1.SetChannel (channel1.Create ());

  YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy2 = YansWifiPhyHelper::Default ();
  phy2.SetChannel (channel2.Create ());

  /* definição dos parametros MAC e configuração da Wifi para todos os nós STA*/
  WifiHelper wifi1;
  wifi1.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiHelper wifi2;
  wifi2.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac1;
  Ssid ssid1 = Ssid ("ns-3-ssid");
  mac1.SetType ("ns3::StaWifiMac","Ssid", SsidValue (ssid1),"ActiveProbing", BooleanValue (false));

  WifiMacHelper mac2;
  Ssid ssid2 = Ssid ("ns-3-ssid");
  mac2.SetType ("ns3::StaWifiMac","Ssid", SsidValue (ssid2),"ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices1;
  staDevices1 = wifi1.Install (phy1, mac1, wifiStaNodes1);

  NetDeviceContainer staDevices2;
  staDevices2 = wifi2.Install (phy2, mac2, wifiStaNodes2);
 
/* configuração do AP (acces point = ponto de acesso)*/
  mac1.SetType ("ns3::ApWifiMac","Ssid", SsidValue (ssid1));

  mac2.SetType ("ns3::ApWifiMac","Ssid", SsidValue (ssid2));

  NetDeviceContainer apDevices1;
  apDevices1 = wifi1.Install (phy1, mac1, wifiApNode1);

  NetDeviceContainer apDevices2;
  apDevices2 = wifi2.Install (phy2, mac2, wifiApNode2);

  MobilityHelper mobility1;
  MobilityHelper mobility2;

  mobility1.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility2.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility1.SetMobilityModel ("ns3::RandomWalk2dMobilityModel", "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility1.Install (wifiStaNodes1);
  mobility2.SetMobilityModel ("ns3::RandomWalk2dMobilityModel", "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility2.Install (wifiStaNodes2);

  mobility1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility1.Install (wifiApNode1);
  mobility2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility2.Install (wifiApNode2);

/* uso do InternetStackHelper para instalar os protocolos seguindo o modelo de
 duas wireless, uma ethernet e duas coneções point to point*/
  InternetStackHelper stack;
  stack.Install (csmaNodes);
  stack.Install (wifiApNode1);
  stack.Install (wifiStaNodes1);
  stack.Install (wifiApNode2);
  stack.Install (wifiStaNodes2);

  Ipv4AddressHelper address; 

  /* definindo o endereço de IP de cada interface*/

  address.SetBase ("10.1.1.0", "255.255.255.0");
  address.Assign (staDevices1);
  address.Assign (apDevices1);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces1;
  p2pInterfaces1 = address.Assign (p2pDevices1);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

  address.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2;
  p2pInterfaces2 = address.Assign (p2pDevices2);

  address.SetBase ("10.1.5.0", "255.255.255.0");
  address.Assign (staDevices2);
  address.Assign (apDevices2);

  /* definição da porta do servidor*/
  UdpEchoServerHelper echoServer (9);

  /* definindo e instalando o servidor em algum nó csma*/
  ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (2));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (35.0));

  /* definindo caracteristicas de pacotes enviados pelo cliente*/
  UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (2), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (15));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (2.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (2048));

  /* definindo e instalnado os clientes em algum nó de wifi*/
  ApplicationContainer clientApps = echoClient.Install (wifiStaNodes1.Get (nWifi-1));
  clientApps.Start (Seconds (2.0));  /* inicio em 2 segundos para não dar conflito com o 1 segundo do servidor*/
  clientApps.Stop (Seconds (35.0));

  ApplicationContainer clientApps1 = echoClient.Install (wifiStaNodes2.Get (nWifi-1));
  clientApps1.Start (Seconds (2.0));
  clientApps1.Stop (Seconds (35.0));

  /* ativação do roteamento internetwork*/
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Stop (Seconds (35.0));

  if (tracing == true)
    {
      pointToPoint.EnablePcapAll ("trabf");
      for (uint32_t i=0; i< apDevices1.GetN();++i)
          phy1.EnablePcap ("trabf", apDevices1.Get (i));
      for (uint32_t i=0; i< apDevices2.GetN();++i)
        phy2.EnablePcap ("trabf", apDevices2.Get (i));
      for (uint32_t i=0; i< csmaDevices.GetN();++i)
        csma.EnablePcap ("trabf", csmaDevices.Get (i), true);
    }

    /* animação do netanim*/
/*
    AnimationInterface anim ("trabf.xml");

    for (uint32_t i=0; i< wifiStaNodes1.GetN();++i){
      anim.UpdateNodeDescription(wifiStaNodes1.Get(i),"STA1");
      anim.UpdateNodeColor(wifiStaNodes1.Get(i), 255, 0, 0);
    }
    for (uint32_t i=0; i< wifiApNode1.GetN();++i){
      anim.UpdateNodeDescription(wifiApNode1.Get(i),"AP1");
      anim.UpdateNodeColor(wifiApNode1.Get(i), 0, 255, 0);
    }

    for (uint32_t i=0; i< wifiStaNodes2.GetN();++i){
      anim.UpdateNodeDescription(wifiStaNodes2.Get(i),"STA2");
      anim.UpdateNodeColor(wifiStaNodes2.Get(i), 0, 0, 255);
    }
    for (uint32_t i=0; i< wifiApNode2.GetN();++i){
      anim.UpdateNodeDescription(wifiApNode2.Get(i),"AP2");
      anim.UpdateNodeColor(wifiApNode2.Get(i), 255, 200, 0);
    }
    for (uint32_t i=0; i< csmaNodes.GetN();++i){
      anim.UpdateNodeDescription(csmaNodes.Get(i),"CSMA");
      anim.UpdateNodeColor(csmaNodes.Get(i), 200, 0, 250);
    }

    anim.EnablePacketMetadata();
    anim.EnableIpv4RouteTracking("routingrable-wireless.xml", Seconds(0), Seconds(5), Seconds(0.25));
    anim.EnableWifiMacCounters(Seconds(0), Seconds(10));
    anim.EnableWifiPhyCounters(Seconds(0), Seconds(10));*/






/* inicio da execução do simulador e então saída do programa*/
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}