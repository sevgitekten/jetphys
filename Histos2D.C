// Purpose:  
// Author:   
// Created: 

#include "Histos2D.h"

const double etas[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.2, 4.7};
const int netas = sizeof(etas)/sizeof(etas[0])-1;

const double etaswid[] = {0.0, 0.8, 1.3, 1.9, 2.5, 3.0, 3.2, 5.2};
const int netaswid = sizeof(etaswid)/sizeof(etaswid[0])-1;

const double etasnar[] = {0.000,  0.261,  0.522,  0.783,  1.044,  1.305,  1.479,  1.653,  1.930, 2.172,  2.322,  2.500,  2.650,  2.853,  2.964,  3.139,  3.489,  3.839, 5.191};
const int netasnar = sizeof(etasnar)/sizeof(etasnar[0])-1;

Histos2D::Histos2D(TDirectory *dir, string trigname, double pttrg, double ptmin, double ptmax, bool ismcdir) {

  TDirectory *curdir = gDirectory;
  bool enter = dir->cd();
  assert(enter);
  this->dir = dir;
  this->ismcdir = ismcdir;

  // phase space
  this->trigname = trigname;

  this->pttrg = pttrg;
  this->ptmin = ptmin;
  this->ptmax = ptmax;

  // Once and for all (even if few too many with Sumw2)
  TH1::SetDefaultSumw2(kTRUE);

  vector<double> x;
  for (unsigned int i = 0; i < jp::nopts_eta and jp::ptrangevseta[0][i]!=0; ++i) {
    if (jp::ptrangevseta[0][i]<0.001) break; // There are zeros in the end of the array when we're out of barrel
    x.push_back(jp::ptrangevseta[0][i]);
  }
  const int nx = x.size()-1;

  hptsdef = new TH2D("hptsdef","", nx, &x[0], netas,etas);    // ptrange: default 0-0.5, "default" eta range for cross-checking
  hptswid = new TH2D("hptswid","", nx, &x[0], netaswid,etaswid);    // ptrange: default 0-0.5, wider eta bins
  hptsnar = new TH2D("hptsnar","", nx, &x[0], netasnar,etasnar);    // ptrange: default 0-0.5, narrower eta bins
  
  curdir->cd();
}

void Histos2D::Write() {
  dir->cd();
  dir->Write();
}

Histos2D::~Histos2D() {
  Write();
};
