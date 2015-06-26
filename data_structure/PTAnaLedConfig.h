#ifndef PTANALEDCONFIG_H
#define PTANALEDCONFIG_H

#include "TNamed.h"

class PTAnaLedConfig : public TNamed
{
public:
    PTAnaLedConfig():TNamed("LED_config","LED_config") {}
    ~PTAnaLedConfig() {}


    ClassDef(PTAnaLedConfig,1)
};

#endif // PTANALEDCONFIG_H
