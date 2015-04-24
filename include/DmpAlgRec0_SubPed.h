/*
 *  $Id: DmpAlgRec0_SubPed.h, 2015-04-24 10:17:44 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 19/07/2014
*/

#ifndef DmpAlgRec0_SubPed_H
#define DmpAlgRec0_SubPed_H

#include <map>
#include "DmpLoadParameters.h"
#include "DmpEvtPsdRaw.h"

#include "DmpVAlg.h"
class DmpEvtStkRaw;
class DmpEvtNudRaw;

class DmpAlgRec0_SubPed : public DmpVAlg{
/*
 *  DmpAlgRec0_SubPed
 *
 */
public:
  DmpAlgRec0_SubPed();
  ~DmpAlgRec0_SubPed();

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

