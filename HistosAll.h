// Purpose:  Histograms that include all trigs (weighting or separation implicit)
// Author:   hannu.siikonen@cern.ch
// Created:  November 19, 2018

#ifndef __HistosAll_h__
#define __HistosAll_h__

#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TProfile3D.h"
#include "TDirectory.h"
#include "TMath.h"
#include "TMatrixD.h"

#include <string>
#include <map>
#include <vector>
#include <iostream>

#include "settings.h"

using std::string;
using std::vector;

class HistosAll {
public:

  // Development histos for the "tag & probes" 2D method
  // These are created using the jp::wwptrange bins
  //TProfile * pmetave;
  //TProfile * pmetave_nol2l3;
  //TProfile * ppttagave;
  //TProfile * ppttagave_nol2l3;
  //vector<TProfile2D *> p2ptjet1;
  //vector<TProfile2D *> p2ptjet2;
  //vector<TProfile2D *> p2ptjet3;
  //vector<TProfile2D *> p2ptjet4plus;
  //vector<TProfile2D *> p2ptjet1_nol2l3;
  //vector<TProfile2D *> p2ptjet2_nol2l3;
  //vector<TProfile2D *> p2ptjet3_nol2l3;
  //vector<TProfile2D *> p2ptjet4plus_nol2l3;
  //vector<TH2D *> h2ptjet1;
  //vector<TH2D *> h2ptjet2;
  //vector<TH2D *> h2ptjet3;
  //vector<TH2D *> h2ptjet4plus;
  //vector<TH2D *> h2ptjet1_nol2l3;
  //vector<TH2D *> h2ptjet2_nol2l3;
  //vector<TH2D *> h2ptjet3_nol2l3;
  //vector<TH2D *> h2ptjet4plus_nol2l3;
  //vector<TH2D *> h2njet1;
  //vector<TH2D *> h2njet2;
  //vector<TH2D *> h2njet3;
  //vector<TH2D *> h2njet4plus;
  //vector<TH2D *> h2njet1_nol2l3;
  //vector<TH2D *> h2njet2_nol2l3;
  //vector<TH2D *> h2njet3_nol2l3;
  //vector<TH2D *> h2njet4plus_nol2l3;
  TProfile2D *p2mpf;
  TProfile2D *p2mpf2;
  TProfile2D *p2htf;
  TProfile2D *p2htf2;
  TProfile2D *p2mpf_fe;
  TProfile2D *p2mpf2_fe;
  TProfile2D *p2htf_fe;
  TProfile2D *p2htf2_fe;
  TH3D *h3mpf;
  TH3D *h3htf;

  vector<double>            mPts;
  vector<double>            mEtas;
  vector<double>            mFullEtas;
  unsigned                  mNPts;
  unsigned                  mNEtas;
  unsigned                  mNFullEtas;
  TMatrixD *mSquare;
  TMatrixD *mColumn;
  TMatrixD *mSingle;
  TMatrixD *mTSquare;
  TMatrixD *mTColumn;

  HistosAll() {}
  HistosAll(TDirectory *dir);
  ~HistosAll();

  void Write();
  int PhaseBin(double pt, double eta);

private:
  TDirectory *dir;
};

#endif // __HistosAll_h__
