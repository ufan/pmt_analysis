#include "PTAnaResultDy58.h"
#include "PTAnaPMTRaw.h"
#include "TKey.h"
#include "TList.h"
#include "TIterator.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TGraph.h"
#include <iostream>
#include "PTAnaLedConfig.h"
#include "PTAnaBase.h"
#include <map>
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TLine.h"
#include "TLatex.h"

void draw_dy58(const char* infile,const char* paramfile,const char* potfile)
{
    TGraph *gLEDvsCM=new TGraph();
    TGraph *gLEDvsPT=new TGraph();
    TH1F *hled=new TH1F("hled","Dy58 from LED test",40,30,70);hled->SetLineColor(kRed);
    TH1F *hcm=new TH1F("hcm","Dy58 from cosmic ray test",40,30,70);hcm->SetLineColor(kBlue);
    TH1F *hpt=new TH1F("hpt","Dy58 from LED test after potting",40,30,70);hpt->SetLineColor(kGreen);
    TH1F *hptled=new TH1F("hptled","Dy58 from LED test",40,30,70);hptled->SetLineColor(kBlack);
    //
    TFile* filein=new TFile(infile);

    PTAnaResultDy58 *dy58=0;
    TDirectory* dir_dy58=filein->GetDirectory("dy58_result");
    if(!dir_dy58){
        printf("error!can't get \"dy58_result\" in %s\n",filein->GetName());
        exit(1);
    }
    //
    Char_t label[10];
    Double_t eff_voltage,dy58_cm;
    TFile* fileparam=new TFile(paramfile);
    TTree* tcm=(TTree*)fileparam->Get("dy58_cm");
    tcm->SetBranchAddress("serial_no",label);
    tcm->SetBranchAddress("effective_voltage",&eff_voltage);
    tcm->SetBranchAddress("dy58",&dy58_cm);

    //
    Int_t entries=tcm->GetEntries();
    for(int i=0;i<entries;i++){
        tcm->GetEntry(i);
        dy58=(PTAnaResultDy58*)dir_dy58->Get(label);

        gLEDvsCM->SetPoint(i,dy58->Evaluate(eff_voltage),dy58_cm);
        std::cout<<eff_voltage<<std::endl;

        hled->Fill(dy58->Evaluate(eff_voltage));
        hcm->Fill(dy58_cm);
    }

    //
    //
    Char_t serial_no[10];
    Double_t dy58_pt,voltage;
    TFile *filept=new TFile(potfile);
    TTree *tpt=(TTree*)filept->Get("dy58_afterpotting");
    tpt->SetBranchAddress("serial_no",serial_no);
    tpt->SetBranchAddress("Voltage",&voltage);
    tpt->SetBranchAddress("dy58_potting",&dy58_pt);

    entries=tpt->GetEntries();
    for(int i=0;i<entries;i++){
        tpt->GetEntry(i);
        dy58=(PTAnaResultDy58*)dir_dy58->Get(serial_no);

        hpt->Fill(dy58_pt);
        hptled->Fill(dy58->Evaluate(voltage));
        gLEDvsPT->SetPoint(i,dy58->Evaluate(voltage),dy58_pt);

    }
    //
    delete fileparam;
    delete filein;

    TCanvas *c1=new TCanvas("c1","c1",1000,500);
    c1->Divide(2,1);
    c1->cd(1);
    gPad->SetTopMargin(0);
    gPad->SetRightMargin(0);
    gPad->SetLeftMargin(0.11);
    gPad->SetBottomMargin(0.11);
    hcm->GetXaxis()->SetTitle("Dynode8/Dynode5");
    hcm->GetXaxis()->CenterTitle();
    hcm->GetXaxis()->SetTitleOffset(1.08);
    hcm->GetYaxis()->SetTitle("Number of PMTs");
    hcm->GetYaxis()->CenterTitle();
    hcm->GetYaxis()->SetTitleOffset(1.18);
    hcm->SetFillStyle(3004);hcm->SetFillColor(kBlue);
    hcm->Draw();
    hled->Draw("same");
    hled->SetFillStyle(3004);hled->SetFillColor(kRed);

    TLegend* legend1=new TLegend(0.3,0.83,0.8,0.95);
    legend1->AddEntry(hcm,"Cosmic Ray Result","l");
    legend1->AddEntry(hled,"LED Result","l");
    legend1->SetTextSize(0.05);
    legend1->Draw();


    c1->cd(2);
    gPad->SetTopMargin(0);
    gPad->SetRightMargin(0);
    gPad->SetLeftMargin(0.12);
    gPad->SetBottomMargin(0.11);
    gPad->SetGridx();
    gPad->SetGridy();

    gLEDvsCM->Draw("A*");
    gLEDvsCM->GetXaxis()->SetTitle("Dy8/Dy5 result from LED Test");
    gLEDvsCM->GetXaxis()->CenterTitle();
    gLEDvsCM->GetXaxis()->SetTitleOffset(1.08);
    gLEDvsCM->GetYaxis()->SetTitle("Dy8/Dy5 result from Cosmic Ray Test");
    gLEDvsCM->GetYaxis()->CenterTitle();
    gLEDvsCM->GetYaxis()->SetTitleOffset(1.18);

    Double_t x[2]={44,60};
    Double_t y[2]={42,55};
    Double_t m=(y[0]-y[1])/(x[0]-x[1]);

    TLine *llimit = new TLine();
    llimit->SetLineWidth(3);
    llimit->SetLineColor(kBlue);
    //llimit->SetLineStyle(2);
    llimit->DrawLine(x[0], y[0], x[1], y[1]);
    //llimit->DrawLine(xrange[0], mean-limit, xrange[1], mean-limit);

    TLatex *text=new TLatex();
    text->SetTextAlign(21);
    text->SetTextFont(132);
    text->SetTextSize(0.06);
    text->SetTextColor(kBlue);
    text->DrawLatex(46,54,Form("Slope=%.2f",m));

    /*
    TCanvas *c2=new TCanvas("c2");
    c2->Divide(2,1);
    c2->cd(1);
    hpt->Draw();hptled->Draw("same");
    c2->cd(2);
    gLEDvsPT->Draw("A*");
    */
}

void draw_dy58_all(const char* infile,float voltage)
{
    TH1F *hist=new TH1F(Form("%.0fV",voltage),"Distribution of Dy8/Dy5",40,36,76);
    hist->SetLineColor(kBlue);
    hist->SetFillStyle(3004);
    hist->SetFillColor(kBlue);

    TFile* filein=new TFile(infile);

    PTAnaResultDy58 *dy58=0;
    TDirectory* dir_dy58=filein->GetDirectory("dy58_result");
    if(!dir_dy58){
        printf("error!can't get \"dy58_result\" in %s\n",filein->GetName());
        exit(1);
    }

    TList *keys=dir_dy58->GetListOfKeys();
    TKey *key;
    TIter next(keys);
    if(keys){
        while (key=(TKey*)next()) {
            dy58=(PTAnaResultDy58*)key->ReadObj();
            hist->Fill(dy58->Evaluate(voltage));
        }
    }

    delete filein;

    int cw=700;
    int ch=500;
    TCanvas* c=new TCanvas("c","c",cw,ch);
    //c->SetCanvasSize(cx,cy); //method 1
    c->SetWindowSize(cw+(cw-c->GetWw()),ch+(ch-c->GetWh()));

    c->SetTopMargin(0);
    c->SetRightMargin(0);
    c->SetLeftMargin(0.11);
    c->SetBottomMargin(0.11);
    hist->GetXaxis()->SetTitle("Dynode8/Dynode5");
    hist->GetXaxis()->CenterTitle();
    hist->GetYaxis()->SetTitle("Number of PMTs");
    hist->GetYaxis()->CenterTitle();
    hist->Draw();
    hist->Fit("gaus");
}

void draw_dy58_example(const char* filename,const char* dy58file,const char* serial_no)
{
    TMultiGraph* mg=new TMultiGraph();
    std::map<int,TGraphErrors*> gres;
    std::map<int,int> counters;
    for(int i=0;i<PTAnaLedConfig::fVoltageStep;i++){
        gres[PTAnaLedConfig::fVoltages[i]]=new TGraphErrors();
        counters[PTAnaLedConfig::fVoltages[i]]=0;
    }
    std::map<int,Color_t> colors;
    std::map<int,Style_t> markers;
    std::map<int,double>  sizes;
    colors[0]=kBlack;colors[3]=kBlue;colors[6]=kRed;
    markers[0]=20;markers[3]=22;markers[6]=3;
    sizes[0]=1.2;sizes[3]=1.6;sizes[6]=1.6;
    for(int i=0;i<PTAnaLedConfig::fVoltageStep;i+=3){
        gres[PTAnaLedConfig::fVoltages[i]]->SetMarkerColor(colors[i]);
        gres[PTAnaLedConfig::fVoltages[i]]->SetMarkerStyle(markers[i]);
        gres[PTAnaLedConfig::fVoltages[i]]->SetMarkerSize(sizes[i]);
        mg->Add(gres[PTAnaLedConfig::fVoltages[i]]);
    }
    //TGraphErrors* gre=new TGraphErrors();

    TFile* filein=new TFile(filename);

    TDirectory* dir_raw=filein->GetDirectory("raw");
    if(!dir_raw){
        printf("error!can't get \"raw\" in %s\n",filein->GetName());
        exit(1);
    }

    PTAnaPMTRaw* raw=(PTAnaPMTRaw*)dir_raw->Get(serial_no);
    std::map<int,PTAnaPMTFitData>::iterator it;
    Int_t voltage;
    for(it=raw->fRawTestData.begin();it!=raw->fRawTestData.end();it++){
        voltage=PTAnaBase::DecodeVoltage(it->first);
        gres[voltage]->SetPoint(counters[voltage],it->second.fDy5Mean,it->second.fDy8Mean);
        gres[voltage]->SetPointError(counters[voltage],it->second.fDy5MeanError,it->second.fDy8MeanError);
        counters[voltage]++;
    }

    delete filein;


    ///
    filein=new TFile(dy58file);

    PTAnaResultDy58 *dy58=0;
    TDirectory* dir_dy58=filein->GetDirectory("dy58_result");
    if(!dir_dy58){
        printf("error!can't get \"dy58_result\" in %s\n",filein->GetName());
        exit(1);
    }
    dy58=(PTAnaResultDy58*)dir_dy58->Get(serial_no);
    TGraphErrors* gdy58=new TGraphErrors(dy58->ExtractGraph());
    TF1* fdy58=new TF1(dy58->ExtractFunction());
    //
    int cw=700;
    int ch=500;
    TCanvas* c=new TCanvas("c","c",cw,ch);
    //c->SetCanvasSize(cx,cy); //method 1
    c->SetWindowSize(cw+(cw-c->GetWw()),ch+(ch-c->GetWh()));

    c->SetTopMargin(0);
    c->SetRightMargin(0);
    c->SetLeftMargin(0.13);
    c->SetBottomMargin(0.13);
    mg->Draw("AP");
    mg->GetXaxis()->SetTitle("Signal of Dynode5 (ADC counts)");
    mg->GetXaxis()->CenterTitle();
    mg->GetXaxis()->SetTitleOffset(1.3);
    mg->GetYaxis()->SetTitle("Signal of Dynode8 (ADC counts)");
    mg->GetYaxis()->SetTitleOffset(1.4);
    mg->GetYaxis()->CenterTitle();
    mg->GetYaxis()->SetNdivisions(505);


    TPad* pinset=new TPad("pinset","pinset",0.48,0.15,0.99,0.7);
    pinset->SetLeftMargin(0.1);
    pinset->SetRightMargin(0);
    pinset->SetTopMargin(0);
    pinset->SetBottomMargin(0.1);
    pinset->Draw();
    pinset->cd();
    gdy58->Draw("AP");
    gdy58->GetXaxis()->SetTitle("Voltage (V)");
    gdy58->GetXaxis()->CenterTitle();
    gdy58->GetYaxis()->SetTitle("Dynode8/Dynode5");
    gdy58->GetYaxis()->CenterTitle();
    fdy58->Draw("same");

    TLatex *text=new TLatex();
    text->SetTextAlign(21);
    text->SetTextFont(132);
    text->SetTextSize(0.09);
    //text->SetTextColor(kBlue);
    text->DrawLatex(842,75,Form("Dy8/Dy5= %.2E #times V^{%.2f}",fdy58->GetParameter(0),fdy58->GetParameter(1)));

}

void print_params(const char* infile,int para_id=1)
{
    TH1F *hist=new TH1F("hist","hist",40,0,10);
    hist->SetLineColor(kBlue);
    hist->SetFillStyle(3004);
    hist->SetFillColor(kBlue);

    TFile* filein=new TFile(infile);

    PTAnaResultDy58 *dy58=0;
    TF1 ftemp;
    TDirectory* dir_dy58=filein->GetDirectory("dy58_result");
    if(!dir_dy58){
        printf("error!can't get \"dy58_result\" in %s\n",filein->GetName());
        exit(1);
    }

    TList *keys=dir_dy58->GetListOfKeys();
    TKey *key;
    TIter next(keys);
    if(keys){
        while (key=(TKey*)next()) {
            dy58=(PTAnaResultDy58*)key->ReadObj();
            ftemp=dy58->ExtractFunction();
            hist->Fill(ftemp.GetParameter(para_id));
        }
    }

    delete filein;

    hist->Draw();
}
