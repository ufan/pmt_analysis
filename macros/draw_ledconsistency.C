#include "PTAnaPMTRefRaw.h"
#include "PTAnaLedConfig.h"
#include "PTAnaBase.h"
#include "TKey.h"
#include "TList.h"
#include "TIterator.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include <iostream>
#include <string>
#include "TMultiGraph.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TCutG.h"
#include "TGaxis.h"
#include "TLine.h"
#include "TLatex.h"

void draw_ledconsistency(const char* infile, Int_t ampid=5,Int_t voltage=1000)
{
    /*
    gStyle->SetOptTitle(0);
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);
    gStyle->SetFrameBorderMode(0);
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetPadBorderMode(0);
    gStyle->SetFrameLineWidth(2);
    gStyle->SetMarkerSize(1.2);
    gStyle->SetMarkerStyle(20);
    gStyle->SetTickLength(0.04,"xyz");
    gStyle->SetTitleFont(22,"xyz");
    gStyle->SetTitleSize(0.05,"xyz");
    gStyle->SetTitleOffset(1.0,"xyz");
    gStyle->SetLabelFont(132,"xyz");
    gStyle->SetLabelSize(0.05,"xyz");
    gStyle->SetLegendFont(132);

    //gStyle->SetPadGridX(true);gStyle->SetPadGridY(true);
    gStyle->SetPaperSize(20,26);
    gROOT->ForceStyle();
*/
///////
    PTAnaLedConfig ledconfig;
    ledconfig.ReadConfig("./LED_config");
    Int_t gid=ledconfig.fGain[ampid-1][voltage].fGID;
    /////

    TFile* filein=new TFile(infile);

    PTAnaPMTRefRaw* refraw=0;
    TDirectory* dir_ref=filein->GetDirectory("reference");
    if(!dir_ref){
        printf("error!can't get \"reference\" in %s\n",filein->GetName());
        exit(1);
    }
    refraw=(PTAnaPMTRefRaw*)dir_ref->Get("retest_9");
    Double_t ref_raw,ref_calib,testid;
    ref_raw=refraw->fRawDataRef2[gid].fDy8Mean;
    ref_calib=refraw->fLEDCalibData[gid].fDy8Mean;

    TGraph* gr_raw=new TGraph();gr_raw->SetMarkerColor(kRed);
    gr_raw->SetLineColor(kRed);gr_raw->SetLineWidth(2);
    TGraph* gr_calib=new TGraph();gr_calib->SetMarkerColor(kBlue);
    gr_calib->SetLineColor(kBlue);gr_calib->SetLineWidth(2);
    TList *keys=dir_ref->GetListOfKeys();
    TKey *key;
    TIter next(keys);
    Int_t counter=0;
    if(keys){
        while (key=(TKey*)next()) {
            refraw=(PTAnaPMTRefRaw*)key->ReadObj();
            testid=refraw->GetTestID();
            gr_raw->SetPoint(counter,testid+1,refraw->fRawDataRef2[gid].fDy8Mean/ref_raw);
            gr_calib->SetPoint(counter,testid+1,refraw->fLEDCalibData[gid].fDy8Mean/ref_calib);
            counter++;
        }
    }

    delete filein;

    ///
    /// \brief c
    ///
    TMultiGraph* mg=new TMultiGraph();
    mg->Add(gr_raw);mg->Add(gr_calib);
    gr_raw->SetMarkerStyle(22);gr_raw->SetMarkerSize(1.6);
    TH2F* h2axis=new TH2F("h2axis","h2axis",2,0,29,2,0.95,1.05);
    h2axis->GetXaxis()->SetTitle("Test Run ID");
    h2axis->GetXaxis()->SetTitleSize(0.07);
    h2axis->GetXaxis()->CenterTitle();
    h2axis->GetXaxis()->SetLabelSize(0.06);
    h2axis->GetXaxis()->SetTitleOffset(0.9);
    h2axis->GetYaxis()->SetTitle("RefPMT2 ADC counts(Normalized)");
    h2axis->GetYaxis()->CenterTitle();
    h2axis->GetYaxis()->SetNdivisions(505);
    h2axis->GetYaxis()->SetLabelSize(0.06);
    h2axis->GetYaxis()->SetTitleOffset(0.7);
    h2axis->GetYaxis()->SetTitleSize(0.06);


    Int_t cw,ch;
    cw=800;ch=380;
    TCanvas* c=new TCanvas("c","c",cw,ch);
    //c->SetCanvasSize(cx,cy); //method 1
    c->SetWindowSize(cw+(cw-c->GetWw()),ch+(ch-c->GetWh()));
    gPad->SetLeftMargin(0.09);
    gPad->SetTopMargin(0.0);
    gPad->SetRightMargin(0.0);
    gPad->SetBottomMargin(0.13);
    h2axis->Draw();
    mg->Draw("LP");

    TLine *llimit = new TLine();
    llimit->SetLineWidth(2);
    llimit->SetLineStyle(2);
    llimit->DrawLine(0, 1.005, 29, 1.005);
    llimit->DrawLine(0, 0.995, 29, 0.995);

    TLatex *text=new TLatex();
    text->SetTextAlign(21);
    text->SetTextFont(132);
    text->SetTextSize(0.08);
    text->DrawLatex(29/2+2,1.005+0.01,"+0.5%");
    text->SetTextAlign(23);
    text->DrawLatex(29/2+2,0.995-0.01,"-0.5%");

    TLegend* legend1=new TLegend(0.3,0.83,0.8,0.95);
    legend1->AddEntry(gr_raw,"Before Corretion","lp");
    legend1->AddEntry(gr_calib,"After Corretion","lp");
    legend1->SetTextSize(0.05);
    legend1->Draw();


}
