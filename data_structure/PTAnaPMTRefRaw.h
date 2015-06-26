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
#ifndef _PTAnaPMTRefRaw_h_
#define _PTAnaPMTRefRaw_h_

#include <map>
#include "PTAnaPMTFitData.h"
#include "TNamed.h"

class PTAnaPMTRefRaw : public TNamed {
public:
  PTAnaPMTRefRaw() {}
  PTAnaPMTRefRaw(const char* testdir,int testid);
  ~PTAnaPMTRefRaw() {}
  
  void SetTestID(int testid)
  {
    fTestID=testid;
  }
  //////////
  int GetTestID()
  {
    return fTestID;
  }
  
private:
  int			fTestID;
  
public:
  std::map<int,PTAnaPMTFitData>		fRawDataRef1;
  std::map<int,PTAnaPMTFitData>		fRawDataRef2;
  
  ClassDef(PTAnaPMTRefRaw,1)
};

#endif // _PTAnaPMTRefRaw_h_