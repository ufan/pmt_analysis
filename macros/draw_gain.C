#include "PTAnaResultGain.h"
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
#include "PTAnaPMTRefRaw.h"
#include "PTAnaBase.h"
#include "PTAnaPMTRaw.h"
#include "TLatex.h"

void draw_gain(const char* infile,const char* paramfile,bool flag=true)
{
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
////////
    TGraph *g2cb=new TGraph();g2cb->SetTitle("gain vs cathode_blue_sens");
    g2cb->SetMarkerColor(kRed);
    g2cb->SetMarkerStyle(3);
    TGraph *g2ab=new TGraph();g2ab->SetTitle("gain vs anode_blue_sens");
    //g2ab->SetMarkerColor(kBlue);

    Char_t label[10];
    std::string ref_label,tmp_label;
    ref_label="AA2437";
    Double_t cb,ab;
    Double_t ref_cb,ref_ab,ref_gain;
    Int_t chid;
    TFile* fileparam=new TFile(paramfile);
    TTree* tree=(TTree*)fileparam->Get("hamamastu_params");
    tree->SetBranchAddress("Label",label);
    tree->SetBranchAddress("Channel_id",&chid);
    tree->SetBranchAddress("cathode_blue_sens",&cb);
    tree->SetBranchAddress("anode_blue_sens",&ab);

    TFile* filein=new TFile(infile);
    TCutG* cutg=0;
    if(flag)
        cutg=(TCutG*)filein->Get("CUTG");

    PTAnaResultGain *gain=0;
    TDirectory* dir_gain=filein->GetDirectory("gain_result");
    if(!dir_gain){
        printf("error!can't get \"gain_result\" in %s\n",filein->GetName());
        exit(1);
    }
    gain=(PTAnaResultGain*)dir_gain->Get("AA2437");
    ref_gain=gain->Evaluate(818.18,5);

    Int_t counter=0;
    Int_t entries=tree->GetEntries();
    for(int i=0;i<entries;i++){
        tree->GetEntry(i);
        tmp_label=label;
        if(tmp_label == ref_label){
            ref_ab=ab;
            ref_cb=cb;
        }
    }

    for(int i=0;i<entries;i++){
        tree->GetEntry(i);
        gain=(PTAnaResultGain*)dir_gain->Get(label);
        if(flag && !cutg->IsInside(gain->Evaluate(818.18,5),ab*200)){
            g2ab->SetPoint(counter,gain->Evaluate(818.18,5)/ref_gain,ab/ref_ab);
            g2cb->SetPoint(counter,gain->Evaluate(818.18,5)/ref_gain,cb/ref_cb);
            counter++;
        }
        else if(!flag){
            g2ab->SetPoint(counter,gain->Evaluate(818.18,5)/ref_gain,ab/ref_ab);
            g2cb->SetPoint(counter,gain->Evaluate(818.18,5)/ref_gain,cb/ref_cb);
            counter++;
        }
    }

    delete filein;

    TH2F* h2axis=new TH2F("h2axis","h2axis",2,0,6.5,2,0,3);
    h2axis->GetXaxis()->SetTitle("Normalized Gain");
    h2axis->GetXaxis()->CenterTitle();
    h2axis->GetYaxis()->SetTitle("Normalized Anode Blue Sens. Index");
    h2axis->GetYaxis()->CenterTitle();
    TGaxis* gaxis=new TGaxis(3.5,0,3.5,3,0.,3.,510,"+");
    gaxis->SetLineColor(kRed);
    gaxis->SetLineWidth(2);
    gaxis->SetTitleColor(kRed);
    gaxis->SetTitle("Normalized Cathode Blue Sens. Index");
    gaxis->CenterTitle();
    gaxis->SetTitleFont(22);
    gaxis->SetTitleSize(0.05);
    //gaxis->SetTitleOffset(0.5);
    gaxis->SetLabelFont(130);
    gaxis->SetLabelColor(kRed);
    gaxis->SetLabelSize(0.05);
    std::cout<<gaxis->GetLabelOffset()<<std::endl;
    gaxis->SetLabelOffset(0.043);

    TMultiGraph* mg=new TMultiGraph();
    mg->Add(g2cb);mg->Add(g2ab);

    TF1* fline=new TF1("fline","pol1",0,3);
    fline->SetParameters(0,1);

    TCanvas* c1=new TCanvas("c1");
    c1->SetGridx();c1->SetGridy();
    h2axis->Draw();
    gaxis->Draw();
    mg->Draw("*");
    fline->Draw("same");

}

void draw_gain_chrange(const char* fitfile,const char* infile,const char* paramfile)
{
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
    ////////
    /// \brief g2cb
    ///
    int chlow=6;
    int chhigh=9;
    int chtmp=17;

    TFile *filefit=new TFile(fitfile);
    PTAnaPMTRaw *testraw=0;
    TDirectory* dir_raw=filefit->GetDirectory("raw");
    if(!dir_raw){
        printf("error!can't get \"raw\" in %s\n",filefit->GetName());
        exit(1);
    }

    TGraph *g2cb=new TGraph();g2cb->SetTitle("gain vs cathode_blue_sens");
    g2cb->SetMarkerColor(kRed);
    g2cb->SetMarkerStyle(3);
    TGraph *g2ab=new TGraph();g2ab->SetTitle("gain vs anode_blue_sens");
    //g2ab->SetMarkerColor(kBlue);

    Char_t label[10];
    std::string ref_label,tmp_label;
    ref_label="AA2437";
    Double_t cb,ab;
    Double_t ref_cb,ref_ab,ref_gain;
    Int_t chid;
    TFile* fileparam=new TFile(paramfile);
    TTree* tree=(TTree*)fileparam->Get("hamamastu_params");
    tree->SetBranchAddress("Label",label);
    tree->SetBranchAddress("Channel_id",&chid);
    tree->SetBranchAddress("cathode_blue_sens",&cb);
    tree->SetBranchAddress("anode_blue_sens",&ab);

    TFile* filein=new TFile(infile);

    PTAnaResultGain *gain=0;
    TDirectory* dir_gain=filein->GetDirectory("gain_result");
    if(!dir_gain){
        printf("error!can't get \"gain_result\" in %s\n",filein->GetName());
        exit(1);
    }
    gain=(PTAnaResultGain*)dir_gain->Get("AA2437");
    ref_gain=gain->Evaluate(818.18,5);

    Int_t counter=0;
    Int_t entries=tree->GetEntries();
    for(int i=0;i<entries;i++){
        tree->GetEntry(i);
        tmp_label=label;
        if(tmp_label == ref_label){
            ref_ab=ab;
            ref_cb=cb;
        }
    }

    for(int i=0;i<entries;i++){
        tree->GetEntry(i);
        testraw=(PTAnaPMTRaw*)dir_raw->Get(label);
        gain=(PTAnaResultGain*)dir_gain->Get(label);

        if(gain->Evaluate(818.18,5)/ref_gain > 3){
            if((testraw->GetChannel()>=chlow && testraw->GetChannel()<=chhigh) || testraw->GetChannel()==chtmp ){
                continue;
            }
            if(gain->Evaluate(818.18,5)/ref_gain > 4){
                continue;
            }
        }

        g2ab->SetPoint(counter,gain->Evaluate(818.18,5)/ref_gain,ab/ref_ab);
        g2cb->SetPoint(counter,gain->Evaluate(818.18,5)/ref_gain,cb/ref_cb);
        counter++;
    }
    std::cout<<counter<<std::endl;
    delete filein;

    TH2F* h2axis=new TH2F("h2axis","h2axis",2,0.2,4.1,2,0.4,2.6);
    h2axis->GetXaxis()->SetTitle("Normalized G_{relative} at Light Intensity 5");
    h2axis->GetXaxis()->CenterTitle();
    h2axis->GetXaxis()->SetTitleOffset(1.2);
    h2axis->GetYaxis()->SetTitle("Normalized Anode Blue Sens. Index");
    h2axis->GetYaxis()->CenterTitle();
    h2axis->GetYaxis()->SetTitleOffset(1.2);
    TGaxis* gaxis=new TGaxis(4.1,0.4,4.1,2.6,0.4,2.6,510,"+");
    gaxis->SetLineColor(kRed);
    gaxis->SetLineWidth(2);
    gaxis->SetTitleColor(kRed);
    gaxis->SetTitle("Normalized Cathode Blue Sens. Index");
    gaxis->CenterTitle();
    gaxis->SetTitleFont(22);
    gaxis->SetTitleSize(0.05);
    //gaxis->SetTitleOffset(0.5);
    gaxis->SetLabelFont(130);
    gaxis->SetLabelColor(kRed);
    gaxis->SetLabelSize(0.05);
    std::cout<<gaxis->GetLabelOffset()<<std::endl;
    gaxis->SetLabelOffset(0.053);
    gaxis->SetTitleOffset(1.2);

    TMultiGraph* mg=new TMultiGraph();
    mg->Add(g2cb);
    mg->Add(g2ab);

    TF1* fline=new TF1("fline","pol1",0.5,2.5);
    fline->SetParameters(0,1);
    fline->SetLineColor(kBlue);
    fline->SetLineWidth(2);

    Int_t cw,ch;
    cw=700;ch=500;
    TCanvas* c=new TCanvas("c","c",cw,ch);
    c->SetWindowSize(cw+(cw-c->GetWw()),ch+(ch-c->GetWh()));
    gPad->SetLeftMargin(0.12);
    gPad->SetTopMargin(0.0);
    gPad->SetRightMargin(0.12);
    gPad->SetBottomMargin(0.12);
    c->SetGridx();c->SetGridy();

    h2axis->Draw();
    gaxis->Draw();
    mg->Draw("*");
    fline->SetLineWidth(3);
    fline->Draw("same");

    TLatex *text=new TLatex();
    text->SetTextAlign(21);
    text->SetTextFont(132);
    text->SetTextSize(0.06);
    text->SetTextColor(kBlue);
    text->DrawLatex(1,2.3,Form("Slope=1.0"));

    delete filefit;
}

void draw_gain_normalized(const char* infile,const char* paramfile,bool flag=true)
{
    TH1F* hist=new TH1F("hist","hist",25,0.5,25.5);
    TH1F* h1gain=new TH1F("h1gain","h1gain",100,400,6000);
    TH1F* h1al=new TH1F("h1al","h1al",100,20,120);
    TH1F* h1ab=new TH1F("h1ab","h1ab",100,4,14);
    TH2F* h2cl=new TH2F("h2cl","gain vs cathode_lumi_sens",25,400,6000,25,40,80);
    TH2F* h2al=new TH2F("h2al","gain vs anode_lumi_sens",100,400,4000,100,400,4000);
    TH2F* h2cb=new TH2F("h2cb","gain vs cathode_blue_sens",25,400,6000,25,5.5,8.5);
    TH2F* h2ab=new TH2F("h2ab","gain vs anode_blue_sens",100,400,4000,100,400,4000);
    TGraph *g2al=new TGraph();g2al->SetTitle("gain vs anode_lumi_sens");
    TGraph *g2ab=new TGraph();g2ab->SetTitle("gain vs anode_blue_sens");

    Char_t label[10];
    Double_t cl,al,cb,ab;
    Int_t chid;
    TFile* fileparam=new TFile(paramfile);
    TTree* tree=(TTree*)fileparam->Get("hamamastu_params");
    tree->SetBranchAddress("Label",label);
    tree->SetBranchAddress("Channel_id",&chid);
    tree->SetBranchAddress("cathode_lumi_sens",&cl);
    tree->SetBranchAddress("anode_lumi_sens",&al);
    tree->SetBranchAddress("cathode_blue_sens",&cb);
    tree->SetBranchAddress("anode_blue_sens",&ab);

    TFile* filein=new TFile(infile);

    TCutG* cutg=0;
    PTAnaResultGain *gain=0;
    TDirectory* dir_gain=filein->GetDirectory("gain_result");
    if(!dir_gain){
        printf("error!can't get \"gain_result\" in %s\n",filein->GetName());
        exit(1);
    }

    if(flag)
        cutg=(TCutG*)filein->Get("CUTG2");

    Int_t counter=0;
    Int_t entries=tree->GetEntries();
    for(int i=0;i<entries;i++){
        tree->GetEntry(i);
        gain=(PTAnaResultGain*)dir_gain->Get(label);
        /*
        if(gain->Evaluate(818.18,5) > 3500){
            //hist->Fill(gain->Evaluate(818.18,5));
            hist->Fill(chid);
        }
        */
        //else{
        //if(chid !=6 && chid!=7 && chid!=8 && chid!=9){
            h2cl->Fill(gain->Evaluate(818.18,5),cl);
            h2al->Fill(gain->Evaluate(818.18,5),al*30);
            h2cb->Fill(gain->Evaluate(818.18,5),cb);
            //h2ab->Fill(gain->Evaluate(818.18,5),ab*200);

        if(flag && cutg->IsInside(gain->Evaluate(818.18,5),ab*200)){
            hist->Fill(chid);
            std::cout<<label<<std::endl;
        }
        else{
            g2ab->SetPoint(counter,gain->Evaluate(818.18,5),ab*200);
            counter++;
        }


        //}
        h1gain->Fill(gain->Evaluate(818.18,5));
        g2al->SetPoint(counter,gain->Evaluate(818.18,5),al*30);

    }

    delete filein;

    TCanvas* c1=new TCanvas("c1");
    hist->Draw();
    TCanvas* c2=new TCanvas("c2","c2");
    //c2->Divide(2,1);
    //c2->cd(1);h2cl->Draw("colz");
    //c2->cd(2);g2al->Draw("A*");//h2al->Draw("colz");
    //c2->cd(1);h2cb->Draw("colz");
    c2->cd(1);g2ab->Draw("A*");//h2ab->Draw("colz");
}

TH1F* get_normalized(const char* fitfile,const char* infile,double voltage, int ampid=5)
{
    int chlow=6;
    int chhigh=9;
    int chtmp=17;

    TFile *filefit=new TFile(fitfile);
    PTAnaPMTRaw *testraw=0;
    TDirectory* dir_raw=filefit->GetDirectory("raw");
    if(!dir_raw){
        printf("error!can't get \"raw\" in %s\n",filefit->GetName());
        exit(1);
    }

    TH1F* hall=new TH1F(Form("hall_%d",ampid),Form("hall_%d",ampid),100,0,6.1);

    TFile* filein=new TFile(infile);

    TDirectory* dir_gain=filein->GetDirectory("gain_result");
    if(!dir_gain){
        printf("error!can't get \"gain_result\" in %s\n",filein->GetName());
        exit(1);
    }
    PTAnaResultGain *gain=0;
    gain=(PTAnaResultGain*)dir_gain->Get("AA2483");
    Double_t refgain=gain->Evaluate(voltage,ampid);

    TList *keys=dir_gain->GetListOfKeys();
    TKey *key;
    TIter next(keys);
    if(keys){
        while (key=(TKey*)next()) {
            gain=(PTAnaResultGain*)key->ReadObj();
            testraw=(PTAnaPMTRaw*)dir_raw->Get(gain->GetName());

            if(gain->Evaluate(voltage,ampid)/refgain > 3){
                if((testraw->GetChannel()>=chlow && testraw->GetChannel()<=chhigh) || testraw->GetChannel()==chtmp ){
                    continue;
                }
                if(gain->Evaluate(voltage,ampid)/refgain > 5.5){
                    continue;
                }
            }
            hall->Fill(gain->Evaluate(voltage,ampid)/refgain);
        }
    }

    delete filein;

    return hall;
}

TGraph* get_normalized_correl(const char* fitfile, const char* infile,double voltage, int ampidx=1, int ampidy=5)
{
    int chlow=6;
    int chhigh=9;
    int chtmp=17;

    TFile *filefit=new TFile(fitfile);
    PTAnaPMTRaw *testraw=0;
    TDirectory* dir_raw=filefit->GetDirectory("raw");
    if(!dir_raw){
        printf("error!can't get \"raw\" in %s\n",filefit->GetName());
        exit(1);
    }

    TGraph* gr=new TGraph();

    TFile* filein=new TFile(infile);

    TDirectory* dir_gain=filein->GetDirectory("gain_result");
    if(!dir_gain){
        printf("error!can't get \"gain_result\" in %s\n",filein->GetName());
        exit(1);
    }
    PTAnaResultGain *gain=0;
    gain=(PTAnaResultGain*)dir_gain->Get("AA2483");
    Double_t refgainx,refgainy;
    refgainx=gain->Evaluate(voltage,ampidx);
    refgainy=gain->Evaluate(voltage,ampidy);

    TList *keys=dir_gain->GetListOfKeys();
    TKey *key;
    TIter next(keys);
    Int_t counter=0;
    if(keys){
        while (key=(TKey*)next()) {
            gain=(PTAnaResultGain*)key->ReadObj();
            testraw=(PTAnaPMTRaw*)dir_raw->Get(gain->GetName());

            if(gain->Evaluate(voltage,ampidx)/refgainx > 3){
                if((testraw->GetChannel()>=chlow && testraw->GetChannel()<=chhigh) || testraw->GetChannel()==chtmp ){
                    continue;
                }
                if(gain->Evaluate(voltage,ampidx)/refgainx > 4){
                    continue;
                }
            }

            gr->SetPoint(counter,gain->Evaluate(voltage,ampidx)/refgainx,gain->Evaluate(voltage,ampidy)/refgainy);
            counter++;
        }
    }

    delete filein;

    return gr;
}

void draw_gain_all(const char* fitfile, const char* infile, int voltage)
{
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

    TH1F* h5=get_normalized(fitfile,infile,voltage,5);
    h5->SetLineColor(kBlue);
    h5->SetFillStyle(3004);
    h5->SetFillColor(kBlue);
    h5->GetXaxis()->SetTitle("Normalized G_{relative} at Light Intensity 5");
    h5->GetXaxis()->CenterTitle();
    h5->GetYaxis()->SetTitle("Number of PMTs");
    h5->GetYaxis()->CenterTitle();
    TH1F* h1=get_normalized(fitfile,infile,voltage,1);
    h1->SetLineColor(kRed);

    TCanvas *cf=new TCanvas("cf");
    h5->Draw();
    h1->Draw("same");

    TLegend* legend1=new TLegend(0.3,0.83,0.8,0.95);
    legend1->AddEntry(h1,"Light Intensity 1","l");
    legend1->AddEntry(h5,"Light Intensity 5","l");
    legend1->SetTextSize(0.05);
    legend1->Draw();

    TGraph* gr=get_normalized_correl(fitfile,infile,voltage,1,5);
    gr->GetXaxis()->SetTitle("Normalized Gain from Light Intensity 1");
    gr->GetYaxis()->SetTitle("Normalized Gain from Light Intensity 5");

    Int_t cw,ch;
    cw=700;ch=500;
    TCanvas* cg=new TCanvas("cg","cg",cw,ch);
      //c->SetCanvasSize(cx,cy); //method 1
      cg->SetWindowSize(cw+(cw-cg->GetWw()),ch+(ch-cg->GetWh()));
        gPad->SetLeftMargin(0.09);
        gPad->SetTopMargin(0.0);
        gPad->SetRightMargin(0.0);
        gPad->SetBottomMargin(0.12);
    h5->GetXaxis()->SetTitleOffset(1.1);
    h5->GetYaxis()->SetTitleOffset(0.95);
    h5->GetYaxis()->SetNdivisions(508);
    h5->Draw();

    TCanvas* c=new TCanvas("c","c",1000,500);
    c->Divide(2,1);
    c->cd(1);
    h1->Draw();
    //legend1->Draw();
    //h5->Draw("same");
    c->cd(2);
    gPad->SetGridx();
    gPad->SetGridy();
    gr->Draw("A*");
}

void draw_example(const char* infile)
{
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

    TFile* filein=new TFile(infile);

    TDirectory* dir_gain=filein->GetDirectory("gain_result");
    if(!dir_gain){
        printf("error!can't get \"gain_result\" in %s\n",filein->GetName());
        exit(1);
    }

    PTAnaResultGain *large=(PTAnaResultGain*)dir_gain->Get("AA2163");
    TGraphErrors* gr_large=new TGraphErrors(large->ExtractGraph(5));
    TF1* f_large=new TF1(large->ExtractFunction(5));
    PTAnaResultGain *small=(PTAnaResultGain*)dir_gain->Get("AA2336");
    TGraphErrors* gr_small=new TGraphErrors(small->ExtractGraph(5));
    TF1* f_small=new TF1(small->ExtractFunction(5));

    TMultiGraph* mg=new TMultiGraph();
    mg->Add(gr_large);mg->Add(gr_small);
    TCanvas *c=new TCanvas("c");
    c->SetLogy();
    c->SetLogx();
    c->SetGridx();
    c->SetGridy();
    mg->Draw("AP");

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

    Int_t cw,ch;
    cw=700;ch=500;
    TCanvas* c=new TCanvas("c","c",cw,ch);
      //c->SetCanvasSize(cx,cy); //method 1
      c->SetWindowSize(cw+(cw-c->GetWw()),ch+(ch-c->GetWh()));
        gPad->SetLeftMargin(0.13);
        gPad->SetTopMargin(0.0);
        gPad->SetRightMargin(0.0);
        gPad->SetBottomMargin(0.12);
        gPad->SetGridx();gPad->SetGridy();
        gre->GetXaxis()->SetTitle("Ref PMT1(ADC counts)");
        gre->GetXaxis()->CenterTitle();
        gre->GetXaxis()->SetTitleOffset(1.2);
        //gre->GetXaxis()->SetNdivisions(505);
        gre->GetYaxis()->SetTitle("Ref PMT2(ADC counts)");
        gre->GetYaxis()->CenterTitle();
        gre->GetYaxis()->SetNdivisions(505);
        gre->GetYaxis()->SetTitleOffset(1.3);

    gre->Draw("AP");
    gre->Fit("pol2");
}

void print_gain(const char* infile,int llimit,int hlimit,double voltage=1000,double ampid=5)
{
    TFile* filein=new TFile(infile);

    TDirectory* dir_gain=filein->GetDirectory("gain_result");
    if(!dir_gain){
        printf("error!can't get \"gain_result\" in %s\n",filein->GetName());
        exit(1);
    }
    PTAnaResultGain *gain=0;
    gain=(PTAnaResultGain*)dir_gain->Get("AA2483");
    Double_t refgain=gain->Evaluate(voltage,ampid);

    TList *keys=dir_gain->GetListOfKeys();
    TKey *key;
    TIter next(keys);
    if(keys){
        while (key=(TKey*)next()) {
            gain=(PTAnaResultGain*)key->ReadObj();

            if(gain->Evaluate(voltage,ampid)/refgain > llimit && gain->Evaluate(voltage,ampid)/refgain < hlimit){
                std::cout<<gain->GetName()<<std::endl;
            }
        }
    }

    delete filein;
}

void print_parameters(const char* infile,int par_id=1)
{
    TH1F* hpara=new TH1F("hpara","hpara",100,0,10);

    TFile* filein=new TFile(infile);

    TDirectory* dir_gain=filein->GetDirectory("gain_result");
    if(!dir_gain){
        printf("error!can't get \"gain_result\" in %s\n",filein->GetName());
        exit(1);
    }
    PTAnaResultGain *gain=0;
    TF1 ftemp;

    TList *keys=dir_gain->GetListOfKeys();
    TKey *key;
    TIter next(keys);
    if(keys){
        while (key=(TKey*)next()) {
            gain=(PTAnaResultGain*)key->ReadObj();
            ftemp=gain->ExtractFunction(5);

            hpara->Fill(ftemp.GetParameter(par_id));
        }
    }

    delete filein;

    hpara->Draw();
}
