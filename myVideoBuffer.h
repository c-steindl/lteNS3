#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "string"

using namespace ns3;
using namespace std;


class myVideoBuffer : public Application
{
  public:
    myVideoBuffer();
    virtual ~myVideoBuffer();
    
    void Setup(Ptr<MyApp> app, Ptr<myFlowManager> flowManager, string logpath);
    void fillBuffer(uint32_t receivedBytes);
    void drainBuffer(uint32_t processedBytes);
    void printBuffer();
    uint32_t getCurrentBuffer();
    string getLogpath();
    void checkBuffer();
    void updateTimeBuffer(double updateTime);
    double getTimeBuffer();
  
  private:
    Ptr<MyApp>          m_app;
    Ptr<myFlowManager>  m_flowManager;
    uint64_t            m_currentBuffer;
    uint64_t            m_processedTotal;
    uint64_t            m_bytesTotal;
    string              m_logpath;
    ofstream            m_logfile;
    double              m_timeBuffer;
    double              m_timeTrans;
    double              m_timePlay;
    bool                m_verbose;
};

myVideoBuffer::myVideoBuffer ()
  :  m_app (0),
     m_flowManager (0),
     //m_bytesTotal (0),
     m_currentBuffer (0),
     m_processedTotal (0),
     m_logfile(0),
     m_timeBuffer(0),
     m_timeTrans (0),
     m_timePlay (0),
     m_verbose(true)
{
}

myVideoBuffer::~myVideoBuffer ()
{
  m_logfile.close();
}

void
myVideoBuffer::Setup (Ptr<MyApp> app, Ptr<myFlowManager> flowManager, string logpath)
{
  m_app = app;
  m_flowManager = flowManager;
  m_currentBuffer = 0;
  m_bytesTotal = 0;
  m_processedTotal = 0;
  Time tNext (Seconds (1.0));
  m_logpath = logpath;
  m_timeBuffer = 0.0;
  m_timeTrans = 0.0;
  m_timePlay = 0.0;

  // generate logfile
  ostringstream sstream;
  sstream.str("");
  sstream << m_logpath << "/buffer.log";
  char const* path_logfile = sstream.str().c_str();
  m_logfile.open(path_logfile);

  printBuffer();
  Simulator::Schedule (tNext, &myVideoBuffer::checkBuffer, this);
  m_verbose = false;
}

void
myVideoBuffer::fillBuffer (uint32_t receivedBytes)
{
  m_bytesTotal += receivedBytes;
  m_currentBuffer += receivedBytes;
  checkBuffer();
}

void
myVideoBuffer::drainBuffer (uint32_t processedBytes)
{
  m_processedTotal += processedBytes;
  m_currentBuffer -= processedBytes;
  checkBuffer();
}

void
myVideoBuffer::checkBuffer ()
{ 
  printBuffer();
  //m_flowManager->checkLimits(m_currentBuffer);
  m_flowManager->checkLimits(m_timeBuffer);
  m_app->updateTimeBuffer(m_timeBuffer);
  m_app->SendPacket();
}

void
myVideoBuffer::printBuffer ()
{
  long double currentTime = Simulator::Now().GetSeconds();
  double currentQuality = m_flowManager->getQuality();
  if (m_verbose) {
    cout << currentTime << " " << m_currentBuffer << " " << m_bytesTotal << " " << m_processedTotal << " " << m_timeBuffer << " " << m_timeTrans << " " << m_timePlay << " " << currentQuality << "\n";  
  }
  
  m_logfile << currentTime << " " << m_currentBuffer << " " << m_bytesTotal << " " << m_processedTotal << " " << m_timeBuffer << " " << m_timeTrans << " " << m_timePlay << " " << currentQuality << "\n";
}

uint32_t
myVideoBuffer::getCurrentBuffer ()
{
  return m_currentBuffer;
}

string
myVideoBuffer::getLogpath ()
{
  return m_logpath;
}

void
myVideoBuffer::updateTimeBuffer (double updateTime)
{
  m_timeBuffer += updateTime;
  if (updateTime > 0) {
    m_timeTrans += updateTime;
  } else {
    m_timePlay += (updateTime * -1);
  }
}

double
myVideoBuffer::getTimeBuffer ()
{
  return m_timeBuffer;
}
