{
  gSystem->AddIncludePath("-I${CMAKE_SOURCE_DIR}/data_structure");
  gSystem->AddIncludePath("-I${CMAKE_SOURCE_DIR}/algorithm");
  gSystem->Load("${CMAKE_BINARY_DIR}/algorithm/libalgorithm.so");
  printf("Load Libraries\n");


  gStyle->SetOptTitle(0);
  //gStyle->SetOptStat(0);
  //gStyle->SetOptFit(0);
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
}

