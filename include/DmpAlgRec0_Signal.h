/*
 *  $Id: DmpAlgRec0_Signal.h, 2015-04-28 13:27:26 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 19/07/2014
*/

#ifndef DmpAlgRec0_Signal_H
#define DmpAlgRec0_Signal_H

#include <map>
#include "DmpLoadParameters.h"
#include "DmpEvtPsdRaw.h"

#include "DmpVAlg.h"
class DmpEvtStkRaw;
class DmpEvtNudRaw;

class DmpAlgRec0_Signal : public DmpVAlg{
/*
 *  DmpAlgRec0_Signal
 *
 */
public:
  DmpAlgRec0_Signal();
  ~DmpAlgRec0_Signal();

  void SetPedestalFile(std::string detectorID,std::string filename);
  bool Initialize();
  bool ProcessThisEvent();
  bool Finalize();

private:
  bool Signal_Stk();

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

