#include "PTAnaLedConfig.h"
#include "PTAnaBase.h"
#include "stdio.h"
#include <iostream>

using namespace std;

int  PTAnaLedConfig::fVoltageStep=7;
int  PTAnaLedConfig::fVoltages[7]={1000,950,900,850,800,750,700};
  
int  PTAnaLedConfig::GROUP1_FILENO[7]={10,10,11,11,11,11,11};
int  PTAnaLedConfig::GROUP1_FILEINDEX[7][15]={{1,2,3,4,5,6,7,8,9,10},{1,2,3,4,5,6,7,8,9,10},{1,2,3,4,5,6,7,8,9,10,11},{1,2,3,4,5,6,7,8,9,10,11}
                                                                   ,{1,2,3,4,5,6,7,8,9,10,11},{1,2,3,4,5,6,7,8,9,10,11},{1,2,3,4,5,6,7,8,9,10,11}};
int  PTAnaLedConfig::GROUP2_FILENO[7]={5,5,5,5,5,5,5};
int  PTAnaLedConfig::GROUP2_FILEINDEX[7][15]={{11,12,13,14,15},{11,12,13,14,15},{12,13,14,15,16},{12,13,14,15,16}
                                                                   ,{12,13,14,15,16},{12,13,14,15,16},{12,13,14,15,16}};
int  PTAnaLedConfig::GROUP3_FILENO[7]={5,5,5,5,5,5,5};
int  PTAnaLedConfig::GROUP3_FILEINDEX[7][15]={{1,2,3,4,5},{1,2,3,4,5},{17,18,19,20,21},{17,18,19,20,21},{17,18,19,20,21}
                                                                   ,{17,18,19,20,21},{17,18,19,20,21}};
								   
PTAnaLedConfig::PTAnaLedConfig():
TNamed("LED_config","LED_config")
{

}


void PTAnaLedConfig::ReadConfig(const char* dir)
{
  FILE* fpconfig;
  int datapoints;
  char buffer[256];
  int fileid,amplitude,width;
  PTAnaLedConfigEntry tmpconfig;
  
  for(int hv_id=0;hv_id<fVoltageStep;hv_id++){
    fpconfig=fopen(Form("%s/%dV.dat",dir,fVoltages[hv_id]),"r");
    
    fscanf(fpconfig,"Total data points: %d\n",&datapoints);
    fgets(buffer,256,fpconfig);
    for(int j=0;j<datapoints;j++){
      fscanf(fpconfig,"%d %d %d\n",&fileid,&amplitude,&width);
      
      tmpconfig.fFileID=fileid;
      tmpconfig.fWidth=width*1.0;
      tmpconfig.fAmplitude=amplitude*0.01;
      tmpconfig.fGID=PTAnaBase::Encode(fVoltages[hv_id],width,amplitude);
      //cout<< tmpconfig.fFileID <<"\t"<< PTAnaBase::DecodeAmplitude(tmpconfig.fGID)<<"\t"<<PTAnaBase::DecodeWidth(tmpconfig.fGID)<<endl;
      ///
      fAll[fVoltages[hv_id]].push_back(tmpconfig);
      
      ///
      for(int file_id=0;file_id<GROUP1_FILENO[hv_id];file_id++){
	if(GROUP1_FILEINDEX[hv_id][file_id] == fileid){
	  fGroup1[fVoltages[hv_id]].push_back(tmpconfig);
	  fDy58[fVoltages[hv_id]].push_back(tmpconfig);
	  break;
	}
      }
      
      ///
      for(int file_id=0;file_id<GROUP2_FILENO[hv_id];file_id++){
	if(GROUP2_FILEINDEX[hv_id][file_id] == fileid){
	  fGroup2[fVoltages[hv_id]].push_back(tmpconfig);
	  fDy58[fVoltages[hv_id]].push_back(tmpconfig);
	  break;
	}
      }
      
      ///
      for(int file_id=0;file_id<GROUP3_FILENO[hv_id];file_id++){
	if(GROUP3_FILEINDEX[hv_id][file_id] == fileid){
	  fGroup3[fVoltages[hv_id]].push_back(tmpconfig);
	  break;
	}
      }
    }
    
    fclose(fpconfig);
  }
  ////////////
  std::map<int,PTAnaLedConfigEntry> tmpgain;
  int size=fGroup3[fVoltages[0]].size();
  int sizee;
  for(int i=0;i<size;i++){
    //cout<< i+1<<endl;
    width=PTAnaBase::DecodeWidth(fGroup3[fVoltages[0]][i].fGID);
    amplitude=PTAnaBase::DecodeAmplitude(fGroup3[fVoltages[0]][i].fGID);
    
    tmpgain.clear();
    tmpgain[fVoltages[0]]=fGroup3[fVoltages[0]][i];
    for(int j=1;j<fVoltageStep;j++){
      //cout<<"\t"<<j+1<<endl;
      sizee=fGroup3[fVoltages[j]].size();
      for(int k=0;k<sizee;k++){
	
	if(width == PTAnaBase::DecodeWidth(fGroup3[fVoltages[j]][k].fGID)
	  && amplitude == PTAnaBase::DecodeAmplitude(fGroup3[fVoltages[j]][k].fGID))
	{
	   //cout<<"\t\t"<<k+1<<endl;
	   tmpgain[fVoltages[j]]=fGroup3[fVoltages[j]][k];
	   break;
	}
      }
    }
    
    ///
    fGain.push_back(tmpgain);
    //cout<< i+1<<endl;
  }
  cout<<fGain.size()<<endl;
}

void PTAnaLedConfig::PrintAll(int v)
{
  std::vector<PTAnaLedConfigEntry> tmpconfig=fAll[v];
  int size=tmpconfig.size();
  int voltage,width,amplitude;
  
  cout<<"Voltage: "<< voltage <<endl;
  cout<<"All Datapoits: "<< size <<endl;
  cout<<"FileID\tAmplitude\tWidth\tVoltage"<<endl;
  for(int i=0;i<size;i++){
    PTAnaBase::Decode(tmpconfig[i].fGID,voltage,width,amplitude);
    cout<< tmpconfig[i].fFileID<<"\t";
    cout<< amplitude<<"\t";
    cout<< width <<"\t";
    cout<< voltage << endl;
  }
}

void PTAnaLedConfig::PrintGroup1(int v)
{
  std::vector<PTAnaLedConfigEntry> tmpconfig=fGroup1[v];
  int size=tmpconfig.size();
  int voltage,width,amplitude;
  
  cout<<"Voltage: "<< voltage <<endl;
  cout<<"Group1 Datapoits: "<< size <<endl;
  cout<<"FileID\tAmplitude\tWidth\tVoltage"<<endl;
  for(int i=0;i<size;i++){
    PTAnaBase::Decode(tmpconfig[i].fGID,voltage,width,amplitude);
    cout<< tmpconfig[i].fFileID<<"\t";
    cout<< amplitude<<"\t";
    cout<< width <<"\t";
    cout<< voltage << endl;
  }
}

void PTAnaLedConfig::PrintGroup2(int v)
{
  std::vector<PTAnaLedConfigEntry> tmpconfig=fGroup2[v];
  int size=tmpconfig.size();
  int voltage,width,amplitude;
  
  cout<<"Voltage: "<< voltage <<endl;
  cout<<"Group2 Datapoits: "<< size <<endl;
  cout<<"FileID\tAmplitude\tWidth\tVoltage"<<endl;
  for(int i=0;i<size;i++){
    PTAnaBase::Decode(tmpconfig[i].fGID,voltage,width,amplitude);
    cout<< tmpconfig[i].fFileID<<"\t";
    cout<< amplitude<<"\t";
    cout<< width <<"\t";
    cout<< voltage << endl;
  }
}

void PTAnaLedConfig::PrintGroup3(int v)
{
  std::vector<PTAnaLedConfigEntry> tmpconfig=fGroup3[v];
  int size=tmpconfig.size();
  int voltage,width,amplitude;
  
  cout<<"Voltage: "<< voltage <<endl;
  cout<<"Group3 Datapoits: "<< size <<endl;
  cout<<"FileID\tAmplitude\tWidth\tVoltage"<<endl;
  for(int i=0;i<size;i++){
    PTAnaBase::Decode(tmpconfig[i].fGID,voltage,width,amplitude);
    cout<< tmpconfig[i].fFileID<<"\t";
    cout<< amplitude<<"\t";
    cout<< width <<"\t";
    cout<< voltage << endl;
  }
}

void PTAnaLedConfig::PrintDy58(int v)
{
  std::vector<PTAnaLedConfigEntry> tmpconfig=fDy58[v];
  int size=tmpconfig.size();
  int voltage,width,amplitude;
  
  cout<<"Voltage: "<< voltage <<endl;
  cout<<"Dy58 Datapoits: "<< size <<endl;
  cout<<"FileID\tAmplitude\tWidth\tVoltage"<<endl;
  for(int i=0;i<size;i++){
    PTAnaBase::Decode(tmpconfig[i].fGID,voltage,width,amplitude);
    cout<< tmpconfig[i].fFileID<<"\t";
    cout<< amplitude<<"\t";
    cout<< width <<"\t";
    cout<< voltage << endl;
  }
}

void PTAnaLedConfig::PrintGain(int amp_id)
{
  if(amp_id>5)
    cout<<"Error! amp_id should from 1 to 5"<<endl;
  
  std::map<int,PTAnaLedConfigEntry> tmpconfig=fGain[amp_id-1];
  int voltage,width,amplitude;
  //cout<<tmpconfig.size()<<endl;
  
  cout<< "Amplitude "<<amp_id<<":"<<endl;
  cout<< "Voltage\tFileID\tAmplitude\tWidth"<<endl;
  for(int hv_id=0;hv_id<fVoltageStep;hv_id++){
    PTAnaBase::Decode(tmpconfig[fVoltages[hv_id]].fGID,voltage,width,amplitude);
    cout<< fVoltages[hv_id]<<"\t";
    cout<< tmpconfig[fVoltages[hv_id]].fFileID<<"\t";
    cout<< amplitude<<"\t";
    cout<< width <<"\t";
    cout<< voltage << endl;
  }
}