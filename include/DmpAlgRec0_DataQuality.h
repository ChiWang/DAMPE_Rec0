/*
 *  $Id: DmpAlgRec0_DataQuality.h, 2015-04-24 13:24:09 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 19/07/2014
*/

#ifndef DmpAlgRec0_DataQuality_H
#define DmpAlgRec0_DataQuality_H

#include <map>
#include "DmpLoadParameters.h"
#include "DmpEvtPsdRaw.h"

#include "DmpVAlg.h"

class DmpAlgRec0_DataQuality : public DmpVAlg{
/*
 *  DmpAlgRec0_DataQuality
 *
 */
public:
  DmpAlgRec0_DataQuality();
  ~DmpAlgRec0_DataQuality();

  void SetPedestalFile(std::string detectorID,std::string filename);
  void SetRelationFile(std::string detectorID,std::string filename);
  bool Initialize();
  bool ProcessThisEvent();
  bool Finalize();

private:    // output
  DmpEvtBgoRaw          *fEvtBgo;
  DmpEvtPsdRaw          *fEvtPsd;

private:
  DmpEvtBgoRaw          *_fLastEvtBgo;
  DmpEvtPsdRaw          *_fLastEvtPsd;

private:
  DmpParameterHolder    fBgoPed; 
  DmpParameterHolder    fPsdPed; 

  DmpParameterHolder    fBgoRel; 
  DmpParameterHolder    fPsdRel; 
  std::map<short,double>  fTotSigmaBgo;
  std::map<short,double>  fTotSigmaPsd;
};

#endif

