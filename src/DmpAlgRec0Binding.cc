/*
 *  $Id: DmpAlgRec0Binding.cc, 2015-04-24 10:18:06 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 03/09/2014
*/

#include <boost/python.hpp>
#include "DmpAlgRec0_SubPed.h"
#include "DmpAlgRec0_RelationCheck.h"
#include "DmpAlgRec0_DataQuality.h"

BOOST_PYTHON_MODULE(libDmpRec0){
  using namespace boost::python;

  class_<DmpAlgRec0_SubPed,boost::noncopyable,bases<DmpVAlg> >("DmpAlgRec0_SubPed",init<>())
     .def("SetPedestalFile",    &DmpAlgRec0_SubPed::SetPedestalFile)
     ;
  class_<DmpAlgRec0_RelationCheck,boost::noncopyable,bases<DmpVAlg> >("DmpAlgRec0_RelationCheck",init<>())
     .def("SetPedestalFile",    &DmpAlgRec0_RelationCheck::SetPedestalFile)
     .def("SetRelationFile",    &DmpAlgRec0_RelationCheck::SetRelationFile)
     ;
  class_<DmpAlgRec0_DataQuality,boost::noncopyable,bases<DmpVAlg> >("DmpAlgRec0_DataQuality",init<>())
     .def("SetPedestalFile",    &DmpAlgRec0_DataQuality::SetPedestalFile)
     .def("SetRelationFile",    &DmpAlgRec0_DataQuality::SetRelationFile)
     ;

}

