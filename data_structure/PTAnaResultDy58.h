#ifndef PTANARESULTDY58_H
#define PTANARESULTDY58_H
#include "TGraphErrors.h"
#include "TF1.h"

class PTAnaResultDy58 : public TNamed
{
public:
    PTAnaResultDy58() {}
    PTAnaResultDy58(const char* serial_no);
    ~PTAnaResultDy58() {}

    void DrawGraph(Option_t* option="");
    void DrawFunction(Option_t* option="");
    Double_t Evaluate(Double_t voltage);

    void SetGraph(TGraphErrors& gr);
    void SetFunction(TF1* f);

private:
    TGraphErrors    fGraphDy58;
    TF1             fFunctionDy58;

    ClassDef(PTAnaResultDy58,1)
};

#endif // PTANARESULTDY58_H
