#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ functions fit_pedestal;
#pragma link C++ functions fit_pedestal_batch;
#pragma link C++ function check_pedestal;
#pragma link C++ function fit_testingdata;
#pragma link C++ function fit_testingdata_batch;
#pragma link C++ function check_testdata;
#pragma link C++ function get_configurations;
#pragma link C++ function get_references;
#pragma link C++ function calibtest_led;
#pragma link C++ function calibref_led;
#pragma link C++ function fit_dy58;

#pragma link C++ namespace PTAnaBase;
#pragma link C++ class PTAnaLedConfig+;
#pragma link C++ class PTAnaLedConfigEntry+;

#endif
