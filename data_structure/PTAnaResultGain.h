#ifndef PTANARESULTGAIN_H
#define PTANARESULTGAIN_H
#include "TGraphErrors.h"
#include "TF1.h"
#include "TNamed.h"
#include <vector>

class PTAnaResultGain : public TNamed
{
public:
    PTAnaResultGain();
    PTAnaResultGain(const char* serial_no);
    ~PTAnaResultGain() {
        int size=fFunctionGains.size();
        for(int i=0;i<size;i++){
            delete fFunctionGains[i];
        }
    }

    void DrawGraph(Option_t* option="",Int_t ampid=1);
    void DrawFunction(Option_t* option="",Int_t ampid=1);
    Double_t Evaluate(Double_t voltage,Int_t ampid=1);
    Double_t GetVoltage(Double_t gain,Int_t ampid=1);

    void AddGraph(TGraphErrors& gr);
    void AddFunction(TF1* f);

private:
    std::vector<TGraphErrors>   fGraphGains;
    std::vector<TF1*>            fFunctionGains;

    ClassDef(PTAnaResultGain,1)
};

#endif // PTANARESULTGAIN_H
