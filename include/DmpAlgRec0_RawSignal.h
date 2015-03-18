/*
 *  $Id: DmpAlgRec0_RawSignal.h, 2015-03-02 20:58:00 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 19/07/2014
*/

#ifndef DmpAlgRec0_RawSignal_H
#define DmpAlgRec0_RawSignal_H

#include "DmpLoadParameters.h"
#include "DmpEvtPsdRaw.h"

#include "DmpVAlg.h"
class DmpEvtStkRaw;
class DmpEvtNudRaw;

class DmpAlgRec0_RawSignal : public DmpVAlg{
/*
 *  DmpAlgRec0_RawSignal
 *
 */
public:
  DmpAlgRec0_RawSignal();
  ~DmpAlgRec0_RawSignal();

  void SetPedestalFile(std::string detectorID,std::string filename);
  bool Initialize();
  bool ProcessThisEvent();
  bool Finalize();

private:
  bool SubPed_Stk();

private:    // output
  DmpEvtBgoRaw          *fEvtBgo;
  DmpEvtPsdRaw          *fEvtPsd;
  DmpEvtNudRaw          *fEvtNud;
  DmpEvtStkRaw          *fEvtStk;

private:
  DmpParameterHolder    fBgoPed; 
  DmpParameterHolder    fPsdPed; 
  DmpParameterHolder    fStkPed; 
  DmpParameterHolder    fNudPed; 
};

#endif

