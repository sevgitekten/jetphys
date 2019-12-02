// Purpose: Estimation of inclusive jet cross section systematics
// Author:  mikko.voutilainen@cern.ch
// Created: March 22, 2010
// Updated: December 2, 2019

 #include "CondFormats/JetMETObjects/src/Utilities.cc"

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/SimpleJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
// For JEC uncertainty
#include "CondFormats/JetMETObjects/interface/SimpleJetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

#include "tools.h"

R__LOAD_LIBRARY(CondFormats/JetMETObjects/src/JetCorrectorParameters.cc+)
R__LOAD_LIBRARY(CondFormats/JetMETObjects/src/SimpleJetCorrector.cc+)
R__LOAD_LIBRARY(CondFormats/JetMETObjects/src/FactorizedJetCorrector.cc+)
R__LOAD_LIBRARY(CondFormats/JetMETObjects/src/SimpleJetCorrectionUncertainty.cc+)
R__LOAD_LIBRARY(CondFormats/JetMETObjects/src/JetCorrectionUncertainty.cc+)

R__LOAD_LIBRARY(tools.C+) 

R__LOAD_LIBRARY(systematics.C+)

#include "settings.h"

void mk_systematics() {

   systematics(jp::type);
  //systematics(type, true); // 38X JEC; always run after 36X
  //jec_systematics();
  //jer_systematics();
  //jid_systematics();
  //lum_systematics();

  // sources(jp::type);
}
