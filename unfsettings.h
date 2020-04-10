#ifndef UFSTNGS
#define UFSTNGS

namespace uf {

  // Use output-MC-1 instead of MC-2b (or NLOxNP) in dagostini.C
  constexpr bool dagfile1 = false;

  // Do ECAL prefire correction for 2016 and 2017 data (eta 2.0-2.5 and 2.5-3.0)
  constexpr bool doECALprefire = true;
  
  
  //{ BEGIN limits for HistosNormalize, dagostini, drawSummary, drawRunHistos
  // Minimum and maximum pT range to be plotted and fitted
  // pT limits for unfolding
  constexpr double ptminreco = 60; // recopt; //40; recopt needs to be bigger than genpt
  constexpr double ptmingen = 40;
  constexpr double ptminnlo = 15;
  constexpr double fitmin = 15;

  constexpr double fitptmin = 15;
  
  // Changed on 2013-05-020: analysis from 49 GeV to 56 GeV
  constexpr double xmin57 = 114;//56;
  constexpr double xminpas = 114;//56;
  constexpr double xmin = 114;//24.;//20.;
  constexpr double xmax = 1497;//TEMP PATCH for partial data //1999.;
  //} END limits
  //} END Legacy settings that might not work anymore


}

#endif
