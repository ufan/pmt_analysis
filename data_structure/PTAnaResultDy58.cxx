#include "PTAnaResultDy58.h"
#include "TAxis.h"

PTAnaResultDy58::PTAnaResultDy58():
    TNamed("NULL","NULL"),
    fGraphDy58(7)
{
    fFunctionDy58=new TF1("fpower","[0]*TMath::Power(x,[1])",500,1200);
}

PTAnaResultDy58::PTAnaResultDy58(const char* serial_no):
    TNamed(serial_no,Form("%s: Dy58 vs Voltage",serial_no)),
    fGraphDy58(7)
{
    fFunctionDy58=new TF1(Form("f%s",serial_no),"[0]*TMath::Power(x,[1])",500,1200);

    fGraphDy58.SetName(Form("g%s",serial_no));
    fGraphDy58.SetTitle(Form("%s: Dy58 vs Voltage",serial_no));
    fGraphDy58.GetXaxis()->SetTitle("Voltage(V)");
    fGraphDy58.GetYaxis()->SetTitle("Dy8/Dy5");

}

void PTAnaResultDy58::SetGraph(TGraphErrors &gr)
{

    fGraphDy58=gr;
    fGraphDy58.SetName(Form("g%s",GetName()));
    fGraphDy58.SetTitle(Form("%s: Dy58 vs Voltage",GetName()));
    fGraphDy58.GetXaxis()->SetTitle("Voltage(V)");
    fGraphDy58.GetYaxis()->SetTitle("Dy8/Dy5");

}

void PTAnaResultDy58::SetFunction(TF1 *f)
{
    fFunctionDy58->SetParameter(0,f->GetParameter(0));
    fFunctionDy58->SetParameter(1,f->GetParameter(1));
}

void PTAnaResultDy58::DrawGraph(Option_t *option)
{
    fGraphDy58.Draw(option);
}

void PTAnaResultDy58::DrawFunction(Option_t *option)
{
    fFunctionDy58->Draw(option);
}

Double_t PTAnaResultDy58::Evaluate(Double_t voltage)
{
    return fFunctionDy58->Eval(voltage);
}

Double_t PTAnaResultDy58::GetVoltage(Double_t dy58)
{
    return fFunctionDy58->GetX(dy58);
}

TGraphErrors PTAnaResultDy58::ExtractGraph()
{
    return fGraphDy58;
}

TF1 PTAnaResultDy58::ExtractFunction()
{
    return *fFunctionDy58;
}

/*
void PTAnaResultDy58::SetGraph(TGraphErrors &gr)
{

}

void PTAnaResultDy58::SetFunction(TF1 *f)
{
}

void PTAnaResultDy58::DrawGraph(Option_t *option)
{
}

void PTAnaResultDy58::DrawFunction(Option_t *option)
{
}

Double_t PTAnaResultDy58::Evaluate(Double_t voltage)
{
}

Double_t PTAnaResultDy58::GetVoltage(Double_t dy58)
{
}
*/
