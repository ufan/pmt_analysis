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

#include "PTAnaAlgorithm.h"
#include "PTAnaPMTFitData.h"
#include "PTAnaPMTRaw.h"
#include "PTAnaPMTRefRaw.h"
#include "PTAnaConfigData.h"
#include "PTAnaBase.h"
#include "PTAnaPMTPedestals.h"
#include "TFile.h"
#include "TString.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "stdio.h"
#include "TH1F.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TList.h"
#include "TKey.h"
#include "TIterator.h"
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

PTAnaPMTPedestals* fit_pedestal(const char* parentdir,const char* testdir,const char* date,int testid)
{
 
  TString rootdir=TString(parentdir)+"/"+testdir+"/root_file";
  
  PTAnaPMTPedestals* pedestals=new PTAnaPMTPedestals(testdir,testid);
  pedestals->SetTitle(date);
  
  //////////////////////////////
  TFitResultPtr result;
  double tmpmean,tmpsigma;
  
  TFile* filein;
  TTree* treein;
  TH1F *hist=new TH1F("hist","hist",2000,-1000,3000);
  
  filein = new TFile(Form("%s/pedestal/begin.root",rootdir.Data()));
  treein = (TTree*)filein->Get("PMT");
  treein->Print();
  hist->SetDirectory(filein);
  for(int j=0;j<64;j++){
    treein->Project("hist",Form("hh[%d]",j));
 
    result=hist->Fit("gaus","SNQ");
    tmpmean=result->Parameter(1);
    tmpsigma=result->Parameter(2);
    
    result=hist->Fit("gaus","SNQ","",tmpmean-10*tmpsigma,tmpmean+10*tmpsigma);
    pedestals->fBegin[j+1].fMean=result->Parameter(1);
    pedestals->fBegin[j+1].fSigma=result->Parameter(2);
    pedestals->fBegin[j+1].fMeanError=result->ParError(1);
    pedestals->fBegin[j+1].fSigmaError=result->ParError(2);
    //printf("%f %f %f %f\n",pedestals->fBegin[j+1].fMean,pedestals->fBegin[j+1].fMeanError,pedestals->fBegin[j+1].fSigma,pedestals->fBegin[j+1].fSigmaError);
    
    hist->Reset();
  }
  hist->SetDirectory(0);
  delete filein;
  
  
  filein = new TFile(Form("%s/pedestal/end.root",rootdir.Data()));
  treein = (TTree*)filein->Get("PMT");
  hist->SetDirectory(filein);
  for(int j=0;j<64;j++){
    hist->Reset();
    treein->Project("hist",Form("hh[%d]",j));
    
    result=hist->Fit("gaus","SNQ");
    tmpmean=result->Parameter(1);
    tmpsigma=result->Parameter(2);
    
    result=hist->Fit("gaus","SNQ","",tmpmean-10*tmpsigma,tmpmean+10*tmpsigma);
    pedestals->fEnd[j+1].fMean=result->Parameter(1);
    pedestals->fEnd[j+1].fSigma=result->Parameter(2);
    pedestals->fEnd[j+1].fMeanError=result->ParError(1);
    pedestals->fEnd[j+1].fSigmaError=result->ParError(2);
  }
  delete filein;
  
  ////////////////////////////////////

  
  return pedestals;
}

void fit_pedestal_batch(const char* outfile,const char* filelist)
{
  string tmpdir,tmpdate;
  int tmpid;
  vector<string> testdirs,testdates;
  vector<int>	testids;
  ifstream files(filelist);
  if(!files){
    cout<< "Can't open "<<filelist<<endl;
  }
  
  while (!files.eof()) {
    files>> tmpdir >> tmpid >> tmpdate;
    testdirs.push_back(tmpdir);
    testids.push_back(tmpid);
    testdates.push_back(tmpdate);
    
    cout<< tmpdir<<"\t"<<tmpid<<"\t"<<tmpdate<<endl;
  }
  
  files.close();
  ////////////////////////////////////////
  TString parentdir(gSystem->DirName(filelist));
  TFile *fileout=new TFile(outfile,"update");
  TDirectory* dir_out=fileout->GetDirectory("pedestals");
  if(!dir_out){
    dir_out=fileout->mkdir("pedestals");
    if(!dir_out){
      printf("error!can't mkdir \"pedestals\" in %s\n",fileout->GetName());
      exit(1);
    }
  }
  
  PTAnaPMTPedestals* pedestals=0;
  int runnum=testdirs.size();  
  for(int i=0;i<runnum;i++){
    pedestals=fit_pedestal(parentdir.Data(),testdirs[i].c_str(),testdates[i].c_str(),testids[i]);
    
    dir_out->cd();
    pedestals->Write(0,TObject::kOverwrite);
    delete pedestals;
  }
  
  delete fileout;
}


void check_pedestal(const char* pardir,const char* testdir)
{
  PTAnaConfigData* config=new PTAnaConfigData(testdir,"",-1);
  config->ReadConfig((TString(pardir)+"/"+testdir+"/root_file/configuration.dat").Data());
  
  TFile* filein=new TFile("fit.root");
  
  TDirectory* dir_in=filein->GetDirectory("pedestals");
  if(!dir_in){
      printf("error!can't mkdir \"pedestals\" in %s\n",filein->GetName());
      exit(1);
  }
  
  //////////
  PTAnaPMTPedestals* pedestals=(PTAnaPMTPedestals*)dir_in->Get(testdir);
  if(!pedestals){
    printf("error\n");
    exit(1);
  }
  pedestals->Print();

  FILE* fp=fopen((TString(testdir)+"_check.dat").Data(),"w");
  if(!fp){
    printf("error\n");
    exit(1);
  }
  int chnum=config->fChannelNum;
  //printf("chnum=%d\n",chnum);
  for(int i=0;i<chnum;i++){    
    fprintf(fp,"%d %s %.4f %.4f %.4f %.4f\n",
      config->fChannels[i],config->fSerialNos[i].c_str(),
      pedestals->fBegin[config->fDy5Channels[i]].fMean,pedestals->fBegin[config->fDy5Channels[i]].fSigma,
      pedestals->fBegin[config->fDy8Channels[i]].fMean,pedestals->fBegin[config->fDy8Channels[i]].fSigma
    );
    
    //fprintf(fp,"%d\n",config->fChannels[i]);//,config->fSerialNos[i].c_str(),config->fDy5Channels[i],config->fDy8Channels[i]);
  }
  fclose(fp);
  
  
  delete filein;
  delete config;
}

void fit_testingdata(const char* outfile,const char* pardir,const char* testdir,int testid,const char* testdate)
{
    ///
    /// \brief Get configuration data
    ///
    std::vector<int> pmt_ids;
    Int_t pmtnum;

    PTAnaConfigData* config=new PTAnaConfigData(testdir,testdate,testid);
    config->ReadConfig((TString(pardir)+"/"+testdir+"/root_file/configuration.dat").Data());

    PTAnaPMTRefRaw refraw(testdir,testid);
    refraw.SetTitle(testdate);

    std::vector<PTAnaPMTRaw> testraw;
    pmt_ids=config->GetTestPMTs();
    pmtnum=pmt_ids.size();
    for(int i=0;i<pmtnum;i++){
        testraw.push_back(PTAnaPMTRaw(config->fSerialNos[pmt_ids[i]].c_str(),config->fPMTIndexs[pmt_ids[i]],config->fPositions[pmt_ids[i]].c_str(),config->fChannels[pmt_ids[i]]));
        testraw[i].SetElectronicChannels(config->fDy5Channels[pmt_ids[i]],config->fDy8Channels[pmt_ids[i]]);
        //
        testraw[i].fRefPMTData=&refraw;
        testraw[i].fConfigData=config;
    }
    ////
    /// \brief Get pedestals from outfile
    ///
    PTAnaPMTPedestals* pedestals=0;
    TFile* fileout=new TFile(outfile,"update");
    TDirectory* dir_ped=fileout->GetDirectory("pedestals");
    if(!dir_ped){
        printf("error!can't get dir \"pedestals\" in %s\n",fileout->GetName());
        exit(1);
    }
    pedestals=(PTAnaPMTPedestals*)dir_ped->Get(testdir);
    if(!pedestals){
      printf("error! no pedestals data in %s\n",fileout->GetName());
      exit(1);
    }

    pmtnum=testraw.size();
    for(int i=0;i<pmtnum;i++){
        testraw[i].fDy5PedMean=pedestals->fBegin[testraw[i].GetDy5Channel()].fMean;
        testraw[i].fDy5PedSigma=pedestals->fBegin[testraw[i].GetDy5Channel()].fSigma;
        testraw[i].fDy8PedMean=pedestals->fBegin[testraw[i].GetDy8Channel()].fMean;
        testraw[i].fDy8PedSigma=pedestals->fBegin[testraw[i].GetDy8Channel()].fSigma;
    }

    ///
    /// \brief Make directory for ref_PMT data ,config data and testing PMT data
    ///
    TDirectory* dir_ref=fileout->GetDirectory("reference");
    if(!dir_ref){
      dir_ref=fileout->mkdir("reference");
      if(!dir_ref){
        printf("error!can't mkdir \"reference\" in %s\n",fileout->GetName());
        exit(1);
      }
    }

    TDirectory* dir_config=fileout->GetDirectory("configuration");
    if(!dir_config){
      dir_config=fileout->mkdir("configuration");
      if(!dir_config){
        printf("error!can't mkdir \"configuration\" in %s\n",fileout->GetName());
        exit(1);
      }
    }

    TDirectory* dir_raw=fileout->GetDirectory("raw");
    if(!dir_raw){
      dir_raw=fileout->mkdir("raw");
      if(!dir_raw){
        printf("error!can't mkdir \"raw\" in %s\n",fileout->GetName());
        exit(1);
      }
    }

    ///
    /// \brief filein
    ///
    TFile* filein=0;
    TTree* treein=0;
    TH1F* hist=new TH1F("hist","hist",1800,-1000,17000);

    const Int_t HIGHVOLTAGE_STEP=7;
    const Int_t VOLTAGE[HIGHVOLTAGE_STEP]={1000,950,900,850,800,750,700};

    TString rootdir;
    Int_t datapoints,fileid,width,amplitude,gid;
    Char_t buffer[256];
    FILE* fpconfig;
    TFitResultPtr result;
    double tmpmean,tmpsigma;
    PTAnaPMTFitData fitdata;

    for(int hv_id=0;hv_id<HIGHVOLTAGE_STEP;hv_id++){
        rootdir.Form("%s/%s/root_file/%dV",pardir,testdir,VOLTAGE[hv_id]);
        ///
        /// Get LED configurations from LED_config
        ///
        fpconfig=fopen(Form("%s/LED_config/%dV.dat",pardir,VOLTAGE[hv_id]),"r");
        fscanf(fpconfig,"Total data points: %d\n",&datapoints);
        printf("Total data points: %d\n",datapoints);
        fgets(buffer,256,fpconfig);
        for(int j=0;j<datapoints;j++){
            fscanf(fpconfig,"%d %d %d\n",&fileid,&amplitude,&width);
            gid=PTAnaBase::Encode(VOLTAGE[hv_id],width,amplitude);
            ///
            filein=new TFile(Form("%s/%d.root",rootdir.Data(),fileid));
            treein=(TTree*)filein->Get("PMT");
            hist->SetDirectory(filein);

            ///
            /// Fit Ref_PMT
            ///
            pmt_ids=config->GetRefPMTs();
            pmtnum=pmt_ids.size();
            for(int pmt_id=0;pmt_id<pmtnum;pmt_id++){
                /// fit dy5
                hist->Reset();
                treein->Project("hist",Form("hh[%d]",config->fDy5Channels[pmt_ids[pmt_id]]-1));

                tmpmean=hist->GetBinContent(hist->GetMaximumBin());
                result=hist->Fit("gaus","SNQ","",tmpmean-200,tmpmean+200);
                tmpmean=result->Parameter(1);
                tmpsigma=result->Parameter(2);


                if((tmpmean-5*tmpsigma)<16000){
                    result=hist->Fit("gaus","SNQ","",tmpmean-5*tmpsigma,tmpmean+5*tmpsigma);
                }

                fitdata.fDy5Mean=result->Parameter(1)-pedestals->fBegin[config->fDy5Channels[pmt_ids[pmt_id]]].fMean;
                fitdata.fDy5Sigma=result->Parameter(2);
                fitdata.fDy5MeanError=result->ParError(1);
                fitdata.fDy5SigmaError=result->ParError(2);
                fitdata.fDy5FitResult=result;

                /// fit dy8
                hist->Reset();
                treein->Project("hist",Form("hh[%d]",config->fDy8Channels[pmt_ids[pmt_id]]-1));

                result=hist->Fit("gaus","SNQ");
                tmpmean=result->Parameter(1);
                tmpsigma=result->Parameter(2);

                /*
                if((tmpmean-5*tmpsigma)<16000){
                    result=hist->Fit("gaus","SNQ","",tmpmean-5*tmpsigma,tmpmean+5*tmpsigma);
                }
                */
                fitdata.fDy8Mean=result->Parameter(1)-pedestals->fBegin[config->fDy8Channels[pmt_ids[pmt_id]]].fMean;
                fitdata.fDy8Sigma=result->Parameter(2);
                fitdata.fDy8MeanError=result->ParError(1);
                fitdata.fDy8SigmaError=result->ParError(2);
                fitdata.fDy8FitResult=result;

                ///
                switch (config->fPMTIndexs[pmt_ids[pmt_id]]) {
                case -1:
                    refraw.fRawDataRef1[gid]=fitdata;
                    break;
                case -2:
                    refraw.fRawDataRef2[gid]=fitdata;
                    break;
                default:
                    break;
                }
            }

            std::cout<< (j+1)<<std::endl;
            ///
            /// Fit PMTs under test
            ///
            pmt_ids=config->GetTestPMTs();
            pmtnum=pmt_ids.size();
            for(int pmt_id=0;pmt_id<pmtnum;pmt_id++){
                /// fit dy5
                hist->Reset();
                treein->Project("hist",Form("hh[%d]",config->fDy5Channels[pmt_ids[pmt_id]]-1));

                //tmpmean=hist->GetMaximum();
                result=hist->Fit("gaus","SNQ");
                tmpmean=result->Parameter(1);
                tmpsigma=result->Parameter(2);

                /*
                if((tmpmean-5*tmpsigma)<16000){
                    result=hist->Fit("gaus","SNQ","",tmpmean-5*tmpsigma,tmpmean+5*tmpsigma);
                }
                */
                fitdata.fDy5Mean=result->Parameter(1)-pedestals->fBegin[config->fDy5Channels[pmt_ids[pmt_id]]].fMean;
                fitdata.fDy5Sigma=result->Parameter(2);
                fitdata.fDy5MeanError=result->ParError(1);
                fitdata.fDy5SigmaError=result->ParError(2);
                fitdata.fDy5FitResult=result;

                /// fit dy8
                hist->Reset();
                treein->Project("hist",Form("hh[%d]",config->fDy8Channels[pmt_ids[pmt_id]]-1));

                result=hist->Fit("gaus","SNQ");
                tmpmean=result->Parameter(1);
                tmpsigma=result->Parameter(2);

                /*
                if((tmpmean-5*tmpsigma)<16000){
                    result=hist->Fit("gaus","SNQ","",tmpmean-5*tmpsigma,tmpmean+5*tmpsigma);
                }
                */
                fitdata.fDy8Mean=result->Parameter(1)-pedestals->fBegin[config->fDy8Channels[pmt_ids[pmt_id]]].fMean;
                fitdata.fDy8Sigma=result->Parameter(2);
                fitdata.fDy8MeanError=result->ParError(1);
                fitdata.fDy8SigmaError=result->ParError(2);
                fitdata.fDy8FitResult=result;

                ///
                testraw[pmt_id].fRawTestData[gid]=fitdata;
            }

            ///
            hist->SetDirectory(0);
            delete filein;
        }
        fclose(fpconfig);
    }

    ///
    /// write to TFile
    ///
    dir_config->cd();
    config->Write(0,TObject::kOverwrite);

    dir_ref->cd();
    refraw.Write(0,TObject::kOverwrite);

    dir_raw->cd();
    pmt_ids=config->GetTestPMTs();
    pmtnum=pmt_ids.size();
    for(int pmt_id=0;pmt_id<pmtnum;pmt_id++){
        testraw[pmt_id].Write(0,TObject::kOverwrite);
    }

    delete config;
    delete fileout;
}

void fit_testingdata_batch(const char* outfile,const char* filelist)
{
    string tmpdir,tmpdate;
    int tmpid;
    vector<string> testdirs,testdates;
    vector<int>	testids;
    ifstream files(filelist);
    if(!files){
      cout<< "Can't open "<<filelist<<endl;
    }

    while (!files.eof()) {
      files>> tmpdir >> tmpid >> tmpdate;
      testdirs.push_back(tmpdir);
      testids.push_back(tmpid);
      testdates.push_back(tmpdate);

      cout<< tmpdir<<"\t"<<tmpid<<"\t"<<tmpdate<<endl;
    }

    files.close();
    ////////////////////////////////////////
    TString parentdir(gSystem->DirName(filelist));

    int runnum=testdirs.size();
    for(int i=0;i<runnum;i++){
      fit_testingdata(outfile,parentdir.Data(),testdirs[i].c_str(),testids[i],testdates[i].c_str());
    }

}

void check_testdata(const char* infile,const char* serialno)
{
    TFile* filein=new TFile(infile);
    get_configurations(filein);

    TDirectory* dir_config=filein->GetDirectory("configuration");
    if(!dir_config){
        printf("error!can't get \"configuration\" in %s\n",filein->GetName());
        exit(1);
    }
    //PTAnaConfigData* config_tmp=(PTAnaConfigData*)dir_config->Get("retest_8");
    //std::cout<< config_tmp <<std::endl;

    TDirectory* dir_raw=filein->GetDirectory("raw");
    if(!dir_raw){
        printf("error!can't get \"raw\" in %s\n",filein->GetName());
        exit(1);
    }
    PTAnaPMTRaw* testraw=(PTAnaPMTRaw*)dir_raw->Get(serialno);
    testraw->Print();

    TRef ref=testraw->fConfigData;
    ref.Print();
    PTAnaConfigData* config=(PTAnaConfigData*)ref.GetObject();
    if(!config)
        std::cout<<"error"<<std::endl;
    config->Print();

/*
    testraw=(PTAnaPMTRaw*)dir_raw->Get("AA2547");
    testraw->Print();
    ref=testraw->fConfigData;
    ref.Print();
    PTAnaConfigData* config_fuck=(PTAnaConfigData*)ref.GetObject();
        if(!config_fuck)
            std::cout<<"error"<<std::endl;
        config_fuck->Print();
        std::cout<< config_fuck <<std::endl;

        for(int i=0;i<config_fuck->fChannelNum;i++){
            std::cout<< config_fuck->fChannels[i] <<"\t";
            std::cout<< config_fuck->fPMTIndexs[i] <<"\t";
            std::cout<< config_fuck->fSerialNos[i] <<"\t";
            std::cout<< config_fuck->fPositions[i] <<"\t";
            std::cout<< config_fuck->fDy5Channels[i] <<"\t";
            std::cout<< config_fuck->fDy8Channels[i] <<std::endl;
        }
        */
    delete filein;
}

void get_configurations(TFile* filein)
{
    //TFile *filein=new TFile(infile);

    TDirectory* dir_config=filein->GetDirectory("configuration");
    if(!dir_config){
        printf("error!can't get \"configuration\" in %s\n",filein->GetName());
        exit(1);
    }

    TList *keys=dir_config->GetListOfKeys();
    TKey *key;
    //PTAnaConfigData* config;
    TIter next(keys);
    if(keys){
        while (key=(TKey*)next()) {
            //config=(PTAnaConfigData*)
            key->ReadObj();
            //config->Print();
        }
    }
    else {
        std::cout<< "Error reading configurations"<<std::endl;
    }

    //delete filein;
}

void get_references(TFile* filein)
{
    TDirectory* dir_ref=filein->GetDirectory("reference");
    if(!dir_ref){
        printf("error!can't get \"reference\" in %s\n",filein->GetName());
        exit(1);
    }

    TList *keys=dir_ref->GetListOfKeys();
    TKey *key;
    TIter next(keys);
    if(keys){
        while (key=(TKey*)next()) {
            key->ReadObj();
        }
    }
    else {
        std::cout<< "Error reading references"<<std::endl;
    }
}
