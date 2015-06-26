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

#include "PTAnaConfigData.h"
#include "stdio.h"
#include <iostream>

bool PTAnaConfigData::ReadConfig(const char* configfile)
{
  int tmpch,tmpdy5,tmpdy8,tmpindex;
  char tmplabel[25],tmpposition[25];
  
  fChannelNum=25;
  ////////////////////////////////////////
  char buffer1[256];
  FILE* fp=fopen(configfile,"r");
  if(!fp){
    printf("error opening %s\n",configfile);
    return false;
  }
  
  fgets(buffer1,256,fp);
  //printf("%s",buffer1);
  fgets(buffer1,256,fp);
  //printf("%s",buffer1);
  for(int i=0;i < fChannelNum;i++){
    fscanf(fp,"%d %s %s %d %d %d\n",&tmpch,tmplabel,tmpposition,&tmpindex,&tmpdy5,&tmpdy8);
    fChannels.push_back(tmpch);
    fPMTIndexs.push_back(tmpindex);
    fSerialNos.push_back(tmplabel);
    fPositions.push_back(tmpposition);
    fDy5Channels.push_back(tmpdy5);
    fDy8Channels.push_back(tmpdy8);
    //printf("%d %s %s %d %d %d\n",tmpch,tmplabel,tmpposition,tmpindex,tmpdy5,tmpdy8);
  }
  
  fclose(fp);
  
  return true;
}

std::vector<int> PTAnaConfigData::GetRefPMTs()
{
    std::vector<int> container;

    for(int i=0;i<fChannelNum;i++){
        if(fPMTIndexs[i]<0 && fPMTIndexs[i]>-10)
            container.push_back(i);
    }
    return container;
}

std::vector<int> PTAnaConfigData::GetTestPMTs()
{
    std::vector<int> container;

    for(int i=0;i<fChannelNum;i++){
        if(fPMTIndexs[i]>0)
            container.push_back(i);
    }
    return container;
}

void PTAnaConfigData::Print()
{
    for(int i=0;i<fChannelNum;i++){
        std::cout<< fChannels[i] <<"\t";
        std::cout<< fPMTIndexs[i] <<"\t";
        std::cout<< fSerialNos[i] <<"\t";
        std::cout<< fPositions[i] <<"\t";
        std::cout<< fDy5Channels[i] <<"\t";
        std::cout<< fDy8Channels[i] <<std::endl;
    }
}
