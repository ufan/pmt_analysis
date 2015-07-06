#include "PTAnaResultDy58.h"
#include "TAxis.h"

PTAnaResultDy58::PTAnaResultDy58(const char* serial_no):
    TNamed(serial_no,Form("%s: Dy58 vs Voltage",serial_no)),
    fFunctionDy58(serial_no,"[0]*TMath::Power(x,[1])",700,1000)
{
    fGraphDy58.SetName(serial_no);
    fGraphDy58.SetTitle(Form("%s: Dy58 vs Voltage",serial_no));
    fGraphDy58.GetXaxis()->SetTitle("Voltage(V)");
    fGraphDy58.GetYaxis()->SetTitle("Dy8/Dy5");
}

void PTAnaResultDy58::SetGraph(TGraphErrors &gr)
{
    fGraphDy58=gr;
    fGraphDy58.SetName(GetName());
    fGraphDy58.SetTitle(Form("%s: Dy58 vs Voltage",GetName()));
    fGraphDy58.GetXaxis()->SetTitle("Voltage(V)");
    fGraphDy58.GetYaxis()->SetTitle("Dy8/Dy5");
}

void PTAnaResultDy58::SetFunction(TF1 *f)
{
    fFunctionDy58.SetParameter(0,f->GetParameter(0));
    fFunctionDy58.SetParameter(1,f->GetParameter(1));
}

void PTAnaResultDy58::DrawGraph(Option_t *option)
{
    fGraphDy58.DrawClone(option);
}

void PTAnaResultDy58::DrawFunction(Option_t *option)
{
    fFunctionDy58.DrawClone(option);
}

