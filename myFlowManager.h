#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "string"

using namespace ns3;
using namespace std;

class myFlowManager : public Application
{
  public:
    myFlowManager ();
    virtual ~myFlowManager ();
 
    void Setup (double startT, double stopT, double startP, double stopP, string logpath);
    void checkLimits (double bufferSize);
    void checkDependencies ();
    bool getPlay ();
    bool getTransmission ();
    bool setNextFrameSize (uint32_t frameSize);
    void logTransmission ();
    void logPlay ();
    void logLimits ();
    void logQuality ();
    void setBufferSize (double currentBuffer);
    void addQuality (double quality);
    void setQuality (double ratio);
    double getQuality ();
    void indicateEOF ();
    bool getEOF ();
    double getStartTransmission ();
    double getStopTransmission ();
    double getStartPlay ();
    double getStopPlay ();
    double getCurrentBuffer();
  
  private:
    double              startTransmission;
    double              stopTransmission;
    double              startPlay;
    double              stopPlay;
    uint32_t            nextFrameSize;
    double              bufferSize;
    bool                transmission;
    bool                play;
    bool                m_eof;
    string              m_logpath;
    ofstream            m_logfile;
    bool                m_verbose;
    vector<double>      qualityValues;
    uint32_t            m_qualityIndex;
};

myFlowManager::myFlowManager ()
  :  /*startPlay (0),
     stopPlay (0),
     startTransmission (0),
     stopTransmission (0),*/
     nextFrameSize (0),
     bufferSize (0),
     transmission (true),
     play (true),
     m_eof (false),
     m_logfile(0),
     m_verbose(true),
     qualityValues(0),
     m_qualityIndex(0)
{
}

myFlowManager::~myFlowManager ()
{
  //logTransmission();
  //logPlay();
  m_logfile.close();
}

void
myFlowManager::Setup (double startT, double stopT, double startP, double stopP, string logpath)
{
  m_logpath = logpath;
  startTransmission = startT;
  stopTransmission = stopT;
  startPlay = startP;
  stopPlay = stopP;
  transmission = true;
  play = true;
  m_eof = false;
  // add initial quality to 100% and add it as quality mode
  m_qualityIndex = 0;
  addQuality(1.0);

  // generate logfile
  ostringstream sstream;
  sstream.str("");
  sstream << m_logpath << "/controls.log";
  char const* path_logfile = sstream.str().c_str();
  m_logfile.open(path_logfile);
  m_verbose = false;
 
  logLimits();
  logTransmission();
  logPlay();
  checkDependencies();
  checkLimits(0.0);
}

void
myFlowManager::checkLimits (double currentBuffer)
{
  setBufferSize (currentBuffer);
  
  // dependencies will become obsolete at the end of the video
  if (!m_eof) {
    checkDependencies ();
  }
  // set play and transmission flag
  if (bufferSize > stopTransmission && transmission == true) {
      transmission = false;
      logTransmission ();
  }
  if (bufferSize < startTransmission && transmission == false) {
      transmission = true;
      logTransmission ();
  }
  if (bufferSize > startPlay && play == false) {
      play = true;
      logPlay ();
  }
  if (bufferSize < stopPlay && play == true) {
      play = false;
      logPlay ();
  }
}

void
myFlowManager::checkDependencies ()
{
  if (startTransmission > stopTransmission) {
    printf("Invalid dependency startTransmission (%f) > stopTransmission (%f)\n", startTransmission, stopTransmission);
    exit(1);
  }
  if (startPlay < stopPlay) {
    printf("Invalid dependency startPlay (%f) < stopPlay (%f)\n", startPlay, stopPlay);
    exit(1); 
  }
  if (startTransmission < stopPlay) {
    printf("Invalid dependency startTransmission (%f) < stopPlay (%f)\n", startTransmission, stopPlay);
    exit(1); 
  }
}

bool
myFlowManager::getPlay ()
{
  return play;
}

bool
myFlowManager::getTransmission ()
{
  return transmission;
}

// only for buffer in bytes relevant
bool
myFlowManager::setNextFrameSize (uint32_t frameSize)
{
  nextFrameSize = frameSize;
  if (stopPlay < nextFrameSize) {
    startTransmission = nextFrameSize + (startTransmission - stopPlay);
    stopTransmission = nextFrameSize + (stopTransmission - stopPlay);
    startPlay = nextFrameSize + (startPlay - stopPlay);
    stopPlay = nextFrameSize;
    logLimits ();
    return true;
  }
  return false;
}

void
myFlowManager::logTransmission ()
{
  long double currentTime = Simulator::Now().GetSeconds();
  double currentQuality = qualityValues[m_qualityIndex];
  if (transmission) {
    if (m_verbose) {
      cout << currentTime << " " << "startTransmission " << bufferSize << " " << startTransmission << " " << stopTransmission << " " << startPlay << " " << stopPlay << " " << currentQuality << "\n";
    }
    m_logfile << currentTime << " " << "startTransmission " << bufferSize << " " << startTransmission << " " << stopTransmission << " " << startPlay << " " << stopPlay << " " << currentQuality << "\n";
  } else {
    if (m_verbose) {
      cout << currentTime << " " << "stopTransmission " << bufferSize << " " << startTransmission << " " << stopTransmission << " " << startPlay << " " << stopPlay << " " << currentQuality << "\n";
    }
    m_logfile << currentTime << " " << "stopTransmission " << bufferSize << " " << startTransmission << " " << stopTransmission << " " << startPlay << " " << stopPlay << " " << currentQuality << "\n";
  }
}

void
myFlowManager::logPlay ()
{
  long double currentTime = Simulator::Now().GetSeconds();
  double currentQuality = qualityValues[m_qualityIndex];
  if (play) {
    if (m_verbose) {
      cout << currentTime << " " << "startPlay " << bufferSize << " " << startTransmission << " " << stopTransmission << " " << startPlay << " " << stopPlay << " " << currentQuality << "\n";
    }
    m_logfile << currentTime << " " << "startPlay " << bufferSize << " " << startTransmission << " " << stopTransmission << " " << startPlay << " " << stopPlay << " " << currentQuality << "\n";
  } else {
    if (m_verbose) {
      cout << currentTime << " " << "stopPlay " << bufferSize << " " << startTransmission << " " << stopTransmission << " " << startPlay << " " << stopPlay << " " << currentQuality << "\n";
    }
    m_logfile << currentTime << " " << "stopPlay " << bufferSize << " " << startTransmission << " " << stopTransmission << " " << startPlay << " " << stopPlay << " " << currentQuality << "\n";
  }
}

void
myFlowManager::logLimits ()
{
  long double currentTime = Simulator::Now().GetSeconds();
  double currentQuality = qualityValues[m_qualityIndex];
  if (m_verbose) {
    cout << currentTime << " " << "newLimits " << bufferSize << " " << startTransmission << " " << stopTransmission << " " << startPlay << " " << stopPlay << " " << currentQuality << "\n";
  }
  
  m_logfile << currentTime << " " << "newLimits " << bufferSize << " " << startTransmission << " " << stopTransmission << " " << startPlay << " " << stopPlay << " " << currentQuality << "\n";
}

void
myFlowManager::logQuality () {
  long double currentTime = Simulator::Now().GetSeconds();
  double currentQuality = qualityValues[m_qualityIndex];
  if (m_verbose) {
    cout << currentTime << " " << "qualityChanged " << bufferSize << " " << startTransmission << " " << stopTransmission << " " << startPlay << " " << stopPlay << " " << currentQuality << "\n";
  }
  
  m_logfile << currentTime << " " << "qualityChanged " << bufferSize << " " << startTransmission << " " << stopTransmission << " " << startPlay << " " << stopPlay << " " << currentQuality << "\n";
}

void
myFlowManager::setBufferSize (double currentBuffer)
{
  bufferSize = currentBuffer;
}

void
myFlowManager::addQuality (double quality)
{
  // neglect wrong qualities
  if (quality <= 0 || quality > 1) {
    return;
  }

  // neglect duplicate qualities
  int size = qualityValues.size();
  for (int i=0; i<size; i++) {
    if (qualityValues[i] == quality) {
      return;
    }
  }

  qualityValues.push_back(quality);
  double oldQuality = qualityValues[m_qualityIndex];
  double ts = Simulator::Now().GetSeconds();
  cout << ts << " Added quality mode " << quality*100.0 << "\%\n";
  sort(qualityValues.begin(), qualityValues.end());
  size++;
  // consistency alert
  // get right index after sorting
  for (int i=0; i<size; i++) {
    if (oldQuality == qualityValues[i]) {
      m_qualityIndex = i;
      break;
    }
  }
}

void
myFlowManager::setQuality (double ratio) {
  double oldQuality = qualityValues[m_qualityIndex];
  for (int i=qualityValues.size(); i>0; i--) {
    m_qualityIndex = i-1;
    if (ratio >= qualityValues[m_qualityIndex]) {
      // only log in case of change
      if (oldQuality != qualityValues[m_qualityIndex]) {
        logQuality();
      }
      return;
    }
  }
  // only log in case of change
  if (oldQuality != qualityValues[m_qualityIndex]) {
    logQuality();
  }
}

double
myFlowManager::getQuality () {
  return qualityValues[m_qualityIndex];
}

void
myFlowManager::indicateEOF () {
  if (!m_eof) {
    m_eof = true;
    startPlay = 0;
    stopPlay = 0;
    logLimits();
    checkLimits(bufferSize);
  }
}

bool
myFlowManager::getEOF () {
  return m_eof;
}

double
myFlowManager::getStartTransmission () {
  return startTransmission;
}

double
myFlowManager::getStopTransmission () {
  return stopTransmission;
}

double
myFlowManager::getStartPlay () {
  return startPlay;
}

double
myFlowManager::getStopPlay () {
  return stopPlay;
}

double
myFlowManager::getCurrentBuffer () {
  return bufferSize;
}