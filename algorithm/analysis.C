#include "stdio.h"
#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include "TGraph.h"
#include "TList.h"
#include "TCollection.h"
#include "TString.h"
#include "TSystemFile.h"
#include "TSystemDirectory.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TSpectrum.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TSQLServer.h"
#include "TMath.h"



/****************Global Variables*********************************************/
//---------------PMT testing system configuration info-------------------------------------------------------------
const Int_t CHANNEL_NUM=25;
const Int_t RefPMT_NO=2;
const Int_t RefPMT_CH[RefPMT_NO]={1,2};//0-->Channel_1  ,1-->Channel_2
const Int_t TestPMT_NO=23;
const Int_t TestPMT_CH[TestPMT_NO]={3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25};
const Int_t HIGHVOLTAGE_STEP=7;
const Int_t VOLTAGE[HIGHVOLTAGE_STEP]={1000,950,900,850,800,750,700};
const Int_t MAXIMUM_DATAPOINTS=25;
const Int_t GAIN_FILENO=5;
const Int_t GAIN_FILEINDEX[HIGHVOLTAGE_STEP][GAIN_FILENO]={{1,2,3,4,5},{1,2,3,4,5},{17,18,19,20,21}
                           ,{17,18,19,20,21},{17,18,19,20,21},{17,18,19,20,21}
                           ,{17,18,19,20,21}};
const Int_t DY58_FILENO[HIGHVOLTAGE_STEP]={15,15,16,16,16,16,16};
const Int_t TOTAL_FILENO[HIGHVOLTAGE_STEP]={15,15,21,21,21,21,21};
const Int_t GROUP1_FILENO[HIGHVOLTAGE_STEP]={10,10,11,11,11,11,11};
const Int_t GROUP1_FILEINDEX[HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS]={{1,2,3,4,5,6,7,8,9,10},{1,2,3,4,5,6,7,8,9,10},{1,2,3,4,5,6,7,8,9,10,11},{1,2,3,4,5,6,7,8,9,10,11}
                                                                   ,{1,2,3,4,5,6,7,8,9,10,11},{1,2,3,4,5,6,7,8,9,10,11},{1,2,3,4,5,6,7,8,9,10,11}};
const Int_t GROUP2_FILENO[HIGHVOLTAGE_STEP]={5,5,5,5,5,5,5};
const Int_t GROUP2_FILEINDEX[HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS]={{11,12,13,14,15},{11,12,13,14,15},{12,13,14,15,16},{12,13,14,15,16}
                                                                   ,{12,13,14,15,16},{12,13,14,15,16},{12,13,14,15,16}};
const Int_t GROUP3_FILENO[HIGHVOLTAGE_STEP]={5,5,5,5,5,5,5};
const Int_t GROUP3_FILEINDEX[HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS]={{1,2,3,4,5},{1,2,3,4,5},{17,18,19,20,21},{17,18,19,20,21},{17,18,19,20,21}
                                                                   ,{17,18,19,20,21},{17,18,19,20,21}};
const Float_t CHANNEL_DIFF[CHANNEL_NUM]={1.0,1.2775765809,1.121589502,0.9579736337,1.0262718486,1.4080747544,1.9571727843,1.7562655577,1.3086306669,0.913844951,1.072783158,
                                         0.9156117824,1.1635673492,1.1189168228,1.0662695461,1.124735192,1.1650342171,1.0904545829,1.0628654897,1.0426446949,1.0697705836,
                                         0.9285420795,0.973728507,0.9789894741,1.0};
//-----------------------------------------------------------------------------------------------------------------

//---------------PMT testing channel configuration info-------------------------------------------------------------
Int_t TEST_CH[CHANNEL_NUM];
Int_t PMT_INDEX[CHANNEL_NUM];//this array is kept for later extendence.PMT_INDEX is the unique number associated with each pmt testing file.
Char_t PMT_LABEL[CHANNEL_NUM][50];
Char_t PMT_POSITION[CHANNEL_NUM][50];//the position of pmt in the box
Int_t FEE_DY5[CHANNEL_NUM],FEE_DY8[CHANNEL_NUM];
Int_t CONFIG_FLAG=0;//if the configuration.txt file has been read CONFIG_FLAG equals to 1,otherwise 0;
//------------------------------------------------------------------------------------------------------------------

//---------------Global Testing-Data buffer---------------------------------------------------------------------------------------
Float_t Data_Dy5[CHANNEL_NUM][HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS];
Float_t Data_Dy8[CHANNEL_NUM][HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS];
Float_t Sigma_Dy5[CHANNEL_NUM][HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS];
Float_t Sigma_Dy8[CHANNEL_NUM][HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS];

Float_t Data_Gain[CHANNEL_NUM][GAIN_FILENO][HIGHVOLTAGE_STEP];
Float_t Sigma_Data_Gain[CHANNEL_NUM][GAIN_FILENO][HIGHVOLTAGE_STEP];
Float_t Calibrated_Data_Gain_Step1[CHANNEL_NUM][GAIN_FILENO][HIGHVOLTAGE_STEP];
Float_t Calibrated_Data_Gain_Step2[CHANNEL_NUM][GAIN_FILENO][HIGHVOLTAGE_STEP];
Int_t DATAUPDATE_FLAG=0;//indicate whether there is valid data in buffer
Int_t TestDirectory_id=0;//indicate which test's data is in buffer

//Float_t Ref_Dy5[RefPMT_NO][HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS];
Float_t Ref_Dy8[RefPMT_NO][HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS];
Float_t Calib_Ratio[RefPMT_NO][HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS];
Int_t REFDATAUPDATE_FLAG=0;
Int_t RefDirectory_id=0;
//-----------------All kinds of flags-----------------------------------------------------------------------------------------------------

enum RefPMT {AA2236=1,AA2233=2};
//RefPMT LED_CalibPMT=AA2236;
//RefPMT Channel_CalibPMT=AA2104;

//------------------MySQL info-------------------------------------------------------
//Char_t dbname[100]="mysql://192.168.1.110/PMT_V2";
Char_t dbname[100]="mysql://localhost/PMT_V2";
Char_t username[20]="root";
Char_t password[20]="111111";
/****************Global Functions*********************************************/

///////////////////////////////////////////////////////////////////////////
//func: convert_64
//      converting PMT testing rawdata into root files.Suitable for 64 channel FEE.
//input:
//      parentDir: the directory containing all the files,i.e. the data home directory
//      voltage: the measurement voltage of the PMT high-volt
//      index:  the amp-index of LED/the index of the testing rawdata files.
//output:
//      0 if everything works well,otherwise -1.
////////////////////////////////////////////////////////////////////////////////
int convert_64(const Char_t* parentDir,Int_t voltage,Int_t index)
{  
  Char_t outfile[200],infname[200];
  sprintf(outfile,"%s/root_file/%dV/%d.root",parentDir,voltage,index);
  TFile *f=new TFile(outfile,"CREATE");
  if(f->IsZombie()){
      printf("this file will not been recreated!\n");
      delete f;
      return 0;
  }
  else if(f->IsOpen()){
    ifstream infile;
    sprintf(infname,"%s/raw_data/%dV/%d.dat",parentDir,voltage,index);
    infile.open(infname,std::ios_base::in | std::ios_base::binary);
    if(!infile.is_open()){
        std::cout << "can't open"<< infname << std::endl;
    }

    int Ch_count=0;
    int Ev_count=0;
    int Ch_tmp,Ch_tmp1,Ch_tmp2;
    int Ch_ID;
    int data,Tmp1,Tmp2,chn_board1,chn_board2,datatmp[64],m;
    unsigned char x;
    TString hname,htitle;

    TH1F *h[64];
    for(int i=0;i<64;i++){
        hname=TString::Format("h%d",i+1);
        htitle=TString::Format("h%d",i+1);
        h[i]=new TH1F(hname,htitle,17400,-1000,16400);
    }
 
    TTree *tree = new TTree("PMT","PMT Testing");
    tree->Branch("event",datatmp,"hh[64]/I");

    chn_board1=0;
    chn_board2=0;
    Tmp1=0;
    Tmp2=0;

    int err8000=0;
    
    while(!infile.eof())
    {
    //for head
        x=infile.get();
        //printf("%d\n",x);
        if(x==0xeb)
        {
            x=infile.get();
           
            if(x==0x90)
            {
                x=infile.get();
                if(Ev_count == 0)
                {
                    m = x & 0xFF;
                    std::cout<<"The event ID starts at "<< m << std::endl;
                }
                 
                if(x==0x00)
                    m=0;

                if(x!=(m&0x00ff))
                {
                    //std::cout<<"事件编号错误  m =  "<<m<<"       x=  "<<x<<" at event "<<Ev_count+1<<std::endl;
                    m = x&0x00ff;
                }
                m=m+1;
                if(m==256)
                    m=0;

                x=infile.get();

                for(int i=0;i<64;i++)
                {
                    datatmp[i]=0;
                }


                if(x==0x20)  //"Normal mode"
                {
                    x=infile.get();
                    Ch_tmp2=((x&0x00ff)<<8);
                    x=infile.get();
                    Ch_tmp1=(x&0x00ff);
                    Ch_tmp=Ch_tmp1+Ch_tmp2;
                    Ch_count=(Ch_tmp-6)/2;

                    for(int i=0;i<64;i++)
                    {
                        x=infile.get();
                        Tmp2=((x&0x00ff)<<8);
                        x=infile.get();
                        Tmp1=(x&0x00ff);
                        data=Tmp1+Tmp2;
                        if(data>0&&data<16384)
                        datatmp[i]=data;
                    else
            		{
                        err8000++;
                        std::cout<<"0x8000 error "<<err8000<<" occured at event "<<Ev_count+1<<" "<<data<<std::endl;
            		}
                }

                for(int i=0;i<2;i++)
                {

                    x=infile.get();
                    if(x!=0x00)
                        std::cout<<"数据包长度不对! Event "<<Ev_count+1<<std::endl;
                    
                }
                for(int k=0;k<18;k++)
                    x=infile.get();
              }
              else if(x==0x60)
              {
                x=infile.get();
                   // std::cout<<"x = "<<x;
                Ch_tmp2=((x&0x00ff)<<8);
                x=infile.get();
                Ch_tmp1=(x&0x00ff);
                Ch_tmp=Ch_tmp1+Ch_tmp2;
                Ch_count=(Ch_tmp-6)/3;
                for(int i=0;i<Ch_count;i++)
                {      
                  x=infile.get();
                  Ch_ID= (x&0x00ff);
                  x=infile.get();
                  Tmp2=((x&0x00ff)<<8);
                  x=infile.get();
                  Tmp1=(x&0x00ff);
                  data=Tmp1+Tmp2;
                  if(data>0&&data<16384)
                  datatmp[Ch_ID]=data;
                  else
                  err8000++;
                    }                   
               for(int i=0;i<2;i++)
                {

                 x=infile.get();
                 if(x!=0x00)
                 std::cout<<"数据包长度不对! Event "<<Ev_count+1<<std::endl;

                }
                for(int k=0;k<18;k++)
                    x=infile.get();
            }
            else
            {
                continue;
            }
////////////////////////////////////////////////
 
              for(int ii=0;ii<64;ii++)
              {
					h[ii]->Fill((float)datatmp[ii]);
              }        
             
////////////////////////////////////////////////    
               
        tree->Fill();
        Ev_count++;
        if(Ev_count%5000 == 0)
        std::cout<<Ev_count<<" events have been converted!"<<std::endl;
        }
  
      }

  }

    std::cout<<Ev_count<<" events converted totally!"<<std::endl;
    std::cout<<err8000<<" data have been found to be 0x8000 (or 0x00)!"<<std::endl;
    infile.close();
    f->Write();
    f->Close();
    delete f;
  }
  else{
      printf("error: %s can not be created!\n",outfile);
      delete f;
      return -1;
  }

  return 0;
}

/////////////////////////////////////////////////////////////////////
//func: convert_pedestal
//input:
//      parentDir:the directory containing all the files,i.e. the data home directory
//      infile: the name of the pedestal data file,including ".dat"
//      outname: the name of the output root file,"begin" or "end"
//output:
//      0 if everything works well,otherwise -1.
/////////////////////////////////////////////////////////////////////
int convert_pedestal(const Char_t *parentDir,const Char_t *rawfilename,const Char_t *outname)
{
    Char_t outfile[200],infname[200];
    sprintf(outfile,"%s/root_file/pedestal/%s.root",parentDir,outname);
    printf("%s\n",outfile);
    TFile *f=new TFile(outfile,"CREATE");
    if(f->IsZombie()){
        printf("this file will not been recreated!\n");
        delete f;
        return 0;
    }
    else if(f->IsOpen()){
        ifstream infile;
        sprintf(infname,"%s/raw_data/pedestal/%s",parentDir,rawfilename);
        printf("%s\n",infname);
    infile.open(infname,std::ios_base::in | std::ios_base::binary);

    int Ch_count=0;
    int Ev_count=0;
    int Ch_tmp,Ch_tmp1,Ch_tmp2;
    int Ch_ID;
    int x,data,Tmp1,Tmp2,chn_board1,chn_board2,datatmp[64],m;
    TString hname,htitle;
    int datasum = 0;

    TH1F *h[64];
    for(int i=0;i<64;i++){
        hname=Form("h%d",i+1);
        htitle=Form("h%d",i+1);
        h[i]=new TH1F(hname,htitle,17400,-1000,16400);
    }

    TTree *tree = new TTree("PMT","PMT Testing");
    tree->Branch("event",datatmp,"hh[64]/I");

    chn_board1=0;
    chn_board2=0;
    Tmp1=0;
    Tmp2=0;

    int err8000=0;

    while(!infile.eof())
    {
      //for head
        x=infile.get();
        if(x==0xeb)
        {
           x=infile.get();

            if(x==0x90)
            {
                x=infile.get();
                if(Ev_count == 0)
                {
                   m = x & 0xFF;
                   std::cout<<"The event ID starts at "<<m<<std::endl;
                }

                if(x==0x00)
                  m=0;

                if(x!=(m&0x00ff))
                {
                   //std::cout<<"事件编号错误  m =  "<<m<<"       x=  "<<x<<" at event "<<Ev_count+1<<std::endl;
                  m = x&0x00ff;
                }
                m=m+1;
                if(m==256)
                m=0;

                x=infile.get();

                  for(int i=0;i<64;i++)
                  {
                      datatmp[i]=0;
                  }


                if(x==0x20)  //"Normal mode"
                {
                  x=infile.get();
                  Ch_tmp2=((x&0x00ff)<<8);
                  x=infile.get();
                  Ch_tmp1=(x&0x00ff);
                  Ch_tmp=Ch_tmp1+Ch_tmp2;
                  Ch_count=(Ch_tmp-6)/2;

                  for(int i=0;i<64;i++)
                  {
                      x=infile.get();
                      Tmp2=((x&0x00ff)<<8);
                      x=infile.get();
                      Tmp1=(x&0x00ff);
                      data=Tmp1+Tmp2;
                      if(data>0&&data<16384)
                      datatmp[i]=data;
                      else
                      {
                      err8000++;
  //                  std::cout<<"0x8000 error "<<err8000<<" occured at event "<<Ev_count+1<<" "<<data<<std::endl;
                      }
                  }

                  for(int i=0;i<2;i++)
                     {

                      x=infile.get();
                       if(x!=0x00)
                      std::cout<<"数据包长度不对! Event "<<Ev_count+1<<std::endl;

                       }
                  for(int k=0;k<18;k++)
                      x=infile.get();
                }
                else if(x==0x60)
                 {
                  x=infile.get();
                     // std::cout<<"x = "<<x;
                  Ch_tmp2=((x&0x00ff)<<8);
                  x=infile.get();
                  Ch_tmp1=(x&0x00ff);
                  Ch_tmp=Ch_tmp1+Ch_tmp2;
                  Ch_count=(Ch_tmp-6)/3;
                  for(int i=0;i<Ch_count;i++)
                  {
                    x=infile.get();
                    Ch_ID= (x&0x00ff);
                    x=infile.get();
                    Tmp2=((x&0x00ff)<<8);
                    x=infile.get();
                    Tmp1=(x&0x00ff);
                    data=Tmp1+Tmp2;
                    if(data>0&&data<16384)
                    datatmp[Ch_ID]=data;
                    else
                    err8000++;
                      }
                 for(int i=0;i<2;i++)
                  {

                   x=infile.get();
                   if(x!=0x00)
                   std::cout<<"数据包长度不对! Event "<<Ev_count+1<<std::endl;

                  }
                  for(int k=0;k<18;k++)
                      x=infile.get();
              }
              else
              {
                  continue;
              }
  ////////////////////////////////////////////////

                for(int ii=0;ii<64;ii++)
                {
                      h[ii]->Fill((float)datatmp[ii]);
                }

  ////////////////////////////////////////////////

          tree->Fill();
          Ev_count++;
          if(Ev_count%5000 == 0)
          std::cout<<Ev_count<<" events have been converted!"<<std::endl;
          }

        }

    }

    std::cout<<Ev_count<<" events converted totally!"<<std::endl;
    std::cout<<err8000<<" data have been found to be 0x8000 (or 0x00)!"<<std::endl;
    infile.close();
    f->Write();
    f->Close();
    }
    else{
        printf("error: %s can not be created!\n",outfile);
        delete f;
        return -1;
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//func: RawDataConv
//      converting raw testing data into rootfile,including pmt testing data and pedestal data.
//      A new directory is created "parentDir/root_file",and the directory structure of raw_data is
//      copied.
//input:
//      parentDir,the parent directory where raw testing data is stored
//output:
//      return 0 if no error occured,otherwise -1
///////////////////////////////////////////////////////////////////////////////////////////////////
int RawDataConv(const char* parentDir)
{
    if(!(gSystem->OpenDirectory(parentDir))){
        printf("error: can't find directory %s\n",parentDir);
        printf("Please check if this is the correct directory!\n");
        return -1;
    }

    Int_t datapoints;
    char raw_dir[300];
    char root_dir[300];
    char buffer1[300];
    char buffer2[300];

    FILE* fp;
    sprintf(raw_dir,"%s/raw_data",parentDir);
    sprintf(root_dir,"%s/root_file",parentDir);
    if(!(gSystem->OpenDirectory(root_dir)))
        gSystem->MakeDirectory(root_dir);
    sprintf(buffer1,"%s/configuration.csv",raw_dir);
    sprintf(buffer2,"%s/configuration.csv",root_dir);
    gSystem->CopyFile(buffer1,buffer2);

    for(int i=0;i<HIGHVOLTAGE_STEP;i++){
        //------------------PMT testing data converting-------------------------------
        sprintf(buffer1,"%s/%dV/LED.config",raw_dir,VOLTAGE[i]);
        if(!(fp=fopen(buffer1,"r"))){
            printf("error: can't open %s!\n",buffer1);
            return -1;
        }
        fgets(buffer2,200,fp);
        fscanf(fp,"Total datapoints: %d\n",&datapoints);
        fclose(fp);

        sprintf(buffer2,"%s/%dV",root_dir,VOLTAGE[i]);
        gSystem->MakeDirectory(buffer2);
        sprintf(buffer2,"%s/%dV/LED.config",root_dir,VOLTAGE[i]);
        gSystem->CopyFile(buffer1,buffer2);
        for(int j=0;j<datapoints;j++){
            convert_64(parentDir,VOLTAGE[i],j+1);
        }

    }

    //-------------------pedestal data converting----------------------------------
    sprintf(buffer2,"%s/pedestal",root_dir);
    gSystem->MakeDirectory(buffer2);
    sprintf(buffer1,"%s/pedestal",raw_dir);
    TString full_pathname;
    full_pathname=gSystem->ExpandPathName(buffer1);
    TSystemDirectory pedestal_dir(full_pathname.Data(),full_pathname.Data());
    TList *files=pedestal_dir.GetListOfFiles();
    if(files){
        if(files->GetSize()!=4){
            printf("error: there should be two pedestal files in directory %s\n",full_pathname.Data());
            return -1;
        }
        sprintf(buffer2,"%s/pedestal/time.dat",root_dir);
        if(!(fp=fopen(buffer2,"w"))){
            printf("error: can't create %s\n",buffer2);
            return -1;
        }
        fprintf(fp,"Measurement date:\n");

        files->Sort();
        TSystemFile *file;
        TIter next(files);
        Char_t *rootname[2]={"begin","end"};
        Int_t couter=0;
        while((file=(TSystemFile*)next())){
            if(!file->IsDirectory()){
                convert_pedestal(parentDir,file->GetName(),rootname[couter]);
                sprintf(buffer1,"%s\t%s.root\n",file->GetName(),rootname[couter]);
                fputs(buffer1,fp);
                couter++;
            }
        }
        fclose(fp);

    }
    else{
        printf("error: can't get files in %s\n",full_pathname.Data());
        return -1;
    }

    printf("All the raw data of %s has been converted.\n",parentDir);
    printf("The root files are saved at %s\n",root_dir);

    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////
//func: ReadConfigFile
//      get the testing confiugration of testing data in parentDir
//input:
//      parentDir is the directory where testing data is stored
//output:
//      return 0 if no error occured, otherwise -1
/////////////////////////////////////////////////////////////////////////////////////////
int ReadConfigFile(const char* parentDir)
{
    Char_t buffer1[300];
    Char_t root_dir[300];
    sprintf(root_dir,"%s/root_file",parentDir);
    //gSystem->ChangeDirectory(root_dir);//change to the root_file directory,all the analysis inputs are under this directory.
    sprintf(buffer1,"%s/configuration.csv",root_dir);
    FILE* fp=fopen(buffer1,"r");
    if(!fp){
        printf("error opening %s\n",buffer1);
        return -1;
    }

    fgets(buffer1,200,fp);
    printf("%s",buffer1);
    fgets(buffer1,200,fp);
    printf("%s",buffer1);
    for(int i=0;i < CHANNEL_NUM;i++){
        fscanf(fp,"%d %s %s %d %d %d\n",TEST_CH+i,PMT_LABEL[i],PMT_POSITION[i],PMT_INDEX+i,FEE_DY5+i,FEE_DY8+i);
        printf("%d %s %s %d %d %d\n",TEST_CH[i],PMT_LABEL[i],PMT_POSITION[i],PMT_INDEX[i],FEE_DY5[i],FEE_DY8[i]);
    }
    CONFIG_FLAG=1;

    fclose(fp);

    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
//func: Fit_PedestalData
//      get the pedestal fitting result
//input:
//      parentDir is the parent directory of the testing data
//output:
//      return 0 if no error occured,otherwise -1
//////////////////////////////////////////////////////////////////////////////////////////////////////
int Fit_PedestalData(const Char_t *parentDir)
{
    char root_dir[300];
    char buffer1[300];
//--------------------Check if root_file dir has been created AND create convert raw data to root file-------------------------
    sprintf(root_dir,"%s/root_file",parentDir);
    RawDataConv(parentDir);
//--------------------Fitting of root file-------------------------------------------------------------------------------------
    gStyle->SetOptFit(11);
   //--------------------------------read testing configuration------------------------
   ReadConfigFile(parentDir);
   //---------------------------------Project corresponding hist--------------------------
   Char_t prefix[2][20]={"begin","end"};
   Char_t infile[100],outfilePDF[100],outfileDAT[100];
   FILE* fp;

   TString htitlePedestalDy5,hnamePedestalDy5,branchPedestalDy5;
   TString htitlePedestalDy8,hnamePedestalDy8,branchPedestalDy8;
   TH1F* histPedestalDy5[CHANNEL_NUM];
   TH1F* histPedestalDy8[CHANNEL_NUM];
   TFile* file;
   TTree* tree;
   TSpectrum* s=new TSpectrum(1);
   TF1 *fitfuncDy5,*fgausDy5,*fitfuncDy8,*fgausDy8;
   Int_t nfound,bin;
   Float_t *xpeaks;
   Float_t xp,yp,xmin,xmax,sigma;

   TCanvas* canvas=new TCanvas("canvas","Pedestal of Dy5 and Dy8",1000,600);
   canvas->Divide(2,1);

   for(int i=0;i<2;i++){
       sprintf(infile,"%s/pedestal/%s.root",root_dir,prefix[i]);
       printf("%s\n",infile);
       sprintf(outfilePDF,"%s/pedestal/%s_result.pdf",root_dir,prefix[i]);
       sprintf(outfileDAT,"%s/pedestal/%s_result.dat",root_dir,prefix[i]);

       file = new TFile(infile);
       tree = (TTree*)file->Get("PMT");
       tree->Print();
       for(int j=0;j<CHANNEL_NUM;j++){
           hnamePedestalDy5.Form("%s_Dy5_Pedestal",PMT_LABEL[j]);
           htitlePedestalDy5.Form("Measurement_%s Pedestal: %s_Dy5,FEE_CH %d,Test_CH %d",prefix[i],PMT_LABEL[j],FEE_DY5[j],TEST_CH[j]);
           branchPedestalDy5.Form("hh[%d]",FEE_DY5[j]-1);
           histPedestalDy5[j]=new TH1F(hnamePedestalDy5.Data(),htitlePedestalDy5.Data(),2000,-1000,3000);
           tree->Project(hnamePedestalDy5.Data(),branchPedestalDy5.Data());

           hnamePedestalDy8.Form("%s_Dy8_Pedestal",PMT_LABEL[j]);
           htitlePedestalDy8.Form("Measurement_%s Pedestal: %s_Dy8,FEE_CH %d,Test_CH %d",prefix[i],PMT_LABEL[j],FEE_DY8[j],TEST_CH[j]);
           branchPedestalDy8.Form("hh[%d]",FEE_DY8[j]-1);
           histPedestalDy8[j]=new TH1F(hnamePedestalDy8.Data(),htitlePedestalDy8.Data(),2000,-1000,3000);
           tree->Project(hnamePedestalDy8.Data(),branchPedestalDy8.Data());
       }
       sprintf(buffer1,"%s[",outfilePDF);
       canvas->Print(buffer1);

       fp=fopen(outfileDAT,"w");
       if(!fp){
           printf("error:creating %s\n",outfileDAT);
           return -1;
       }
       fprintf(fp,"Fitting result of pedestal:\n");
       char* tmp="Ch\tPMT\tDy5\tDy5sigma\tDy8\tDy8sigma\n";
       fputs(tmp,fp);

       for(int k=0;k<CHANNEL_NUM;k++){
           canvas->cd(1);

           nfound = s->Search(histPedestalDy5[k],2,"goff",0.05);
           if(nfound != 1){
               printf("In file %s/pedestal/%s.root, %s_Dy5 pedestal:\n",root_dir,prefix[i],PMT_LABEL[k]);
               printf("error: %d peak was found!\n",nfound);
               return -1;
           }
           xpeaks = s->GetPositionX();
           xp = xpeaks[0];
           bin = histPedestalDy5[k]->GetXaxis()->FindBin(xp);
           yp = histPedestalDy5[k]->GetBinContent(bin);
           xmin = xp-200;
           xmax = xp+200;
           histPedestalDy5[k]->Fit("gaus","q0","goff",xmin,xmax);
           fitfuncDy5 = (TF1*)histPedestalDy5[k]->GetFunction("gaus");
           sigma = fitfuncDy5->GetParameter(2);
           xmin = xp-3*sigma;
           xmax = xp+3*sigma;
           fgausDy5 = new TF1("fgausDy5","gaus",xmin,xmax);
           fgausDy5->SetNpx(1000);
           histPedestalDy5[k]->Fit("fgausDy5","q");
           //histPedestalDy5[k]->Fit("gaus","q","",100,500);
           //fitfuncDy5 = (TF1*)histPedestalDy5[k]->GetFunction("gaus");

           canvas->cd(2);
           nfound = s->Search(histPedestalDy8[k],2,"goff",0.05);
           if(nfound != 1){
               printf("In file %s/pedestal/%s.root, %s_Dy8 pedestal:\n",root_dir,prefix[i],PMT_LABEL[k]);
               printf("error: %d peak was found!\n",nfound);
               return -1;
           }
           xpeaks = s->GetPositionX();
           xp = xpeaks[0];
           bin = histPedestalDy8[k]->GetXaxis()->FindBin(xp);
           yp = histPedestalDy8[k]->GetBinContent(bin);
           xmin = xp-200;
           xmax = xp+200;
           histPedestalDy8[k]->Fit("gaus","q","",xmin,xmax);
           fitfuncDy8 = (TF1*)histPedestalDy8[k]->GetFunction("gaus");
           sigma = fitfuncDy8->GetParameter(2);
           xmin = xp-3*sigma;
           xmax = xp+3*sigma;
           fgausDy8 = new TF1("fgausDy8","gaus",xmin,xmax);
           fgausDy8->SetNpx(1000);
           histPedestalDy8[k]->Fit("fgausDy8","q");
           //histPedestalDy8[k]->Fit("gaus","q","",100,500);
           //fitfuncDy8 = (TF1*)histPedestalDy8[k]->GetFunction("gaus");

           fprintf(fp,"%d\t%s\t%f\t%f\t%f\t%f\n",TEST_CH[k],PMT_LABEL[k],fgausDy5->GetParameter(1),fgausDy5->GetParameter(2),fgausDy8->GetParameter(1),fgausDy8->GetParameter(2));
           canvas->Print(outfilePDF);

           //delete fitfuncDy5;
           //delete fitfuncDy8;//for some reason,you can't delete fitfuncDy5/fitfuncDy8.Error will occur.
           delete fgausDy5;
           delete fgausDy8;
       }
       fclose(fp);
       sprintf(buffer1,"%s]",outfilePDF);
       canvas->Print(buffer1);

       for(int k=0;k<CHANNEL_NUM;k++){
           delete histPedestalDy5[k];
           delete histPedestalDy8[k];
       }

       file->Close();

   }
    delete canvas;

   return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//func: CheckForPedestal
//      check the pedestals in test files from test'from' to test'to'
//input:
//      dir is the directory where the whole testing files is stored
//      from/to is the index of the testing files you wanna check
//      if flag is 0,begin.root will be checked;if flag is 1 end.root will be checked.the default is 0.
//output:
//      return 0 if no error occured,otherwise -1
//////////////////////////////////////////////////////////////////////////////////////////////////////
int CheckForPedestal(const Char_t* dir,Int_t from,Int_t to,Int_t flag=0)
{
    Char_t buffer[200];
    Char_t filename[300];
    Char_t datfile[20];
    if(flag == 0){
        sprintf(datfile,"begin_result.dat");
    }
    else{
        sprintf(datfile,"end_result.dat");
    }
    Int_t datapoints=from-to+1;
    Float_t file_index[200];
    Float_t ped_dy5[CHANNEL_NUM][200],ped_dy8[CHANNEL_NUM][200];
    Float_t ped_sigmady5[CHANNEL_NUM][200],ped_sigmady8[CHANNEL_NUM][200];
    Int_t tmp_ch;
    Char_t tmp_label[20];

    if(CONFIG_FLAG==0){
        sprintf(filename,"%s/test%d",dir,from);//read any one test data to get the FEE channels
        ReadConfigFile(filename);
    }

    FILE* fp;
    for(int i=0;i<datapoints;i++){
        file_index[i]=from+i;
        sprintf(filename,"%s/test%d/root_file/pedestal/%s",dir,from+i,datfile);
        if(!(fp=fopen(filename,"r"))){
            printf("can't open %s\n",filename);
            return -1;
        }

        fgets(buffer,200,fp);
        printf("%s",buffer);
        fgets(buffer,200,fp);
        printf("%s",buffer);
        for(int j=0;j<CHANNEL_NUM;j++){
            fscanf(fp,"%d %s %f %f %f %f\n",&tmp_ch,tmp_label,ped_dy5[j]+i,ped_sigmady5[j]+i,ped_dy8[j]+i,ped_sigmady8[j]+i);
            printf("%d %s %f %f %f %f\n",tmp_ch,tmp_label,ped_dy5[j][i],ped_sigmady5[j][i],ped_dy8[j][i],ped_sigmady8[j][i]);

        }
        fclose(fp);
    }

    TGraphErrors *gdy5[10];
    TGraphErrors *gdy8[10];
    TString title;
    TCanvas *canvas=new TCanvas("canvas","Pedestal Consistency",800,500);
    canvas->Divide(2,1);
    sprintf(filename,"%s/pedestal.pdf",dir);
    sprintf(buffer,"%s[",filename);
    canvas->Print(buffer);
    for(int i=0;i<CHANNEL_NUM;i++){
        canvas->cd(1);
        gdy5[i]=new TGraphErrors(datapoints,file_index,ped_dy5[i],0,ped_sigmady5[i]);
        title.Form("Pedestal Consistency of Channel_%d,FEE#%d",TEST_CH[i],FEE_DY5[i]);
        gdy5[i]->SetTitle(title.Data());
        gdy5[i]->Draw("AL*");

        canvas->cd(2);
        gdy8[i]=new TGraphErrors(datapoints,file_index,ped_dy8[i],0,ped_sigmady8[i]);
        title.Form("Pedestal Consistency of Channel_%d,FEE#%d",TEST_CH[i],FEE_DY8[i]);
        gdy8[i]->SetTitle(title.Data());
        gdy8[i]->Draw("AL*");

        canvas->Print(filename);

    }
    sprintf(buffer,"%s]",filename);
    canvas->Print(buffer);
    delete canvas;

    return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//func: Fit_TestingData
//      fit the testing data and print out the result.the saved result has substracted pedestal value.
//input:
//      parentDir is the directory where the whole testing data files is saved.
//      when flag=0,the corresponding testing pedestal file will be used(begin.root/end.root);
//      when flag=1,the standard pedestal will be used;the default flag is 0.
//      pedfilename is the filename of corresponding pedestal file,meaningful when flag=0,otherwise it
//      is ignored.
//output:
//      return 0 is no error occured,otherwise -1
////////////////////////////////////////////////////////////////////////////////////////////////////////
int Fit_TestingData(const char* parentDir,int flag=0,const char* pedfilename="begin")
{
    char root_dir[300];
    char buffer1[300];
    sprintf(root_dir,"%s/root_file",parentDir);
    Float_t ped_meandy8[CHANNEL_NUM]={384.78,334.83,283.12,424.56,372.32,150.28,209.96,117.97,218.61,260.51,231.83,203.87,
                                      140.88,191.56,245.26,252.8,306.82,210.78,218.46,294.79,265.94,245.36,277.63,286.93,302.64};
    Float_t ped_meandy5[CHANNEL_NUM]={460.59,391.71,403.94,364.81,407.12,411.44,414.69,425.21,413.63,460.96,383.86,418.74,
                                      373.81,343.06,431.66,353.26,336.83,432.93,327.56,350.62,312.8,311.81,272.68,332.59,432.88};
//--------------------Check if root_file dir has been created AND create convert raw data to root file-------------------------
    sprintf(root_dir,"%s/root_file",parentDir);
    RawDataConv(parentDir);
    if(flag==0){
        Fit_PedestalData(parentDir);

        Int_t tmp_ch;
        Float_t tmp_sigmady5,tmp_sigmady8;
        Char_t tmp_label[20];
        FILE *fp;
        sprintf(buffer1,"%s/pedestal/%s_result.dat",root_dir,pedfilename);
        if(!(fp=fopen(buffer1,"r"))){
            printf("error: can't open %s\n",buffer1);
            return -1;
        }
        fgets(buffer1,300,fp);
        printf("%s",buffer1);
        fgets(buffer1,300,fp);
        printf("%s",buffer1);
        for(int i=0;i<CHANNEL_NUM;i++){
            fscanf(fp,"%d %s %f %f %f %f\n",&tmp_ch,tmp_label,ped_meandy5+i,&tmp_sigmady5,ped_meandy8+i,&tmp_sigmady8);
            printf("%d %s %f %f %f %f\n",tmp_ch,tmp_label,ped_meandy5[i],tmp_sigmady5,ped_meandy8[i],tmp_sigmady8);
        }
        fclose(fp);
    }
//--------------------Fitting of root file-------------------------------------------------------------------------------------
    gStyle->SetOptFit(11);
   //--------------------------------read testing configuration------------------------
   ReadConfigFile(parentDir);
   //---------------------------------Project corresponding hist--------------------------
   Int_t datapoints,index[MAXIMUM_DATAPOINTS];
   Float_t width[MAXIMUM_DATAPOINTS],high_level[MAXIMUM_DATAPOINTS],temp1,temp2,temp3,temp4;
   Char_t ledconfig[200],infile[200],outfilePDF[200],outfileDAT[200],outfileNoPed[200];
   FILE* fp_ledconfig;
   FILE* fp;
   FILE* fp_noped;

   TString htitleDy5,hnameDy5,branchDy5;
   TString htitleDy8,hnameDy8,branchDy8;
   TH1F* histDy5[CHANNEL_NUM];
   TH1F* histDy8[CHANNEL_NUM];
   TFile* file;
   TTree* tree;
   TSpectrum* s=new TSpectrum(1);
   TF1 *fitfuncDy5,*fgausDy5,*fitfuncDy8,*fgausDy8;
   Int_t nfound,bin;
   Float_t *xpeaks;
   Float_t xp,yp,xmin,xmax,sigma;

   TCanvas* canvas=new TCanvas("canvas","Peaking of Dy5 and Dy8",1000,600);
   canvas->Divide(2,1);

   for(int i=0;i<HIGHVOLTAGE_STEP;i++){
       //------------------------read LED.config------------------------------------------------------------
       sprintf(ledconfig,"%s/%dV/LED.config",root_dir,VOLTAGE[i]);
       if(!(fp_ledconfig=fopen(ledconfig,"r"))){
           printf("error: can't open %s\n",ledconfig);
           return -1;
       }
       fgets(buffer1,200,fp_ledconfig);
       printf("%s",buffer1);
       fscanf(fp_ledconfig,"Total datapoints: %d\n",&datapoints);
       printf("Total datapoints: %d\n",datapoints);
       fgets(buffer1,200,fp_ledconfig);
       printf("%s",buffer1);
       for(int j=0;j<datapoints;j++){
           fscanf(fp_ledconfig,"%d %E %E %E %E %E %E\n",index+j,&temp1,width+j,high_level+j,&temp2,&temp3,&temp4);
           printf("%d %f %5.2f %f %f %.12f %.12f\n",index[j],temp1,width[j]*1e6,high_level[j],temp2,temp3,temp4);
       }
       fclose(fp_ledconfig);
       //------------------------------------------------------------------------------------------------------------
       for(int j=0;j<datapoints;j++){
           sprintf(infile,"%s/%dV/%d.root",root_dir,VOLTAGE[i],j+1);
           sprintf(outfilePDF,"%s/%dV/%d_result.pdf",root_dir,VOLTAGE[i],j+1);
           sprintf(outfileDAT,"%s/%dV/%d_result.dat",root_dir,VOLTAGE[i],j+1);
           sprintf(outfileNoPed,"%s/%dV/%d_noped.dat",root_dir,VOLTAGE[i],j+1);

           file=new TFile(infile);
           tree=(TTree*)file->Get("PMT");
           //tree->Print();

           for(int k=0;k<CHANNEL_NUM;k++){
               hnameDy5.Form("%s_Dy5",PMT_LABEL[k]);
               htitleDy5.Form("%5.2fus,%3.2fV,%s_Dy5,FEE_Channel %d,Testing_Channel %d",width[j]*1e6,high_level[j],PMT_LABEL[k],FEE_DY5[k],TEST_CH[k]);
               branchDy5.Form("hh[%d]",FEE_DY5[k]-1);
               histDy5[k]=new TH1F(hnameDy5.Data(),htitleDy5.Data(),1800,-1000,17000);
               tree->Project(hnameDy5.Data(),branchDy5.Data());

               hnameDy8.Form("%s_Dy8",PMT_LABEL[k]);
               htitleDy8.Form("%5.2fus,%3.2fV,%s_Dy8,FEE_Channel %d,Testing_Channel %d",width[j]*1e6,high_level[j],PMT_LABEL[k],FEE_DY8[k],TEST_CH[k]);
               branchDy8.Form("hh[%d]",FEE_DY8[k]-1);
               histDy8[k]=new TH1F(hnameDy8.Data(),htitleDy8.Data(),1800,-1000,17000);
               tree->Project(hnameDy8.Data(),branchDy8.Data());
           }
           //----------------------------------------------------------------------------------------------------
           //------------------------------find peak and save/print the result-------------------------------------------------------------
           sprintf(buffer1,"%s[",outfilePDF);
           canvas->Print(buffer1);

           fp=fopen(outfileDAT,"w");
           if(!fp){
               printf("error: can't create %s\n",outfileDAT);
               return -1;
           }
           fp_noped=fopen(outfileNoPed,"w");
           if(!fp_noped){
               printf("error: can't create %s\n",outfileNoPed);
               return -1;
           }
           fprintf(fp,"Fitting result of %d.dat at %5.2fus,%3.2fV:\n",j+1,width[j]*1e6,high_level[j]);
           fprintf(fp_noped,"No Pedestal,fitting result of %d.dat at %5.2fus,%3.2fV:\n",j+1,width[j]*1e6,high_level[j]);
           char* tmp="Ch\tPMT\tDy5\tDy5sigma\tDy8\tDy8sigma\n";
           fputs(tmp,fp);
           fputs(tmp,fp_noped);

           for(int k=0;k<CHANNEL_NUM;k++){
               canvas->cd(1);
               nfound = s->Search(histDy5[k],2,"goff",0.05);
               if(nfound != 1){
                   printf("In file %s/%dV/%d.dat, %s_Dy5:\n",root_dir,VOLTAGE[i],j+1,PMT_LABEL[k]);
                   printf("error: %d peak was found!\n",nfound);
                   return -1;
               }
               xpeaks = s->GetPositionX();
               xp = xpeaks[0];
               bin = histDy5[k]->GetXaxis()->FindBin(xp);
               yp = histDy5[k]->GetBinContent(bin);
               xmin = xp-200;
               xmax = xp+200;
               histDy5[k]->Fit("gaus","q0","goff",xmin,xmax);
               fitfuncDy5 = (TF1*)histDy5[k]->GetFunction("gaus");
               sigma = fitfuncDy5->GetParameter(2);
               xmin = xp-5*sigma;
               xmax = xp+5*sigma;
               fgausDy5 = new TF1("fgausDy5","gaus",xmin,xmax);
               fgausDy5->SetNpx(1000);
               histDy5[k]->Fit("fgausDy5","q");

               canvas->cd(2);
               nfound = s->Search(histDy8[k],2,"goff",0.05);
               if(nfound != 1){
                   printf("In file %s/%dV/%d.dat, %s_Dy8:\n",root_dir,VOLTAGE[i],j+1,PMT_LABEL[k]);
                   printf("error: %d peak was found!\n",nfound);
                   return -1;
               }
               xpeaks = s->GetPositionX();
               xp = xpeaks[0];
               bin = histDy8[k]->GetXaxis()->FindBin(xp);
               yp = histDy8[k]->GetBinContent(bin);
               xmin = xp-200;
               xmax = xp+200;
               histDy8[k]->Fit("gaus","q","",xmin,xmax);
               fitfuncDy8 = (TF1*)histDy8[k]->GetFunction("gaus");
               sigma = fitfuncDy8->GetParameter(2);
               xmin = xp-5*sigma;
               xmax = xp+5*sigma;
               fgausDy8 = new TF1("fgausDy8","gaus",xmin,xmax);
               fgausDy8->SetNpx(1000);
               histDy8[k]->Fit("fgausDy8","q");

               fprintf(fp,"%d\t%s\t%f\t%f\t%f\t%f\n",TEST_CH[k],PMT_LABEL[k],fgausDy5->GetParameter(1),fgausDy5->GetParameter(2),fgausDy8->GetParameter(1),fgausDy8->GetParameter(2));
               fprintf(fp_noped,"%d\t%s\t%f\t%f\t%f\t%f\n",TEST_CH[k],PMT_LABEL[k],fgausDy5->GetParameter(1)-ped_meandy5[k],fgausDy5->GetParameter(2),fgausDy8->GetParameter(1)-ped_meandy8[k],fgausDy8->GetParameter(2));
               canvas->Print(outfilePDF);

               //delete fitfuncDy5;
               //delete fitfuncDy8;//for some reason,you can't delete fitfuncDy5/fitfuncDy8.Error will occur.
               delete fgausDy5;
               delete fgausDy8;
           }
           fclose(fp);
           fclose(fp_noped);
           sprintf(buffer1,"%s]",outfilePDF);
           canvas->Print(buffer1);
           for(int k=0;k<CHANNEL_NUM;k++){
               delete histDy5[k];
               delete histDy8[k];
           }
           file->Close();

       }
   }

   delete canvas;
   //-------------------------------------------------------------------------------------
   return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//func: CheckForConsistency
//      Check if the LED light intensity has changed over time,using the 2/3 reference PMTs
//input:
//      dir is the directory the whole testing data files are saved which includes test1,test2...
//      from/to is the file index which defines the testing files to check.
//      voltage defines the voltage level under which the testing is conducted.
//      index is the file index which defines the AFG3252 configuration under which the testing is conducted
//      num_ch is number of the Reference PMT channels that will be checked.there are two permanent ref_pmts
//      and one temparory out_ref pmt,so the num_ch can be 2 or 3.the default is 2,i.e. the two permanent ref_
//      pmts be checked.
///////////////////////////////////////////////////////////////////////////////////////////////////////////
int CheckForConsistency(const char* dir,Int_t from,Int_t to,Int_t voltage,Int_t index,const Int_t num_ch=2)
{
    char* PMT_Ref[]={"AA0820","AA0847","AA0749"};
    Int_t ch[3];//ch1 is AA0820, ch2 is AA0847,ch11 or outer_ref is AA0749
    char pmt_label[3][20];
    Float_t mean_dy5[3],mean_dy8[3][200],sigma_dy5[3],sigma_dy8[3][200];
    Float_t file_index[200];
	char filename[300];
	char buffer[200];
	Int_t datapoints=to-from+1;
	FILE* fp;
	for(int i=0;i<datapoints;i++){
		file_index[i]=from+i;
        sprintf(filename,"%s/test%d/root_file/%dV/%d_noped.dat",dir,from+i,voltage,index);
		if(!(fp=fopen(filename,"r"))){
			printf("error: can't open %s",filename);
			return -1;
		}
		fgets(buffer,200,fp);
		fgets(buffer,200,fp);
		//AA0820
        fscanf(fp,"%d %s %f %f %f %f\n",ch,pmt_label[0],mean_dy5,sigma_dy5,mean_dy8[0]+i,sigma_dy8[0]+i);
		//AA0847
        fscanf(fp,"%d %s %f %f %f %f\n",ch+1,pmt_label[1],mean_dy5+1,sigma_dy5+1,mean_dy8[1]+i,sigma_dy8[1]+i);
        fscanf(fp,"%d %s %f %f %f %f\n",ch+2,pmt_label[2],mean_dy5+2,sigma_dy5+2,mean_dy8[2]+i,sigma_dy8[2]+i);
		fclose(fp);
	}

    if(num_ch == 2){
        TGraphErrors *gConsistency[2];
        TString gTitle[2];
        TCanvas* canvas=new TCanvas("canvas","Consistency of Reference PMTs",1000,600);
        canvas->Divide(2,1);
        for(int i=0;i<2;i++){
            canvas->cd(i+1);
            gConsistency[i]=new TGraphErrors(datapoints,file_index,mean_dy8[i],0,sigma_dy8[i]);
            gTitle[i].Form("Consistency of %s, at %dV of amp_%d",pmt_label[i],voltage,index);
            gConsistency[i]->SetTitle(gTitle[i].Data());
            gConsistency[i]->Draw("AL*");
        }
    }
    else if(num_ch == 3){
        TGraphErrors *gConsistency[3];
        TString gTitle[3];
        TCanvas* canvas=new TCanvas("canvas","Consistency of Reference PMTs",1200,500);
        canvas->Divide(3,1);
        for(int i=0;i<3;i++){
            canvas->cd(i+1);
            gConsistency[i]=new TGraphErrors(datapoints,file_index,mean_dy8[i],0,sigma_dy8[i]);
            gTitle[i].Form("Consistency of %s, at %dV of amp_%d",pmt_label[i],voltage,index);
            gConsistency[i]->SetTitle(gTitle[i].Data());
            gConsistency[i]->Draw("AL*");
        }
    }

	return 0;
}

//-----------------------------------------------------------------------------------
int ReadIntoDataBuffer(const Char_t* parDir,Int_t testdir_id){

    Char_t test_dir[300];
    Char_t root_dir[300];
    Char_t filename[300];
    Char_t buffer[200];
    TestDirectory_id=testdir_id;

    sprintf(test_dir,"%s",gSystem->ExpandPathName(parDir));
    sprintf(root_dir,"%s/root_file",test_dir);
    //--------------------
    ReadConfigFile(test_dir);
    //----------------------
    FILE* fp_noped;
    Int_t tmp_ch;
    Char_t tmp_label[20];
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        for(int Amplitude_id=0;Amplitude_id<TOTAL_FILENO[HV_id];Amplitude_id++){
            sprintf(filename,"%s/%dV/%d_noped.dat",root_dir,VOLTAGE[HV_id],Amplitude_id+1);
            fp_noped=fopen(filename,"r");
            if(!fp_noped){
                printf("error opening %s!\n",filename);
            }
            fgets(buffer,200,fp_noped);
            //printf("%s",buffer);
            fgets(buffer,200,fp_noped);
            //printf("%s",buffer);
            for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
                fscanf(fp_noped,"%d %s %f %f %f %f\n",&tmp_ch,tmp_label,&(Data_Dy5[PMT_id][HV_id][Amplitude_id]),&(Sigma_Dy5[PMT_id][HV_id][Amplitude_id])
                       ,&(Data_Dy8[PMT_id][HV_id][Amplitude_id]),&(Sigma_Dy8[PMT_id][HV_id][Amplitude_id]));
                //printf("%d %s %f %f %f %f\n",tmp_ch,tmp_label,(Data_Dy5[PMT_id][HV_id][Amplitude_id]),(Sigma_Dy5[PMT_id][HV_id][Amplitude_id])
                //       ,(Data_Dy8[PMT_id][HV_id][Amplitude_id]),(Sigma_Dy8[PMT_id][HV_id][Amplitude_id]));
            }
            fclose(fp_noped);
        }
    }

    for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
        for(int Amplitude_id=0;Amplitude_id<GAIN_FILENO;Amplitude_id++){
            for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
                Data_Gain[PMT_id][Amplitude_id][HV_id]=Data_Dy8[PMT_id][HV_id][GAIN_FILEINDEX[HV_id][Amplitude_id]-1];
                Sigma_Data_Gain[PMT_id][Amplitude_id][HV_id]=Sigma_Dy8[PMT_id][HV_id][GAIN_FILEINDEX[HV_id][Amplitude_id]-1];
            }
        }
    }

    DATAUPDATE_FLAG=1;
    printf("Testing data have been read into Global Buffer\n");

    return 0;
}

int ReadRefIntoDataBuffer(const Char_t* parDir,Int_t refdir_id)
{
    Char_t test_dir[300];
    Char_t root_dir[300];
    Char_t filename[300];
    Char_t buffer[200];

    sprintf(test_dir,"%s",gSystem->ExpandPathName(parDir));
    sprintf(root_dir,"%s/root_file",test_dir);

    ReadConfigFile(test_dir);
    RefDirectory_id=refdir_id;

    FILE* fp_noped;
    Int_t tmp_ch;
    Char_t tmp_label[20];
    Float_t tmp_dy5_mean,tmp_dy5_sigma,tmp_dy8_mean,tmp_dy8_sigma;
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        for(int Amplitude_id=0;Amplitude_id<TOTAL_FILENO[HV_id];Amplitude_id++){
            sprintf(filename,"%s/%dV/%d_noped.dat",root_dir,VOLTAGE[HV_id],Amplitude_id+1);
            fp_noped=fopen(filename,"r");
            if(!fp_noped){
                printf("error opening %s!\n",filename);
            }
            fgets(buffer,200,fp_noped);
            //printf("%s",buffer);
            fgets(buffer,200,fp_noped);
            //printf("%s",buffer);
            for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
                fscanf(fp_noped,"%d %s %f %f %f %f\n",&tmp_ch,tmp_label,&tmp_dy5_mean,&tmp_dy5_sigma
                       ,&tmp_dy8_mean,&tmp_dy8_sigma);
                for(int i=0;i<RefPMT_NO;i++){
                    if(RefPMT_CH[i] == TEST_CH[PMT_id]){
                        //Ref_Dy5[i][HV_id][Amplitude_id]=tmp_dy5_mean;
                        Ref_Dy8[i][HV_id][Amplitude_id]=tmp_dy8_mean;
                    }
                }
            }
            fclose(fp_noped);
        }
    }

    REFDATAUPDATE_FLAG=1;
    return 0;
}

int SetDBProfile(Char_t  *server_ip,Char_t *db_name,Char_t *uname,Char_t *psswd)
{
    sprintf(dbname,"msyql://%s/%s",server_ip,db_name);
    sprintf(username,"%s",uname);
    sprintf(password,"%s",psswd);

    return 0;
}

int InsertRawData(Int_t TestingRound=1)
{

    TSQLServer *db=TSQLServer::Connect(dbname,username,password);
    if(!db){
        printf("can't connect to MySQL database\n");
        return -1;
    }

    Char_t sql[400];
    TString dy5_sql,dy8_sql,tmp_string;
    TString dy5_sigma_sql,dy8_sigma_sql;
    TString gain_sql;
    TString gain_sigma_sql;
    //-------------Update table 'TestDirectory'-----------------------------------------------------------------------------------------------------
    sprintf(sql,"INSERT INTO `pmt_v2`.`testdirectory` (`TestDirectory_id`,`DirectoryName`) VALUES (%d,'test%d')",TestDirectory_id,TestDirectory_id);
    if(!(db->Exec(sql))){
        printf("%s\n",db->GetErrorMsg());
    }

    //-------------Update table 'PMTInfo'------------------------------------------------------------------------------------------------------------
    for(int PMT_id=0;PMT_id<TestPMT_NO;PMT_id++){
        if(PMT_INDEX[TestPMT_CH[PMT_id]-1] == 0){
            continue;
        }
        else{
            sprintf(sql,"INSERT INTO `pmt_v2`.`pmtinfo` (`Index`,`Label`,`Position`,`Channel_id`,`TestDirectory_id`,`TestingRound`)"
                "VALUES (%d,'%s','%s',%d,%d,'%d')",PMT_INDEX[TestPMT_CH[PMT_id]-1],PMT_LABEL[TestPMT_CH[PMT_id]-1],PMT_POSITION[TestPMT_CH[PMT_id]-1],TEST_CH[TestPMT_CH[PMT_id]-1],TestDirectory_id,TestingRound);
            if(!(db->Exec(sql))){
                printf("%s\n",db->GetErrorMsg());
            }
        }
    }

    //--------------Update table 'DataDy5_*V/DataDy8_*V/DataDy5_Sigma_*V/DataDy8_Sigma_*V'----------------------------------------------------------------------------------------------
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        for(int PMT_id=0;PMT_id<TestPMT_NO;PMT_id++){
            if(PMT_INDEX[TestPMT_CH[PMT_id]-1]==0){
                continue;
            }
            else{
                dy5_sql.Form("INSERT INTO `pmt_v2`.`datady5_%dv` (`Index`",VOLTAGE[HV_id]);
                dy8_sql.Form("INSERT INTO `pmt_v2`.`datady8_%dv` (`Index`",VOLTAGE[HV_id]);
                dy5_sigma_sql.Form("INSERT INTO `pmt_v2`.`datady5_sigma_%dv` (`Index`",VOLTAGE[HV_id]);
                dy8_sigma_sql.Form("INSERT INTO `pmt_v2`.`datady8_sigma_%dv` (`Index`",VOLTAGE[HV_id]);
                for(int Amplitude_id=0;Amplitude_id<DY58_FILENO[HV_id];Amplitude_id++){
                    tmp_string.Form(",`%d`",Amplitude_id+1);
                    dy5_sql.Append(tmp_string);
                    dy8_sql.Append(tmp_string);
                    dy5_sigma_sql.Append(tmp_string);
                    dy8_sigma_sql.Append(tmp_string);
                }
                tmp_string.Form(") VALUES (%d",PMT_INDEX[TestPMT_CH[PMT_id]-1]);
                dy5_sql.Append(tmp_string);
                dy8_sql.Append(tmp_string);
                dy5_sigma_sql.Append(tmp_string);
                dy8_sigma_sql.Append(tmp_string);
                for(int Amplitude_id=0;Amplitude_id<DY58_FILENO[HV_id];Amplitude_id++){
                    tmp_string.Form(",%f",Data_Dy5[TestPMT_CH[PMT_id]-1][HV_id][Amplitude_id]);
                    dy5_sql.Append(tmp_string);
                    tmp_string.Form(",%f",Data_Dy8[TestPMT_CH[PMT_id]-1][HV_id][Amplitude_id]);
                    dy8_sql.Append(tmp_string);
                    tmp_string.Form(",%f",Sigma_Dy5[TestPMT_CH[PMT_id]-1][HV_id][Amplitude_id]);
                    dy5_sigma_sql.Append(tmp_string);
                    tmp_string.Form(",%f",Sigma_Dy8[TestPMT_CH[PMT_id]-1][HV_id][Amplitude_id]);
                    dy8_sigma_sql.Append(tmp_string);
                }
                dy5_sql.Append(")");
                dy8_sql.Append(")");
                dy5_sigma_sql.Append(")");
                dy8_sigma_sql.Append(")");
                if(!(db->Exec(dy5_sql.Data()))){
                    printf("%s\n",db->GetErrorMsg());
                }
                if(!(db->Exec(dy8_sql.Data()))){
                    printf("%s\n",db->GetErrorMsg());
                }
                if(!(db->Exec(dy5_sigma_sql.Data()))){
                    printf("%s\n",db->GetErrorMsg());
                }
                if(!(db->Exec(dy8_sigma_sql.Data()))){
                    printf("%s\n",db->GetErrorMsg());
                }
            }
        }
    }

    //--------------Update table 'DataGain/DataGain_Sigma'---------------------------------------------------------------------------
    for(int PMT_id=0;PMT_id<TestPMT_NO;PMT_id++){
        if(PMT_INDEX[TestPMT_CH[PMT_id]-1]==0){
            continue;
        }
        else{
            for(int Amplitude_id=0;Amplitude_id<GAIN_FILENO;Amplitude_id++){
                gain_sql.Form("INSERT INTO `PMT_V2`.`DataGain` (`Index`,`Amplitude_id`,`1000V`,`950V`,`900V`,`850V`,`800V`,`750V`,`700V`) VALUES (%d,%d",PMT_INDEX[TestPMT_CH[PMT_id]-1],Amplitude_id+1);
                gain_sigma_sql.Form("INSERT INTO `PMT_V2`.`DataGain_Sigma` (`Index`,`Amplitude_id`,`1000V`,`950V`,`900V`,`850V`,`800V`,`750V`,`700V`) VALUES (%d,%d",PMT_INDEX[TestPMT_CH[PMT_id]-1],Amplitude_id+1);
                for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
                    tmp_string.Form(",%f",Data_Gain[TestPMT_CH[PMT_id]-1][Amplitude_id][HV_id]);
                    gain_sql.Append(tmp_string);
                    tmp_string.Form(",%f",Sigma_Data_Gain[TestPMT_CH[PMT_id]-1][Amplitude_id][HV_id]);
                    gain_sigma_sql.Append(tmp_string);
                }
                gain_sql.Append(")");
                gain_sigma_sql.Append(")");
                if(!(db->Exec(gain_sql.Data()))){
                    printf("%s\n",db->GetErrorMsg());
                }
                if(!(db->Exec(gain_sigma_sql.Data()))){
                    printf("%s\n",db->GetErrorMsg());
                }
            }
        }
    }

    //----------------------------------Update table `RefDy5_*V/RefDy8_*V/RefDy5_Sigma_*V/RefDy8_Sigma_*V`--------------------------------------------------------------------------------------------------
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        for(int PMT_id=0;PMT_id<RefPMT_NO;PMT_id++){
                dy5_sql.Form("INSERT INTO `pmt_v2`.`refdy5_%dv` (`TestDirectory_id`,`RefPMT_id`",VOLTAGE[HV_id]);
                dy8_sql.Form("INSERT INTO `pmt_v2`.`refdy8_%dv` (`TestDirectory_id`,`RefPMT_id`",VOLTAGE[HV_id]);
                dy5_sigma_sql.Form("INSERT INTO `pmt_v2`.`refdy5_sigma_%dv` (`TestDirectory_id`,`RefPMT_id`",VOLTAGE[HV_id]);
                dy8_sigma_sql.Form("INSERT INTO `pmt_v2`.`refdy8_sigma_%dv` (`TestDirectory_id`,`RefPMT_id`",VOLTAGE[HV_id]);
                for(int Amplitude_id=0;Amplitude_id<DY58_FILENO[HV_id];Amplitude_id++){
                    tmp_string.Form(",`%d`",Amplitude_id+1);
                    dy5_sql.Append(tmp_string);
                    dy8_sql.Append(tmp_string);
                    dy5_sigma_sql.Append(tmp_string);
                    dy8_sigma_sql.Append(tmp_string);
                }
                tmp_string.Form(") VALUES (%d,%d",TestDirectory_id,RefPMT_CH[PMT_id]);//for RefPMT Test channel no equals to RefPMT_id
                dy5_sql.Append(tmp_string);
                dy8_sql.Append(tmp_string);
                dy5_sigma_sql.Append(tmp_string);
                dy8_sigma_sql.Append(tmp_string);
                for(int Amplitude_id=0;Amplitude_id<DY58_FILENO[HV_id];Amplitude_id++){
                    tmp_string.Form(",%f",Data_Dy5[RefPMT_CH[PMT_id]-1][HV_id][Amplitude_id]);
                    dy5_sql.Append(tmp_string);
                    tmp_string.Form(",%f",Data_Dy8[RefPMT_CH[PMT_id]-1][HV_id][Amplitude_id]);
                    dy8_sql.Append(tmp_string);
                    tmp_string.Form(",%f",Sigma_Dy5[RefPMT_CH[PMT_id]-1][HV_id][Amplitude_id]);
                    dy5_sigma_sql.Append(tmp_string);
                    tmp_string.Form(",%f",Sigma_Dy8[RefPMT_CH[PMT_id]-1][HV_id][Amplitude_id]);
                    dy8_sigma_sql.Append(tmp_string);
                }
                dy5_sql.Append(")");
                dy8_sql.Append(")");
                dy5_sigma_sql.Append(")");
                dy8_sigma_sql.Append(")");
                if(!(db->Exec(dy5_sql.Data()))){
                    printf("%s\n",db->GetErrorMsg());
                }
                if(!(db->Exec(dy8_sql.Data()))){
                    printf("%s\n",db->GetErrorMsg());
                }
                if(!(db->Exec(dy5_sigma_sql.Data()))){
                    printf("%s\n",db->GetErrorMsg());
                }
                if(!(db->Exec(dy8_sigma_sql.Data()))){
                    printf("%s\n",db->GetErrorMsg());
                }
        }
    }

    //----------------------------Update table `RefGain/RefGain_Sigma`------------------------------------------------------------------------------------------------------------
    for(int PMT_id=0;PMT_id<RefPMT_NO;PMT_id++){
        for(int Amplitude_id=0;Amplitude_id<GAIN_FILENO;Amplitude_id++){
                gain_sql.Form("INSERT INTO `PMT_V2`.`RefGain` (`RefPMT_id`,`Amplitude_id`,`TestDirectory_id`,`1000V`,`950V`,`900V`,`850V`,`800V`,`750V`,`700V`) VALUES (%d,%d,%d",RefPMT_CH[PMT_id],Amplitude_id+1,TestDirectory_id);
                gain_sigma_sql.Form("INSERT INTO `PMT_V2`.`RefGain_Sigma` (`RefPMT_id`,`Amplitude_id`,`TestDirectory_id`,`1000V`,`950V`,`900V`,`850V`,`800V`,`750V`,`700V`) VALUES (%d,%d,%d",RefPMT_CH[PMT_id],Amplitude_id+1,TestDirectory_id);
                for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
                    tmp_string.Form(",%f",Data_Gain[RefPMT_CH[PMT_id]-1][Amplitude_id][HV_id]);
                    gain_sql.Append(tmp_string);
                    tmp_string.Form(",%f",Sigma_Data_Gain[RefPMT_CH[PMT_id]-1][Amplitude_id][HV_id]);
                    gain_sigma_sql.Append(tmp_string);
                }
                gain_sql.Append(")");
                gain_sigma_sql.Append(")");
                if(!(db->Exec(gain_sql.Data()))){
                    printf("%s\n",db->GetErrorMsg());
                }
                if(!(db->Exec(gain_sigma_sql.Data()))){
                    printf("%s\n",db->GetErrorMsg());
                }
            }

    }

    printf("Insert Raw Data into Database Successfully\n");
    return 0;
}
//---------------------------Dy58 old/new-------------------
int Get_Dy58_old(const Char_t* parentDir)
{
    ReadConfigFile(parentDir);

    //----------------------------------
    char root_dir[300],analysis_dir[300],dy58_dir[300];
    char buffer1[300];

    //--------------------Check if root_file dir has been created-------------------------
    sprintf(root_dir,"%s/root_file",parentDir);
    if(!(gSystem->OpenDirectory(root_dir))){
        RawDataConv(parentDir);
        Fit_PedestalData(parentDir);
        Fit_TestingData(parentDir);
    }

    //--------------------Read configuration and create corresponding directory-----------
    sprintf(analysis_dir,"%s/analysis",parentDir);
    if(!(gSystem->OpenDirectory(analysis_dir)))
        gSystem->MakeDirectory(analysis_dir);
    sprintf(dy58_dir,"%s/dy58",analysis_dir);
    if(!(gSystem->OpenDirectory(dy58_dir)))
        gSystem->MakeDirectory(dy58_dir);

    Int_t voltage[HIGHVOLTAGE_STEP]={1000,950,900,850,800,750,700};
    Int_t amp_limit[HIGHVOLTAGE_STEP]={0,0,0,0,0,0,0};
    Char_t PMT_label[CHANNEL_NUM][30];
    Float_t meanDy5[CHANNEL_NUM][25],sigmaDy5[CHANNEL_NUM][25],meanDy8[CHANNEL_NUM][25],sigmaDy8[CHANNEL_NUM][25];
    Int_t Channel[CHANNEL_NUM];
    Int_t pmt_index,file_index,fileno;

    FILE* fp;
    char filename[300];
    char dir[300];

    TString gtitle;
    TGraph* gdy58[CHANNEL_NUM];
    float recip,ratio;
    char outDAT[200],outPDF[200];
    TCanvas* canvas=new TCanvas("canvas","dy8/dy5",500,600);

    for(int volt_index=0;volt_index<HIGHVOLTAGE_STEP;volt_index++){

        sprintf(dir,"%s/%dV",root_dir,voltage[volt_index]);
        sprintf(filename,"%s/LED.config",dir);
        if(!(fp=fopen(filename,"r"))){
            printf("error: can't opne %s\n",filename);
            return -1;
        }
        fgets(buffer1,200,fp);
        fscanf(fp,"Total datapoints: %d\n",&fileno);
        fclose(fp);

        fileno=fileno-amp_limit[volt_index];//some of the led_setup is not for the dy58 measurement
        for(file_index=0;file_index<fileno;file_index++){
            sprintf(filename,"%s/%d_result.dat",dir,file_index+1);
            if(!(fp=fopen(filename,"r"))){
                printf("error: can't open %s\n",filename);
                return -1;
            }
            fgets(buffer1,200,fp);
            fgets(buffer1,200,fp);
            for(pmt_index=0;pmt_index<CHANNEL_NUM;pmt_index++){
                fscanf(fp,"%d %s %f %f %f %f\n",Channel+pmt_index,PMT_label[pmt_index],meanDy5[pmt_index]+file_index,sigmaDy5[pmt_index]+file_index,meanDy8[pmt_index]+file_index,sigmaDy8[pmt_index]+file_index);
            }
            fclose(fp);
        }

        sprintf(dir,"%s/%dV",dy58_dir,voltage[volt_index]);
        gSystem->MakeDirectory(dir);
        sprintf(outDAT,"%s/%dV_dy58.dat",dir,voltage[volt_index]);
        sprintf(outPDF,"%s/%dV_dy58.pdf",dir,voltage[volt_index]);
        fp=fopen(outDAT,"w");
        if(!fp)
        {
            printf("error: can't create %s\n",outDAT);
            return -1;
        }
        sprintf(buffer1,"%dV dy58 ratio result:\n",voltage[volt_index]);
        fputs(buffer1,fp);
        sprintf(buffer1,"%s[",outPDF);
        canvas->Print(buffer1);
        for(pmt_index=0;pmt_index<CHANNEL_NUM;pmt_index++)
        {
            //printf("PMT_INDEX=%d\n",PMT_INDEX[pmt_index]);
            if(PMT_INDEX[pmt_index] != 0){
                gtitle.Form("%dV,PMT_%s",voltage[volt_index],PMT_label[pmt_index]);
                gdy58[pmt_index]=new TGraph(fileno,meanDy8[pmt_index],meanDy5[pmt_index]);
                gdy58[pmt_index]->SetTitle(gtitle.Data());
                gdy58[pmt_index]->Draw("A*");
                gdy58[pmt_index]->Fit("pol1","","",0,14000);
                TF1* fitfuc=(TF1*)gdy58[pmt_index]->GetFunction("pol1");
                recip=fitfuc->GetParameter(1);
                ratio=1.0/recip;
                fprintf(fp,"%d\t%s\t%f\n",pmt_index+1,PMT_label[pmt_index],ratio);
                canvas->Print(outPDF);
            }
            else{
                fprintf(fp,"%d\tnull\t%f\n",pmt_index+1,0.0);
            }

        }

        sprintf(buffer1,"%s]",outPDF);
        canvas->Print(buffer1);
        fclose(fp);

        //----------------------------------------------------------------------------------------------------------------
    }


    return 0;
}

int Get_Dy58(const Char_t* parentDir,const Int_t testdir_id)
{
    ReadConfigFile(parentDir);
    if(DATAUPDATE_FLAG==0 || TestDirectory_id!=testdir_id){
        ReadIntoDataBuffer(parentDir,testdir_id);
    }
    //---------------------
    char root_dir[300],analysis_dir[300],dy58_dir[300];
    char buffer1[300];
    sprintf(root_dir,"%s/root_file",parentDir);
    sprintf(analysis_dir,"%s/analysis",parentDir);
    if(!(gSystem->OpenDirectory(analysis_dir)))
        gSystem->MakeDirectory(analysis_dir);
    sprintf(dy58_dir,"%s/dy58",analysis_dir);
    if(!(gSystem->OpenDirectory(dy58_dir)))
        gSystem->MakeDirectory(dy58_dir);
    //-----------------------
    FILE* fp;
    char dir[300];

    TString gtitle;
    TGraph* gdy58[CHANNEL_NUM];
    Float_t  recip,ratio;
    Float_t tmp_dy8[MAXIMUM_DATAPOINTS],tmp_dy5[MAXIMUM_DATAPOINTS];
    char outDAT[200],outPDF[200];
    TCanvas* canvas=new TCanvas("canvas","Dy58",500,600);
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        sprintf(dir,"%s/%dV",dy58_dir,VOLTAGE[HV_id]);
        gSystem->MakeDirectory(dir);
        sprintf(outDAT,"%s/%dV_dy58.dat",dir,VOLTAGE[HV_id]);
        sprintf(outPDF,"%s/%dV_dy58.pdf",dir,VOLTAGE[HV_id]);
        fp=fopen(outDAT,"w");
        if(!fp)
        {
            printf("error: can't create %s\n",outDAT);
            return -1;
        }
        sprintf(buffer1,"%dV dy58 result:\n",VOLTAGE[HV_id]);
        fputs(buffer1,fp);
        sprintf(buffer1,"%s[",outPDF);
        canvas->Print(buffer1);
        for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
            if(PMT_INDEX[PMT_id]==0){
                fprintf(fp,"%d\tnull\t%f\n",PMT_id+1,0.0);
            }
            else{
                for(int Amplitude_id=0;Amplitude_id<DY58_FILENO[HV_id];Amplitude_id++){
                    tmp_dy8[Amplitude_id]=Data_Dy8[PMT_id][HV_id][Amplitude_id];
                    tmp_dy5[Amplitude_id]=Data_Dy5[PMT_id][HV_id][Amplitude_id];
                }
                gtitle.Form("Dy58: Channel_%d, %dV, %s",PMT_id+1,VOLTAGE[HV_id],PMT_LABEL[PMT_id]);
                gdy58[PMT_id]=new TGraph(DY58_FILENO[HV_id],tmp_dy8,tmp_dy5);
                gdy58[PMT_id]->SetTitle(gtitle.Data());
                gdy58[PMT_id]->Draw("A*");
                gdy58[PMT_id]->Fit("pol1","","",0,12000);
                TF1* fitfuc=(TF1*)gdy58[PMT_id]->GetFunction("pol1");
                ratio=fitfuc->GetParameter(1);
                recip=1.0/ratio;
                fprintf(fp,"%d\t%s\t%f\n",PMT_id+1,PMT_LABEL[PMT_id],recip);
                canvas->Print(outPDF);
                delete gdy58[PMT_id];
            }
        }
        sprintf(buffer1,"%s]",outPDF);
        canvas->Print(buffer1);
        fclose(fp);
    }
    delete canvas;

    return 0;
}

//-----------------Gain Method1-----------------------------------
int Get_Gain_Method1_All(const char* parentDir,const Int_t testdir_id)
{
    ReadConfigFile(parentDir);
    if(DATAUPDATE_FLAG==0 || TestDirectory_id!=testdir_id){
        ReadIntoDataBuffer(parentDir,testdir_id);
    }
    //---------------------
    char root_dir[300],analysis_dir[300],gain_dir[300];
    char buffer1[300];
    sprintf(root_dir,"%s/root_file",parentDir);
    sprintf(analysis_dir,"%s/analysis",parentDir);
    if(!(gSystem->OpenDirectory(analysis_dir)))
        gSystem->MakeDirectory(analysis_dir);
    sprintf(gain_dir,"%s/gain",analysis_dir);
    if(!(gSystem->OpenDirectory(gain_dir)))
        gSystem->MakeDirectory(gain_dir);
    //-----------------------
    FILE* fp;
    char dir[300];

    TString gtitle;
    TGraph* ggain[CHANNEL_NUM];
    Float_t ratio;
    Float_t tmp_mean[MAXIMUM_DATAPOINTS],tmp_variance[MAXIMUM_DATAPOINTS];
    char outDAT[200],outPDF[200];
    TCanvas* canvas=new TCanvas("canvas","Gain Method_1 (ALL)",500,600);
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        sprintf(dir,"%s/%dV",gain_dir,VOLTAGE[HV_id]);
        gSystem->MakeDirectory(dir);
        sprintf(outDAT,"%s/%dV_gain_method1_all.dat",dir,VOLTAGE[HV_id]);
        sprintf(outPDF,"%s/%dV_gain_method1_all.pdf",dir,VOLTAGE[HV_id]);
        fp=fopen(outDAT,"w");
        if(!fp)
        {
            printf("error: can't create %s\n",outDAT);
            return -1;
        }
        sprintf(buffer1,"%dV gain result(method1,all):\n",VOLTAGE[HV_id]);
        fputs(buffer1,fp);
        sprintf(buffer1,"%s[",outPDF);
        canvas->Print(buffer1);
        for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
            if(PMT_INDEX[PMT_id]==0){
                fprintf(fp,"%d\tnull\t%f\n",PMT_id+1,0.0);
            }
            else{
                for(int Amplitude_id=0;Amplitude_id<TOTAL_FILENO[HV_id];Amplitude_id++){
                    tmp_mean[Amplitude_id]=Data_Dy8[PMT_id][HV_id][Amplitude_id];
                    tmp_variance[Amplitude_id]=TMath::Power(Sigma_Dy8[PMT_id][HV_id][Amplitude_id],2);
                }
                gtitle.Form("Gain: Channel_%d, %dV, %s (method1,all)",PMT_id+1,VOLTAGE[HV_id],PMT_LABEL[PMT_id]);
                ggain[PMT_id]=new TGraph(TOTAL_FILENO[HV_id],tmp_mean,tmp_variance);
                ggain[PMT_id]->SetTitle(gtitle.Data());
                ggain[PMT_id]->Draw("A*");
                ggain[PMT_id]->Fit("pol1","","",0,12000);
                TF1* fitfuc=(TF1*)ggain[PMT_id]->GetFunction("pol1");
                ratio=fitfuc->GetParameter(1);
                fprintf(fp,"%d\t%s\t%f\n",PMT_id+1,PMT_LABEL[PMT_id],ratio);
                canvas->Print(outPDF);
                delete ggain[PMT_id];
            }
        }
        sprintf(buffer1,"%s]",outPDF);
        canvas->Print(buffer1);
        fclose(fp);
    }

    return 0;
}

int Get_Gain_Method1_Group1(const char* parentDir,const Int_t testdir_id)
{
    ReadConfigFile(parentDir);
    if(DATAUPDATE_FLAG==0 || TestDirectory_id!=testdir_id){
        ReadIntoDataBuffer(parentDir,testdir_id);
    }
    //---------------------
    char root_dir[300],analysis_dir[300],gain_dir[300];
    char buffer1[300];
    sprintf(root_dir,"%s/root_file",parentDir);
    sprintf(analysis_dir,"%s/analysis",parentDir);
    if(!(gSystem->OpenDirectory(analysis_dir)))
        gSystem->MakeDirectory(analysis_dir);
    sprintf(gain_dir,"%s/gain",analysis_dir);
    if(!(gSystem->OpenDirectory(gain_dir)))
        gSystem->MakeDirectory(gain_dir);
    //---------------------------
    FILE* fp;
    char dir[300];

    TString gtitle;
    TGraph* ggain[CHANNEL_NUM];
    Float_t ratio;
    Float_t tmp_mean[MAXIMUM_DATAPOINTS],tmp_variance[MAXIMUM_DATAPOINTS];
    char outDAT[200],outPDF[200];
    TCanvas* canvas=new TCanvas("canvas","Gain Method_1(Group_1)",500,600);
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        sprintf(dir,"%s/%dV",gain_dir,VOLTAGE[HV_id]);
        gSystem->MakeDirectory(dir);
        sprintf(outDAT,"%s/%dV_gain_method1_group1.dat",dir,VOLTAGE[HV_id]);
        sprintf(outPDF,"%s/%dV_gain_method1_group1.pdf",dir,VOLTAGE[HV_id]);
        fp=fopen(outDAT,"w");
        if(!fp)
        {
            printf("error: can't create %s\n",outDAT);
            return -1;
        }
        sprintf(buffer1,"%dV gain result(method1,group1):\n",VOLTAGE[HV_id]);
        fputs(buffer1,fp);
        sprintf(buffer1,"%s[",outPDF);
        canvas->Print(buffer1);
        for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
            if(PMT_INDEX[PMT_id]==0){
                fprintf(fp,"%d\tnull\t%f\n",PMT_id+1,0.0);
            }
            else{
                for(int Amplitude_id=0;Amplitude_id<GROUP1_FILENO[HV_id];Amplitude_id++){
                    tmp_mean[Amplitude_id]=Data_Dy8[PMT_id][HV_id][GROUP1_FILEINDEX[HV_id][Amplitude_id]-1];
                    tmp_variance[Amplitude_id]=TMath::Power(Sigma_Dy8[PMT_id][HV_id][GROUP1_FILEINDEX[HV_id][Amplitude_id]-1],2);
                }
                gtitle.Form("Gain: Channel_%d, %dV, %s (method1, group1)",PMT_id+1,VOLTAGE[HV_id],PMT_LABEL[PMT_id]);
                ggain[PMT_id]=new TGraph(GROUP1_FILENO[HV_id],tmp_mean,tmp_variance);
                ggain[PMT_id]->SetTitle(gtitle.Data());
                ggain[PMT_id]->Draw("A*");
                ggain[PMT_id]->Fit("pol1","","",0,12000);
                TF1* fitfuc=(TF1*)ggain[PMT_id]->GetFunction("pol1");
                ratio=fitfuc->GetParameter(1);
                fprintf(fp,"%d\t%s\t%f\n",PMT_id+1,PMT_LABEL[PMT_id],ratio);
                canvas->Print(outPDF);
                delete ggain[PMT_id];
            }
        }
        sprintf(buffer1,"%s]",outPDF);
        canvas->Print(buffer1);
        fclose(fp);
    }

    return 0;
}

int Get_Gain_Method1_Group2(const char* parentDir,const Int_t testdir_id)
{
    ReadConfigFile(parentDir);
    if(DATAUPDATE_FLAG==0 || TestDirectory_id!=testdir_id){
        ReadIntoDataBuffer(parentDir,testdir_id);
    }
    //---------------------
    char root_dir[300],analysis_dir[300],gain_dir[300];
    char buffer1[300];
    sprintf(root_dir,"%s/root_file",parentDir);
    sprintf(analysis_dir,"%s/analysis",parentDir);
    if(!(gSystem->OpenDirectory(analysis_dir)))
        gSystem->MakeDirectory(analysis_dir);
    sprintf(gain_dir,"%s/gain",analysis_dir);
    if(!(gSystem->OpenDirectory(gain_dir)))
        gSystem->MakeDirectory(gain_dir);
    //---------------------------
    FILE* fp;
    char dir[300];

    TString gtitle;
    TGraph* ggain[CHANNEL_NUM];
    Float_t ratio;
    Float_t tmp_mean[MAXIMUM_DATAPOINTS],tmp_variance[MAXIMUM_DATAPOINTS];
    char outDAT[200],outPDF[200];
    TCanvas* canvas=new TCanvas("canvas","Gain Method_1(Group_2)",500,600);
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        sprintf(dir,"%s/%dV",gain_dir,VOLTAGE[HV_id]);
        gSystem->MakeDirectory(dir);
        sprintf(outDAT,"%s/%dV_gain_method1_group2.dat",dir,VOLTAGE[HV_id]);
        sprintf(outPDF,"%s/%dV_gain_method1_group2.pdf",dir,VOLTAGE[HV_id]);
        fp=fopen(outDAT,"w");
        if(!fp)
        {
            printf("error: can't create %s\n",outDAT);
            return -1;
        }
        sprintf(buffer1,"%dV gain result(method1,group2):\n",VOLTAGE[HV_id]);
        fputs(buffer1,fp);
        sprintf(buffer1,"%s[",outPDF);
        canvas->Print(buffer1);
        for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
            if(PMT_INDEX[PMT_id]==0){
                fprintf(fp,"%d\tnull\t%f\n",PMT_id+1,0.0);
            }
            else{
                for(int Amplitude_id=0;Amplitude_id<GROUP2_FILENO[HV_id];Amplitude_id++){
                    tmp_mean[Amplitude_id]=Data_Dy8[PMT_id][HV_id][GROUP2_FILEINDEX[HV_id][Amplitude_id]-1];
                    tmp_variance[Amplitude_id]=TMath::Power(Sigma_Dy8[PMT_id][HV_id][GROUP2_FILEINDEX[HV_id][Amplitude_id]-1],2);
                }
                gtitle.Form("Gain: Channel_%d, %dV, %s (method1, group2)",PMT_id+1,VOLTAGE[HV_id],PMT_LABEL[PMT_id]);
                ggain[PMT_id]=new TGraph(GROUP2_FILENO[HV_id],tmp_mean,tmp_variance);
                ggain[PMT_id]->SetTitle(gtitle.Data());
                ggain[PMT_id]->Draw("A*");
                ggain[PMT_id]->Fit("pol1","","",0,12000);
                TF1* fitfuc=(TF1*)ggain[PMT_id]->GetFunction("pol1");
                ratio=fitfuc->GetParameter(1);
                fprintf(fp,"%d\t%s\t%f\n",PMT_id+1,PMT_LABEL[PMT_id],ratio);
                canvas->Print(outPDF);
                delete ggain[PMT_id];
            }
        }
        sprintf(buffer1,"%s]",outPDF);
        canvas->Print(buffer1);
        fclose(fp);
    }

    return 0;
}

int Get_Gain_Method1_Group3(const char* parentDir,const Int_t testdir_id)
{
    ReadConfigFile(parentDir);
    if(DATAUPDATE_FLAG==0 || TestDirectory_id!=testdir_id){
        ReadIntoDataBuffer(parentDir,testdir_id);
    }
    //---------------------
    char root_dir[300],analysis_dir[300],gain_dir[300];
    char buffer1[300];
    sprintf(root_dir,"%s/root_file",parentDir);
    sprintf(analysis_dir,"%s/analysis",parentDir);
    if(!(gSystem->OpenDirectory(analysis_dir)))
        gSystem->MakeDirectory(analysis_dir);
    sprintf(gain_dir,"%s/gain",analysis_dir);
    if(!(gSystem->OpenDirectory(gain_dir)))
        gSystem->MakeDirectory(gain_dir);
    //---------------------------
    FILE* fp;
    char dir[300];

    TString gtitle;
    TGraph* ggain[CHANNEL_NUM];
    Float_t ratio;
    Float_t tmp_mean[MAXIMUM_DATAPOINTS],tmp_variance[MAXIMUM_DATAPOINTS];
    char outDAT[200],outPDF[200];
    TCanvas* canvas=new TCanvas("canvas","Gain Method_1(Group_3)",500,600);
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        sprintf(dir,"%s/%dV",gain_dir,VOLTAGE[HV_id]);
        gSystem->MakeDirectory(dir);
        sprintf(outDAT,"%s/%dV_gain_method1_group3.dat",dir,VOLTAGE[HV_id]);
        sprintf(outPDF,"%s/%dV_gain_method1_group3.pdf",dir,VOLTAGE[HV_id]);
        fp=fopen(outDAT,"w");
        if(!fp)
        {
            printf("error: can't create %s\n",outDAT);
            return -1;
        }
        sprintf(buffer1,"%dV gain result(method1,group3):\n",VOLTAGE[HV_id]);
        fputs(buffer1,fp);
        sprintf(buffer1,"%s[",outPDF);
        canvas->Print(buffer1);
        for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
            if(PMT_INDEX[PMT_id]==0){
                fprintf(fp,"%d\tnull\t%f\n",PMT_id+1,0.0);
            }
            else{
                for(int Amplitude_id=0;Amplitude_id<GROUP3_FILENO[HV_id];Amplitude_id++){
                    tmp_mean[Amplitude_id]=Data_Dy8[PMT_id][HV_id][GROUP3_FILEINDEX[HV_id][Amplitude_id]-1];
                    tmp_variance[Amplitude_id]=TMath::Power(Sigma_Dy8[PMT_id][HV_id][GROUP3_FILEINDEX[HV_id][Amplitude_id]-1],2);
                }
                gtitle.Form("Gain: Channel_%d, %dV, %s (method1, group3)",PMT_id+1,VOLTAGE[HV_id],PMT_LABEL[PMT_id]);
                ggain[PMT_id]=new TGraph(GROUP3_FILENO[HV_id],tmp_mean,tmp_variance);
                ggain[PMT_id]->SetTitle(gtitle.Data());
                ggain[PMT_id]->Draw("A*");
                ggain[PMT_id]->Fit("pol1","","",0,12000);
                TF1* fitfuc=(TF1*)ggain[PMT_id]->GetFunction("pol1");
                ratio=fitfuc->GetParameter(1);
                fprintf(fp,"%d\t%s\t%f\n",PMT_id+1,PMT_LABEL[PMT_id],ratio);
                canvas->Print(outPDF);
                delete ggain[PMT_id];
            }
        }
        sprintf(buffer1,"%s]",outPDF);
        canvas->Print(buffer1);
        fclose(fp);
    }

    return 0;
}

int print_channel_diff_M(const Char_t* parentDir,const Int_t ch_num,const Int_t group_id=0)
{
    char root_dir[300],analysis_dir[300],gain_dir[300];
    char buffer[256];
    sprintf(root_dir,"%s/root_file",parentDir);
    sprintf(analysis_dir,"%s/analysis",parentDir);
    sprintf(gain_dir,"%s/gain",analysis_dir);
    FILE* fp_out;
    fp_out=fopen("channel_diff.csv","w");
    fprintf(fp_out,"%s channel_%d:\n",parentDir,ch_num);
    fprintf(fp_out,"voltage:\t1000V\t950V\t900V\t850V\t800V\t750V\t700V\n");

    FILE* fp_in;
    char dir[300];
    int tmp_ch;
    char tmp_label[20];
    Float_t tmp_value;
    //--M electron multiplier gain---
    fprintf(fp_out,"M:");
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        if(!group_id){
            sprintf(dir,"%s/%dV/%dV_gain_method1_all.dat",gain_dir,VOLTAGE[HV_id],VOLTAGE[HV_id]);
        }
        else{
            sprintf(dir,"%s/%dV/%dV_gain_method1_group%d.dat",gain_dir,VOLTAGE[HV_id],VOLTAGE[HV_id],group_id);
        }
        fp_in=fopen(dir,"r");
        fgets(buffer,256,fp_in);
        for(int i=1;i<ch_num;i++){
            fgets(buffer,256,fp_in);
        }
        fscanf(fp_in,"%d %s %f\n",&tmp_ch,tmp_label,&tmp_value);
        fprintf(fp_out,"\t%f",tmp_value);
        fclose(fp_in);
    }
    fprintf(fp_out,"\n");

    fclose(fp_out);
    return 0;
}
//-----------------Gain Method 2---------------------
int Calib_Step1(const char* testDir,const Int_t testdir_id,const char* refDir="retest_1",const Int_t refdir_id=1)
{
    ReadConfigFile(testDir);
    if(DATAUPDATE_FLAG==0 || TestDirectory_id!=testdir_id){
        ReadIntoDataBuffer(testDir,testdir_id);
    }
    if(REFDATAUPDATE_FLAG==0 || RefDirectory_id!=refdir_id){
        ReadRefIntoDataBuffer(refDir,refdir_id);
    }

    for(int refPMT_id=0;refPMT_id<RefPMT_NO;refPMT_id++){
        for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
            for(int Amplitude_id=0;Amplitude_id<TOTAL_FILENO[HV_id];Amplitude_id++){
                Calib_Ratio[refPMT_id][HV_id][Amplitude_id]=Data_Dy8[refPMT_id][HV_id][Amplitude_id]/Ref_Dy8[refPMT_id][HV_id][Amplitude_id];
            }
        }
    }

    return 0;
}

int Calib_Step2(const Int_t refPMT_channel=1)
{
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        for(int Amplitude_id=0;Amplitude_id<GAIN_FILENO;Amplitude_id++){
            for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
                Calibrated_Data_Gain_Step1[PMT_id][Amplitude_id][HV_id]=Data_Gain[PMT_id][Amplitude_id][HV_id]/
                        Calib_Ratio[refPMT_channel-1][HV_id][GAIN_FILEINDEX[HV_id][Amplitude_id]-1];
            }
        }
    }

    return 0;
}

int Calib_Step3()
{
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        for(int Amplitude_id=0;Amplitude_id<GAIN_FILENO;Amplitude_id++){
            for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
                Calibrated_Data_Gain_Step2[PMT_id][Amplitude_id][HV_id]=Calibrated_Data_Gain_Step1[PMT_id][Amplitude_id][HV_id]/
                        CHANNEL_DIFF[PMT_id];
            }
        }
    }

    return 0;
}

int print_calib_result(const char* testDir,const Int_t testdir_id,const char* refDir="retest_1",const Int_t refdir_id=1,const Int_t refPMT_channel=1)
{
    Calib_Step1(testDir,testdir_id,refDir,refdir_id);
    Calib_Step2(refPMT_channel);
    Calib_Step3();
    //-----------------------------
    char analysis_dir[300],gain_dir[300];
    char buffer1[300];
    sprintf(analysis_dir,"%s/analysis",testDir);
    if(!(gSystem->OpenDirectory(analysis_dir)))
        gSystem->MakeDirectory(analysis_dir);
    sprintf(gain_dir,"%s/gain",analysis_dir);
    if(!(gSystem->OpenDirectory(gain_dir)))
        gSystem->MakeDirectory(gain_dir);
    //---------------------------
    FILE* fp;
    char dir[300],outDAT[256];
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        sprintf(dir,"%s/%dV",gain_dir,VOLTAGE[HV_id]);
        for(int Amplitude_id=0;Amplitude_id<GAIN_FILENO;Amplitude_id++){
            sprintf(outDAT,"%s/%dV_gain_method2_amp%d_step1_ref%d.dat",dir,VOLTAGE[HV_id],Amplitude_id+1,refPMT_channel);
            fp=fopen(outDAT,"w");
            if(!fp)
            {
                printf("error: can't create %s\n",outDAT);
                return -1;
            }
            sprintf(buffer1,"%dV gain result(method2,amplitude%d,step1,ref%d):\n",VOLTAGE[HV_id],Amplitude_id+1,refPMT_channel);
            fputs(buffer1,fp);
            for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
                if(PMT_INDEX[PMT_id]==0){
                    fprintf(fp,"%d\tnull\t%f\n",PMT_id+1,0.0);
                }
                else{
                    fprintf(fp,"%d\t%s\t%f\n",PMT_id+1,PMT_LABEL[PMT_id],Calibrated_Data_Gain_Step1[PMT_id][Amplitude_id][HV_id]);
                }
            }
            fclose(fp);
        }

        for(int Amplitude_id=0;Amplitude_id<GAIN_FILENO;Amplitude_id++){
            sprintf(outDAT,"%s/%dV_gain_method2_amp%d_step2_ref%d.dat",dir,VOLTAGE[HV_id],Amplitude_id+1,refPMT_channel);
            fp=fopen(outDAT,"w");
            if(!fp)
            {
                printf("error: can't create %s\n",outDAT);
                return -1;
            }
            sprintf(buffer1,"%dV gain result(method2,amplitude%d,step2,ref%d):\n",VOLTAGE[HV_id],Amplitude_id+1,refPMT_channel);
            fputs(buffer1,fp);
            for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
                if(PMT_INDEX[PMT_id]==0){
                    fprintf(fp,"%d\tnull\t%f\n",PMT_id+1,0.0);
                }
                else{
                    fprintf(fp,"%d\t%s\t%f\n",PMT_id+1,PMT_LABEL[PMT_id],Calibrated_Data_Gain_Step2[PMT_id][Amplitude_id][HV_id]);
                }
            }
            fclose(fp);
        }

    }

    return 0;
}

int print_channel_diff_datagain(const Char_t* parentDir,const Int_t ch_num,const Int_t refPMT_id=1)
{
    char root_dir[300],analysis_dir[300],gain_dir[300];
    char buffer[256];
    sprintf(root_dir,"%s/root_file",parentDir);
    sprintf(analysis_dir,"%s/analysis",parentDir);
    sprintf(gain_dir,"%s/gain",analysis_dir);
    FILE* fp_out;
    fp_out=fopen("channel_diff.csv","w");
    fprintf(fp_out,"%s channel_%d:\n",parentDir,ch_num);
    fprintf(fp_out,"voltage:\t1000V\t950V\t900V\t850V\t800V\t750V\t700V\n");

    FILE* fp_in;
    char dir[300];
    int tmp_ch;
    char tmp_label[20];
    Float_t tmp_value;
    //--calibrated datagain---
    for(int Amplitude_id=0;Amplitude_id<GAIN_FILENO;Amplitude_id++){
        fprintf(fp_out,"Amp%d:",Amplitude_id+1);
        for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
            sprintf(dir,"%s/%dV/%dV_gain_method2_amp%d_step1_ref%d.dat",gain_dir,VOLTAGE[HV_id],VOLTAGE[HV_id],Amplitude_id+1,refPMT_id);
            fp_in=fopen(dir,"r");
            fgets(buffer,256,fp_in);
            for(int i=1;i<ch_num;i++){
                fgets(buffer,256,fp_in);
            }
            fscanf(fp_in,"%d %s %f\n",&tmp_ch,tmp_label,&tmp_value);
            fprintf(fp_out,"\t%f",tmp_value);
            fclose(fp_in);
        }
        fprintf(fp_out,"\n");
    }

    fclose(fp_out);
    return 0;
}

//-----------------Gain Method 3-----------------------
int Get_Gain_Method3_Step1(const char* parentDir,const Int_t testdir_id,const Int_t refPMT_id=1)
{
    ReadConfigFile(parentDir);
    if(DATAUPDATE_FLAG==0 || TestDirectory_id!=testdir_id){
        ReadIntoDataBuffer(parentDir,testdir_id);
    }
    //---------------------
    char root_dir[300],analysis_dir[300],gain_dir[300];
    char buffer1[300];
    sprintf(root_dir,"%s/root_file",parentDir);
    sprintf(analysis_dir,"%s/analysis",parentDir);
    if(!(gSystem->OpenDirectory(analysis_dir)))
        gSystem->MakeDirectory(analysis_dir);
    sprintf(gain_dir,"%s/gain",analysis_dir);
    if(!(gSystem->OpenDirectory(gain_dir)))
        gSystem->MakeDirectory(gain_dir);
    //-----------------------
    FILE* fp;
    char dir[300];

    TString gtitle;
    TGraph* ggain[CHANNEL_NUM];
    Float_t ratio;
    Float_t tmp_ref[MAXIMUM_DATAPOINTS],tmp_test[MAXIMUM_DATAPOINTS];
    char outDAT[200],outPDF[200];
    TCanvas* canvas=new TCanvas("canvas","Gain Method_3,Step_1",500,600);
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        sprintf(dir,"%s/%dV",gain_dir,VOLTAGE[HV_id]);
        gSystem->MakeDirectory(dir);
        sprintf(outDAT,"%s/%dV_gain_method3_step1_ref%d.dat",dir,VOLTAGE[HV_id],refPMT_id);
        sprintf(outPDF,"%s/%dV_gain_method3_step1_ref%d.pdf",dir,VOLTAGE[HV_id],refPMT_id);
        fp=fopen(outDAT,"w");
        if(!fp)
        {
            printf("error: can't create %s\n",outDAT);
            return -1;
        }
        sprintf(buffer1,"%dV gain result(method3,step1,ref%d):\n",VOLTAGE[HV_id],refPMT_id);
        fputs(buffer1,fp);
        sprintf(buffer1,"%s[",outPDF);
        canvas->Print(buffer1);
        for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
            if(PMT_INDEX[PMT_id]==0){
                fprintf(fp,"%d\tnull\t%f\n",PMT_id+1,0.0);
            }
            else{
                for(int Amplitude_id=0;Amplitude_id<DY58_FILENO[HV_id];Amplitude_id++){
                    tmp_ref[Amplitude_id]=Data_Dy8[refPMT_id-1][HV_id][Amplitude_id];
                    tmp_test[Amplitude_id]=Data_Dy8[PMT_id][HV_id][Amplitude_id];
                }
                gtitle.Form("Gain: Channel_%d, %dV, %s (method3,step1,ref%d)",PMT_id+1,VOLTAGE[HV_id],PMT_LABEL[PMT_id],refPMT_id);
                ggain[PMT_id]=new TGraph(DY58_FILENO[HV_id],tmp_ref,tmp_test);
                ggain[PMT_id]->SetTitle(gtitle.Data());
                ggain[PMT_id]->Draw("A*");
                ggain[PMT_id]->Fit("pol1","","",0,6000);
                TF1* fitfuc=(TF1*)ggain[PMT_id]->GetFunction("pol1");
                ratio=fitfuc->GetParameter(1);
                fprintf(fp,"%d\t%s\t%f\n",PMT_id+1,PMT_LABEL[PMT_id],ratio);
                canvas->Print(outPDF);
                delete ggain[PMT_id];
            }
        }
        sprintf(buffer1,"%s]",outPDF);
        canvas->Print(buffer1);
        fclose(fp);
    }

    delete canvas;
    return 0;
}

//---------print channel diff result---------------------
int print_channel_diff(const Char_t* parentDir,const Int_t ch_num)
{
    char root_dir[300],analysis_dir[300],gain_dir[300],dy58_dir[300];
    char buffer[256];
    sprintf(root_dir,"%s/root_file",parentDir);
    sprintf(analysis_dir,"%s/analysis",parentDir);
    sprintf(gain_dir,"%s/gain",analysis_dir);
    sprintf(dy58_dir,"%s/dy58",analysis_dir);
    FILE* fp_out;
    fp_out=fopen("channel_diff.csv","w");
    fprintf(fp_out,"%s channel_%d:\n",parentDir,ch_num);
    fprintf(fp_out,"voltage:\t1000V\t950V\t900V\t850V\t800V\t750V\t700V\n");

    FILE* fp_in;
    char dir[300];
    int tmp_ch;
    char tmp_label[20];
    Float_t tmp_value;
    //--dy58--
    fprintf(fp_out,"dy58:");
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        sprintf(dir,"%s/%dV/%dV_dy58.dat",dy58_dir,VOLTAGE[HV_id],VOLTAGE[HV_id]);
        fp_in=fopen(dir,"r");
        fgets(buffer,256,fp_in);
        for(int i=1;i<ch_num;i++){
            fgets(buffer,256,fp_in);
        }
        fscanf(fp_in,"%d %s %f\n",&tmp_ch,tmp_label,&tmp_value);
        fprintf(fp_out,"\t%f",tmp_value);
        fclose(fp_in);
    }
    fprintf(fp_out,"\n");
    //--gain method3---
    fprintf(fp_out,"gain_method3:");
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        sprintf(dir,"%s/%dV/%dV_gain_method3_step1_ref1.dat",gain_dir,VOLTAGE[HV_id],VOLTAGE[HV_id]);
        fp_in=fopen(dir,"r");
        fgets(buffer,256,fp_in);
        for(int i=1;i<ch_num;i++){
            fgets(buffer,256,fp_in);
        }
        fscanf(fp_in,"%d %s %f\n",&tmp_ch,tmp_label,&tmp_value);
        fprintf(fp_out,"\t%f",tmp_value);
        fclose(fp_in);
    }
    fprintf(fp_out,"\n");

    fclose(fp_out);
    return 0;
}

//------------------------------------------------------
int process(const Char_t* parentDir)
{
    RawDataConv(parentDir);
    ReadConfigFile(parentDir);
    Fit_PedestalData(parentDir);
    Fit_TestingData(parentDir);
    //-------------------------------------------------------------------------------------
    return 0;
}

