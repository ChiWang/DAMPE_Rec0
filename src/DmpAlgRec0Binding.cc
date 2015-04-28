/*
 *  $Id: DmpAlgRec0Binding.cc, 2015-04-24 10:18:06 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 03/09/2014
*/

#include <boost/python.hpp>
#include "DmpAlgRec0_Signal.h"
#include "DmpAlgRec0_DataQuality.h"

BOOST_PYTHON_MODULE(libDmpRec0){
  using namespace boost::python;

  class_<DmpAlgRec0_Signal,boost::noncopyable,bases<DmpVAlg> >("DmpAlgRec0_Signal",init<>())
     .def("SetPedestalFile",    &DmpAlgRec0_Signal::SetPedestalFile)
     ;
  class_<DmpAlgRec0_DataQuality,boost::noncopyable,bases<DmpVAlg> >("DmpAlgRec0_DataQuality",init<>())
     .def("SetPedestalFile",    &DmpAlgRec0_DataQuality::SetPedestalFile)
     .def("SetRelationFile",    &DmpAlgRec0_DataQuality::SetRelationFile)
     ;

}

