#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "string"

using namespace ns3;
using namespace std;


class MyApp : public Application
{
public:

  MyApp ();
  virtual ~MyApp();

  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, Ptr<myFlowManager> flowManager, double frameRate);
  void SendPacket ();
  void SetPacketSize (uint32_t packetSize);
  uint32_t GetPacketSize ();
  void printsomething (string message);
  void updateSendRest (uint32_t size);
  void updateFrameSizeList (list<uint32_t> list);
  void updateTimeBuffer (double buffer);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_sendRest;
  EventId         m_sendEvent;
  bool            m_running;
  Ptr<myFlowManager> m_flowManager;
  list<uint32_t>  m_frameList;
  double          m_frameRate;
  double          m_timeBuffer;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_sendRest (0),
    m_sendEvent (),
    m_running (false),
    m_frameList (0),
    m_frameRate (0),
    m_timeBuffer (0)
{
}

MyApp::~MyApp()
{
  m_socket = 0;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, Ptr<myFlowManager> flowManager, double frameRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_sendRest = 0;
  m_flowManager = flowManager;
  m_frameRate = frameRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket();
  ScheduleTx();
}

void
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void
MyApp::SendPacket ()
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  if ((m_packetSize > 0) && m_flowManager->getTransmission()) {
    //double limit =  m_flowManager->getStopTransmission();
    //double buffer = m_timeBuffer;
    m_socket->Send (packet);
    updateSendRest(m_packetSize);

    // send only packets if still space in buffer
    // if (buffer < limit) {
      
    //   // send only packets if not already enough sent
    //   uint32_t rest = m_sendRest;
    //   list<uint32_t>  flist = m_frameList;
    //   while (rest >= 0) {
    //     if (rest > flist.front()) {
    //       rest -= flist.front();
    //       flist.pop_front();
    //       buffer += 1/m_frameRate;
    //     } else {
    //       break;
    //     }
    //   }
    //   if (buffer < 2*limit) {
    //     m_socket->Send (packet);
    //     updateSendRest(m_packetSize);
    //   }
    // }
  }
  ScheduleTx();
}

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (1/m_frameRate));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

void
MyApp::printsomething (string message)
{
  cout << "test source\t" << message << "\n";
}

void
MyApp::SetPacketSize (uint32_t packetSize)
{
  m_packetSize = packetSize;
}

uint32_t
MyApp::GetPacketSize ()
{
  return m_packetSize;
}

void
MyApp::updateSendRest (uint32_t size)
{
  m_sendRest += size;
}

void
MyApp::updateFrameSizeList (list<uint32_t> list)
{
  m_frameList = list;
}

void
MyApp::updateTimeBuffer (double buffer)
{
  m_timeBuffer = buffer;
}
