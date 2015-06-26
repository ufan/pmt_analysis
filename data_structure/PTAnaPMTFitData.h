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
#ifndef _PTAnaPMTFitData_h_
#define _PTAnaPMTFitData_h_

#include "TFitResultPtr.h"
#include "TFitResult.h"

class PTAnaPMTFitData{
public:
  PTAnaPMTFitData() {}
  ~PTAnaPMTFitData() {}
    
  ////////////////
  /*
   * Check the validity of the fitting result based on whether in the linear range
   * More conditions may be added future
   */
  bool IsValid()
  {
    if(fDy8FitResult->Parameter(1) > fgLinearityLimit)
      return false;
    return true;
  }
  
public:
    static int fgLinearityLimit;
    ///////////////
    double fDy5Mean;//may substract pedestal,calibration and so on
    double fDy5Sigma;
    double fDy5MeanError;
    double fDy5SigmaError;
    TFitResultPtr	fDy5FitResult;//original fitting result
    
    double fDy8Mean;
    double fDy8Sigma;
    double fDy8MeanError;
    double fDy8SigmaError;
    double fDy8Chi2;
    TFitResultPtr	fDy8FitResult;
    
    ClassDef(PTAnaPMTFitData,1)
};

#endif // _PTAnaPMTFitData_h_