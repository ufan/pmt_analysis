/*
 T his *library is free software; you can redistribute it and/or
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

#include "PTAnaPMTRefRaw.h"

PTAnaPMTRefRaw::PTAnaPMTRefRaw(const char* testdir,int testid):
TNamed(testdir,testdir),
fTestID(testid)
{
  
}

std::map<int,double> PTAnaPMTRefRaw::GetCalibRatioDy5(PTAnaPMTRefRaw *ref,int refid)
{
    std::map<int,double> ratios;

    std::map<int,PTAnaPMTFitData> tmpref,tmpcurrent;
    switch(refid)
    {
    case 1:
        tmpref=ref->fRawDataRef1;
        tmpcurrent=fRawDataRef1;
        break;
    case 2:
        tmpref=ref->fRawDataRef2;
        tmpcurrent=fRawDataRef2;
        break;
    default:
        break;
    }

    std::map<int,PTAnaPMTFitData>::iterator it;
    for(it=tmpcurrent.begin();it!=tmpcurrent.end();it++){
        ratios[it->first]=tmpref[it->first].fDy5Mean/(it->second).fDy5Mean;
    }

    return ratios;
}

std::map<int,double> PTAnaPMTRefRaw::GetCalibRatioDy8(PTAnaPMTRefRaw *ref,int refid)
{
    std::map<int,double> ratios;

    std::map<int,PTAnaPMTFitData> tmpref,tmpcurrent;
    switch(refid)
    {
    case 1:
        tmpref=ref->fRawDataRef1;
        tmpcurrent=fRawDataRef1;
        break;
    case 2:
        tmpref=ref->fRawDataRef2;
        tmpcurrent=fRawDataRef2;
        break;
    default:
        break;
    }

    std::map<int,PTAnaPMTFitData>::iterator it;
    for(it=tmpcurrent.begin();it!=tmpcurrent.end();it++){
        ratios[it->first]=tmpref[it->first].fDy8Mean/(it->second).fDy8Mean;
    }

    return ratios;
}
