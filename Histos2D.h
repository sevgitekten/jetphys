// Purpose:  2D histograms that include inclusive jet spectra with different eta binnings 
// Author:   
// Created:  

#ifndef __Histos2D_h__
#define __Histos2D_h__

#include "TH2D.h"
#include "TDirectory.h"
#include "TMath.h"

#include <string>
#include <map>
#include <vector>
#include <iostream>

#include "settings.h"

using std::string;
using std::vector;

class Histos2D {
  public:
  // phase space
  string trigname;

  double pttrg;
  double ptmin;
  double ptmax;

  bool ismcdir;

  TH2D *hptsdef;
  TH2D *hptswid;
  TH2D *hptsnar;
 
  const vector<float> alpharange = {0.05,0.10,0.15,0.20,0.25,0.30};

  Histos2D() {}
  Histos2D(TDirectory *dir, string trigname, double pttrg, double ptmin, double ptmax, bool ismcdir = false);
  ~Histos2D();

  void Write();

 private:
  TDirectory *dir;
};

#endif // __Histos2D_h__
