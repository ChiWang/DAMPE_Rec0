/*
 *  $Id: DmpAlgRec0_RelationCheck.cc, 2015-04-24 13:24:11 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 19/07/2014
*/

//#include <stdio.h>
#include <stdlib.h>     // getenv()
#include <boost/lexical_cast.hpp>
//#include "TFile.h"
#include "TMath.h"

#include "DmpEvtBgoRaw.h"
#include "DmpDataBuffer.h"
#include "DmpBgoBase.h"
#include "DmpPsdBase.h"
#include "DmpCore.h"
#include "DmpAlgRec0_RelationCheck.h"
//#define LastBigADC  2000

//-------------------------------------------------------------------
DmpAlgRec0_RelationCheck::DmpAlgRec0_RelationCheck()
 :DmpVAlg("DmpAlgRec0_RelationCheck"),
  fEvtBgo(0),
  fEvtPsd(0)
  //_fLastEvtBgo(0),
  //_fLastEvtPsd(0)
{
  gRootIOSvc->SetOutFileKey("Rec0");
  std::string root_path = (std::string)getenv("DMPSWSYS")+"/share/Calibration";
  gCore->GetJobOption()->SetOption(this->Name()+"/BgoPedestal",root_path+"/Bgo.ped");
  gCore->GetJobOption()->SetOption(this->Name()+"/PsdPedestal",root_path+"/Psd.ped");
  gCore->GetJobOption()->SetOption(this->Name()+"/BgoRelation",root_path+"/Bgo.rel");
  gCore->GetJobOption()->SetOption(this->Name()+"/PsdRelation",root_path+"/Psd.rel");
  //_fLastEvtBgo = new  DmpEvtBgoRaw();
  //_fLastEvtPsd = new  DmpEvtPsdRaw();
}

//-------------------------------------------------------------------
DmpAlgRec0_RelationCheck::~DmpAlgRec0_RelationCheck(){
        //delete _fLastEvtBgo;
        //delete _fLastEvtPsd;
}

void DmpAlgRec0_RelationCheck::SetPedestalFile(std::string ID,std::string f)
{
        TString xx = f;
        if(not xx.Contains(".ped")){
                DmpLogWarning<<f<<"("<<ID<<") is not a pedestal file... will use the defaur one"<<DmpLogEndl;
                return;
        }
  TString Id = ID;
  Id.ToUpper();
  if(Id == "BGO"){
    gCore->GetJobOption()->SetOption(this->Name()+"/BgoPedestal",f);
  }else if(Id == "PSD"){
    gCore->GetJobOption()->SetOption(this->Name()+"/PsdPedestal",f);
  }
}

void DmpAlgRec0_RelationCheck::SetRelationFile(std::string ID,std::string f)
{
        TString xx = f;
        if(not xx.Contains(".rel")){
                DmpLogWarning<<f<<"("<<ID<<") is not a relation file... will use the defaur one"<<DmpLogEndl;
                return;
        }
  TString Id = ID;
  Id.ToUpper();
  if(Id == "BGO"){
    gCore->GetJobOption()->SetOption(this->Name()+"/BgoRelation",f);
  }else if(Id == "PSD"){
    gCore->GetJobOption()->SetOption(this->Name()+"/PsdRelation",f);
  }
}

//-------------------------------------------------------------------
bool DmpAlgRec0_RelationCheck::Initialize()
{
  // read input data
  std::string path_i = "Event/Rec0";
  fEvtBgo = dynamic_cast<DmpEvtBgoRaw*>(gDataBuffer->ReadObject(path_i+"/Bgo"));
  if(0 == fEvtBgo){
    fEvtBgo = new DmpEvtBgoRaw();
    gDataBuffer->LinkRootFile(path_i+"/Bgo",fEvtBgo);
  }
  fEvtPsd = dynamic_cast<DmpEvtPsdRaw*>(gDataBuffer->ReadObject(path_i+"/Psd"));
  if(0 == fEvtPsd){
    fEvtPsd = new DmpEvtPsdRaw();
    gDataBuffer->LinkRootFile(path_i+"/Psd",fEvtPsd);
  }
  //std::string path_o = "Event/Rec0.1";
  //gDataBuffer->RegisterObject(path_o+"/Bgo",fEvtBgo);
  //gDataBuffer->RegisterObject(path_o+"/Psd",fEvtPsd);
  DmpParameterSteering steering;
  std::string inputFile = gCore->GetJobOption()->GetValue(this->Name()+"/BgoPedestal");
  bool loadSta = DAMPE::LoadParameters(inputFile,fBgoPed,steering);
  if(not loadSta){
          return false;
  }else{
    gCore->GetJobOption()->SetOption(this->Name()+"/BgoPedestal/t0",steering["StartTime"]);
    gCore->GetJobOption()->SetOption(this->Name()+"/BgoPedestal/t1",steering["StopTime"]);
  }
  inputFile = gCore->GetJobOption()->GetValue(this->Name()+"/BgoRelation");
  loadSta = DAMPE::LoadParameters(inputFile,fBgoRel,steering);
  if(not loadSta){
          return false;
  }else{
    gCore->GetJobOption()->SetOption(this->Name()+"/BgoRelation/t0",steering["StartTime"]);
    gCore->GetJobOption()->SetOption(this->Name()+"/BgoRelation/t1",steering["StopTime"]);
  }

  inputFile = gCore->GetJobOption()->GetValue(this->Name()+"/PsdPedestal");
  loadSta = DAMPE::LoadParameters(inputFile,fPsdPed,steering);
  if(not loadSta){
          return false;
  }else{
    gCore->GetJobOption()->SetOption(this->Name()+"/PsdPedestal/t0",steering["StartTime"]);
    gCore->GetJobOption()->SetOption(this->Name()+"/PsdPedestal/t1",steering["StopTime"]);
  }
  inputFile = gCore->GetJobOption()->GetValue(this->Name()+"/PsdRelation");
  loadSta = DAMPE::LoadParameters(inputFile,fPsdRel,steering);
  if(not loadSta){
          return false;
  }else{
    gCore->GetJobOption()->SetOption(this->Name()+"/PsdRelation/t0",steering["StartTime"]);
    gCore->GetJobOption()->SetOption(this->Name()+"/PsdRelation/t1",steering["StopTime"]);
  }
  for(short l=0;l<DmpParameterBgo::kPlaneNo*2;++l){
    for(short b=0;b<DmpParameterBgo::kBarNo;++b){
      for(short s=0;s<2;++s){
        for(short d=0;d<2;++d){  // only for dy2,5
          short gid_s = DmpBgoBase::ConstructGlobalDynodeID(l,b,s,d*3+2);
          short gid_b = DmpBgoBase::ConstructGlobalDynodeID(l,b,s,(d+1)*3+2);
          //std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")\tl="<<l<<"\tb ="<<b<<"\ts="<<s<<"\td="<<d<<"\t\tped size ="<<fBgoPed[gid_s].size()<<"\t\t"<<fBgoPed[gid_b].size()<<"\t\t"<<fBgoRel[gid_s].size()<<std::endl;
          fTotSigmaBgo[gid_s] = TMath::Sqrt(TMath::Power(fBgoPed[gid_s].at(5),2) + TMath::Power(fBgoPed[gid_b].at(5)*fBgoRel[gid_s].at(5),2));
          //fTotSigmaBgo[gid_s] = TMath::Sqrt(TMath::Power(fBgoPed[gid_s].at(5),2) + TMath::Power(fBgoPed[gid_b].at(5)*fBgoRel[gid_s].at(5),2) + TMath::Power(fBgoRel[gid_s].at(4)/2,2));
        }
      }
    }
  }
  for(short l=0;l<DmpParameterPsd::kPlaneNo*2;++l){
    for(short b=0;b<DmpParameterPsd::kStripNo;++b){
      for(short s=0;s<2;++s){
        short gid_s = DmpPsdBase::ConstructGlobalDynodeID(l,b,s,5);
        short gid_b = DmpPsdBase::ConstructGlobalDynodeID(l,b,s,8);
          //std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")\tgid = "<<gid_s<<"\tl="<<l<<"\tb ="<<b<<"\ts="<<s<<"\t\tped size ="<<fPsdPed[gid_s].size()<<"\t\t"<<fPsdPed[gid_b].size()<<"\t\t"<<fPsdRel[gid_s].size()<<std::endl;
        fTotSigmaPsd[gid_s] = TMath::Sqrt(TMath::Power(fPsdPed[gid_s].at(5),2) + TMath::Power(fPsdPed[gid_b].at(5)*fPsdRel[gid_s].at(5),2));
      }
    }
  }
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgRec0_RelationCheck::ProcessThisEvent()
{
  // Bgo relation check
  short gid[2][3]; //  side0,1;  2,5,8;
  double sign[2][3];// side0,1;  2,5,8;
  for(short l=0;l<DmpParameterBgo::kPlaneNo*2;++l){
    for(short b=0;b<DmpParameterBgo::kBarNo;++b){
      for(short s=0;s<DmpParameterBgo::kSideNo;++s){
        for(short dy=0;dy<3;++dy){ // load signal
          gid[s][dy] = DmpBgoBase::ConstructGlobalDynodeID(l,b,s,3*dy+2);
          if(fEvtBgo->fADC.find(gid[s][dy]) != fEvtBgo->fADC.end()){
            sign[s][dy] = fEvtBgo->fADC.at(gid[s][dy]);
          }else{
            sign[s][dy] = 0;
          }
        }
        for(short dy=0;dy<3;++dy){ // compress overflow
          if(sign[s][dy] > 14000){
            fEvtBgo->fADC.erase(gid[s][dy]);
            sign[s][dy] = 0;
          }
        }
        for(short dy=0;dy<2;++dy){ // use small check bigger
          if(sign[s][dy] < 5 * fBgoPed[gid[s][dy]].at(5)) continue;    // must bigger than 5 *sigma
          if(sign[s][dy+1] < 10 * fBgoPed[gid[s][dy+1]].at(5)) continue;    // must bigger than 10 *sigma
          double deltaADC_small = sign[s][dy+1]*fBgoRel[gid[s][dy]].at(5) + fBgoRel[gid[s][dy]].at(4)  - sign[s][dy];
          bool comp = TMath::Abs(deltaADC_small) > 3*fTotSigmaBgo.at(gid[s][dy]); // affected by last event
          //bool lastBigEvt = false;
          /*
          if(_fLastEvtBgo->fADC.find(gid[s][dy+1]) != _fLastEvtBgo->fADC.end()){
            if(_fLastEvtBgo->fADC.at(gid[s][dy+1]) > LastBigADC){ // last big event
              lastBigEvt = true;
            }
          }
          */
          if(comp){
          //if(comp && lastBigEvt){
            fEvtBgo->fADC.erase(gid[s][dy+1]);
            DmpLogWarning<<"Bgo pileup"<<__FILE__<<"("<<__LINE__<<")\tl="<<l<<"\tb="<<b<<"\ts="<<s<<"\td="<<dy<<"\t\tdelte = "<<comp<<std::endl;
          }
        }
      }
    }
  }
  //_fLastEvtBgo->LoadFrom(fEvtBgo);

  // PSD relation check
  short gidP[2][2]; //  side0,1;  5,8;
  double signP[2][2];// side0,1;  5,8;
  for(short l=0;l<DmpParameterPsd::kPlaneNo*2;++l){
    for(short b=0;b<DmpParameterPsd::kStripNo;++b){
      for(short s=0;s<DmpParameterPsd::kSideNo;++s){
        for(short dy=0;dy<2;++dy){ // load signal
          gidP[s][dy] = DmpPsdBase::ConstructGlobalDynodeID(l,b,s,3*dy+5);
          if(fEvtPsd->fADC.find(gidP[s][dy]) != fEvtPsd->fADC.end()){
            signP[s][dy] = fEvtPsd->fADC.at(gidP[s][dy]);
          }else{
            signP[s][dy] = 0;
          }
        }
        for(short dy=0;dy<2;++dy){ // compress overflow
          if(signP[s][dy] > 14000){
            fEvtPsd->fADC.erase(gidP[s][dy]);
            signP[s][dy] = 0;
          }
        }
          // use small check bigger
          if(signP[s][0] < 5 * fPsdPed[gidP[s][0]].at(5)) continue;    // must bigger than 5 *sigma
          if(signP[s][1] < 10* fPsdPed[gidP[s][1]].at(5)) continue;    // must bigger than 10 *sigma
          double deltaADC_small = signP[s][1]*fPsdRel[gidP[s][0]].at(5) + fPsdRel[gidP[s][0]].at(4)  - signP[s][0];
          bool comp = TMath::Abs(deltaADC_small) > 3*fTotSigmaPsd.at(gidP[s][0]); // affected by last event
          //bool lastBigEvt = false;
          /*
          if(_fLastEvtPsd->fADC.find(gidP[s][1]) != _fLastEvtPsd->fADC.end()){
            if(_fLastEvtPsd->fADC.at(gidP[s][1]) > LastBigADC){ // last big event
              lastBigEvt = true;
            }
          }
          */
          if(comp){
          //if(comp && lastBigEvt){
            fEvtPsd->fADC.erase(gidP[s][1]);
            DmpLogWarning<<"Psd pileup"<<__FILE__<<"("<<__LINE__<<")\tl="<<l<<"\tb="<<b<<"\ts="<<s<<"\t\tdelte = "<<comp<<std::endl;
          }
      }
    }
  }
  //_fLastEvtPsd->LoadFrom(fEvtPsd);
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgRec0_RelationCheck::Finalize(){
  //std::string histFileName = gRootIOSvc->GetOutputPath()+gRootIOSvc->GetInputStem()+"_LastAffectCheck.root";
  //TFile *histFile = new TFile(histFileName.c_str(),"RECREATE");
  //histFile->mkdir("Bgo");
  //histFile->cd("Bgo");
  //histFile->mkdir("Psd");
  //histFile->cd("Psd");
  //histFile->Save();
  //delete histFile;
  return true;
}


