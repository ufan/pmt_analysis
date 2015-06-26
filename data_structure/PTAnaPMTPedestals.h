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
#ifndef _PTAnaPMTPedestals_h_
#define _PTAnaPMTPedestals_h_

#include "TNamed.h"
#include <map>

struct PTAnaPedFitData {
  double fMean;
  double fMeanError;
  double fSigma;
  double fSigmaError;
  
  ClassDef(PTAnaPedFitData,1)
};

class PTAnaPMTPedestals : public TNamed {
public:
  PTAnaPMTPedestals():TNamed("null","null") {}
  PTAnaPMTPedestals(const char* testdir,int testid );
  ~PTAnaPMTPedestals() {}
  
public:
  int					fTestID;
  std::map<int,PTAnaPedFitData>		fBegin;
  std::map<int,PTAnaPedFitData>		fEnd;
  
  ClassDef(PTAnaPMTPedestals,1)
};

#endif // _PTAnaPMTPedestals_h_