/*
 *  $Id: DmpAlgRec0_RawSignal.cc, 2015-03-02 20:59:37 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 19/07/2014
*/

//#include <stdio.h>
#include <stdlib.h>     // getenv()
#include <boost/lexical_cast.hpp>
//#include <fstream>

#include "DmpEvtBgoRaw.h"
#include "DmpEvtNudRaw.h"
#include "DmpDataBuffer.h"
#include "DmpCore.h"
#include "DmpAlgRec0_RawSignal.h"

//-------------------------------------------------------------------
DmpAlgRec0_RawSignal::DmpAlgRec0_RawSignal()
 :DmpVAlg("DmpAlgRec0_RawSignal"),
  fEvtBgo(0),
  fEvtPsd(0),
  fEvtNud(0),
  fEvtStk(0)
{
  gRootIOSvc->SetOutFileKey("Rec0");
  std::string root_path = (std::string)getenv("DMPSWSYS")+"/share/Calibration";
  gCore->GetJobOption()->SetOption(this->Name()+"/BgoPedestal",root_path+"/Bgo.ped");
  gCore->GetJobOption()->SetOption(this->Name()+"/PsdPedestal",root_path+"/Psd.ped");
  gCore->GetJobOption()->SetOption(this->Name()+"/StkPedestal",root_path+"/Stk.ped");
  gCore->GetJobOption()->SetOption(this->Name()+"/NudPedestal",root_path+"/Nud.ped");
}

//-------------------------------------------------------------------
DmpAlgRec0_RawSignal::~DmpAlgRec0_RawSignal(){
}

void DmpAlgRec0_RawSignal::SetPedestalFile(std::string Id,std::string f)
{
        TString xx = f;
        if(not xx.Contains(".ped")){
                DmpLogWarning<<f<<"("<<Id<<") is not a pedestal file... will use the defaur one"<<DmpLogEndl;
                return;
        }
  if(Id == "Bgo" || Id == "BGO"){
    gCore->GetJobOption()->SetOption(this->Name()+"/BgoPedestal",f);
  }else if(Id == "Psd" || Id == "PSD"){
    gCore->GetJobOption()->SetOption(this->Name()+"/PsdPedestal",f);
  }else if(Id == "Nud" || Id == "Nud"){
    gCore->GetJobOption()->SetOption(this->Name()+"/NudPedestal",f);
  }else if(Id == "Stk" || Id == "Stk"){
    gCore->GetJobOption()->SetOption(this->Name()+"/StkPedestal",f);
  }
}

//-------------------------------------------------------------------
bool DmpAlgRec0_RawSignal::Initialize(){
  // read input data
  std::string path_i = "Event/Rdc";
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
  fEvtNud = dynamic_cast<DmpEvtNudRaw*>(gDataBuffer->ReadObject(path_i+"/Nud"));
  if(0 == fEvtNud){
    fEvtNud = new DmpEvtNudRaw();
    gDataBuffer->LinkRootFile(path_i+"/Nud",fEvtNud);
  }
  std::string path_o = "Event/Rec0";
  gDataBuffer->RegisterObject(path_o+"/Bgo",fEvtBgo);
  gDataBuffer->RegisterObject(path_o+"/Psd",fEvtPsd);
  gDataBuffer->RegisterObject(path_o+"/Nud",fEvtNud);
  DmpParameterSteering steering;
  std::string inputFile = gCore->GetJobOption()->GetValue(this->Name()+"/BgoPedestal");
  bool loadPed = DAMPE::Bgo::LoadPedestal(inputFile,fBgoPed,steering);
  if(not loadPed){
          return false;
  }else{
    gCore->GetJobOption()->SetOption(this->Name()+"/BgoPedestal/t0",steering["StartTime"]);
    gCore->GetJobOption()->SetOption(this->Name()+"/BgoPedestal/t1",steering["StopTime"]);
  }
  inputFile = gCore->GetJobOption()->GetValue(this->Name()+"/PsdPedestal");
  steering.clear();
  loadPed = DAMPE::Psd::LoadPedestal(inputFile,fPsdPed,steering);
  if(not loadPed){
          return false;
  }else{
    gCore->GetJobOption()->SetOption(this->Name()+"/PsdPedestal/t0",steering["StartTime"]);
    gCore->GetJobOption()->SetOption(this->Name()+"/PsdPedestal/t1",steering["StopTime"]);
  }
  inputFile = gCore->GetJobOption()->GetValue(this->Name()+"/StkPedestal");
  steering.clear();
  loadPed = DAMPE::Stk::LoadPedestal(inputFile,fStkPed,steering);
  if(not loadPed){
          return false;
  }else{
    gCore->GetJobOption()->SetOption(this->Name()+"/StkPedestal/t0",steering["StartTime"]);
    gCore->GetJobOption()->SetOption(this->Name()+"/StkPedestal/t1",steering["StopTime"]);
  }
  inputFile = gCore->GetJobOption()->GetValue(this->Name()+"/NudPedestal");
  steering.clear();
  loadPed = DAMPE::Nud::LoadPedestal(inputFile,fNudPed,steering);
  if(not loadPed){
          return false;
  }else{
    gCore->GetJobOption()->SetOption(this->Name()+"/NudPedestal/t0",steering["StartTime"]);
    gCore->GetJobOption()->SetOption(this->Name()+"/NudPedestal/t1",steering["StopTime"]);
  }
  return true;
}

bool DmpAlgRec0_RawSignal::SubPed_Stk()
{
return true;
}

//-------------------------------------------------------------------
bool DmpAlgRec0_RawSignal::ProcessThisEvent(){
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
  bool procesSTK = SubPed_Stk();
  return true && procesSTK;
}

//-------------------------------------------------------------------
bool DmpAlgRec0_RawSignal::Finalize(){
  return true;
}


