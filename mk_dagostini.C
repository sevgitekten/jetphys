// Purpose: d'Agostini ("Bayesian" or Richardson-Lucy) unfolding, including
//          response matrix generation from NLO theory and parameterized JER
// Author:  mikko.voutilainen@cern.ch
// Created: September 2, 2012
// Updated: June 4, 2015


  #include "tools.h"
   R__LOAD_LIBRARY(tools.C+)

  // Retrieve RooUnfold package from
  //   http://hepunx.rl.ac.uk/~adye/software/unfold/RooUnfold.html
  //   svn co https://svnsrv.desy.de/public/unfolding/RooUnfold/trunk RooUnfold
  //   cd RooUnfold; make; cd ..

  #include "RooUnfold/src/RooUnfold.h"
  #include "RooUnfold/src/RooUnfoldResponse.h"


   // For official JERs. Need moriond2019-branch of jecsys
   #include "CondFormats/JetMETObjects/src/JetResolutionObject.cc+"
   #include "JetMETCorrections/Modules/src/JetResolution.cc+"

   
  R__LOAD_LIBRARY(RooUnfold/libRooUnfold.so)
  R__LOAD_LIBRARY(dagostini.C+)
   //  R__LOAD_LIBRARY(drawdagostini.C+)
  R__LOAD_LIBRARY(drawdagostiniWSversion.C+)
   
  #include "settings.h"

 void mk_dagostini() {
     dagostiniUnfold(jp::type);


     drawDagostini(jp::type);
}
