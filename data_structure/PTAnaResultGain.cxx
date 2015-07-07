#include "PTAnaResultGain.h"
#include "TAxis.h"
#include <iostream>

PTAnaResultGain::PTAnaResultGain():
    TNamed("NULL","NULL")
{

}

PTAnaResultGain::PTAnaResultGain(const char *serial_no):
    TNamed(serial_no,Form("%s: Relative Gain vs Voltage",serial_no))
{

}

void PTAnaResultGain::AddGraph(TGraphErrors &gr)
{
    Int_t size=fGraphGains.size();
    TGraphErrors newgr=gr;
    newgr.SetName(Form("g%s_amp%d",GetName(),size+1));
    newgr.SetTitle(Form("%s_amp%d: Relative Gain vs Voltage",GetName(),size+1));
    newgr.GetXaxis()->SetTitle("Voltage(V)");
    newgr.GetYaxis()->SetTitle("Relative Gain");

    fGraphGains.push_back(newgr);
}

void PTAnaResultGain::AddFunction(TF1 *f)
{
    Int_t size=fFunctionGains.size();
    TF1 *fpower=new TF1(Form("f%s_amp%d",GetName(),size+1),"[0]*TMath::Power(x,[1])",500,1200);
    fpower->SetParameter(0,f->GetParameter(0));
    fpower->SetParameter(1,f->GetParameter(1));

    fFunctionGains.push_back(fpower);
}

void PTAnaResultGain::DrawGraph(Option_t *option, Int_t ampid)
{
    Int_t size=fGraphGains.size();
    if(ampid>size){
        std::cout<<"Error: out of range:1-"<<size<<std::endl;
        return;
    }

    fGraphGains[ampid-1].Draw(option);
}

void PTAnaResultGain::DrawFunction(Option_t *option, Int_t ampid)
{
    Int_t size=fFunctionGains.size();
    if(ampid>size){
        std::cout<<"Error: out of range:1-"<<size<<std::endl;
        return;
    }

    fFunctionGains[ampid-1]->Draw(option);
}

Double_t PTAnaResultGain::Evaluate(Double_t voltage, Int_t ampid)
{
    Int_t size=fFunctionGains.size();
    if(ampid>size){
        std::cout<<"Error: out of range:1-"<<size<<std::endl;
        return -1;
    }

    return fFunctionGains[ampid-1]->Eval(voltage);
}

Double_t PTAnaResultGain::GetVoltage(Double_t gain, Int_t ampid)
{
    Int_t size=fFunctionGains.size();
    if(ampid>size){
        std::cout<<"Error: out of range:1-"<<size<<std::endl;
        return -1;
    }

    return fFunctionGains[ampid-1]->GetX(gain);
}
