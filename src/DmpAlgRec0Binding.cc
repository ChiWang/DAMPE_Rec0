/*
 *  $Id: DmpAlgSubstractBgoPedBinding.cc, 2015-03-01 20:22:00 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 03/09/2014
*/

#include <boost/python.hpp>
#include "DmpAlgRec0_RawSignal.h"

BOOST_PYTHON_MODULE(libDmpRec0_RawSignal){
  using namespace boost::python;

  class_<DmpAlgRec0_RawSignal,boost::noncopyable,bases<DmpVAlg> >("DmpAlgRec0_RawSignal",init<>())
     .def("SetPedestalFile",    &DmpAlgRec0_RawSignal::SetPedestalFile)
     ;
}

