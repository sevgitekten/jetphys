// Purpose:  Create eta-dependent histograms for inclusive jets analysis
// Author:   hannu.siikonen@cern.ch
// Created:  April 3, 2017

#include "HistosEta.h"

HistosEta::HistosEta(TDirectory *dir, string trigname, double pttrg, double ptmin, double ptmax, bool ismcdir) {

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

  const int na = 200;
  vector<double> va(na+1);
  for (unsigned int i = 0; i != na+1; ++i) va[i] = -1. + 2.*i/na;

  if (jp::do3dHistos) {
    // Loop over alpha entries of interest
    for (auto alpha : alpharange) {
      int major_alpha = 100*alpha;
      // Start by coming up with a nice number identifier
      int padding = TMath::Log10(major_alpha);
      padding = 2-padding;
      while (major_alpha%10==0)
        major_alpha /= 10;
      string number = std::to_string(major_alpha);
      for (int i = 0; i < padding; ++i)
        number = string("0")+number;

      // Fill all histo types with a corresponding histogram
      hdjasymm.push_back(  new TH3D((string("hdjasymm_a")+number).c_str(),
                                ";p_{T,ave};#eta;Asymmetry",
                                jp::npts,&jp::ptrange[0],jp::nwetas,&jp::wetarange[0],na,&va[0]) );
      hdjasymmtp.push_back(new TH3D((string("hdjasymmtp_a")+number).c_str(),
                                ";p_{T,tag};#eta;Asymmetry",
                                jp::npts,&jp::ptrange[0],jp::nwetas,&jp::wetarange[0],na,&va[0]) );
      //hdjasymmpt.push_back(new TH3D((string("hdjasymmpt_a")+number).c_str(),
      //                          ";p_{T,probe};#eta;Asymmetry",
      //                          jp::npts,&jp::ptrange[0],jp::nwetas,&jp::wetarange[0],na,&va[0]) );
      hdjmpf.push_back(    new TH3D((string("hdjmpf_a")+number).c_str(),
                                ";p_{T,ave};#eta;MPF",
                                jp::npts,&jp::ptrange[0],jp::nwetas,&jp::wetarange[0],na,&va[0]) );
      hdjmpftp.push_back(  new TH3D((string("hdjmpftp_a")+number).c_str(),
                                ";p_{T,tag};#eta;MPF",
                                jp::npts,&jp::ptrange[0],jp::nwetas,&jp::wetarange[0],na,&va[0]) );
     // hdjmpfpt.push_back(  new TH3D((string("hdjmpfpt_a")+number).c_str(),
     //                           ";p_{T,probe};#eta;MPF",
     //                           jp::npts,&jp::ptrange[0],jp::nwetas,&jp::wetarange[0],na,&va[0]) );
    }

    // Weights:
    for (unsigned i = 0; i < alpharange.size(); ++i) {
      hdjasymm[i]->Sumw2();
      hdjasymmtp[i]->Sumw2();
      //hdjasymmpt[i]->Sumw2();
      hdjmpf[i]->Sumw2();
      hdjmpftp[i]->Sumw2();
      //hdjmpfpt[i]->Sumw2();
    }
  }

  // Special studies for tag between 50 and 60 GeV.
  hnpvall_pt50to60 = new TH1D("hnpvall_pt50to60","",jp::npvs,&jp::npvrange[0]);
  hnpv_pt50to60    = new TH1D("hnpv_pt50to60"   ,"",jp::npvs,&jp::npvrange[0]);
  htrpu_pt50to60    = new TH1D("htrpu_pt50to60"   ,"",jp::npvs,&jp::npvrange[0]);
  hrho_pt50to60    = new TH1D("hrho_pt50to60"   ,"",jp::npvs,&jp::npvrange[0]);
  hpuf_pt50to60    = new TH1D("hpuf_pt50to60"   ,"",100,0,1);
  hchf_pt50to60    = new TH1D("hchf_pt50to60"   ,"",100,0,1);

  // eta profiles
  heta = new TH1D("heta","",jp::netas,jp::etarange);
  hetaphi = new TH2D("hetaphi","",jp::netas,jp::etarange,jp::nphis,jp::phirange);
  hetaphi_chf = new TH2D("hetaphi_chf","",jp::netas,jp::etarange,jp::nphis,jp::phirange);
  hetaphi_nef = new TH2D("hetaphi_nef","",jp::netas,jp::etarange,jp::nphis,jp::phirange);
  hetaphi_nhf = new TH2D("hetaphi_nhf","",jp::netas,jp::etarange,jp::nphis,jp::phirange);
  hetaphi_cef = new TH2D("hetaphi_cef","",jp::netas,jp::etarange,jp::nphis,jp::phirange);
  hetaphi_muf = new TH2D("hetaphi_muf","",jp::netas,jp::etarange,jp::nphis,jp::phirange);
  hetaphi_hhf = new TH2D("hetaphi_hhf","",jp::netas,jp::etarange,jp::nphis,jp::phirange);
  hetaphi_hef = new TH2D("hetaphi_hef","",jp::netas,jp::etarange,jp::nphis,jp::phirange);
  hetaphi_puf = new TH2D("hetaphi_puf","",jp::netas,jp::etarange,jp::nphis,jp::phirange);
  petaphi_chf = new TProfile2D("petaphi_chf","",jp::netas,jp::etarange,jp::nphis,jp::phirange);
  petaphi_nef = new TProfile2D("petaphi_nef","",jp::netas,jp::etarange,jp::nphis,jp::phirange);
  petaphi_nhf = new TProfile2D("petaphi_nhf","",jp::netas,jp::etarange,jp::nphis,jp::phirange);
  petaphi_cef = new TProfile2D("petaphi_cef","",jp::netas,jp::etarange,jp::nphis,jp::phirange);
  petaphi_muf = new TProfile2D("petaphi_muf","",jp::netas,jp::etarange,jp::nphis,jp::phirange);
  petaphi_hhf = new TProfile2D("petaphi_hhf","",jp::netas,jp::etarange,jp::nphis,jp::phirange);
  petaphi_hef = new TProfile2D("petaphi_hef","",jp::netas,jp::etarange,jp::nphis,jp::phirange);
  petaphi_puf = new TProfile2D("petaphi_puf","",jp::netas,jp::etarange,jp::nphis,jp::phirange);

  // components vs eta
  pncandtp_vseta = new TProfile("pcandtp_vseta","",jp::netas,jp::etarange);
  pnchtp_vseta = new TProfile("pnchtp_vseta","",jp::netas,jp::etarange);
  pnnetp_vseta = new TProfile("pnnetp_vseta","",jp::netas,jp::etarange);
  pnnhtp_vseta = new TProfile("pnnhtp_vseta","",jp::netas,jp::etarange);
  pncetp_vseta = new TProfile("pncetp_vseta","",jp::netas,jp::etarange);
  pnmutp_vseta = new TProfile("pnmutp_vseta","",jp::netas,jp::etarange);
  pnhhtp_vseta = new TProfile("pnhhtp_vseta","",jp::netas,jp::etarange);
  pnhetp_vseta = new TProfile("pnhetp_vseta","",jp::netas,jp::etarange);
  pchftp_vseta = new TProfile("pchftp_vseta","",jp::netas,jp::etarange);
  pneftp_vseta = new TProfile("pneftp_vseta","",jp::netas,jp::etarange);
  pnhftp_vseta = new TProfile("pnhftp_vseta","",jp::netas,jp::etarange);
  pceftp_vseta = new TProfile("pceftp_vseta","",jp::netas,jp::etarange);
  pmuftp_vseta = new TProfile("pmuftp_vseta","",jp::netas,jp::etarange);
  phhftp_vseta = new TProfile("phhftp_vseta","",jp::netas,jp::etarange);
  pheftp_vseta = new TProfile("pheftp_vseta","",jp::netas,jp::etarange);
  ppuftp_vseta = new TProfile("ppuftp_vseta","",jp::netas,jp::etarange);

  if (this->ismcdir) { // response closure
    p3rvsnpv = new TProfile3D("p3rvsnpv","",jp::npts,&jp::ptrange[0],jp::netas,&jp::etarange[0],jp::npvs,&jp::npvrange[0]);
    p3rvsnpvW = new TProfile3D("p3rvsnpvW","",jp::nwpts,&jp::wptrange[0],jp::nposetas,&jp::posetarange[0],jp::npvs,&jp::npvrange[0]);
  }

  curdir->cd();
}

void HistosEta::Write() {
  dir->cd();
  dir->Write();
}

HistosEta::~HistosEta() {
  Write();
};
