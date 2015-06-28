/*
 T his library is free s*oftware; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License version 2 as published by the Free Software Foundation.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.
 
 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
 
 ---
 Copyright (C) 2015, ufan <>
 */
#ifndef _PTAnaPMTRaw_h_
#define _PTAnaPMTRaw_h_

#include <string>
#include <map>
#include "PTAnaPMTFitData.h"
#include "PTAnaPMTRefRaw.h"
#include "PTAnaConfigData.h"
#include "TRef.h"
#include "TNamed.h"

class PTAnaPMTRaw : public TNamed {
public:
  PTAnaPMTRaw() {}
  PTAnaPMTRaw(const char *pmt_serialno,int index,const char *pmt_position,int channel);
  ~PTAnaPMTRaw() {}
  
  //////////////////////
  void SetSerialNo(const char *pmt_serialno)
  {
    fSerialNo=pmt_serialno;
  }
  void SetIndex(int index)
  {
    fIndex=index;
  }
  void SetPosition(const char *pmt_position)
  {
    fPosition=pmt_position;
  }
  void SetChannel(int channel)
  {
    fChannel=channel;
  }
  void SetElectronicChannels(int dy5_channel,int dy8_channel)
  {
    fDy5Channel=dy5_channel;
    fDy8Channel=dy8_channel;
  }
  //////////////////////
  std::string GetSerialNo()
  {
    return fSerialNo;
  }
  int GetIndex()
  {
    return fIndex;
  }
  std::string GetPosition()
  {
    return fPosition;
  }
  int GetChannel()
  {
    return fChannel;
  }
  int GetDy5Channel()
  {
    return fDy5Channel;
  }
  int GetDy8Channel()
  {
    return fDy8Channel;
  }
  bool GetLEDCalibRatioDy5(int gid,PTAnaPMTRaw* testrun,double& ratio);
  bool GetLEDCalibRatioDy8(int gid,PTAnaPMTRaw* testrun,double& ratio);
  void CalibLED(PTAnaPMTRefRaw* refraw);
private:
  std::string		fSerialNo;
  int			fIndex;
  std::string		fPosition;
  int			fChannel;
  int			fDy5Channel;
  int 			fDy8Channel;
  ////////////////
public:
  double		fDy5PedMean;
  double		fDy8PedMean;
  double 		fDy5PedSigma;
  double		fDy8PedSigma;
  std::map<int,PTAnaPMTFitData>		fRawTestData;//no pedestal	
  std::map<int,PTAnaPMTFitData>		fLEDCalibData;//initial calibration of the light intesity with respect to the first test run
  TRef					fRefPMTData;
  TRef					fConfigData;
  //PTAnaPMTRefRaw*					fRefPMTData;
  //PTAnaConfigData*					fConfigData;

  ClassDef(PTAnaPMTRaw,1)
};

#endif // _PTAnaPMTRaw_h_
