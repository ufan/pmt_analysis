{
  gSystem->AddIncludePath("-I${CMAKE_SOURCE_DIR}/data_structure");
  gSystem->AddIncludePath("-I${CMAKE_SOURCE_DIR}/algorithm");
  gSystem->Load("${CMAKE_BINARY_DIR}/algorithm/libalgorithm.so");
  printf("Load Libraries\n");
}

