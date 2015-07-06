#include "PTAnaPMTRefRaw.h"
#include "PTAnaLedConfig.h"
#include "PTAnaBase.h"
#include "TList.h"
#include "TKey.h"
#include "TIterator.h"
#include "TH1F.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include <iostream>
#include <vector>

void draw_test(const char* infile)
{
    TFile *filein=new TFile(infile);

    TDirectory* dir_ref=filein->GetDirectory("reference");
    if(!dir_ref){
        printf("error!can't get \"reference\" in %s\n",filein->GetName());
        exit(1);
    }

    TList *keys=dir_ref->GetListOfKeys();
    std::cout<< "total run numbers:"<<keys->GetSize()<<std::endl;

    TH1F *hist=new TH1F("hist","hist",1000,0.98,1.02);
    std::vector<PTAnaPMTRefRaw*> raws;
    TKey *key;
    TIter next(keys);
    Int_t counter=0;
    if(keys){
        while (key=(TKey*)next()) {
            raws.push_back(dynamic_cast<PTAnaPMTRefRaw*>(key->ReadObj()));
            std::cout<<raws[counter]->GetName()<<std::endl;
            counter++;
        }
    }
    else {
        std::cout<< "Error reading references"<<std::endl;
    }

    ///
    //std::vector<int,double> averages;
    std::map<int,TArrayD>  arrays;
    std::map<int,PTAnaPMTFitData>::iterator it;
    for(it=raws[0]->fLEDCalibData.begin();it!=raws[0]->fLEDCalibData.end();it++){
        if(it->second.IsValid()){
            for(int i=0;i<counter;i++){
                if(i==0){
                    arrays[it->first].Set(counter);
                }
                arrays[it->first].AddAt(raws[i]->fLEDCalibData[it->first].fDy8Mean,i);
            }
        }
    }
    std::cout<<"Total tested amplitudes:"<<raws[0]->fLEDCalibData.size()<<std::endl;
    std::cout<<"Valid tested amplitudes:"<<arrays.size()<<std::endl;

    //
    std::map<int,TArrayD>::iterator it2;
    for(it2=arrays.begin();it2!=arrays.end();it2++){
        //if(700==PTAnaBase::DecodeVoltage(it2->first)){
            for(int i=0;i<counter;i++){
                hist->Fill(it2->second.At(i)/it2->second.GetSum()*counter);
            }
        //}
    }
    hist->SetDirectory(0);
    hist->Draw();

    delete filein;
}

void draw_relativegain_ref(const char* filename,const char* dirname,int voltage)
{
    //PTAnaLedConfig ledconfig;
    //ledconfig.ReadConfig("./LED_config");

    TGraphErrors* gre=new TGraphErrors();

    TFile* filein=new TFile(filename);

    TDirectory* dir_ref=filein->GetDirectory("reference");
    if(!dir_ref){
        printf("error!can't get \"reference\" in %s\n",filein->GetName());
        exit(1);
    }

    PTAnaPMTRefRaw* refraw=(PTAnaPMTRefRaw*)dir_ref->Get(dirname);
    std::map<int,PTAnaPMTFitData>::iterator it;
    Int_t counter=0;
    for(it=refraw->fRawDataRef1.begin();it!=refraw->fRawDataRef1.end();it++){
        if(voltage==PTAnaBase::DecodeVoltage(it->first)){
            if(it->second.IsValid()){
                gre->SetPoint(counter,it->second.fDy8Mean,refraw->fRawDataRef2[it->first].fDy8Mean);
                gre->SetPointError(counter,it->second.fDy8MeanError,refraw->fRawDataRef2[it->first].fDy8MeanError);
                counter++;
            }
        }
    }

    delete filein;
    gre->Draw("A*");
}

void draw_dy58(const char* filename,const char* dirname)
{
    TMultiGraph* mg=new TMultiGraph();
    std::map<int,TGraphErrors*> gres;
    std::map<int,int> counters;
    for(int i=0;i<PTAnaLedConfig::fVoltageStep;i++){
        gres[PTAnaLedConfig::fVoltages[i]]=new TGraphErrors();
        gres[PTAnaLedConfig::fVoltages[i]]->SetMarkerColor(i+1);
        mg->Add(gres[PTAnaLedConfig::fVoltages[i]]);
        counters[PTAnaLedConfig::fVoltages[i]]=0;
    }
    //TGraphErrors* gre=new TGraphErrors();

    TFile* filein=new TFile(filename);

    TDirectory* dir_ref=filein->GetDirectory("reference");
    if(!dir_ref){
        printf("error!can't get \"reference\" in %s\n",filein->GetName());
        exit(1);
    }

    PTAnaPMTRefRaw* refraw=(PTAnaPMTRefRaw*)dir_ref->Get(dirname);
    std::map<int,PTAnaPMTFitData>::iterator it;
    Int_t voltage;
    for(it=refraw->fRawDataRef1.begin();it!=refraw->fRawDataRef1.end();it++){
        voltage=PTAnaBase::DecodeVoltage(it->first);
        gres[voltage]->SetPoint(counters[voltage],it->second.fDy5Mean,it->second.fDy8Mean);
        gres[voltage]->SetPointError(counters[voltage],it->second.fDy5MeanError,it->second.fDy8MeanError);
        counters[voltage]++;
    }

    delete filein;


    for(int i=0;i<PTAnaLedConfig::fVoltageStep;i++){
        gres[PTAnaLedConfig::fVoltages[i]]->Fit("pol1","","same");
    }

    //mg->Draw("A*");

}
