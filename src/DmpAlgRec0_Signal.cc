/*
 *  $Id: DmpAlgRec0_Signal.cc, 2015-04-28 13:27:23 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 19/07/2014
*/

#include <stdlib.h>     // getenv()
#include <boost/lexical_cast.hpp>
#include "TMath.h"

#include "DmpEvtBgoRaw.h"
#include "DmpEvtNudRaw.h"
#include "DmpDataBuffer.h"
#include "DmpCore.h"
#include "DmpAlgRec0_Signal.h"
#define Overflow  14000

//-------------------------------------------------------------------
DmpAlgRec0_Signal::DmpAlgRec0_Signal()
 :DmpVAlg("DmpAlgRec0_Signal"),
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
DmpAlgRec0_Signal::~DmpAlgRec0_Signal(){
}

void DmpAlgRec0_Signal::SetPedestalFile(std::string ID,std::string f)
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
  }else if(Id == "Nud"){
    gCore->GetJobOption()->SetOption(this->Name()+"/NudPedestal",f);
  }else if(Id == "Stk"){
    gCore->GetJobOption()->SetOption(this->Name()+"/StkPedestal",f);
  }
}

//-------------------------------------------------------------------
bool DmpAlgRec0_Signal::Initialize(){
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
  bool loadPed = DAMPE::LoadParameters(inputFile,fBgoPed,steering);
  if(not loadPed){
          return false;
  }else{
    gCore->GetJobOption()->SetOption(this->Name()+"/BgoPedestal/t0",steering["StartTime"]);
    gCore->GetJobOption()->SetOption(this->Name()+"/BgoPedestal/t1",steering["StopTime"]);
  }
  
  inputFile = gCore->GetJobOption()->GetValue(this->Name()+"/PsdPedestal");
  loadPed = DAMPE::LoadParameters(inputFile,fPsdPed,steering);
  if(not loadPed){
          return false;
  }else{
    gCore->GetJobOption()->SetOption(this->Name()+"/PsdPedestal/t0",steering["StartTime"]);
    gCore->GetJobOption()->SetOption(this->Name()+"/PsdPedestal/t1",steering["StopTime"]);
  }

  inputFile = gCore->GetJobOption()->GetValue(this->Name()+"/StkPedestal");
  loadPed = DAMPE::LoadParameters(inputFile,fStkPed,steering);
  if(not loadPed){
          return false;
  }else{
    gCore->GetJobOption()->SetOption(this->Name()+"/StkPedestal/t0",steering["StartTime"]);
    gCore->GetJobOption()->SetOption(this->Name()+"/StkPedestal/t1",steering["StopTime"]);
  }

  inputFile = gCore->GetJobOption()->GetValue(this->Name()+"/NudPedestal");
  loadPed = DAMPE::LoadParameters(inputFile,fNudPed,steering);
  if(not loadPed){
          return false;
  }else{
    gCore->GetJobOption()->SetOption(this->Name()+"/NudPedestal/t0",steering["StartTime"]);
    gCore->GetJobOption()->SetOption(this->Name()+"/NudPedestal/t1",steering["StopTime"]);
  }

  return true;
}

bool DmpAlgRec0_Signal::Signal_Stk()
{
return true;
}

//-------------------------------------------------------------------
bool DmpAlgRec0_Signal::ProcessThisEvent()
{
  std::vector<short>   eraseIDs;
  // for BGO
  for(std::map<short,double>::iterator it=fEvtBgo->fADC.begin();it != fEvtBgo->fADC.end();++it){
    it->second -= fBgoPed.at(it->first).at(4);
    if(it->second < 3*fBgoPed.at(it->first).at(5) || it->second > Overflow){
      eraseIDs.push_back(it->first);
    }
  }
  for(short i=0;i<eraseIDs.size();++i){
    fEvtBgo->fADC.erase(eraseIDs.at(i));
  }

  // PSD
  eraseIDs.clear();
  for(std::map<short,double>::iterator it=fEvtPsd->fADC.begin();it != fEvtPsd->fADC.end();++it){
    it->second -= fPsdPed.at(it->first).at(4);
    if(it->second < 3*fPsdPed.at(it->first).at(5) || it->second > Overflow){
      eraseIDs.push_back(it->first);
    }
  }
  for(short i=0;i<eraseIDs.size();++i){
    fEvtPsd->fADC.erase(eraseIDs.at(i));
  }

  // NUD
  for(short i=0;i<4;++i){
    fEvtNud->fADC[i] -= fNudPed.at(i).at(0);
    if(fEvtNud->fADC[i] < 3*fNudPed.at(i).at(1)){
      fEvtNud->fADC[i] = 0;
    }
  }
  // STK
  bool procesSTK = Signal_Stk();

  // check current 8 affected by last event... if affected, delete it

  return true && procesSTK;
}

//-------------------------------------------------------------------
bool DmpAlgRec0_Signal::Finalize(){
  return true;
}


