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
#include "PTAnaPMTRaw.h"

PTAnaPMTRaw::PTAnaPMTRaw(const char *pmt_serialno,int index,const char *pmt_position,int channel):
TNamed(pmt_serialno,pmt_position),
fIndex(index),
fPosition(pmt_position),
fChannel(channel),
fDy5Channel(-1),
fDy8Channel(-1)
{
  
}

bool PTAnaPMTRaw::GetLEDCalibRatioDy5(int gid, PTAnaPMTRaw *testrun,double& ratio)
{
    std::map<int,PTAnaPMTFitData>::iterator it;

    it=fRawTestData.find(gid);
    if(it == fRawTestData.end())
        return false;

    it=testrun->fRawTestData.find(gid);
    if(it == testrun->fRawTestData.end())
        return false;

    ratio=(fRawTestData[gid].fDy5Mean)/(testrun->fRawTestData[gid].fDy5Mean);
    return true;
}

bool PTAnaPMTRaw::GetLEDCalibRatioDy8(int gid, PTAnaPMTRaw *testrun,double& ratio)
{
    std::map<int,PTAnaPMTFitData>::iterator it;

    it=fRawTestData.find(gid);
    if(it == fRawTestData.end())
        return false;

    it=testrun->fRawTestData.find(gid);
    if(it == testrun->fRawTestData.end())
        return false;

    ratio=(fRawTestData[gid].fDy8Mean)/(testrun->fRawTestData[gid].fDy8Mean);
    return true;
}

void PTAnaPMTRaw::CalibLED(PTAnaPMTRefRaw *refraw)
{
    fLEDCalibData.clear();
    //
    PTAnaPMTRefRaw* currentraw=(PTAnaPMTRefRaw*)fRefPMTData.GetObject();

    std::map<int,double> dy5ratios,dy8ratios;
    dy5ratios=currentraw->GetCalibRatioDy5(refraw);
    dy8ratios=currentraw->GetCalibRatioDy8(refraw);

    //
    fLEDCalibData=fRawTestData;
    std::map<int,PTAnaPMTFitData>::iterator	it;
    for(it=fRawTestData.begin();it!=fRawTestData.end();it++){
        fLEDCalibData[it->first].fDy5Mean=((it->second).fDy5Mean)*dy5ratios[it->first];
        fLEDCalibData[it->first].fDy8Mean=((it->second).fDy8Mean)*dy8ratios[it->first];
    }
}
