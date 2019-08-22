// Purpose:  Create histos for all trigs
// Author:   hannu.siikonen@cern.ch
// Created:  November 19, 2018

#include "HistosAll.h"

HistosAll::HistosAll(TDirectory *dir) {
  TDirectory *curdir = gDirectory;
  bool enter = dir->cd();
  assert(enter);
  this->dir = dir;

  // Once and for all (even if few too many with Sumw2)
  TH1::SetDefaultSumw2(kTRUE);

  //pmetave        = new TProfile("pmetave",       ";p_T^{tag};MET^{proj}_{ave};",jp::nwwpts,jp::wwptrange);
  //pmetave_nol2l3 = new TProfile("pmetave_nol2l3",";p_T^{tag};MET^{proj}_{ave};",jp::nwwpts,jp::wwptrange);
  //ppttagave        = new TProfile("ppttagave",       ";p_T^{tag};p^{proj}_{T,ave};",jp::nwwpts,jp::wwptrange);
  //ppttagave_nol2l3 = new TProfile("ppttagave_nol2l3",";p_T^{tag};p^{proj}_{T,ave};",jp::nwwpts,jp::wwptrange);
  //for (unsigned idx = 0; idx < jp::nwwpts; ++idx) {
  //  int num = jp::wwptrange[idx];
  //  string number = std::to_string(num);
  //  p2ptjet1.push_back           ( new TProfile2D((string("p2ptj1_")        +number+string("GeV")).c_str(),";p_T;#eta;",jp::npts_semi,jp::ptrange_semi,jp::netas,jp::etarange) );
  //  p2ptjet2.push_back           ( new TProfile2D((string("p2ptj2_")        +number+string("GeV")).c_str(),";p_T;#eta;",jp::npts_semi,jp::ptrange_semi,jp::netas,jp::etarange) );
  //  p2ptjet3.push_back           ( new TProfile2D((string("p2ptj3_")        +number+string("GeV")).c_str(),";p_T;#eta;",jp::npts_semi,jp::ptrange_semi,jp::netas,jp::etarange) );
  //  p2ptjet4plus.push_back       ( new TProfile2D((string("p2ptj4p_")       +number+string("GeV")).c_str(),";p_T;#eta;",jp::npts_semi,jp::ptrange_semi,jp::netas,jp::etarange) );
  //  p2ptjet1_nol2l3.push_back    ( new TProfile2D((string("p2ptj1_nol2l3_") +number+string("GeV")).c_str(),";p_T;#eta;",jp::npts_semi,jp::ptrange_semi,jp::netas,jp::etarange) );
  //  p2ptjet2_nol2l3.push_back    ( new TProfile2D((string("p2ptj2_nol2l3_") +number+string("GeV")).c_str(),";p_T;#eta;",jp::npts_semi,jp::ptrange_semi,jp::netas,jp::etarange) );
  //  p2ptjet3_nol2l3.push_back    ( new TProfile2D((string("p2ptj3_nol2l3_") +number+string("GeV")).c_str(),";p_T;#eta;",jp::npts_semi,jp::ptrange_semi,jp::netas,jp::etarange) );
  //  p2ptjet4plus_nol2l3.push_back( new TProfile2D((string("p2ptj4p_nol2l3_")+number+string("GeV")).c_str(),";p_T;#eta;",jp::npts_semi,jp::ptrange_semi,jp::netas,jp::etarange) );
  //  h2njet1.push_back           ( new TH2D((string("h2nj1_")        +number+string("GeV")).c_str(),";p_T;#eta;",jp::npts_semi,jp::ptrange_semi,jp::netas,jp::etarange) );
  //  h2njet2.push_back           ( new TH2D((string("h2nj2_")        +number+string("GeV")).c_str(),";p_T;#eta;",jp::npts_semi,jp::ptrange_semi,jp::netas,jp::etarange) );
  //  h2njet3.push_back           ( new TH2D((string("h2nj3_")        +number+string("GeV")).c_str(),";p_T;#eta;",jp::npts_semi,jp::ptrange_semi,jp::netas,jp::etarange) );
  //  h2njet4plus.push_back       ( new TH2D((string("h2nj4p_")       +number+string("GeV")).c_str(),";p_T;#eta;",jp::npts_semi,jp::ptrange_semi,jp::netas,jp::etarange) );
  //  h2njet1_nol2l3.push_back    ( new TH2D((string("h2nj1_nol2l3_") +number+string("GeV")).c_str(),";p_T;#eta;",jp::npts_semi,jp::ptrange_semi,jp::netas,jp::etarange) );
  //  h2njet2_nol2l3.push_back    ( new TH2D((string("h2nj2_nol2l3_") +number+string("GeV")).c_str(),";p_T;#eta;",jp::npts_semi,jp::ptrange_semi,jp::netas,jp::etarange) );
  //  h2njet3_nol2l3.push_back    ( new TH2D((string("h2nj3_nol2l3_") +number+string("GeV")).c_str(),";p_T;#eta;",jp::npts_semi,jp::ptrange_semi,jp::netas,jp::etarange) );
  //  h2njet4plus_nol2l3.push_back( new TH2D((string("h2nj4p_nol2l3_")+number+string("GeV")).c_str(),";p_T;#eta;",jp::npts_semi,jp::ptrange_semi,jp::netas,jp::etarange) );
  //}
  //h2ptjet1            = vector<TH2D*>(jp::nwwpts,0);
  //h2ptjet2            = vector<TH2D*>(jp::nwwpts,0);
  //h2ptjet3            = vector<TH2D*>(jp::nwwpts,0);
  //h2ptjet4plus        = vector<TH2D*>(jp::nwwpts,0);
  //h2ptjet1_nol2l3     = vector<TH2D*>(jp::nwwpts,0);
  //h2ptjet2_nol2l3     = vector<TH2D*>(jp::nwwpts,0);
  //h2ptjet3_nol2l3     = vector<TH2D*>(jp::nwwpts,0);
  //h2ptjet4plus_nol2l3 = vector<TH2D*>(jp::nwwpts,0);

  //mPts = {15, 21, 28, 37, 49, 64, 84, 114, 153, 196, 272, 330, 395, 468, 548, 686, 846, 1032, 1248, 1588, 2500, 100000};
  //mEtas = {0.0,0.5,1.0,1.5,2.0,2.5,3.5,5.0};
  mPts = {30, 45, 65, 85, 105, 130, 175, 230, 300, 400, 500, 600, 700, 800, 900, 1000, 1200, 1600, 2500};
  mEtas = {0,0.522,0.783,1.131,1.74,1.93,2.043,2.322,2.5,2.65,2.853,2.964,3.139,5.191};
  for (unsigned i = mEtas.size()-1; i>0; --i) mFullEtas.push_back(-mEtas[i]);
  for (unsigned i = 0; i<mEtas.size(); ++i) mFullEtas.push_back(mEtas[i]);
  mNPts = mPts.size()-1;
  mNEtas = mEtas.size()-1;
  mNFullEtas = mFullEtas.size()-1;
  Double_t fracs[101];
  for (int i = 0; i<=100; ++i) fracs[i] = 0.5+0.01*i;

  Double_t *pth = &mPts[0];
  Double_t *etah = &mEtas[0];
  Double_t *fetah = &mFullEtas[0];
  p2mpf  = new TProfile2D("p2mpf" ,";p_{T};#eta;",mNPts,pth,mNEtas,etah);
  p2mpf2 = new TProfile2D("p2mpf2",";p_{T};#eta;",mNPts,pth,mNEtas,etah);
  p2htf  = new TProfile2D("p2htf" ,";p_{T};#eta;",mNPts,pth,mNEtas,etah);
  p2htf2 = new TProfile2D("p2htf2",";p_{T};#eta;",mNPts,pth,mNEtas,etah);
  p2mpf_fe  = new TProfile2D("p2mpf_fe" ,";p_{T};#eta;",mNPts,pth,mNFullEtas,fetah);
  p2mpf2_fe = new TProfile2D("p2mpf2_fe",";p_{T};#eta;",mNPts,pth,mNFullEtas,fetah);
  p2htf_fe  = new TProfile2D("p2htf_fe" ,";p_{T};#eta;",mNPts,pth,mNFullEtas,fetah);
  p2htf2_fe = new TProfile2D("p2htf2_fe",";p_{T};#eta;",mNPts,pth,mNFullEtas,fetah);
  h3mpf = new TH3D("h3mpf",";p_{T};#eta;MPF",mNPts,pth,mNFullEtas,fetah,100,fracs);
  h3htf = new TH3D("h3htf",";p_{T};#eta;HTF",mNPts,pth,mNFullEtas,fetah,100,fracs);
  //int ntot = mNPts*mNEtas;
  //mSquare = new TMatrixD(ntot,2*ntot);
  //mColumn = new TMatrixD(ntot,5);
  //mSingle = new TMatrixD(1,2);
  //mTSquare = new TMatrixD(3*ntot,6*ntot);
  //mTColumn = new TMatrixD(3*ntot,2);
  ////mTSquare = new TMatrixD(4*ntot,4*ntot);
  ////mTColumn = new TMatrixD(4*ntot,2);
  //mSquareT = new TMatrixD(ntot,3*ntot);
  //mColumnT = new TMatrixD(ntot,3);
  ////mTSquareT = new TMatrixD(4*ntot,4*ntot);
  ////mTColumnT = new TMatrixD(4*ntot,1);

  curdir->cd();
}

int HistosAll::PhaseBin(double pt, double eta) {
    eta = fabs(eta);
    int ebin = -1;
    int ptbin = -1;
    for (unsigned ieta = 0; ieta < mNEtas; ++ieta) {
        if (eta < mEtas[ieta+1]) {
            ebin = ieta;
            break;
        }
    }
    for (unsigned ipt = 0; ipt < mNPts; ++ipt) {
        if (pt < mPts[ipt+1]) {
            ptbin = ipt;
            break;
        }
    }
    if (ebin==-1 or ptbin==-1) return -1;

    return ebin*mNPts+ptbin;
}

void HistosAll::Write() {
  dir->cd();
  dir->Write();
  //mSquare->Write();
  //mColumn->Write();
  //mSingle->Write();
  //mTSquare->Write();
  //mTColumn->Write();
  //mSquareT->Write();
  //mColumnT->Write();
  //mTSquareT->Write();
  //mTColumnT->Write();
}

HistosAll::~HistosAll() {
  Write();
};
