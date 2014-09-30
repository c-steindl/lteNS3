#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "string"
#include "myVideoBuffer.h"

using namespace ns3;
using namespace std;


class myBufferApp : public Application
{
public:

  myBufferApp ();
  virtual ~myBufferApp ();

  void Setup (Ptr<MyApp> app, uint32_t frameSize, double frameRate, Ptr<myVideoBuffer> vidBuffer, Ptr<myFlowManager> flowManager, string pathframelist);
  void mySinkRx (uint32_t receivedBytes, double timestamp);
  void printsomething (string message);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);
  void UpdateBuffer (void);
  void readConfigFiles (void);
  void ScheduleBandwidth(string value);
  void ScheduleDelay(string value);
  void ScheduleLoss(string value);
  void ScheduleQuality(string value);

  Ptr<MyApp>          m_app;
  Ptr<myVideoBuffer>  m_vidBuffer;
  Ptr<myFlowManager>  m_flowManager;
  uint32_t            m_currentBuffer;
  //in byte
  uint32_t            m_frameSize;
  //in byte/s
  double              m_frameRate;
  bool                m_running;
  bool                m_eof;
  EventId             m_bufferEvent;
  double              m_timestamp;
  list<uint32_t>      m_frameSizeList;
  list<uint32_t>      m_transmittedFrames;
  uint32_t            m_restFrameTime;
  string              m_pathframelist;
};

myBufferApp::myBufferApp ()
  : m_app (0),
    m_vidBuffer (0),
    m_flowManager (0),
    m_frameSize (0),
    m_frameRate (0),
    m_running (false),
    m_eof (false),
    m_bufferEvent (),
    m_frameSizeList (),
    m_transmittedFrames (),
    m_restFrameTime (0)
{
}

myBufferApp::~myBufferApp ()
{
}

void
myBufferApp::Setup (Ptr<MyApp> app, uint32_t frameSize, double frameRate, Ptr<myVideoBuffer> vidBuffer, Ptr<myFlowManager> flowManager, string pathframelist)
{
  m_app = app;
  m_vidBuffer = vidBuffer;
  m_flowManager = flowManager;
  m_frameSize = frameSize;
  m_frameRate = frameRate;
  m_restFrameTime = 0;
  m_pathframelist = pathframelist;

  //set standard segment size
  m_app->SetPacketSize(10000);


  readConfigFiles();
  Time tNext (Seconds (1));
  m_bufferEvent = Simulator::Schedule (tNext, &myBufferApp::UpdateBuffer, this);
  //UpdateBuffer();
}

void
myBufferApp::StartApplication (void)
{
  m_running = true;
  m_timestamp = Simulator::Now().GetSeconds();
}

void
myBufferApp::StopApplication (void)
{
  m_running = false;
  if (m_bufferEvent.IsRunning ())
    {
      Simulator::Cancel (m_bufferEvent);
    }

}

void
myBufferApp::UpdateBuffer (void)
{ 
  if (m_frameSizeList.empty()) {
    m_frameSize = 0;
    m_app->SetPacketSize(m_frameSize);
    m_flowManager->indicateEOF();
  }
  if ((m_vidBuffer->getCurrentBuffer() >= m_transmittedFrames.front()) && m_flowManager->getPlay()) {
    m_vidBuffer->updateTimeBuffer(-(1/m_frameRate));
    m_vidBuffer->drainBuffer(m_transmittedFrames.front());
    m_transmittedFrames.pop_front();
  }

  // terminate in case no more frames to transmit and to play
  if(m_flowManager->getEOF() == true && m_vidBuffer->getTimeBuffer() < (1/m_frameRate)) {
    m_vidBuffer->updateTimeBuffer(-m_vidBuffer->getTimeBuffer());
    m_vidBuffer->drainBuffer(m_vidBuffer->getCurrentBuffer());
    cout << "Reached end of video.\n";
    Simulator::Stop();
    exit(0);
  }

  // set quality
  double ratio = double(m_vidBuffer->getCurrentBuffer()) / double(m_frameSizeList.front());
  m_flowManager->setQuality(ratio);

  // only for buffer in bytes relevant
  /*
  if (m_flowManager->setNextFrameSize(m_frameSizeList.front())) {
    m_vidBuffer->checkBuffer ();
  }*/

  //24 Bilder/s
  Time tNext (Seconds (1/m_frameRate));
  m_bufferEvent = Simulator::Schedule (tNext, &myBufferApp::UpdateBuffer, this);
}

void
myBufferApp::mySinkRx (uint32_t receivedBytes, double timestamp)
{
  m_restFrameTime += receivedBytes;
  while (m_restFrameTime >= (m_frameSizeList.front()) * m_flowManager->getQuality()) {
    m_restFrameTime -= m_frameSizeList.front() * m_flowManager->getQuality();
    m_transmittedFrames.push_back(m_frameSizeList.front() * m_flowManager->getQuality());
    m_frameSizeList.pop_front();
    m_vidBuffer->updateTimeBuffer((1/m_frameRate));
    m_app -> updateFrameSizeList(m_frameSizeList);
  }
  m_vidBuffer->fillBuffer(receivedBytes);
}

void
myBufferApp::printsomething (string message)
{
  cout << "test sink\t" << message << "\n";
}

void
myBufferApp::readConfigFiles ()
{
  ostringstream sstream;

  // read framsize
  ifstream input_fs(m_pathframelist.c_str());
  for(string line; getline(input_fs, line);)
  {
    uint32_t fs = atoi(line.c_str());
    m_frameSizeList.insert(m_frameSizeList.end(), fs);
  }
  m_app -> updateFrameSizeList(m_frameSizeList);

  // copy framelist.txt to output directory
  sstream.str("");
  sstream << "cp " << m_pathframelist << " " << m_vidBuffer->getLogpath() << "/";
  char const* cmd_cp1 = sstream.str().c_str();
  system(cmd_cp1);

  // read link configurations
  ifstream input_link("scratch/linkconfig.txt");
  for(string line; getline(input_link, line);)
  {
    // split by character " "
    vector<string> tokens;
    string tmp_string;
    istringstream ss_line(line);
    while(getline(ss_line, tmp_string, ' '))
    {
      tokens.push_back(tmp_string);
    }

    // schedule new link configuration
    if (tokens[0] == "bw") {
      double seconds = atof(tokens[1].c_str());
      string value = tokens[2] + "Mb/s";
      Time tNext (Seconds (seconds));
      Simulator::Schedule (tNext, &myBufferApp::ScheduleBandwidth, this, value);
    } else if (tokens[0] == "del") {
      double seconds = atof(tokens[1].c_str());
      string value = tokens[2];
      Time tNext (Seconds (seconds));
      Simulator::Schedule (tNext, &myBufferApp::ScheduleDelay, this, value);
    } else if (tokens[0] == "loss") {
      double seconds = atof(tokens[1].c_str());
      string value = tokens[2];
      Time tNext (Seconds (seconds));
      Simulator::Schedule (tNext, &myBufferApp::ScheduleLoss, this, value);
    } else if (tokens[0] == "qu") {
      double seconds = atof(tokens[1].c_str());
      Time tNext (Seconds (seconds));
      string value = tokens[2];
      Simulator::Schedule (tNext, &myBufferApp::ScheduleQuality, this, value);
    }
  }
  // copy linkconfig.txt to output directory
  sstream.str("");
  sstream << "cp scratch/linkconfig.txt " << m_vidBuffer->getLogpath() << "/";
  char const* cmd_cp2 = sstream.str().c_str();
  system(cmd_cp2);
}

void
myBufferApp::ScheduleBandwidth (string value) {
  double ts = Simulator::Now().GetSeconds();
  cout << ts << " Changed bandwidth to " << value << "\n";
  Simulator::Schedule (Seconds (ts), Config::Set,"/NodeList/1/DeviceList/1/$ns3::PointToPointNetDevice/DataRate", DataRateValue (DataRate (value)));
}

void
myBufferApp::ScheduleDelay (string value) {
  double ts = Simulator::Now().GetSeconds();
  cout << ts << " Changed delay to " << value << "ms\n";
  // double double_value = atof(value.c_str());
  // m_p2ph.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (double_value)));
}

void
myBufferApp::ScheduleLoss (string value) {
  double ts = Simulator::Now().GetSeconds();
  cout << ts << " Changed loss to " << value << "\%\n";

}

void
myBufferApp::ScheduleQuality (string value) {
  // cout statement -> FlowManager
  double double_value = atof(value.c_str());
  m_flowManager->addQuality(double_value);
}