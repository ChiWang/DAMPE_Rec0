/*
 *  $Id: DmpAlgRec0_RawSignal.cc, 2015-03-02 20:59:37 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 19/07/2014
*/

//#include <stdio.h>
#include <stdlib.h>     // getenv()
#include <boost/lexical_cast.hpp>
//#include <fstream>

#include "DmpEvtHeader.h"
#include "DmpEvtBgoRaw.h"
#include "DmpEvtNudRaw.h"
#include "DmpDataBuffer.h"
#include "DmpCore.h"
#include "DmpAlgRec0_RawSignal.h"

//-------------------------------------------------------------------
DmpAlgRec0_RawSignal::DmpAlgRec0_RawSignal()
 :DmpVAlg("Reco/SubstractPed"),
  fEvtHeader(0),
  fBgoRaw(0),
  fPsdRaw(0),
  fNudRaw(0),
  fStkRaw(0),
  fHeader(0),
  fEvtBgo(0),
  fEvtPsd(0),
  fEvtNud(0),
  fEvtStk(0)
{
  gRootIOSvc->SetOutputKey("RawSignal");
  std::string root_path = (std::string)getenv("DMPSWSYS")+"/share/Calibration";
  gRootIOSvc->JobOption()->SetOption(this->Name()+"/BgoPedestal",root_path+"/Bgo.ped");
  gRootIOSvc->JobOption()->SetOption(this->Name()+"/PsdPedestal",root_path+"/Psd.ped");
  gRootIOSvc->JobOption()->SetOption(this->Name()+"/StkPedestal",root_path+"/Stk.ped");
  gRootIOSvc->JobOption()->SetOption(this->Name()+"/NudPedestal",root_path+"/Nud.ped");
}

//-------------------------------------------------------------------
DmpAlgRec0_RawSignal::~DmpAlgRec0_RawSignal(){
}

void DmpAlgRec0_RawSignal::SetPedestalFile_Bgo(std::string f)
{
  gRootIOSvc->JobOption()->SetOption(this->Name()+"/BgoPedestal",f);
}

void DmpAlgRec0_RawSignal::SetPedestalFile_Psd(std::string f)
{
  gRootIOSvc->JobOption()->SetOption(this->Name()+"/PsdPedestal",f);
}

void DmpAlgRec0_RawSignal::SetPedestalFile_Stk(std::string f)
{
  gRootIOSvc->JobOption()->SetOption(this->Name()+"/StkPedestal",f);
}

void DmpAlgRec0_RawSignal::SetPedestalFile_Nud(std::string f)
{
  gRootIOSvc->JobOption()->SetOption(this->Name()+"/NudPedestal",f);
}

//-------------------------------------------------------------------
bool DmpAlgRec0_RawSignal::Initialize(){
  // read input data
  fEvtHeader = dynamic_cast<DmpEvtHeader*>(gDataBuffer->ReadObject("Event/Rdc/EventHeader"));
  if(0 == fEvtHeader){
    fEvtHeader = new DmpEvtHeader();
    gDataBuffer->LinkRootFile("Event/Rdc/EventHeader",fEvtHeader);
  }
  fBgoRaw = dynamic_cast<DmpEvtBgoRaw*>(gDataBuffer->ReadObject("Event/Rdc/Bgo"));
  if(0 == fBgoRaw){
    fBgoRaw = new DmpEvtBgoRaw();
    gDataBuffer->LinkRootFile("Event/Rdc/Bgo",fBgoRaw);
  }
  fPsdRaw = dynamic_cast<DmpEvtPsdRaw*>(gDataBuffer->ReadObject("Event/Rdc/Psd"));
  if(0 == fPsdRaw){
    fPsdRaw = new DmpEvtPsdRaw();
    gDataBuffer->LinkRootFile("Event/Rdc/Psd",fPsdRaw);
  }
  fNudRaw = dynamic_cast<DmpEvtNudRaw*>(gDataBuffer->ReadObject("Event/Rdc/Nud"));
  if(0 == fNudRaw){
    fNudRaw = new DmpEvtNudRaw();
    gDataBuffer->LinkRootFile("Event/Rdc/Nud",fNudRaw);
  }
  std::string path0 = "Event/Rec0";
  fHeader = new DmpEvtHeader();
  gDataBuffer->RegisterObject(path0+"/EventHeader",fHeader);
  fEvtBgo = new DmpEvtBgoRaw();
  gDataBuffer->RegisterObject(path0+"/Bgo",fEvtBgo);
  fEvtPsd = new DmpEvtPsdRaw();
  gDataBuffer->RegisterObject(path0+"/Psd",fEvtPsd);
  fEvtNud= new DmpEvtNudRaw();
  gDataBuffer->RegisterObject(path0+"/Nud",fEvtNud);
  std::string t0,t1;
  std::string inputFile = gRootIOSvc->JobOption()->GetValue(this->Name()+"/BgoPedestal");
  bool loadPed = DAMPE::Bgo::LoadPedestal(inputFile,fBgoPed,t0,t1);
  if(not loadPed){
          return false;
  }else{
    gRootIOSvc->JobOption()->SetOption(this->Name()+"/BgoPedestal/t0",t0);
    gRootIOSvc->JobOption()->SetOption(this->Name()+"/BgoPedestal/t1",t1);
  }
  inputFile = gRootIOSvc->JobOption()->GetValue(this->Name()+"/PsdPedestal");
  loadPed = DAMPE::Psd::LoadPedestal(inputFile,fPsdPed,t0,t1);
  if(not loadPed){
          return false;
  }else{
    gRootIOSvc->JobOption()->SetOption(this->Name()+"/PsdPedestal/t0",t0);
    gRootIOSvc->JobOption()->SetOption(this->Name()+"/PsdPedestal/t1",t1);
  }
  inputFile = gRootIOSvc->JobOption()->GetValue(this->Name()+"/StkPedestal");
  loadPed = DAMPE::Stk::LoadPedestal(inputFile,fStkPed,t0,t1);
  if(not loadPed){
          return false;
  }else{
    gRootIOSvc->JobOption()->SetOption(this->Name()+"/StkPedestal/t0",t0);
    gRootIOSvc->JobOption()->SetOption(this->Name()+"/StkPedestal/t1",t1);
  }
  inputFile = gRootIOSvc->JobOption()->GetValue(this->Name()+"/NudPedestal");
  loadPed = DAMPE::Nud::LoadPedestal(inputFile,fNudPed,t0,t1);
  if(not loadPed){
          return false;
  }else{
    gRootIOSvc->JobOption()->SetOption(this->Name()+"/NudPedestal/t0",t0);
    gRootIOSvc->JobOption()->SetOption(this->Name()+"/NudPedestal/t1",t1);
  }
  return true;
}

bool DmpAlgRec0_RawSignal::SubPed_Stk()
{
return true;
}

//-------------------------------------------------------------------
bool DmpAlgRec0_RawSignal::ProcessThisEvent(){
  fHeader->LoadFrom(fEvtHeader);
  fEvtBgo->LoadFrom(fBgoRaw);
  for(std::vector<double>::iterator it = fEvtBgo->fADC.begin(); it != fEvtBgo->fADC.end();){
    int index = it - fEvtBgo->fADC.begin();
    short gid = fEvtBgo->fGlobalDynodeID.at(index);
    fEvtBgo->fADC[index] -= fBgoPed.at(gid).at(0);
    if(fEvtBgo->fADC[index] < 3*fBgoPed.at(gid).at(1)){
      it = fEvtBgo->fADC.erase(it);
      fEvtBgo->fGlobalDynodeID.erase(fEvtBgo->fGlobalDynodeID.begin() + index);
    }else{
      ++it;
    }
  }
  fEvtPsd->LoadFrom(fPsdRaw);
  for(std::vector<double>::iterator it = fEvtPsd->fADC.begin(); it != fEvtPsd->fADC.end();){
    int index = it - fEvtPsd->fADC.begin();
    short gid = fEvtPsd->fGlobalDynodeID.at(index);
    fEvtPsd->fADC[index] -= fPsdPed.at(gid).at(0);
    if(fEvtPsd->fADC[index] < 3*fPsdPed.at(gid).at(1)){
      it = fEvtPsd->fADC.erase(it);
      fEvtPsd->fGlobalDynodeID.erase(fEvtPsd->fGlobalDynodeID.begin() + index);
    }else{
      ++it;
    }
  }
  fEvtNud->LoadFrom(fNudRaw);
  for(std::vector<double>::iterator it = fEvtNud->fADC.begin(); it != fEvtNud->fADC.end();){
    int index = it - fEvtNud->fADC.begin();
    short gid = fEvtNud->fChannelID.at(index);
    fEvtNud->fADC[index] -= fNudPed.at(gid).at(0);
    if(fEvtNud->fADC[index] < 3*fNudPed.at(gid).at(1)){
      it = fEvtNud->fADC.erase(it);
      fEvtNud->fChannelID.erase(fEvtNud->fChannelID.begin() + index);
    }else{
      ++it;
    }
  }
  //fEvtStk->LoadFrom(fStkRaw);
  bool procesSTK = SubPed_Stk();
  return true && procesSTK;
}

//-------------------------------------------------------------------
bool DmpAlgRec0_RawSignal::Finalize(){
  return true;
}


