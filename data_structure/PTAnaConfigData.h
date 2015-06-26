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
#ifndef _PTAnaConfigData_h_
#define _PTAnaConfigData_h_

#include "TNamed.h"
#include <string>
#include <vector>

class PTAnaConfigData : public TNamed {
public:
  PTAnaConfigData() {}
  PTAnaConfigData(const char* name,const char* title,int testid):TNamed(name,title),fTestID(testid)
  {}
  ~PTAnaConfigData() {}

  bool	ReadConfig(const char* configfile);
  std::vector<int>   GetRefPMTs();
  std::vector<int>   GetTestPMTs();
  void Print();

public:
  int		fTestID;
  
  int		fChannelNum;
  std::vector<int>		fChannels;
  std::vector<int>		fPMTIndexs;
  std::vector<std::string>	fSerialNos;
  std::vector<std::string>	fPositions;
  std::vector<int>		fDy5Channels;
  std::vector<int>		fDy8Channels;
  
  ClassDef(PTAnaConfigData,1)
};

#endif // _PTAnaConfigData_h_
