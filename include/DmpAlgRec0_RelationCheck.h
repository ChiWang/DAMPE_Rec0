/*
 *  $Id: DmpAlgRec0_RelationCheck.h, 2015-04-24 13:24:09 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 19/07/2014
*/

#ifndef DmpAlgRec0_RelationCheck_H
#define DmpAlgRec0_RelationCheck_H

#include <map>
#include "DmpLoadParameters.h"
#include "DmpEvtPsdRaw.h"

#include "DmpVAlg.h"

class DmpAlgRec0_RelationCheck : public DmpVAlg{
/*
 *  DmpAlgRec0_RelationCheck
 *
 */
public:
  DmpAlgRec0_RelationCheck();
  ~DmpAlgRec0_RelationCheck();

  void SetPedestalFile(std::string detectorID,std::string filename);
  void SetRelationFile(std::string detectorID,std::string filename);
  bool Initialize();
  bool ProcessThisEvent();
  bool Finalize();

private:    // output
  DmpEvtBgoRaw          *fEvtBgo;
  DmpEvtPsdRaw          *fEvtPsd;

private:
  //DmpEvtBgoRaw          *_fLastEvtBgo;
  //DmpEvtPsdRaw          *_fLastEvtPsd;

private:
  DmpParameterHolder    fBgoPed; 
  DmpParameterHolder    fPsdPed; 

  DmpParameterHolder    fBgoRel; 
  DmpParameterHolder    fPsdRel; 
  std::map<short,double>  fTotSigmaBgo;
  std::map<short,double>  fTotSigmaPsd;
};

#endif

