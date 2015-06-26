#ifndef PTANALEDCONFIG_H
#define PTANALEDCONFIG_H

#include "TNamed.h"
#include <vector>
#include <map>

struct PTAnaLedConfigEntry{
  int fFileID;
  float fWidth;
  float fAmplitude;
  int fGID;
  
  ClassDef(PTAnaLedConfigEntry,1)
};

class PTAnaLedConfig : public TNamed
{
public:
    PTAnaLedConfig();
    ~PTAnaLedConfig() {}

public:
  void ReadConfig(const char* dir);
  void PrintAll(int voltage);
  void PrintGroup1(int voltage);
  void PrintGroup2(int voltage);
  void PrintGroup3(int voltage);
  void PrintDy58(int voltage);
  void PrintGain(int amplitude_id);
  
public:
  static int fVoltageStep;
  static int fVoltages[7];
  static Int_t GROUP1_FILENO[7];
  static Int_t GROUP1_FILEINDEX[7][15];
  static Int_t GROUP2_FILENO[7];
  static Int_t GROUP2_FILEINDEX[7][15];
  static Int_t GROUP3_FILENO[7];
  static Int_t GROUP3_FILEINDEX[7][15];
  
  std::map<int,std::vector<PTAnaLedConfigEntry> > fAll;
  std::map<int,std::vector<PTAnaLedConfigEntry> > fGroup1;
  std::map<int,std::vector<PTAnaLedConfigEntry> > fGroup2;
  std::map<int,std::vector<PTAnaLedConfigEntry> > fGroup3;
  std::map<int,std::vector<PTAnaLedConfigEntry> > fDy58;
  std::vector<std::map<int,PTAnaLedConfigEntry> > fGain;
    
  ClassDef(PTAnaLedConfig,1)
};

#endif // PTANALEDCONFIG_H
