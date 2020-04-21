// Fill jet physics analysis histograms
// Author:  mikko.voutilainen@cern.ch
// Created: April 19, 2010
// Updated: Continuously, see git

#define HistosFill_cxx
#include "HistosFill.h"


// Set the shortcuts for variables
HistosFill::HistosFill(TChain *tree, int eraIdx) :
  pthat(EvtHdr__mPthat),
  weight(EvtHdr__mWeight),
  run(EvtHdr__mRun),
  evt(EvtHdr__mEvent),
  lbn(EvtHdr__mLumi),
  itpu(EvtHdr__mINTPU),
  ootpulate(EvtHdr__mOOTPULate),
  ootpuearly(EvtHdr__mOOTPUEarly),
  npv(EvtHdr__mNVtx),
  npvgood(EvtHdr__mNVtxGood),
  pvx(EvtHdr__mPVx),
  pvy(EvtHdr__mPVy),
  pvz(EvtHdr__mPVz),
  pvndof(EvtHdr__mPVndof),
  bsx(EvtHdr__mBSx),
  bsy(EvtHdr__mBSy),
  njt(PFJetsCHS__),
  gen_njt(GenJets__),
  rho(EvtHdr__mPFRho),
  met(PFMet__et_),
  metphi(PFMet__phi_),
  metsumet(PFMet__sumEt_),
#ifdef NEWMODE
  met0(PFMetT0__et_),
  metphi0(PFMetT0__phi_),
  metsumet0(PFMetT0__sumEt_),
  met01(PFMetT0T1__et_),
  metphi01(PFMetT0T1__phi_),
  metsumet01(PFMetT0T1__sumEt_)
#endif
{
  assert(tree);
  _initsuccess = Init(tree);
  _eraIdx = eraIdx;
}


// Print the same info on a file and conditionally to the output
void HistosFill::PrintInfo(string info, bool printcout)
{
  *ferr << info << endl << flush;
  if (printcout) cout << info << endl << flush;
}


// Report memory info
void HistosFill::PrintMemInfo(bool printcout)
{
  gSystem->GetMemInfo(&_info);
  PrintInfo(Form("MemInfo(Tot:%d, Used:%d, Free:%d, Stot:%d, SUsed:%d, SFree:%d",
                 _info.fMemTotal,  _info.fMemUsed,  _info.fMemFree,
                 _info.fSwapTotal, _info.fSwapUsed, _info.fSwapFree),printcout);
}


// Mostly setting up the root tree and its branches
bool HistosFill::Init(TChain *tree)
{
  ferr = 0;
  ferr = new ofstream(Form("reports/HistosFill-%s.log",jp::type),ios::out);

  if (!tree) {
    PrintInfo("No tree given, processing makes no sense.",true);
    return false; // With no tree, Loop will be interrupted
  }
  string type = jp::type;
  if (jp::pthatbins) type = "Pthat";
  else if (jp::htbins) type = "HT";
  _outfile = new TFile(Form("output-%s-1.root",type.c_str()), "NEW");
  if (!_outfile or _outfile->IsZombie()) {
    PrintInfo(Form("Opening the output file output-%s-1.root failed. Check if the file already exists.",jp::type),false);
    return false;
  }

  fChain = tree;
  fCurrent = -1;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("EvtHdr_.mIsPVgood", &EvtHdr__mIsPVgood);
  fChain->SetBranchAddress("EvtHdr_.mRun", &EvtHdr__mRun);
  fChain->SetBranchAddress("EvtHdr_.mEvent", &EvtHdr__mEvent);
  fChain->SetBranchAddress("EvtHdr_.mLumi", &EvtHdr__mLumi);
  fChain->SetBranchAddress("EvtHdr_.mBunch", &EvtHdr__mBunch);
  fChain->SetBranchAddress("EvtHdr_.mNVtx", &EvtHdr__mNVtx);
  fChain->SetBranchAddress("EvtHdr_.mNVtxGood", &EvtHdr__mNVtxGood);
  fChain->SetBranchAddress("EvtHdr_.mOOTPUEarly", &EvtHdr__mOOTPUEarly);
  fChain->SetBranchAddress("EvtHdr_.mOOTPULate", &EvtHdr__mOOTPULate);
  fChain->SetBranchAddress("EvtHdr_.mINTPU", &EvtHdr__mINTPU);
  fChain->SetBranchAddress("EvtHdr_.mNBX", &EvtHdr__mNBX);
  fChain->SetBranchAddress("EvtHdr_.mPVndof", &EvtHdr__mPVndof);
  fChain->SetBranchAddress("EvtHdr_.mTrPu", &EvtHdr__mTrPu);
  fChain->SetBranchAddress("EvtHdr_.mPVx", &EvtHdr__mPVx);
  fChain->SetBranchAddress("EvtHdr_.mPVy", &EvtHdr__mPVy);
  fChain->SetBranchAddress("EvtHdr_.mPVz", &EvtHdr__mPVz);
  fChain->SetBranchAddress("EvtHdr_.mBSx", &EvtHdr__mBSx);
  fChain->SetBranchAddress("EvtHdr_.mBSy", &EvtHdr__mBSy);
  fChain->SetBranchAddress("EvtHdr_.mBSz", &EvtHdr__mBSz);
  fChain->SetBranchAddress("EvtHdr_.mPthat", &EvtHdr__mPthat);
  fChain->SetBranchAddress("EvtHdr_.mWeight", &EvtHdr__mWeight);
  fChain->SetBranchAddress("EvtHdr_.mCaloRho", &EvtHdr__mCaloRho);
  fChain->SetBranchAddress("EvtHdr_.mPFRho", &EvtHdr__mPFRho);
#ifdef NEWMODE
  fChain->SetBranchAddress("PFMetT0_.et_", &PFMetT0__et_);
  fChain->SetBranchAddress("PFMetT0_.sumEt_", &PFMetT0__sumEt_);
  fChain->SetBranchAddress("PFMetT0_.phi_", &PFMetT0__phi_);
  fChain->SetBranchAddress("PFMetT0T1_.et_", &PFMetT0T1__et_);
  fChain->SetBranchAddress("PFMetT0T1_.sumEt_", &PFMetT0T1__sumEt_);
  fChain->SetBranchAddress("PFMetT0T1_.phi_", &PFMetT0T1__phi_);
#endif
  fChain->SetBranchAddress("PFMet_.et_", &PFMet__et_);
  fChain->SetBranchAddress("PFMet_.sumEt_", &PFMet__sumEt_);
  fChain->SetBranchAddress("PFMet_.phi_", &PFMet__phi_);
  if (jp::fetchMETFilters and jp::doMETFiltering) fChain->SetBranchAddress("FilterDecision_", &FilterDecision_);
  fChain->SetBranchAddress("TriggerDecision_", &TriggerDecision_);
  fChain->SetBranchAddress("L1Prescale_", &L1Prescale_);
  fChain->SetBranchAddress("HLTPrescale_", &HLTPrescale_);
  fChain->SetBranchAddress("GenJets_", &GenJets__);
  fChain->SetBranchAddress("GenJets_.fCoordinates.fX", GenJets__fCoordinates_fX);
  fChain->SetBranchAddress("GenJets_.fCoordinates.fY", GenJets__fCoordinates_fY);
  fChain->SetBranchAddress("GenJets_.fCoordinates.fZ", GenJets__fCoordinates_fZ);
  fChain->SetBranchAddress("GenJets_.fCoordinates.fT", GenJets__fCoordinates_fT);
#ifdef NEWMODE
  fChain->SetBranchAddress("PFJetsCHS_.genIdx_", PFJetsCHS__genIdx_);
#else
  fChain->SetBranchAddress(Form("PFJets%s_.genP4_.fCoordinates.fX",jp::chs), PFJetsCHS__genP4__fCoordinates_fX);
  fChain->SetBranchAddress(Form("PFJets%s_.genP4_.fCoordinates.fY",jp::chs), PFJetsCHS__genP4__fCoordinates_fY);
  fChain->SetBranchAddress(Form("PFJets%s_.genP4_.fCoordinates.fZ",jp::chs), PFJetsCHS__genP4__fCoordinates_fZ);
  fChain->SetBranchAddress(Form("PFJets%s_.genP4_.fCoordinates.fT",jp::chs), PFJetsCHS__genP4__fCoordinates_fT);
#endif
#ifdef NEWMODE
  fChain->SetBranchAddress(Form("PFJets%s_.QGL_",jp::chs), PFJetsCHS__QGL_); // qgl
#else
  fChain->SetBranchAddress(Form("PFJets%s_.QGtagger_",jp::chs), PFJetsCHS__QGtagger_); // qgl
#endif
  fChain->SetBranchAddress(Form("PFJets%s_",jp::chs), &PFJetsCHS__);
  fChain->SetBranchAddress(Form("PFJets%s_.P4_.fCoordinates.fX",jp::chs), PFJetsCHS__P4__fCoordinates_fX);
  fChain->SetBranchAddress(Form("PFJets%s_.P4_.fCoordinates.fY",jp::chs), PFJetsCHS__P4__fCoordinates_fY);
  fChain->SetBranchAddress(Form("PFJets%s_.P4_.fCoordinates.fZ",jp::chs), PFJetsCHS__P4__fCoordinates_fZ);
  fChain->SetBranchAddress(Form("PFJets%s_.P4_.fCoordinates.fT",jp::chs), PFJetsCHS__P4__fCoordinates_fT);
  fChain->SetBranchAddress(Form("PFJets%s_.genR_",jp::chs), PFJetsCHS__genR_);
  fChain->SetBranchAddress(Form("PFJets%s_.cor_",jp::chs), PFJetsCHS__cor_);
  fChain->SetBranchAddress(Form("PFJets%s_.area_",jp::chs), PFJetsCHS__area_);
  fChain->SetBranchAddress(Form("PFJets%s_.looseID_",jp::chs), PFJetsCHS__looseID_);
  fChain->SetBranchAddress(Form("PFJets%s_.tightID_",jp::chs), PFJetsCHS__tightID_);
  fChain->SetBranchAddress(Form("PFJets%s_.partonFlavour_",jp::chs), PFJetsCHS__partonFlavour_);
  fChain->SetBranchAddress(Form("PFJets%s_.hadronFlavour_",jp::chs), PFJetsCHS__hadronFlavour_);
#ifdef NEWMODE
  fChain->SetBranchAddress(Form("PFJets%s_.partonFlavourPhysicsDef_",jp::chs), PFJetsCHS__partonFlavourPhysicsDef_);
#endif
  fChain->SetBranchAddress(Form("PFJets%s_.chf_",jp::chs), PFJetsCHS__chf_);
  fChain->SetBranchAddress(Form("PFJets%s_.nhf_",jp::chs), PFJetsCHS__nhf_);
  fChain->SetBranchAddress(Form("PFJets%s_.nemf_",jp::chs), PFJetsCHS__nemf_);
  fChain->SetBranchAddress(Form("PFJets%s_.cemf_",jp::chs), PFJetsCHS__cemf_);
  fChain->SetBranchAddress(Form("PFJets%s_.muf_",jp::chs), PFJetsCHS__muf_);
  fChain->SetBranchAddress(Form("PFJets%s_.hf_hf_",jp::chs), PFJetsCHS__hf_hf_);
  fChain->SetBranchAddress(Form("PFJets%s_.hf_phf_",jp::chs), PFJetsCHS__hf_phf_);
  fChain->SetBranchAddress(Form("PFJets%s_.chm_",jp::chs), PFJetsCHS__chm_);
  fChain->SetBranchAddress(Form("PFJets%s_.nhm_",jp::chs), PFJetsCHS__nhm_);
  fChain->SetBranchAddress(Form("PFJets%s_.phm_",jp::chs), PFJetsCHS__phm_);
  fChain->SetBranchAddress(Form("PFJets%s_.elm_",jp::chs), PFJetsCHS__elm_);
  fChain->SetBranchAddress(Form("PFJets%s_.mum_",jp::chs), PFJetsCHS__mum_);
  fChain->SetBranchAddress(Form("PFJets%s_.hf_hm_",jp::chs), PFJetsCHS__hf_hm_);
  fChain->SetBranchAddress(Form("PFJets%s_.hf_phm_",jp::chs), PFJetsCHS__hf_phm_);
  fChain->SetBranchAddress(Form("PFJets%s_.ncand_",jp::chs), PFJetsCHS__ncand_);
  fChain->SetBranchAddress(Form("PFJets%s_.betaPrime_",jp::chs), PFJetsCHS__betaPrime_);
  fChain->SetBranchAddress("genFlavour_", &genFlavour_);
  fChain->SetBranchAddress("genFlavourHadron_", &genFlavourHadron_);
#ifdef NEWMODE
  fChain->SetBranchAddress("genFlavourPartonPhysicsDef_", &genFlavourPartonPhysicsDef_);
#endif

  if (jp::quick) { // Activate only some branches
    fChain->SetBranchStatus("*",0);
    // Luminosity calculation
    if (jp::ismc) fChain->SetBranchStatus("EvtHdr_.mPthat",1); // pthat
    if (jp::ismc) fChain->SetBranchStatus("EvtHdr_.mWeight",1); // weight
    if (jp::isdt) fChain->SetBranchStatus("EvtHdr_.mRun",1); // run
    if (jp::isdt) fChain->SetBranchStatus("EvtHdr_.mEvent",1); // evt
    if (jp::isdt) fChain->SetBranchStatus("EvtHdr_.mLumi",1); // lbn

    // Event properties
    fChain->SetBranchStatus("EvtHdr_.mNVtx",1); // npv
    fChain->SetBranchStatus("EvtHdr_.mNVtxGood",1); // npvgood
    fChain->SetBranchStatus("EvtHdr_.mPFRho",1); // rho

    // Jet properties (jtpt, jte, jteta, jty, jtphi etc.)
    fChain->SetBranchStatus(Form("PFJets%s_",jp::chs),1); // njt
    fChain->SetBranchStatus(Form("PFJets%s_.P4_*",jp::chs),1); // jtp4*
    fChain->SetBranchStatus(Form("PFJets%s_.cor_",jp::chs),1); // jtjes
    fChain->SetBranchStatus(Form("PFJets%s_.area_",jp::chs),1); // jta

    if (jp::ismc) {

      fChain->SetBranchStatus(Form("PFJets%s_.genR_",jp::chs),1); // jtgenr
#ifdef NEWMODE
      fChain->SetBranchStatus(Form("PFJets%s_.genIdx_",jp::chs),1); // jtgenidx
#else
      fChain->SetBranchStatus(Form("PFJets%s_.genP4_*",jp::chs),1); // jtgenp4*
#endif
    }

    // for quark/gluon study (Ozlem)
#ifdef NEWMODE
    fChain->SetBranchStatus(Form("PFJets%s_.QGL_",jp::chs),1); // qgl
#else
    fChain->SetBranchStatus(Form("PFJets%s_.QGtagger_",jp::chs),1); // qgl
#endif
    if (jp::ismc) fChain->SetBranchStatus(Form("PFJets%s_.partonFlavour_",jp::chs),1);
    if (jp::ismc) fChain->SetBranchStatus(Form("PFJets%s_.partonFlavourPhysicsDef_",jp::chs),1);

    // Component fractions
    fChain->SetBranchStatus(Form("PFJets%s_.chf_",jp::chs),1); // jtchf
    fChain->SetBranchStatus(Form("PFJets%s_.nemf_",jp::chs),1); // jtnef
    fChain->SetBranchStatus(Form("PFJets%s_.nhf_",jp::chs),1); // jtnhf
    fChain->SetBranchStatus(Form("PFJets%s_.cemf_",jp::chs),1); // jtcef !!
    fChain->SetBranchStatus(Form("PFJets%s_.muf_",jp::chs),1); // jtmuf !!
    fChain->SetBranchStatus(Form("PFJets%s_.hf_hf_",jp::chs),1); // jtmuf !!
    fChain->SetBranchStatus(Form("PFJets%s_.hf_phf_",jp::chs),1); // jtmuf !!
    fChain->SetBranchStatus(Form("PFJets%s_.ncand_",jp::chs),1); // jtn
    fChain->SetBranchStatus(Form("PFJets%s_.betaPrime_",jp::chs),1); // jtbetaprime
    fChain->SetBranchStatus(Form("PFJets%s_.chm_",jp::chs),1); // jtnch
    fChain->SetBranchStatus(Form("PFJets%s_.phm_",jp::chs),1); // jtnne
    fChain->SetBranchStatus(Form("PFJets%s_.nhm_",jp::chs),1); // jtnnh
    fChain->SetBranchStatus(Form("PFJets%s_.elm_",jp::chs),1); // jtnce !!
    fChain->SetBranchStatus(Form("PFJets%s_.mum_",jp::chs),1); // jtnmu !!
    fChain->SetBranchStatus(Form("PFJets%s_.hf_hm_",jp::chs),1); // jtnmu !!
    fChain->SetBranchStatus(Form("PFJets%s_.hf_phm_",jp::chs),1); // jtnmu !!
    fChain->SetBranchStatus(Form("PFJets%s_.tightID_",jp::chs),1); // jtidtight
    fChain->SetBranchStatus(Form("PFJets%s_.looseID_",jp::chs),1); // jtidloose

    //fChain->SetBranchStatus("rho",1);
    fChain->SetBranchStatus("PFMet_.et_",1); // met
    fChain->SetBranchStatus("PFMet_.phi_",1); // metphi
    fChain->SetBranchStatus("PFMet_.sumEt_",1); // metsumet
#ifdef NEWMODE
    fChain->SetBranchStatus("PFMetT0_.et_",1); // met
    fChain->SetBranchStatus("PFMetT0_.phi_",1); // metphi
    fChain->SetBranchStatus("PFMetT0_.sumEt_",1); // metsumet
    fChain->SetBranchStatus("PFMetT0T1_.et_",1); // met
    fChain->SetBranchStatus("PFMetT0T1_.phi_",1); // metphi
    fChain->SetBranchStatus("PFMetT0T1_.sumEt_",1); // metsumet
#endif

    if (jp::fetchMETFilters and jp::doMETFiltering) fChain->SetBranchStatus("FilterDecision_",1);
    fChain->SetBranchStatus("TriggerDecision_",1);
    fChain->SetBranchStatus("L1Prescale_",1);
    fChain->SetBranchStatus("HLTPrescale_",1);

    // Event cleaning
    //fChain->SetBranchStatus("pvrho",1);
    fChain->SetBranchStatus("EvtHdr_.mPVx",1); // pvx
    fChain->SetBranchStatus("EvtHdr_.mPVy",1); // pvy
    fChain->SetBranchStatus("EvtHdr_.mPVz",1); // pvz
    fChain->SetBranchStatus("EvtHdr_.mPVndof",1); // pvndof
    fChain->SetBranchStatus("EvtHdr_.mBSx",1); // bsx
    fChain->SetBranchStatus("EvtHdr_.mBSy",1); // bsy

    if (jp::ismc) {
      fChain->SetBranchStatus("EvtHdr_.mTrPu",1); // trpu
      fChain->SetBranchStatus("EvtHdr_.mINTPU",1); // itpu
      fChain->SetBranchStatus("EvtHdr_.mOOTPULate",1); // ootpulate
      fChain->SetBranchStatus("EvtHdr_.mOOTPUEarly",1); // ootpuearly
      fChain->SetBranchStatus("GenJets_",1); // gen_njt
      fChain->SetBranchStatus("GenJets_.fCoordinates.fX",1); // gen_jtp4x
      fChain->SetBranchStatus("GenJets_.fCoordinates.fY",1); // gen_jtp4y
      fChain->SetBranchStatus("GenJets_.fCoordinates.fZ",1); // gen_jtp4z
      fChain->SetBranchStatus("GenJets_.fCoordinates.fT",1); // gen_jtp4t
    }
  } else {
    fChain->SetBranchStatus("*",1);
  } // quick/slow

  // Set pointers to branches. NOTE: These are here so that the PFJetsCHS stuff etc. can be easily updated
  // if the file format changes.
  jtp4x = &PFJetsCHS__P4__fCoordinates_fX[0];
  jtp4y = &PFJetsCHS__P4__fCoordinates_fY[0];
  jtp4z = &PFJetsCHS__P4__fCoordinates_fZ[0];
  jtp4t = &PFJetsCHS__P4__fCoordinates_fT[0];
  jta = &PFJetsCHS__area_[0];
  jtjes = &PFJetsCHS__cor_[0];
  jtbetaprime = &PFJetsCHS__betaPrime_[0];
  jtidloose = &PFJetsCHS__looseID_[0];
  jtidtight = &PFJetsCHS__tightID_[0];
  //
  jtgenr = &PFJetsCHS__genR_[0];
#ifdef NEWMODE
  jtgenidx = &PFJetsCHS__genIdx_[0];
#else
  jtgenp4x = &PFJetsCHS__P4__fCoordinates_fX[0];
  jtgenp4y = &PFJetsCHS__P4__fCoordinates_fY[0];
  jtgenp4z = &PFJetsCHS__P4__fCoordinates_fZ[0];
  jtgenp4t = &PFJetsCHS__P4__fCoordinates_fT[0];
#endif
  //

  partonflavor = &PFJetsCHS__partonFlavour_[0];
#ifdef NEWMODE
  partonflavorphys = &PFJetsCHS__partonFlavourPhysicsDef_[0];
  // for quark/gluon study (Ozlem)
  qgl = &PFJetsCHS__QGL_[0];
#else
  // for quark/gluon study (Ozlem)
  qgl = &PFJetsCHS__QGtagger_[0];
#endif

  jtn = &PFJetsCHS__ncand_[0];
  jtnch = &PFJetsCHS__chm_[0];
  jtnnh = &PFJetsCHS__nhm_[0];
  jtnne = &PFJetsCHS__phm_[0];
  jtnce = &PFJetsCHS__elm_[0];
  jtnmu = &PFJetsCHS__mum_[0];
  jtnhh = &PFJetsCHS__hf_hm_[0];
  jtnhe = &PFJetsCHS__hf_phm_[0];

  jtchf = &PFJetsCHS__chf_[0];
  jtnhf = &PFJetsCHS__nhf_[0];
  jtnef = &PFJetsCHS__nemf_[0];
  jtcef = &PFJetsCHS__cemf_[0];
  jtmuf = &PFJetsCHS__muf_[0];
  jthhf = &PFJetsCHS__hf_hf_[0];
  jthef = &PFJetsCHS__hf_phf_[0];

  gen_jtp4x = &GenJets__fCoordinates_fX[0];
  gen_jtp4y = &GenJets__fCoordinates_fY[0];
  gen_jtp4z = &GenJets__fCoordinates_fZ[0];
  gen_jtp4t = &GenJets__fCoordinates_fT[0];

  // Put some logging info into the file
  _runinfo = "\n";
  vector<string> infofiles = {"settings.h","HistosFill.h","HistosFill.C","HistosBasic.h","HistosBasic.C","HistosEta.h","HistosEta.C","HistosAll.h","HistosAll.C","HistosNormalize.C","HistosCombine.C"};
  if (jp::gitinfo) {
    gROOT->ProcessLine(".!git log > gitlog.txt");
    infofiles.push_back("gitlog.txt");
  }
  for (auto &infofile : infofiles) {
    std::ifstream tmpstrm(infofile.c_str());
    string tmpstr;

    tmpstrm.seekg(0, std::ios::end);
    tmpstr.reserve(tmpstrm.tellg());
    tmpstrm.seekg(0, std::ios::beg);
    tmpstr.assign((std::istreambuf_iterator<char>(tmpstrm)),std::istreambuf_iterator<char>());
    _runinfo += Form("\n###I\n###I %s Block\n###I\n\n",infofile.c_str());
    _runinfo += tmpstr;
  }
  if (jp::gitinfo) gROOT->ProcessLine(".!rm gitlog.txt");

  return true;
}


// Loop over events
void HistosFill::Loop()
{
  if (!_initsuccess) return;

  // Report memory usage to avoid malloc problems when writing file
  PrintInfo("Starting Loop() initialization:");
  PrintMemInfo(true);

  if (!PreRun()) return;

  // Report memory usage to avoid malloc problems when writing file
  PrintInfo("Beginning Loop() proper:");
  PrintMemInfo(true);

  TStopwatch stop;
  stop.Start();
  TDatime bgn;

  ///////////////
  // Event loop
  ///////////////
  Long64_t nbytes = 0, nb = 0;
  Long64_t hopval = 1000000;
  Long64_t repval = 100001;
  for (Long64_t djentry=0; djentry<_nentries;djentry+=1+jp::skim) { // Event loop
    _jentry = djentry+_nskip; // Add a shift from beginning
    if (LoadTree(_jentry) < 0) break;
    nb = fChain->GetEntry(_jentry);   nbytes += nb;

    int testval = djentry;
    if (jp::skim>0) testval /= 1+jp::skim;

    if (testval%hopval==repval) { // 1M report (first report timed to be early)
      // Report memory usage to avoid malloc problems when writing file
      PrintInfo(Form("Doing Loop(), %dM events:",int(_jentry/1e6 + 0.5)),true);
      PrintMemInfo();
      PrintInfo(Form("\nProcessed %lld events (%1.1f%%) in %1.0f sec.",_jentry-1, 100.*djentry/_nentries, stop.RealTime()),true);
      cout << "BGN: ";
      bgn.Print();
      TDatime now;
      cout << "NOW: ";
      now.Print();
      TDatime nxt;
      nxt.Set(nxt.Convert()+static_cast<UInt_t>(stop.RealTime()*(static_cast<Double_t>(hopval)/static_cast<Double_t>(testval))));
      cout << "NXT: ";
      nxt.Print();
      now.Set(now.Convert()+static_cast<UInt_t>(stop.RealTime()*(static_cast<Double_t>(_nentries)/static_cast<Double_t>(djentry)-1.0)));
      cout << "ETA: ";
      now.Print();

      if (jp::save) {
        for (auto &manyhists : _histos)
          for (auto &onehist : manyhists.second)
            onehist->Write();
          if (jp::doEtaHistos) {
            for (auto &manyhists : _etahistos)
              for (auto & onehist : manyhists.second)
                onehist->Write();
            if (jp::doEtaHistosMcResponse) {
              for (auto &manyhists : _mchistos)
                for (auto & onehist : manyhists.second)
                  onehist->Write();
            }
          }
      }
      stop.Continue();
    }

    if (!AcceptEvent()) continue;

    if (jp::debug) PrintInfo("Histos are being filled!",true);
    // Here can categorize events into different triggers, epochs, topologies etc.
    // Eta and pT binning are handled in the FillSingleBasic class
    if (jp::doBasicHistos) {
      FillBasic("Standard");
    }

    if (jp::doEtaHistos and _pass) {
      FillEta("FullEta_Reco", jtpt, jteta, jtphi);
      if (jp::ismc and jp::doEtaHistosMcResponse) {
        FillEta("FullEta_Gen", jtgenpt, jtgeneta, jtgenphi);
        FillMC("FullEta_RecoPerGen_vReco", jtpt, jtgenpt, jtpt,    jteta,    jtphi);
        FillMC("FullEta_RecoPerGen_vGen",  jtpt, jtgenpt, jtgenpt, jtgeneta, jtgenphi);
      }
    }

    // Run quality checks
    if (jp::isdt and jp::doRunHistos) {
      FillRun("Runs");
      FillRun("RunsBarrel");
      FillRun("RunsTransition");
      FillRun("RunsEndcap");
    }

    if (jp::doMpfHistos and _pass) {
      FillAll("AllTrigs");
    }
  } // for jentry
  PrintInfo("",true);

  // Report memory usage to avoid malloc problems when writing file
  PrintInfo(Form("Finished processing %lld entries:",_nentries),true);
  PrintMemInfo(true);

  if (jp::isdt and jp::doRunHistos)   WriteRun();
  if (jp::doEtaHistos)   WriteEta();
  if (jp::ismc and jp::doEtaHistos and jp::doEtaHistosMcResponse) WriteMC();
  if (jp::doMpfHistos)   WriteAll();
  if (jp::doBasicHistos) WriteBasic(); // this needs to be last, output file closed

  Report();

  PrintInfo("Stopping processing at: ",true);
  TDatime now1;
  now1.Print();
  PrintInfo(Form("Processing used %f s CPU time (%f h)",stop.CpuTime(),stop.CpuTime()/3600.),true);
  PrintInfo(Form("Processing used %f s real time (%f h)",stop.RealTime(),stop.RealTime()/3600.),true);

  // Gather info from the report file
  std::ifstream tmpstrm(Form("reports/HistosFill-%s.log",jp::type));
  string tmpstr;

  tmpstrm.seekg(0, std::ios::end);
  tmpstr.reserve(tmpstrm.tellg());
  tmpstrm.seekg(0, std::ios::beg);
  tmpstr.assign((std::istreambuf_iterator<char>(tmpstrm)),std::istreambuf_iterator<char>());
  _runinfo += "\n###I\n###I Run Info Block\n###I\n\n";
  _runinfo += tmpstr;

  // Write RunInfo and close the file
  _outfile->mkdir("infodir");
  _outfile->cd("infodir");
  TNamed runinfo("info",_runinfo);
  runinfo.Write();
  _outfile->Close();

  PrintInfo("File closed, exiting at: ",true);
  TDatime now2;
  now2.Print();
  PrintInfo(Form("Processing and logistics used %f s CPU time (%f h)",stop.CpuTime(),stop.CpuTime()/3600.),true);
  PrintInfo(Form("Processing and logistics used %f s real time (%f h)",stop.RealTime(),stop.RealTime()/3600.),true);
  stop.Continue();
}


// Setup before event loop
bool HistosFill::PreRun()
{
  _nentries = fChain->GetEntriesFast();
  _ntot = fChain->GetEntries();
  _nskip = jp::nskip;//0;
  _nentries = (jp::nentries==-1 ? _ntot-_nskip : min(_ntot-_nskip, jp::nentries));
  assert(_nskip+_nentries);

  _nbadevts_json = _nbadevts_dup = _nbadevts_run = _nbadevts_ls = _nbadevts_lum = 0;
  _bscounter_bad = _bscounter_good = 0;
  _rhocounter_good = _rhocounter_bad = 0;
  _trgcounter = _evtcounter = _totcounter = 0;

  // Initialize _binnedmcweight. It will be loaded for each tree separately.
  if (jp::pthatbins or jp::htbins) {
    _binnedmcweight = 0;
    _binnedmcrepeats = 0;
  }

  PrintInfo("\nCONFIGURATION DUMP:");
  PrintInfo("-------------------");
  PrintInfo(Form("Running over %sPF",jp::algo));
  PrintInfo(Form("Running over %s",jp::ismc?"MC":"data"));
  PrintInfo(Form("%s time-dependent JEC (IOV)",jp::useIOV ? "Applying" : "Not applying"));
  PrintInfo(Form("%s Hot zone exclusion in eta-phi plane",jp::doVetoHot ? "Doing" : "Not doing"));
  *ferr << endl;

  if (jp::isdt) {
    PrintInfo(Form("%s additional JSON selection",jp::dojson ? "Applying" : "Not applying"));
    PrintInfo(Form("%s luminosities.",jp::dolumi ? "Recalculating" : "Not recalculating"));
    PrintInfo(Form("%s additional run-level histograms",jp::doRunHistos ? "Storing" : "Not storing"));
    PrintInfo(Form("%s basic set of histograms",jp::doBasicHistos ? "Storing" : "Not storing"));
    PrintInfo(Form("%s histograms with a full eta-range",jp::doEtaHistos ? "Storing" : "Not storing"));
  } else if (jp::ismc) {
    PrintInfo(Form("%s pileup profile in MC to data",jp::reweighPU ? "Reweighting" : "Not reweighting"));
    PrintInfo(Form("Processing %s samples", jp::pthatbins ? "pThat binned" : "\"flat\""));
  }
  *ferr << endl;

  _JEC = 0;
  _L1RC = 0;
  _jecUnc = 0;

  // Time dependent JEC (only for dt)
  if (jp::redojes) {
    if (jp::isdt and jp::useIOV) {
      // If multiple IOV's are used, we set _JEC etc for each event separately, checking that the IOV is correct
      for (unsigned iovidx=0; iovidx<jp::IOVnames.size(); ++iovidx)
        _iov.addJEC(jp::IOVnames[iovidx],jp::IOVranges.at(iovidx).at(0),jp::IOVranges.at(iovidx).at(1));
    } else {
      // If only one great IOV is used, we can set _JEC etc. directly here.
      _iov.addJEC("");
      bool setcorrection = _iov.setJEC(&_JEC,&_L1RC,&_jecUnc);
      if (!setcorrection or !_JEC or !_L1RC or (jp::isdt and !_jecUnc)) {
        PrintInfo("Issues while loading JEC; aborting...",true);
        return false;
      }
    } // JEC redone
  }

  // Load latest JSON selection
  if (jp::isdt and jp::dojson) {
    if (!LoadJSON()) {
      PrintInfo("Issues loading the JSON file; aborting...",true);
      return false;
    }
  }

  // Load PU profiles for MC reweighing
  if (jp::ismc and jp::reweighPU) {
    if (!LoadPuProfiles()) {
      PrintInfo("Issues loading the PU histograms for reweighting; aborting...",true);
      return false;
    }
  }

  // load luminosity tables (prescales now stored in event)
  if (jp::isdt and jp::dolumi) {
    if (!LoadLumi()) {
      PrintInfo("Issues loading the Lumi file; aborting...",true);
      return false;
    }
  }

  if (jp::ismc) PrintInfo(Form("Running on MC produced with %1.3g nb-1 (%lld evts)",
                          1000. * _ntot / jp::xsecMinBias, _ntot),true);
  if (jp::isdt) PrintInfo(Form("Running on %lld events of data",_ntot),true);

  // Initialize histograms for different epochs and DQM selections
  if (jp::doBasicHistos) {
    InitBasic("Standard");
  }

  if (jp::doEtaHistos) {
    InitEta("FullEta_Reco");
    if (jp::ismc) {
      InitEta("FullEta_Gen");
      if (jp::doEtaHistosMcResponse) {
        InitMC("FullEta_RecoPerGen_vReco");
        InitMC("FullEta_RecoPerGen_vGen");
      }
    }
  }

  if (jp::doMpfHistos) {
    InitAll("AllTrigs");
  }

  if (jp::isdt and jp::doRunHistos) {
    InitRun("Runs",0.,3.);
    InitRun("RunsBarrel",0.,1.);
    InitRun("RunsTransition",1.,2.);
    InitRun("RunsEndcap",2.,3.);
  }

  // For debugging purposes, save the weights used for pileup profiles.
  if (jp::ismc and jp::reweighPU) {
    _outfile->cd();
    _outfile->mkdir("puwgt");
    _outfile->cd("puwgt");
    for (auto &puprof : _pudist) puprof.second->Write();
  }

  if (jp::doVetoHot) {
    string HotTag = "";
    string HotYr = "";
    if (jp::yid==0) {
      HotYr = "16";
      if      (std::regex_search(jp::run,regex("^[BCD]"))) HotTag = "BCD";
      else if (std::regex_search(jp::run,regex("^E")))     HotTag = "EF";
      else if (std::regex_search(jp::run,regex("^Fe")))    HotTag = "EF";
      else if (std::regex_search(jp::run,regex("^Fl")))    HotTag = "GH";
      else if (std::regex_search(jp::run,regex("^[GH]")))  HotTag = "GH";
    } else if (jp::yid==1 or jp::yid==2) {
      HotYr = "17";
      if      (std::regex_search(jp::run,regex("^B"))) HotTag = "B";
      else if (std::regex_search(jp::run,regex("^C"))) HotTag = "C";
      else if (std::regex_search(jp::run,regex("^D"))) HotTag = "D";
      else if (std::regex_search(jp::run,regex("^E"))) HotTag = "E";
      else if (std::regex_search(jp::run,regex("^F"))) HotTag = "F";
    } else if (jp::yid==3) {
      HotYr = "18";
      if      (std::regex_search(jp::run,regex("^A"))) HotTag = "A";
      else if (std::regex_search(jp::run,regex("^B"))) HotTag = "B";
      else if (std::regex_search(jp::run,regex("^C"))) HotTag = "C";
      else if (std::regex_search(jp::run,regex("^D"))) HotTag = "D";
    }
    assert(HotTag!="");
    fHotExcl = new TFile(Form("rootfiles/hotjets-%srun%s.root",HotYr.c_str(),HotTag.c_str()),"READ");
    assert(fHotExcl and !fHotExcl->IsZombie() and Form("file rootfiles/hotjets-run%s.root missing",HotTag.c_str()));
    h2HotExcl = (TH2D*)fHotExcl->Get(Form("h2hot%s",jp::HotType));
    assert(h2HotExcl and "erroneous eta-phi exclusion type");
    PrintInfo(Form("Loading hot zone corrections rootfiles/hotjets-run%s.root with h2hot %s",HotTag.c_str(),jp::HotType));
  }
  // Qgl: load quark/gluon probability histos (Ozlem)
  // 1. open the previous output-MC-1_iteration1.root in the beginning
  // 2. get the hqgl_q and hqgl_g for each eta bin, store in array
  // 3. find correct hqgl_q and hqgl_g from array (normalized)
  // 4. calculate probg = g / (q+g)
  if (jp::doqglfile) {
    TFile *finmc = new TFile(jp::qglfile,"READ");
    assert(finmc && !finmc->IsZombie());

    double veta[] = {0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.2, 4.7};
    const int neta = sizeof(veta)/sizeof(veta[0])-1;
    // same or vpt, vtrigpt, npt
    const int npt = jp::notrigs;
    const double *vtrigpt = &jp::trigthr[0];
    double vpt[npt+1]; vpt[0] = 0;
    for (int trigidx = 0; trigidx != npt; ++trigidx) vpt[trigidx+1] = jp::trigranges[trigidx][1];
    const int nqgl = 101; double vqgl[nqgl+1];
    for (int qglidx = 0; qglidx != nqgl+1; ++qglidx) vqgl[qglidx] = 0. + 0.01*qglidx;

    _h3probg = new TH3D("_h3probg","Gluon prob.;#eta_{jet};p_{T,jet};QGL",
                        //6,0,3.0, 10,0,1000, 101,0,1.01);
                        neta,veta, npt,vpt, nqgl,vqgl); //101,0,1.01);

    // Loop over pt and eta bins in the gql histos
    for (int ieta = 1; ieta != _h3probg->GetNbinsX()+1; ++ieta) {
      for (int ipt = 1; ipt != _h3probg->GetNbinsY()+1; ++ipt) {
        // Gluons in the given eta/pt slice
        string sg = Form("Standard/Eta_%1.1f-%1.1f/jt%1.0f/hqgl_g",
                          veta[ieta-1], veta[ieta], vtrigpt[ipt-1]);
        PrintInfo(sg,true);
        TH1D *hqgl_g = dynamic_cast<TH1D*>(finmc->Get(sg.c_str())); assert(hqgl_g);
        hqgl_g->Scale(1./hqgl_g->Integral());

        // Quarks in the given eta/pt slice
        string sq = Form("Standard/Eta_%1.1f-%1.1f/jt%1.0f/hqgl_q", veta[ieta-1], veta[ieta], vtrigpt[ipt-1]);
        TH1D *hqgl_q = dynamic_cast<TH1D*>(finmc->Get(sq.c_str())); assert(hqgl_q);
        hqgl_q->Scale(1./hqgl_q->Integral());

        // Loop over qgl indices
        for (int iqgl = 1; iqgl != _h3probg->GetNbinsZ()+1; ++iqgl) {
          // Fetch gluon probability
          double probg = hqgl_g->GetBinContent(iqgl) /
          (hqgl_g->GetBinContent(iqgl) + hqgl_q->GetBinContent(iqgl));
          _h3probg->SetBinContent(ieta, ipt, iqgl, probg);
        } // for iqgl
      } // for ipt
    } // for ieta
  }
  PrintInfo("Finished pre-run processing!",true);
  return true;
}


// Routines and selections before histograms are filled
bool HistosFill::AcceptEvent()
{
  if (jp::isdt) // For DT fetch true pileup from the json or histogram info
    trpu = _avgpu[run][lbn];
  else // For MC the info exists in the SMPJ tree
    trpu = EvtHdr__mTrPu;

  if (njt > _maxnjt) {
    PrintInfo(Form("Array overflow: njt = %d > njtmax= %d",njt,_maxnjt),true);
    assert(njt<_maxnjt);
  }

  if (jp::ismc and gen_njt > _maxnjt) {
    PrintInfo(Form("Array overflow: gen_njt = %d > njtmax= %d",gen_njt,_maxnjt),true);
    assert(gen_njt<_maxnjt);
  }

  if (jp::debug) {
    PrintInfo("",true);
    Show(_jentry);
    PrintInfo("",true);
    PrintInfo("",true);

    PrintInfo("***Checking basic event variables are read out:",true);
    PrintInfo(Form("isdata = %d / ismc = %d",jp::isdt,jp::ismc),true);
    PrintInfo(Form("trpu = %f",trpu),true);
    PrintInfo(Form("pthat = %f",pthat),true);
    PrintInfo(Form("weight = %f",weight),true);
    PrintInfo(Form("njt = %d",njt),true);
    PrintInfo(Form("idloose[0] = %d",njt>0 ? jtidloose[0] : -1),true);
    PrintInfo(Form("idtight[0] = %d",njt>0 ? jtidtight[0] : -1),true);
    PrintInfo("***end basic event variables");
    PrintInfo("",true);
  }

  // Check if duplicate
  if (jp::isdt and jp::checkduplicates) {
    set<int>& events = _duplicates[run][lbn];
    if (events.find(evt)!=events.end()) {
      ++_nbadevts_dup;
      return false;
    }
    events.insert(evt);
  }
  ++_cnt["01all"];

  // Check if good run/LS, including JSON selection
  if (jp::isdt) {
    if (jp::dojson) {
      // Does the run/LS pass the latest JSON selection?
      if (_json[run][lbn]==0) {
        _badjson.insert(pair<int, int>(run, lbn));
        ++_nbadevts_json;
        return false;
      }
    } // jp::dojson

    if (jp::dolumi) {
      // Do we have the run listed in the .csv file?
      auto irun = _lums.find(run);
      if (irun==_lums.end()) {
        _badruns.insert(run);
        ++_nbadevts_run;
        return false;
      }

      // Do we have the LS listed in the .csv file?
      auto ils = irun->second.find(lbn);
      if (ils==irun->second.end()) {
        _badlums.insert(pair<int, int>(run,lbn));
        ++_nbadevts_ls;
        return false;
      }

      // Does the .csv file list a non-zero luminosity?
      if (ils->second==0) {
        _nolums.insert(pair<int, int>(run, lbn));
        ++_nbadevts_lum;
        //continue; // Could be Poisson fluctuation to zero
      }
    } // jp::dolumi
  } // jp::isdt

  // Keep track of LBNs
  _passedlumis.insert(pair<int, int>(run, lbn));

  ++_cnt["02ls"];

  // Reset event ID
  _pass = true;


  if (jp::fetchMETFilters) {
    if (FilterDecision_.size()==0) ++_cnt["03METFlt"];
    else {
      // If we perform MET filtering, any filter firing will cause the event to be discarded.
      if (jp::doMETFiltering) return false;
      _pass = false;

      // If we don't perform MET filtering, we can do something more elaborate with the filter info
      for (unsigned FDec : FilterDecision_) {
        assert(FDec<_availFlts.size());
        auto &FName = _availFlts[FDec];

        // TODO: Do something interesting?

        if (jp::debug) PrintInfo(Form("%s %u",FName.c_str(),FDec),true);
      } // for FDec (_FilterDecision)
    }
  } else {
    // This is equal to the SMPJ GoodVertexFilter and is redundant with MET filters on
    pvrho = tools::oplus(pvx, pvy);
    _pass = _pass and pvndof > 4 and fabs(pvz) <= 24. and pvrho <= 2. and npvgood>0.;
    if (_pass) ++_cnt["03vtx"];
  }

  // Event cuts against beam backgrounds
  if (_pass) {
    if (tools::oplus(pvx-bsx, pvy-bsy)>0.15) {
      ++_bscounter_bad;
      _pass = false;
    } else {
      ++_bscounter_good;
      ++_cnt["04bsc"];
    }
  }

  // load correct IOV for JEC
  if (jp::redojes and jp::isdt and jp::useIOV) {
    bool setcorrection = _iov.setJEC(&_JEC,&_L1RC,&_jecUnc,run);
    if (!setcorrection or !_JEC or !_L1RC or !_jecUnc) {
      PrintInfo("Issues while loading JEC; aborting...",true);
      assert(false);
    }
  }

  if (jp::debug) PrintInfo("JEC and MET calculation and leading jets info!",true);
  // Calculate jec.
  // Recalculate new met1 from the old met1 (met)
  // Attempt to calculate met2.
  // Estimate raw hadronic recoil from Raw chs met (here, type0).
  metsumet1 = metsumet;
  double mex = met*cos(metphi);
  double mey = met*sin(metphi);
  double mex_nol2l3 = mex;
  double mey_nol2l3 = mey;
  double htx = -met0*cos(metphi0);
  double hty = -met0*sin(metphi0);
  double htx0 = 0;
  double hty0 = 0;
  double ucx = mex;
  double ucy = mey;
  // Find leading jets (residual JEC may change ordering)
  // CAUTION: for less than 3 jets, we input -1 on the place of the index
  // NOTE: Above this we only use the initial corrections. This serves as a good approximation,
  // as the values before reapplying JEC are more like the ones we knew when collecting data.
  for (int i = 0; i<3; ++i) jt3leads[i] = -1;
  ///////////////
  // Jet loop
  ///////////////
  for (int jetidx = 0; jetidx != njt; ++jetidx) {
    p4.SetPxPyPzE(jtp4x[jetidx],jtp4y[jetidx],jtp4z[jetidx],jtp4t[jetidx]);
    // Divide by the original JES
    if (jp::debug) PrintInfo("Entering jet loop!",true);
    double jtptold = p4.Pt();
    if (jp::undojes) p4 *= 1/jtjes[jetidx];

    jtptu[jetidx] = p4.Pt();
    jteu[jetidx] = p4.E();

    double jec_res = 1;
    if (jp::redojes) {
      if (jp::debug) PrintInfo("Recalculating JEC!",true);
      // Recalculate JEC
      _JEC->setRho(rho);
      _JEC->setNPV(npvgood);
      _JEC->setJetA(jta[jetidx]);
      _JEC->setJetPt(jtptu[jetidx]);
      _JEC->setJetE(jteu[jetidx]);
      _JEC->setJetEta(p4.Eta());
      vector<float> v = _JEC->getSubCorrections();
      jtjesnew[jetidx] = v.back();

      if (jp::ismc or jp::skipl2l3res) {
        assert(v.size()==3);
      } else {
        assert(v.size()==4);
        jec_res = v[3]/v[2];
      }
      double jec_l1 = v[0];
      double jec_l2l3 = v[2]/v[0];
      jtjes_l1[jetidx] = jec_l1;
      jtjes_l2l3[jetidx] = jec_l2l3;
      jtjes_res[jetidx] = jec_res;
    } else {
      jtjesnew[jetidx] = 1.;
      jtjes_l1[jetidx] = 1.;
      jtjes_l2l3[jetidx] = 1.;
      jtjes_res[jetidx] = 1.;
    }

    // Calculate gen level info
    if (jp::ismc) {
      if (jp::debug) PrintInfo("Gen info!",true);
#ifdef NEWMODE
      auto &gjetidx = jtgenidx[jetidx];
      if (gjetidx>=0 and gjetidx<GenJets__)
        gp4.SetPxPyPzE(gen_jtp4x[gjetidx],gen_jtp4y[gjetidx],gen_jtp4z[gjetidx],gen_jtp4t[gjetidx]);
      else
        gp4.SetPxPyPzE(0.,0.,0.,0.);
#else
      gp4.SetPxPyPzE(jtgenp4x[jetidx],jtgenp4y[jetidx],jtgenp4z[jetidx],jtgenp4t[jetidx]);
#endif
      jtgenpt[jetidx] = gp4.Pt();
      jtgeny[jetidx] = gp4.Rapidity();
      jtgeneta[jetidx] = gp4.Eta();
      jtgenphi[jetidx] = gp4.Phi();
    }

    if (jp::debug) PrintInfo("Reapplying JEC!",true);
    if (jp::redojes) p4 *= jtjesnew[jetidx];

#if JETRESO == 1
    // For DATA, we have redundant scale factor files, which return a "1" for SF.
    // We save the processing time by skipping this.
    if (jp::ismc) {
      double genpt = gp4.Pt();
      // We enforce the gen match, see https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetResolution
      if (genpt>0) {
        double genDR = p4.DeltaR(gp4);
        // We require the match to be closer than half cone, otherwise gen Pt is worthless
        if (genDR>0.2) genpt = -4;
      }
      p4 *= _iov.getJERCF(p4.Pt(),p4.Eta(),p4.E(),rho,genpt);
    }
#endif

    jte[jetidx] = p4.E();
    jtpt[jetidx] = p4.Pt();
    jteta[jetidx] = p4.Eta();
    jtphi[jetidx] = p4.Phi();
    jty[jetidx] = p4.Rapidity();
    if (jp::doMpfHistos) jtpt_nol2l3[jetidx] = p4.Pt()/jec_res;

    if (jp::debug) PrintInfo(Form("Jet %d corrected!",jetidx),true);

    // Only use jets with corr. pT>recopt GeV to equalize data and MC thresholds
    if (fabs(jteta[jetidx])<4.7) {
      if (jtptold > jp::recopt) {
        // oldish: double dpt = - jtpt[jetidx] + l1corr*jtptu[jetidx]; // old: + (l1chs - l1pf + l1corr)*jtptu[jetidx];
        // Instead of the old routines, we perform a simple shift for the T1 MET
        double dpt = -jtpt[jetidx] + jtptold;
        mex += dpt * cos(jtphi[jetidx]);
        mey += dpt * sin(jtphi[jetidx]);
        metsumet1 += jtpt[jetidx] - jtptold;
      }
      if (jtpt[jetidx] > jp::recopt) {
        // MET 1: the one where JEC is applied. MET1 needs to be recalculated as JEC changes.
        // Subtract uncorrected jet pT from met, put back corrected & add L1RC offset to keep PU isotropic.
        double l1corr = 1.;
        if (jp::redojes) {
          _L1RC->setRho(rho);
          _L1RC->setJetA(jta[jetidx]);
          _L1RC->setJetPt(jtptu[jetidx]);
          _L1RC->setJetE(jteu[jetidx]);
          _L1RC->setJetEta(jteta[jetidx]);
          l1corr = _L1RC->getCorrection();
        } else {
          l1corr = jtjes[jetidx];
        }

        if (jp::doMpfHistos) {
          double dpt_nol2l3 = - jtpt_nol2l3[jetidx] + l1corr*jtptu[jetidx];
          mex_nol2l3 += dpt_nol2l3 * cos(jtphi[jetidx]);
          mey_nol2l3 += dpt_nol2l3 * sin(jtphi[jetidx]);
        }

        // MET 2: record unclustered energy (more or less deprecated).
        // Keep track of remaining pT in unclustered energy, add to MET l1corr jets, from which ue is substracted.
        // Effectively this means substracting jets (without their PU and UE) from MET (=> homogeneous background).
        double ue = 1.068 * jta[jetidx]; // CAUTION: One should check this magical coefficient is good.
        double dptu = -ue + l1corr*jtptu[jetidx];
        ucx += dptu * cos(jtphi[jetidx]);
        ucy += dptu * sin(jtphi[jetidx]);
      }
    }
    double jtux = jtptu[jetidx]*cos(jtphi[jetidx]);
    double jtuy = jtptu[jetidx]*sin(jtphi[jetidx]);
    if (fabs(jteta[jetidx])<4.7 and jtpt[jetidx] > jp::recopt) {
      // Collect the raw hadronic recoil (on the level of jets that we see).
      htx -= jtux;
      hty -= jtuy;
    } else {
      // Check the potential to improve the recoil.
      htx0 -= jtux;
      hty0 -= jtuy;
    }

    if (jt3leads[0]==-1 or jtpt[jt3leads[0]]<jtpt[jetidx]) {
      jt3leads[2] = jt3leads[1];
      jt3leads[1] = jt3leads[0];
      jt3leads[0] = jetidx;
    } else if (jt3leads[1]==-1 or jtpt[jt3leads[1]]<jtpt[jetidx]) {
      jt3leads[2] = jt3leads[1];
      jt3leads[1] = jetidx;
    } else if (jt3leads[2]==-1 or jtpt[jt3leads[2]]<jtpt[jetidx]) {
      jt3leads[2] = jetidx;
    }
  } // for jetidx

  // Type I MET (this is the best one we've got).
  // Works optimally if we keep T0Txy MET as the raw MET and apply here the newest JEC)
  met1 = tools::oplus(mex,mey);
  metphi1 = atan2(mey,mex);
  if (jp::doMpfHistos) {
    met1_nol2l3 = tools::oplus(mex_nol2l3,mey_nol2l3);
    metphi1_nol2l3 = atan2(mey_nol2l3, mex_nol2l3);
  }
  mht = tools::oplus(htx,hty);
  mhtphi = atan2(hty,htx);

  // Correct unclustered energy; jec for 10 GeV jets varies between 1.1-1.22 at |y|<2.5,
  // 2.5-3.0 even goes up to 1.35 => assume 1.15  => try 1.5 => to 1.25 (high pT threshold on jets)
  mex += 0.25*ucx;
  mey += 0.25*ucy;
  // Type II MET witch C = 1.25 (This is not recommended for pfJets).
  // met2 = met1 - C*uncl.ptsum = met1 + 0.25*(l1corr*jtptu[jetidx]-ue[jetidx]) = 1.25*metraw + 1.25*l1corr*jtptu[jetidx] - jtpt[jetidx] - 0.25ue[jetidx]
  met2 = tools::oplus(mex, mey);
  metphi2 = atan2(mey, mex);

  // The leading indices
  int i0 = jt3leads[0];
  int i1 = jt3leads[1];
  int i2 = jt3leads[2];

  // Zero jets not sensible
  _pass = _pass and i0>=0;
  if (_pass) ++_cnt["05njt"];

  if (jp::debug) PrintInfo(Form("Indices for the three leading jets: %d %d %d\nGen flav calculation!",i0,i1,i2),true);

  // Check rho
  if (_pass) {
    if (rho>500.) {
    //if (rho>40.) { // This was for run 1 settings
      ++_rhocounter_bad;
      _pass = false;
      if (jp::debug) PrintInfo(Form("\nrun:ev:ls %d:%d:%lld : rho=%1.1f njt=%d npv=%d jtpt0=%1.1f sumet=%1.1f met=%1.1f\n",run, lbn, evt, rho, njt, npv, (njt>0 ? jtpt[i0] :0.),metsumet,met),true);
    } else {
      ++_rhocounter_good;
      ++_cnt["06rho"];
    }
  }

  // Reset prescales (dynamic can change within run)
  for (auto &scaleit : _prescales)
    scaleit.second[run] = 0;

  // Fill trigger information
  _trigs.clear();

  // Simulate other triggers for MC, if so wished
  // (this is slow, though)
  if (jp::ismc) {
    // Always insert the generic mc trigger
    if (jp::debug) PrintInfo("Entering PU weight calculation!",true);
#ifdef NEWMODE
      // The PU gen jets are not saved => impossible to do this in SingleNeutrino
    if (_pass and (jtgenidx[i0]!=-1 or jp::isnu)) ++_cnt["07mcgenjet"];
    else return false;
#endif
    // In SingleNeutrino, there is one extra PV. See also LoadPuProfiles
    double PUVal = trpu;
    if (jp::domctrigsim and njt>0) {
      // Only add the greatest trigger present
      // Calculate trigger PU weight
      bool found = false;
      bool wcond = false;
      for (unsigned itrg = 0; itrg < jp::triggers.size(); ++itrg) {
        double wtrue = 1.0;
        // We fire all the triggers up to the unfeasible turn-on point
        if (jtpt[i0]>jp::trigranges.at(itrg).at(0)) {
          string trg_name = jp::triggers.at(itrg);
          _trigs.insert(trg_name);
          _wt[trg_name] = 1.;

          // Reweight in-time pile-up
          if (jp::reweighPU) {
            int k = _pudist[trg_name]->FindBin(PUVal);
            wtrue = _pudist[trg_name]->GetBinContent(k);
            _wt[trg_name] *= wtrue;
            wcond |= wtrue!=0;
          }
          found = true;
        } else {
          break;
        }
      }
      _pass = _pass and found;
      if (_pass) ++_cnt["07mctrg"];
      else return false; // Leading jet is weak

      // check for non-zero PU weight
      _pass = _pass and wcond;
      if (_pass) ++_cnt["07puw"];
      else return false; // Bad pu areas with zero weight excluded
    } // jp::domctrigsim
    _trigs.insert("mc");
    _wt["mc"] = 1.0;
    if (jp::reweighPU) {
      int k = _pudist[jp::reftrig]->FindBin(PUVal);
      _wt["mc"] *= _pudist[jp::reftrig]->GetBinContent(k);
    }
  } else if (jp::isdt) {
    // For data, check trigger bits
    if (jp::debug) {
      PrintInfo(Form("TriggerDecision_.size()==%zu",TriggerDecision_.size()));
      PrintInfo(Form("_availTrigs.size()==%zu",_availTrigs.size()));
      PrintInfo(Form("_goodTrigs.size()==%zu",_goodTrigs.size()));
    }
    #ifndef NEWMODE
    assert(TriggerDecision_.size()==_availTrigs.size());
    #endif

    // New and old mode: TriggerDecision and L1/HLT Prescales have the same indexing.
    for (auto itrg = 0u; itrg != jp::notrigs; ++itrg) _wt[jp::triggers[itrg]] = 1.0;

    #ifdef NEWMODE
    for (unsigned itrg = 0; itrg<TriggerDecision_.size(); ++itrg) {
      unsigned TDec = TriggerDecision_[itrg]; // Location of the current place
      assert(TDec<_availTrigs.size());
      auto &TName = _availTrigs[TDec];

      auto trgPlace = std::find(_goodTrigs.begin(),_goodTrigs.end(),TDec);
      if (trgPlace==_goodTrigs.end()) continue;
      unsigned goodIdx = static_cast<unsigned int>(trgPlace-_goodTrigs.begin());
    #else
    for (auto goodIdx = 0u; goodIdx < _goodTrigs.size(); ++goodIdx) {
      auto &itrg = _goodTrigs[goodIdx];
      auto &TDec = TriggerDecision_[itrg]; // Trigger fired or not: -1, 0, 1
      if (TDec!=1) continue;
      assert(itrg<_availTrigs.size());
      auto &TName = _availTrigs[itrg];
    #endif // NEWMODE

      #ifdef NEWMODE
      if (jp::debug)
      #else
      if (jp::debug and TDec>0)
      #endif
      PrintInfo(Form("%s %d %d %d %d",TName.c_str(),itrg,TDec,L1Prescale_[itrg],HLTPrescale_[itrg]),true);

      // Set prescale from event for now
      //if (L1Prescale_[itrg]>0 and HLTPrescale_[itrg]>0) { There's trouble in 2017 L1, so we let it pass
      if (HLTPrescale_[itrg]>0 or L1Prescale_[itrg]>0) {
        double l1 = L1Prescale_[itrg];
        double hlt = HLTPrescale_[itrg];
        if (l1==0) l1 = 1;
        if (hlt==0) hlt = 1;
        _prescales[TName][run] = l1 * hlt;
      } else {
        PrintInfo(Form("Error for trigger %s prescales: L1 = %d HLT = %d",TName.c_str(),L1Prescale_[itrg],HLTPrescale_[itrg]));
        _prescales[TName][run] = 0;
        if (jp::debug) { // check prescale
          double prescale = _prescales[TName][run];
          if (L1Prescale_[itrg]*HLTPrescale_[itrg]!=prescale) {
            PrintInfo(Form("Trigger %s, Prescale (txt file) = %f",TName.c_str(),prescale),true);
            PrintInfo(Form("L1 = %d, HLT = %d",L1Prescale_[itrg],HLTPrescale_[itrg]),true);
            assert(false);
          }
        } // debug
      }

      if (_prescales[TName][run]>0) {
        // Set trigger only if prescale information is known
        _trigs.insert(TName);
        _wt[TName] = 1.0;
      } else {
        // Make sure all info is good! This is crucial if there is something odd with the tuples
        PrintInfo(Form("Missing prescale for %s in run %d",TName.c_str(),run),true);
      }
    } // for itrg (FilterDecision or _goodTrigs)
  } // if isdt

  ++_totcounter;
  if (_pass) ++_evtcounter;
  _pass = _pass and _trigs.size()>0;
  if (_pass) {
    ++_trgcounter;
    if (jp::isdt) ++_cnt["08trg"];
  }

  // Retrieve event weight. _w0 is static, _w is chanching with the trigger
  _w0 = 1.0;
  if (jp::ismc) {
    _w0 *= weight;
    if (jp::pthatbins or jp::htbins) _w0 *= _binnedmcweight;
    assert(_w0>0);
  }
  _w = _w0;

  // TODO: implement reweighing for k-factor (NLO*NP/LOMC)
  if (jp::ismc) {
    ///////////////
    // Gen Jet loop
    ///////////////
    // For SingleNeutrino and the HT binned MG samples, emulate PtHat using a jet ht sum.
    bool doht = jp::isnu or (jp::htbins and !jp::pthatbins);
    double htsum = 0.0;
    for (int gjetidx = 0; gjetidx != gen_njt; ++gjetidx) {
      genp4.SetPxPyPzE(gen_jtp4x[gjetidx],gen_jtp4y[gjetidx],gen_jtp4z[gjetidx],gen_jtp4t[gjetidx]);
      gen_jtpt[gjetidx] = genp4.Pt();
      gen_jteta[gjetidx] = genp4.Eta(); // for matching
      gen_jtphi[gjetidx] = genp4.Phi(); // for matching
      gen_jty[gjetidx] = genp4.Rapidity();
      if (doht) htsum += genp4.Pt();

      // Ozlem: loop for finding partonflavor by matching genjets and jets
      int ireco = -1;
      for (int j = 0; j != njt && ireco == -1; ++j) {
        p4.SetPxPyPzE(jtp4x[j],jtp4y[j],jtp4z[j],jtp4t[j]);
        if (genp4.DeltaR(p4) < 0.4)
          ireco = j;
      } // for j
      if (ireco!=-1)
#ifdef NEWMODE
        gen_partonflavor[gjetidx] = Int_t(partonflavorphys[ireco]+0.25); //qglokan
#else
        gen_partonflavor[gjetidx] = Int_t(partonflavor[ireco]+0.25); //qglokan
#endif
      else
        gen_partonflavor[gjetidx] = -1;
    } // for gjetidx
    for (int jetidx = 0; jetidx != njt; ++jetidx) htsum += jtpt[jetidx];
    if (doht) pthat = htsum/2.0;

    // Check if overweight PU event
    if (_pass) {
      // The PU gen jets are not saved => impossible to do this in SingleNeutrino
      if (jtpt[i0] < 1.5*jtgenpt[i0] or jp::isnu) ++_cnt["09ptgenlim"];
      else _pass = false;

      if (_pass) {
        if (doht) {
          if (jtpt[i0] < 2.0*pthat) ++_cnt["10htlim"];
          else _pass = false;
        } else {
          double lim = (pthat < 100) ? 2.0 : 1.5;
          if (jtpt[i0] < lim*pthat) ++_cnt["10pthatlim"];
          else _pass = false;
        }
      }
    }
  } // MC

  _jetids.resize(njt);
  for (unsigned int jetid = 0; jetid != _jetids.size(); ++jetid) _jetids[jetid] = true;
  FillJetID(_jetids);

  if (_pass and _jetids[i0]) ++_cnt["11jtid"]; // Non-restrictive
#ifdef NEWMODE
  // Equipped in FillBasic and FillRun
  _pass_qcdmet = met1 < 45. or met1 < 0.3 * metsumet1; // updated 4/2018
#else
  _pass_qcdmet = met < 45. or met < 0.3 * metsumet;
#endif

  return true;
}


// Report event stuff
void HistosFill::Report()
{
  // List bad runs
  PrintInfo(Form("Processed %d events in total",_totcounter));
  PrintInfo(Form("Processed %d events passing basic data quality and trigger cuts",_trgcounter));
  PrintInfo(Form("(out of %d passing data quality cuts)",_evtcounter));
  if (jp::dojson)
    PrintInfo(Form("Found %d bad events according to new JSON (events cut)",_nbadevts_json));
  if (jp::dolumi) {
    PrintInfo(Form("Found %d bad events according to lumi file.",_nbadevts_run));
    if (_badruns.size()>0) {
      *ferr << "The found " << _badruns.size() << " bad runs:";
      for (auto &runit : _badruns)
        *ferr << " " << runit;
      *ferr << endl;
    }
    PrintInfo(Form("Found %lu bad LS and %lu non-normalizable LS in good runs",_badlums.size(),_nolums.size()));
    PrintInfo(Form("These contained %d discarded events in bad LS and %d in non-normalizable LS",_nbadevts_ls,_nbadevts_lum));
    *ferr << endl;
  }
  if (jp::checkduplicates)
    PrintInfo(Form("Found %d duplicate events, which were properly discarded",_nbadevts_dup));

  // Report beam spot cut efficiency
  PrintInfo(Form("Beam spot counter discarded %d events out of %d (%f %%)\nBeam spot expectation is less than 0.5%%",
                 _bscounter_bad,_bscounter_good,double(_bscounter_bad)/double(_bscounter_good)*100.));

  // Report rho veto efficiency
  PrintInfo(Form("Rho<40 veto counter discarded %d events out of %d (%f %%)\nRho veto expectation is less than 1 ppm",
                _rhocounter_bad,_rhocounter_good,double(_rhocounter_bad)/double(_rhocounter_good)*100.));
  for (auto &cit : _cnt)
    PrintInfo(Form("%s: %d (%1.1f%%)",cit.first.c_str(),cit.second,100.*cit.second/max(1,_cnt["01all"])),true);

  if (jp::isdt) PrintInfo("Note that for DT it is likely that we lose a large percentage of events for the trigger.\n"
                          "Events triggered by JetHT and AK8PFJet are included in addition to AK4PFJet.");
  *ferr << endl;

  PrintInfo("Reporting lumis not discarded in reports/passedlumis.json",true);
  ofstream fout(Form("reports/passedlumis-%s.json",jp::type), ios::out);
  for (auto &lumit : _passedlumis) fout << lumit.first << " " << lumit.second << endl;

  if (jp::dojson and _badjson.size()>0) {
    PrintInfo("Reporting lumis discarded by json selection (jp::dojson) in reports/badlumis_json",true);
    ofstream fout2(Form("reports/badlumis_json-%s.json",jp::type), ios::out);
    for (auto &jsit : _badjson) fout2 << jsit.first << " " << jsit.second << endl;
  } // jp::dojson

  if (jp::dolumi) {
    if (_badlums.size()>0) {
      PrintInfo("Reporting lumis discarded by lumifile selection (jp::dolumi) in reports/badlumis_lumi.json",true);
      ofstream fout2(Form("reports/badlumis_lumi-%s.json",jp::type), ios::out);
      for (auto &jsit : _nolums) fout2 << jsit.first << " " << jsit.second << endl;
    } // _badlums
    if (_nolums.size()>0) {
      PrintInfo("Reporting lumis discarded fby lumifile selection (jp::dolumi) @ zero luminosity in reports/badlumis_zerolumi.json",true);
      ofstream fout2(Form("reports/badlumis_zerolumi-%s.json",jp::type), ios::out);
      for (auto &jsit : _nolums) fout2 << jsit.first << " " << jsit.second << endl;
    } // _nolums
  } // jp::dolumi
}


// Initialize basic histograms for trigger and eta bins
void HistosFill::InitBasic(string name)
{
  // Report memory usage to avoid malloc problems when writing file
  PrintInfo(Form("InitBasic(%s):",name.c_str()));
  PrintMemInfo();

  TDirectory *curdir = gDirectory;

  // open file for output
  TFile *f = _outfile;
  assert(f && !f->IsZombie());
  f->mkdir(name.c_str());
  bool enteroutdir = f->cd(name.c_str());
  assert(enteroutdir);
  //TDirectory *topdir = gDirectory;
  TDirectory *topdir = f->GetDirectory(name.c_str()); assert(topdir);
  topdir->cd();

  // Pseudorapidity bins + HF + barrel
  double etas[] = {0., 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.2, 4.7, 0., 1.3};
  const int netas = sizeof(etas)/sizeof(etas[0])-1;

  // define triggers
  vector<string> triggers;
  // define efficient pT ranges for triggers for control plots
  map<string, pair<double, double> > pt;
  // define pT values for triggers
  map<string, double> pttrg;
  if (jp::ismc) {
    triggers.push_back("mc");
    pt["mc"] = pair<double, double>(jp::recopt, jp::emax);
    pttrg["mc"] = jp::recopt;
  }
  if (jp::isdt or jp::domctrigsim) {
    // This is done both for data and MC, because why not?
    for (unsigned itrg = 0; itrg != jp::notrigs; ++itrg) {
      string trg = jp::triggers[itrg];
      triggers.push_back(trg);
      double pt1 = jp::trigranges[itrg][0];
      double pt2 = jp::trigranges[itrg][1];
      pt[trg] = pair<double, double>(pt1, pt2);
      double pt0 = jp::trigthr[itrg];
      pttrg[trg] = pt0;
    }
  }

  // Loop over pseudorapidity, trigger bins
  for (int etaidx = 0; etaidx != netas; ++etaidx) {
    if (etas[etaidx+1] > etas[etaidx]) { // create real bins only
      // subdirectory for rapidity bin
      const char *etaname = Form("Eta_%1.1f-%1.1f", etas[etaidx], etas[etaidx+1]);
      assert(topdir);
      //assert(topdir->mkdir(yname));
      topdir->mkdir(etaname);
      bool enteretadir = topdir->cd(etaname);
      assert(enteretadir);
      //TDirectory *ydir = gDirectory;
      TDirectory *etadir = topdir->GetDirectory(etaname); assert(etadir);
      etadir->cd();

      for (unsigned int j = 0; j != triggers.size(); ++j) {
        // subdirectory for trigger
        const char *trg = triggers[j].c_str();
        assert(etadir);
        //assert(ydir->mkdir(trg));
        etadir->mkdir(trg);
        bool entertrgdir = etadir->cd(trg);
        assert(entertrgdir);
        //TDirectory *dir = gDirectory;
        TDirectory *dir = etadir->GetDirectory(trg); assert(dir);
        dir->cd();

        // Initialize and store
        assert(dir);
        HistosBasic *h = new HistosBasic(dir, trg, etas[etaidx], etas[etaidx+1], pttrg[trg],
                                         pt[trg].first, pt[trg].second, triggers[j]=="mc");
        _histos[name].push_back(h);
      } // for j
    } // real bin
  } // for etaidx

  curdir->cd();

  // Report memory usage to avoid malloc problems when writing file
  PrintInfo(Form("InitBasic(%s) finished:",name.c_str()));
  PrintMemInfo();
} // initBasic


// Loop over basic histogram containers to fill all
void HistosFill::FillBasic(string name)
{
  for (unsigned int histidx = 0; histidx != _histos[name].size(); ++histidx)
    FillSingleBasic(_histos[name][histidx]);
}


// Fill basic histograms after applying pt, y cuts
void HistosFill::FillSingleBasic(HistosBasic *h)
{
  assert(h);
  h->hpttmp->Reset();
  h->hpt_tmp->Reset();
  if (h->ismcdir) {
    assert(h->hpt_g0_tmp);
    h->hpt_g0_tmp->Reset();
  }

  _w = _w0 * _wt[h->trigname];
  if (_w <= 0) return;

  bool fired = (_trigs.find(h->trigname)!=_trigs.end());
  if (!fired) return;

  // Luminosity information
  if (jp::isdt and h->lums[run][lbn]==0) {
    double lum = _lums[run][lbn];
    double lum2 = _lums2[run][lbn];
    double prescale(0);
    auto ip = _prescales[h->trigname].find(run);
    if (ip==_prescales[h->trigname].end()) {
      PrintInfo(Form("No prescale info for trigger %s in run %d!",h->trigname.c_str(),run));
      assert(false);
    } else {
      prescale = ip->second;
      if (prescale==0) {
        PrintInfo(Form("Prescale zero for trigger %s in run %d!",h->trigname.c_str(),run));
        prescale = 1.;
        assert(false);
      }
    }

    h->lumsum += lum / prescale;
    h->lumsum2 += lum2 / prescale;
    h->lums[run][lbn] = 1;

    h->hlumi_vstrpu->Fill(trpu, lum / prescale);
  }

  // For MC vs truePU
  if (jp::ismc)
    h->hlumi_vstrpu->Fill(trpu, _w);

  if (jp::debug) {
    if (h == _histos.begin()->second[0]) {
      PrintInfo(Form("Triggers size: %lu",_trigs.size()),true);
      for (auto trgit = _trigs.begin(); trgit != _trigs.end(); ++trgit) cout << *trgit << ", ";
      PrintInfo(Form("Current: (%s)",h->trigname.c_str()),true);
    }
  }

  // Are all of our conditions met?
  if (!_pass) return;

  if (jp::debug) PrintInfo(Form("Subdirectory Eta_%1.1f-%1.1f/%s",h->etamin,h->etamax,h->trigname.c_str()),true);
  if (jp::debug) PrintInfo("Calculate and fill dijet mass",true);

  if (h->ismcdir) {
    h->hpthat->Fill(pthat, _w);
    h->hpthatnlo->Fill(pthat);
  }

  //{ Pre-calculate some nice garden tools
  int i0 = jt3leads[0];
  int i1 = jt3leads[1];
  int i2 = jt3leads[2];
  if (i0 < 0.) return; // This should not happen, but check just in case

  double ptave = (i1>=0 ? 0.5 * (jtpt[i0] + jtpt[i1]) : jtpt[i0]);
  double dphi = (i1>=0 ? DPhi(jtphi[i0], jtphi[i1]) : 0.);
  double dpt = (i1>=0 ? fabs(jtpt[i0]-jtpt[i1])/(2*ptave) : 0.999);
  // If the jetID is bad for the third jet (and the third jet is visible), we set pt3 to ptave (alpha = 1)
  double pt3 = ((i1>=0 and i2>=0 and jtpt[i2]>jp::recopt) ? (_jetids[i2] ? jtpt[i2] : ptave) : 0.);
  double alpha = pt3/ptave;
  //} Garden tools

  if (_pass_qcdmet and i0>=0 and _jetids[i0] and jtpt[i0]>jp::recopt) { // First leading jet
    if (i1>=0 and _jetids[i1] and jtpt[i1]>jp::recopt) { // Second leading jet
      //{ Calculate and fill dijet mass.
      _j1.SetPtEtaPhiE(jtpt[i0],jteta[i0],jtphi[i0],jte[i0]);
      _j2.SetPtEtaPhiE(jtpt[i1],jteta[i1],jtphi[i1],jte[i1]);
      double djmass = (_j1+_j2).M();
      double etamaxdj = max(fabs(jteta[i0]),fabs(jteta[i1]));
      bool goodjets = (jtpt[i0]>30. and jtpt[i1]>30.);
      // The eta sectors are filled according to max eta
      if (goodjets and etamaxdj >= h->etamin and etamaxdj < h->etamax) {
        assert(h->hdjmass); h->hdjmass->Fill(djmass, _w);
        assert(h->hdjmass0); h->hdjmass0->Fill(djmass, _w);
        assert(h->pdjmass_ptratio); h->pdjmass_ptratio->Fill(djmass, _j1.Pt()/_j2.Pt(), _w);
        assert(h->pdjmass0_ptratio); h->pdjmass0_ptratio->Fill(djmass, _j1.Pt()/_j2.Pt(), _w);
        if (dphi > 2.7) { // Back-to-back condition
          if (alpha<0.1) { assert(h->hdjmass_a01); h->hdjmass_a01->Fill(djmass, _w); }
          if (alpha<0.2) { assert(h->hdjmass_a02); h->hdjmass_a02->Fill(djmass, _w); }
          if (alpha<0.3) { assert(h->hdjmass_a03); h->hdjmass_a03->Fill(djmass, _w); }
        }
      }
      //} Dijet mass
      //{ Calculate and fill jet mass.
      assert(h->hjmass);  h->hjmass ->Fill(_j1.M(),weight);  h->hjmass->Fill(_j2.M(),weight);
      assert(h->hjmass0); h->hjmass0->Fill(_j1.M(),weight); h->hjmass0->Fill(_j2.M(),weight);
      if (dphi > 2.7) { // Back-to-back condition
        if (alpha<0.1) { assert(h->hjmass_a01); h->hjmass_a01->Fill(_j1.M(),weight); h->hjmass_a01->Fill(_j2.M(),weight); }
        if (alpha<0.2) { assert(h->hjmass_a02); h->hjmass_a02->Fill(_j1.M(),weight); h->hjmass_a02->Fill(_j2.M(),weight); }
        if (alpha<0.3) { assert(h->hjmass_a03); h->hjmass_a03->Fill(_j1.M(),weight); h->hjmass_a03->Fill(_j2.M(),weight); }
      }
      //}

      //{ Tag & probe hoods: Tag in barrel and fires trigger, probe in eta bin unbiased
      if (jp::debug) PrintInfo("Calculate and fill dijet balance",true);

      if (dphi > 2.7) { // Back-to-back condition
        for (auto itag_lead = 0u; itag_lead<2u; ++itag_lead) { // Look for both t&p combos for the leading jets
          int itag = jt3leads[itag_lead];
          int iprobe = jt3leads[(itag_lead==0 ? 1 : 0)];
          double etatag = jteta[itag];
          double etaprobe = jteta[iprobe];

          // Eta sel: tag in barrel, probe in current slice
          if (fabs(etatag) < 1.3 and fabs(etaprobe) >= h->etamin and fabs(etaprobe) < h->etamax) { // Eta sel.
            double pttag = jtpt[itag];
            double ptprobe = jtpt[iprobe];
            double phiprobe = jtphi[iprobe];

            //{ Dijet balance histograms
            if (jp::do3dHistos) {
              double asymm = (ptprobe - pttag)/(2*ptave);
              double asymmtp = (ptprobe - pttag)/(2*pttag);
              double mpf = met1*cos(DPhi(metphi1,jtphi[itag]))/2;
              double mpftp = mpf/pttag;
              mpf /= ptave;

              // Asymmetry and mpf
              assert(h->hdjasymm);   h->hdjasymm->Fill(ptave, alpha, asymm, _w);
              assert(h->hdjasymmtp); h->hdjasymmtp->Fill(pttag, alpha, asymmtp, _w);
              assert(h->hdjmpf);     h->hdjmpf->Fill(ptave, alpha, mpf, _w);
              assert(h->hdjmpftp);   h->hdjmpftp->Fill(pttag, alpha, mpftp, _w);
            }
            assert(h->ppttagptprobe_noa); h->ppttagptprobe_noa->Fill(pttag,ptprobe, _w);
            assert(h->h2pttagptprobe_noa); h->h2pttagptprobe_noa->Fill(pttag,ptprobe, _w);
            assert(h->ppttagmu_noa); h->ppttagmu_noa->Fill(pttag,trpu, _w);
            assert(h->h2pttagmu_noa); h->h2pttagmu_noa->Fill(pttag,trpu, _w);

            //} // Dijet balance
            if (alpha >= 0.3) {
              assert(h->ppttageff); h->ppttageff->Fill(pttag, 0, _w);
            } else {
              assert(h->ppttagptprobe); h->ppttagptprobe->Fill(pttag,ptprobe, _w);
              assert(h->h2pttagptprobe); h->h2pttagptprobe->Fill(pttag,ptprobe, _w);
              assert(h->ppttageff); h->ppttageff->Fill(pttag, 1, _w);
              assert(h->ppttagmu);  h->ppttagmu ->Fill(pttag,trpu, _w);
              assert(h->h2pttagmu); h->h2pttagmu->Fill(pttag,trpu, _w);
              //{ Composition vs pt tag pt
              // Fractions vs pt: we do pt selection later in HistosCombine
              assert(h->pncandtp);    h->pncandtp->Fill(pttag, jtn[iprobe]  , _w);
              assert(h->pnchtp);      h->pnchtp  ->Fill(pttag, jtnch[iprobe], _w);
              assert(h->pnnetp);      h->pnnetp  ->Fill(pttag, jtnne[iprobe]-jtnhe[iprobe], _w);
              assert(h->pnnhtp);      h->pnnhtp  ->Fill(pttag, jtnnh[iprobe]-jtnhh[iprobe], _w);
              assert(h->pncetp);      h->pncetp  ->Fill(pttag, jtnce[iprobe], _w);
              assert(h->pnmutp);      h->pnmutp  ->Fill(pttag, jtnmu[iprobe], _w);

              assert(h->pchftp);      h->pchftp  ->Fill(pttag, jtchf[iprobe], _w);
              assert(h->pneftp);      h->pneftp  ->Fill(pttag, (jtnef[iprobe]-jthef[iprobe]), _w);
              assert(h->pnhftp);      h->pnhftp  ->Fill(pttag, (jtnhf[iprobe]-jthhf[iprobe]), _w);
              assert(h->pceftp);      h->pceftp  ->Fill(pttag, jtcef[iprobe], _w);
              assert(h->pmuftp);      h->pmuftp  ->Fill(pttag, jtmuf[iprobe], _w);
              assert(h->phhftp);      h->phhftp  ->Fill(pttag, jthhf[iprobe], _w);
              assert(h->pheftp);      h->pheftp  ->Fill(pttag, jthef[iprobe], _w);
              assert(h->ppuftp);      h->ppuftp  ->Fill(pttag, jtbetaprime[iprobe], _w);

              assert(h->ppt_probepertag); h->ppt_probepertag->Fill(pttag,ptprobe/pttag,_w);

              double metstuff = met1 * cos(DPhi(metphi1, phiprobe));
              assert(h->pmpfz); h->pmpfz->Fill(ptave, 1 + metstuff / ptave, _w);
              if (ptave >= h->ptmin and ptave < h->ptmax) { // Ave fires trigger
                assert(h->hmpfz); h->hmpfz->Fill(1 + metstuff / ptave, _w);
              }
              assert(h->pmpfy); h->pmpfy->Fill(ptprobe, 1 + metstuff / ptprobe, _w);
              if (ptprobe >= h->ptmin and ptprobe < h->ptmax) { // Probe fires trigger
                assert(h->hmpfy); h->hmpfy->Fill(1 + metstuff / ptprobe, _w);
              }
              assert(h->pmpfx); h->pmpfx->Fill(pttag, 1 + metstuff / pttag, _w);
              if (pttag >= h->ptmin and pttag < h->ptmax) { // Tag fires trigger
                assert(h->hmpfx); h->hmpfx->Fill(1 + metstuff / pttag, _w);

                // The distributions of event counts per fraction
                assert(h->hncandtp);    h->hncandtp->Fill(jtn[iprobe], _w);
                assert(h->hnchtp);      h->hnchtp->Fill(jtnch[iprobe], _w);
                assert(h->hnnetp);      h->hnnetp->Fill(jtnne[iprobe]-jtnhe[iprobe], _w);
                assert(h->hnnhtp);      h->hnnhtp->Fill(jtnnh[iprobe]-jtnhh[iprobe], _w);
                assert(h->hncetp);      h->hncetp->Fill(jtnce[iprobe], _w);
                assert(h->hnmutp);      h->hnmutp->Fill(jtnmu[iprobe], _w);
                assert(h->hnhhtp);      h->hnhhtp->Fill(jtnhh[iprobe], _w);
                assert(h->hnhetp);      h->hnhetp->Fill(jtnhe[iprobe], _w);
                assert(h->hchftp);      h->hchftp->Fill(jtchf[iprobe], _w);
                assert(h->hneftp);      h->hneftp->Fill((jtnef[iprobe]-jthef[iprobe]), _w);
                assert(h->hnhftp);      h->hnhftp->Fill((jtnhf[iprobe]-jthhf[iprobe]), _w);
                assert(h->hceftp);      h->hceftp->Fill(jtcef[iprobe], _w);
                assert(h->hmuftp);      h->hmuftp->Fill(jtmuf[iprobe], _w);
                assert(h->hhhftp);      h->hhhftp->Fill(jthhf[iprobe], _w);
                assert(h->hheftp);      h->hheftp->Fill(jthef[iprobe], _w);
                assert(h->hpuftp);      h->hpuftp->Fill(jtbetaprime[iprobe], _w);

                // Fractions vs number of primary vertices
                assert(h->pncandtp_vsnpv);    h->pncandtp_vsnpv->Fill(npvgood, jtn[iprobe], _w);
                assert(h->pnchtp_vsnpv);      h->pnchtp_vsnpv->Fill(npvgood, jtnch[iprobe], _w);
                assert(h->pnnetp_vsnpv);      h->pnnetp_vsnpv->Fill(npvgood, jtnne[iprobe]-jtnhe[iprobe], _w);
                assert(h->pnnhtp_vsnpv);      h->pnnhtp_vsnpv->Fill(npvgood, jtnnh[iprobe]-jtnhh[iprobe], _w);
                assert(h->pncetp_vsnpv);      h->pncetp_vsnpv->Fill(npvgood, jtnce[iprobe], _w);
                assert(h->pnmutp_vsnpv);      h->pnmutp_vsnpv->Fill(npvgood, jtnmu[iprobe], _w);
                assert(h->pnhhtp_vsnpv);      h->pnhhtp_vsnpv->Fill(npvgood, jtnhh[iprobe], _w);
                assert(h->pnhetp_vsnpv);      h->pnhetp_vsnpv->Fill(npvgood, jtnhe[iprobe], _w);
                assert(h->pchftp_vsnpv);      h->pchftp_vsnpv->Fill(npvgood, jtchf[iprobe], _w);
                assert(h->pneftp_vsnpv);      h->pneftp_vsnpv->Fill(npvgood, (jtnef[iprobe]-jthef[iprobe]), _w);
                assert(h->pnhftp_vsnpv);      h->pnhftp_vsnpv->Fill(npvgood, (jtnhf[iprobe]-jthhf[iprobe]), _w);
                assert(h->pceftp_vsnpv);      h->pceftp_vsnpv->Fill(npvgood, jtcef[iprobe], _w);
                assert(h->pmuftp_vsnpv);      h->pmuftp_vsnpv->Fill(npvgood, jtmuf[iprobe], _w);
                assert(h->phhftp_vsnpv);      h->phhftp_vsnpv->Fill(npvgood, jthhf[iprobe], _w);
                assert(h->pheftp_vsnpv);      h->pheftp_vsnpv->Fill(npvgood, jthef[iprobe], _w);
                assert(h->ppuftp_vsnpv);      h->ppuftp_vsnpv->Fill(npvgood, jtbetaprime[iprobe], _w);

                // Fractions vs true pileup
                assert(h->pchftp_vstrpu);      h->pchftp_vstrpu->Fill(trpu, jtchf[iprobe], _w);
                assert(h->pneftp_vstrpu);      h->pneftp_vstrpu->Fill(trpu, (jtnef[iprobe]-jthef[iprobe]), _w);
                assert(h->pnhftp_vstrpu);      h->pnhftp_vstrpu->Fill(trpu, (jtnhf[iprobe]-jthhf[iprobe]), _w);
                assert(h->pceftp_vstrpu);      h->pceftp_vstrpu->Fill(trpu, jtcef[iprobe], _w);
                assert(h->pmuftp_vstrpu);      h->pmuftp_vstrpu->Fill(trpu, jtmuf[iprobe], _w);
                assert(h->phhftp_vstrpu);      h->phhftp_vstrpu->Fill(trpu, jthhf[iprobe], _w);
                assert(h->pheftp_vstrpu);      h->pheftp_vstrpu->Fill(trpu, jthef[iprobe], _w);
                assert(h->ppuftp_vstrpu);      h->ppuftp_vstrpu->Fill(trpu, jtbetaprime[iprobe], _w);

                if (jp::doPhiHistos) {
                  if (etaprobe>0) {
                    assert(h->pchfpostp_vsphi);      h->pchfpostp_vsphi->Fill(phiprobe, jtchf[iprobe], _w);
                    assert(h->pnefpostp_vsphi);      h->pnefpostp_vsphi->Fill(phiprobe, (jtnef[iprobe]-jthef[iprobe]), _w);
                    assert(h->pnhfpostp_vsphi);      h->pnhfpostp_vsphi->Fill(phiprobe, (jtnhf[iprobe]-jthhf[iprobe]), _w);
                    assert(h->pcefpostp_vsphi);      h->pcefpostp_vsphi->Fill(phiprobe, jtcef[iprobe], _w);
                    assert(h->pmufpostp_vsphi);      h->pmufpostp_vsphi->Fill(phiprobe, jtmuf[iprobe], _w);
                    assert(h->phhfpostp_vsphi);      h->phhfpostp_vsphi->Fill(phiprobe, jthhf[iprobe], _w);
                    assert(h->phefpostp_vsphi);      h->phefpostp_vsphi->Fill(phiprobe, jthef[iprobe], _w);
                    assert(h->ppufpostp_vsphi);      h->ppufpostp_vsphi->Fill(phiprobe, jtbetaprime[iprobe], _w);
                  } else {
                    assert(h->pchfnegtp_vsphi);      h->pchfnegtp_vsphi->Fill(phiprobe, jtchf[iprobe], _w);
                    assert(h->pnefnegtp_vsphi);      h->pnefnegtp_vsphi->Fill(phiprobe, (jtnef[iprobe]-jthef[iprobe]), _w);
                    assert(h->pnhfnegtp_vsphi);      h->pnhfnegtp_vsphi->Fill(phiprobe, (jtnhf[iprobe]-jthhf[iprobe]), _w);
                    assert(h->pcefnegtp_vsphi);      h->pcefnegtp_vsphi->Fill(phiprobe, jtcef[iprobe], _w);
                    assert(h->pmufnegtp_vsphi);      h->pmufnegtp_vsphi->Fill(phiprobe, jtmuf[iprobe], _w);
                    assert(h->phhfnegtp_vsphi);      h->phhfnegtp_vsphi->Fill(phiprobe, jthhf[iprobe], _w);
                    assert(h->phefnegtp_vsphi);      h->phefnegtp_vsphi->Fill(phiprobe, jthef[iprobe], _w);
                    assert(h->ppufnegtp_vsphi);      h->ppufnegtp_vsphi->Fill(phiprobe, jtbetaprime[iprobe], _w);
                  }
                }
              } // Tag fires trigger
            //} Composition vs pt tag pt
            } // dijet system
          } // etatag < 1.3
        } // for itag (two leading jets)
      } // Back-to-back
    //} Tag and probe
    } // Second leading jet

    if (jtpt[i0]>=h->ptmin and jtpt[i0]<h->ptmax and fabs(jteta[i0]) < 1.3) { // Jet quality stats
      bool has2 = (i1>=0 and jtpt[i1] > jp::recopt and fabs(jteta[i1])>=h->etamin and fabs(jteta[i1])<h->etamax);
      bool has3 = (i2>=0 and jtpt[i2] > jp::recopt and fabs(jteta[i2])>=h->etamin and fabs(jteta[i2])<h->etamax and jtpt[i1] > 0.70 * jtpt[i0]);
      bool has32 = (has3 and fabs(jteta[i1]) < 1.3);

      h->hr21->Fill(has2  ? jtpt[i1] / jtpt[i0] : 0.);
      h->hr31->Fill(has3  ? jtpt[i2] / jtpt[i0] : 0.);
      h->hr32->Fill(has32 ? jtpt[i2] / jtpt[i1] : 0.);
      if (has2)  h->pr21->Fill(npvgood, has2 ? jtpt[i1] / jtpt[i0] : 0.);
      if (has3)  h->pr31->Fill(npvgood, has3 ? jtpt[i2] / jtpt[i0] : 0.);
      if (has32) h->pr32->Fill(npvgood, has3 ? jtpt[i2] / jtpt[i1] : 0.);
      h->px21->Fill(npvgood, has2  ? 1 : 0);
      h->px31->Fill(npvgood, has3  ? 1 : 0);
      h->px32->Fill(npvgood, has32 ? 1 : 0);
    } // Jet quality stats
  } // First leading jet

  if (jp::debug) PrintInfo("Entering jet loop",true);
  for (int jetidx = 0; jetidx != njt; ++jetidx) {
    if (jp::debug) PrintInfo(Form("Loop over jet %d/%d",jetidx,njt));

    // adapt variable names from different trees
    double pt = jtpt[jetidx];
    double eta = jteta[jetidx];
    double energy = jte[jetidx];
    double mass = sqrt(fabs(pow(energy,2) - pow(pt*cosh(eta),2)));
    double y = jty[jetidx];
    double phi = jtphi[jetidx];
    double jec = jtjesnew[jetidx];
    bool pass_id = _jetids[jetidx];

    double jec2 = jtjesnew[jetidx]/jtjes[jetidx];
    bool etarange = fabs(eta) >= h->etamin and fabs(eta) < h->etamax;
    double ptgen = jtgenpt[jetidx];

    // Check effect of ID cuts
    if (etarange) { // Jet in eta range
      if (jp::debug) {
        PrintInfo(Form("... %s |  index %d/%d jet pt: %f eta : %f id: %d jec: %f",h->trigname.c_str(),jetidx,njt,pt,eta,pass_id,jec),true);
        PrintInfo(Form("...evt id: %d weight: %f met: %f metsumet: %f",_pass_qcdmet,_w,met,metsumet),true);
      }

      assert(h->hpt_noid); h->hpt_noid->Fill(pt, _w);
      assert(h->hpt_nojetid); if (_pass_qcdmet) h->hpt_nojetid->Fill(pt, _w);
      assert(h->hpt_noevtid); if (pass_id) h->hpt_noevtid->Fill(pt, _w);
      // Same versus generator pT as MC extra
      // to decouple efficiency from JEC and JER
      if (h->ismcdir) {
        h->hpt_noid_g->Fill(ptgen, _w);
        if (_pass_qcdmet) h->hpt_nojetid_g->Fill(ptgen, _w);
        if (pass_id)    h->hpt_noevtid_g->Fill(ptgen, _w);
      }
    } // Jet in eta range

    if (pt>jp::recopt) { // pt visible
      // Flags for studying gen eta vs reco eta effects
      bool mcgendr = jtgenr[jetidx] < 0.25;
      bool mcgenetarange = fabs(jtgeneta[jetidx]) >= h->etamin && fabs(jtgeneta[jetidx]) < h->etamax;

      if (h->ismcdir and mcgendr and mcgenetarange) // GenJets matched to any reco jets in any events
        h->hpt_gg0->Fill(ptgen, _w);

      if (_pass_qcdmet and pass_id) { // id OK
        if (h->ismcdir and mcgendr and mcgenetarange) // GenJets matched to good reco jets in good events
          h->hpt_gg->Fill(ptgen, _w);

        if (etarange) { // Correct jet eta range
          if (jp::debug) PrintInfo("..unfolding",true);
          if (h->ismcdir) {
            // unfolding studies (Mikael)
            h->my->Fill(pt, _w);
            h->myf->Fill(pt, _w);
            h->myuw->Fill(pt);
            h->myfuw->Fill(pt);

            if (mcgendr) {
              if (ptgen>jp::recopt) {
                double etagen = jtgeneta[jetidx];
                if (fabs(etagen) >= h->etamin and fabs(etagen) < h->etamax) {
                  h->mT->Fill(ptgen, jtpt[jetidx], _w);
                  h->mTf->Fill(ptgen, jtpt[jetidx], _w);
                  h->mTuw->Fill(ptgen, jtpt[jetidx]);
                  h->mTfuw->Fill(ptgen, jtpt[jetidx]);
                }
              }
            } // Delta r
          }

          if (jp::debug) PrintInfo("..jec uncertainty",true);
          // Get JEC uncertainty
          double unc = 0.01; // default for MC
          if (jp::isdt and _jecUnc) {
            _jecUnc->setJetEta(eta);
            _jecUnc->setJetPt(pt);
            unc = _jecUnc->getUncertainty(true);
            //_jecUnc2->Rjet(pt, unc); // use Fall10 absolute scale uncertainty
          }

          if (jp::dotrigeffsimple) {
            // calculate and/or retrieve efficiencies
            // REMOVED: "For trigger efficiency" (only dummies left)
            double ideff = 1.;
            double vtxeff = 1.;
            double dqmeff = 1.;
            double trigeff = 1.;
            double eff = ideff * vtxeff * dqmeff * trigeff;

            if (jp::debug) PrintInfo("..raw spectrum",true);

            // efficiencies
            assert(h->peff); h->peff->Fill(pt, eff, _w);
            assert(h->pideff); h->pideff->Fill(pt, ideff, _w);
            assert(h->pvtxeff); h->pvtxeff->Fill(pt, vtxeff, _w);
            assert(h->pdqmeff); h->pdqmeff->Fill(pt, dqmeff, _w);
          }

          // raw spectrum
          assert(h->hpt); h->hpt->Fill(pt,_w);
          assert(h->hpt0); h->hpt0->Fill(pt, _w); // Finer binning
          // leading jets (1,2,3)
          if (jetidx==i0) {
            assert(h->hpt1); h->hpt1->Fill(pt, _w);
          } else if (jetidx==i1) {
            assert(h->hpt2); h->hpt2->Fill(pt,_w);
          } else if (jetidx==i2) {
            assert(h->hpt3); h->hpt3->Fill(pt,_w);
          }
          assert(h->hpt_tmp); h->hpt_tmp->Fill(pt); // Event statistics
          assert(h->hpt_pre);
          if (jp::isdt) h->hpt_pre->Fill(pt, _w*_prescales[h->trigname][run] / _wt[h->trigname]);
          if (jp::ismc) h->hpt_pre->Fill(pt, _w0*_wt["mc"]);
          // Do proper event statistics
          assert(h->hpttmp); assert(h->hptevt);
          if (h->hpttmp->GetBinContent(h->hpttmp->FindBin(pt))==0)
            h->hptevt->Fill(pt, _w);
          h->hpttmp->Fill(pt);

          if (jp::debug) PrintInfo("..basic properties",true);

          // basic properties
          assert(h->ppt); h->ppt->Fill(pt, pt, _w);
          assert(h->pmass); h->pmass->Fill(pt, mass/energy, _w);
          assert(h->pjec); h->pjec->Fill(pt, jec, _w);
          assert(h->pjec2); h->pjec2->Fill(pt, jec2, _w);
          assert(h->punc); h->punc->Fill(pt, unc, _w);
          // JEC monitoring
          assert(h->pjec_l1); h->pjec_l1->Fill(pt, jtjes_l1[jetidx], _w);
          assert(h->pjec_l2l3); h->pjec_l2l3->Fill(pt, jtjes_l2l3[jetidx], _w);
          assert(h->pjec_res); h->pjec_res->Fill(pt, jtjes_res[jetidx], _w);

          if (jp::debug) PrintInfo("..control plots of components",true);

          // Composition stuff without T&P (according to the triggers)
          assert(h->pncand); h->pncand->Fill(pt, jtn[jetidx], _w);
          assert(h->pnch); h->pnch->Fill(pt, jtnch[jetidx], _w);
          assert(h->pnne); h->pnne->Fill(pt, jtnne[jetidx]-jtnhe[jetidx], _w);
          assert(h->pnnh); h->pnnh->Fill(pt, jtnnh[jetidx]-jtnhh[jetidx], _w);
          assert(h->pnce); h->pnce->Fill(pt, jtnce[jetidx], _w);
          assert(h->pnmu); h->pnmu->Fill(pt, jtnmu[jetidx], _w);
          assert(h->pnhh); h->pnhh->Fill(pt, jtnhh[jetidx], _w);
          assert(h->pnhe); h->pnhe->Fill(pt, jtnhe[jetidx], _w);
          //
          assert(h->pchf); h->pchf->Fill(pt, jtchf[jetidx], _w);
          assert(h->pnef); h->pnef->Fill(pt, jtnef[jetidx]-jthef[jetidx], _w);
          assert(h->pnhf); h->pnhf->Fill(pt, jtnhf[jetidx]-jthhf[jetidx], _w);
          assert(h->pcef); h->pcef->Fill(pt, jtcef[jetidx], _w);
          assert(h->pmuf); h->pmuf->Fill(pt, jtmuf[jetidx], _w);
          assert(h->phhf); h->phhf->Fill(pt, jthhf[jetidx], _w);
          assert(h->phhf); h->phef->Fill(pt, jthef[jetidx], _w);
          assert(h->ppuf); h->ppuf->Fill(pt, jtbetaprime[jetidx], _w);

          // control plots for topology (JEC)
          h->pa->Fill(pt, jta[jetidx], _w);
          h->ptrpu->Fill(pt, trpu, _w);
          h->prho->Fill(pt, rho, _w);
          h->pnpv->Fill(pt, npvgood, _w);
          h->pnpvall->Fill(pt, npv, _w);
          if (pt >= h->ptmin and pt < h->ptmax) { // Trigger pt range
            h->htrpu->Fill(trpu, _w);
            //
            h->pnpvvsrho->Fill(rho, npvgood, _w);
            h->prhovsnpv->Fill(npvgood, rho, _w);
            h->prhovsnpvall->Fill(npv, rho, _w);
            h->h2rhovsnpv->Fill(npvgood, rho, _w);
            h->h2trpuvsrho->Fill(trpu, rho, _w);
            //
            h->prhovstrpu->Fill(trpu, rho, _w);
            h->pnpvvstrpu->Fill(trpu, npvgood, _w);
            h->pnpvallvstrpu->Fill(trpu, npv, _w);
            h->pitpuvstrpu->Fill(trpu, itpu, _w);
            h->hjet_vstrpu->Fill(trpu, _w);

            if (jp::debug) PrintInfo("..control plots for topology",true);

            if (h->ismcdir) {
              h->hitpu->Fill(itpu, _w);
              h->hootpuearly->Fill(ootpuearly, _w);
              h->hootpulate->Fill(ootpulate, _w);
              h->h2itvsoot->Fill(itpu, ootpulate, _w);
            }

            h->hnpvgood->Fill(npvgood, _w);
            h->hrho->Fill(rho, _w);
            h->hselpt->Fill(pt, _w);
            h->hmass->Fill(mass/energy, _w);
            h->hy->Fill(y, _w);
            h->hy2->Fill(y, _w);
            h->hphi->Fill(phi, _w);
            h->hdphi->Fill(dphi, _w);
            h->hdpt->Fill(dpt, _w);
            h->hjet->Fill(pt   / metsumet1, _w);
            h->hmet->Fill(met1 / metsumet1, _w);
            h->hmetphi->Fill(DPhi(metphi1, phi), _w);
            // control plots for vertex
            h->hpvndof->Fill(pvndof);
            h->hpvx->Fill(pvx-bsx);
            h->hpvy->Fill(pvy-bsy);
            h->hpvz->Fill(pvz-0.);
            h->hpvr->Fill(tools::oplus(pvx-bsx, pvy-bsy));
            h->hpvrho->Fill(pvrho-tools::oplus(bsx, bsy));
            // closure plots for JEC
            h->hmpf ->Fill(1 + met  * cos(DPhi(metphi,  phi)) / pt, _w);
            h->hmpf1->Fill(1 + met1 * cos(DPhi(metphi1, phi)) / pt, _w);
            h->hmpf2->Fill(1 + met2 * cos(DPhi(metphi2, phi)) / pt, _w);
            // Component fractions
            h->hncand->Fill(jtn[jetidx], _w);
            h->hnch->Fill(jtnch[jetidx], _w);
            h->hnne->Fill(jtnne[jetidx]-jtnhe[jetidx], _w);
            h->hnnh->Fill(jtnnh[jetidx]-jtnhh[jetidx], _w);
            h->hnce->Fill(jtnce[jetidx], _w);
            h->hnmu->Fill(jtnmu[jetidx], _w);
            //
            h->hchf->Fill(jtchf[jetidx], _w);
            h->hnef->Fill((jtnef[jetidx]-jthef[jetidx]), _w);
            h->hnhf->Fill((jtnhf[jetidx]-jthhf[jetidx]), _w);
            h->hcef->Fill(jtcef[jetidx], _w);
            h->hmuf->Fill(jtmuf[jetidx], _w);
            h->hhhf->Fill(jthhf[jetidx], _w);
            h->hhef->Fill(jthef[jetidx], _w);
            h->hpuf->Fill(jtbetaprime[jetidx], _w);

            h->hyeta->Fill(TMath::Sign(y-eta,y), _w);
            h->hyeta2->Fill(y-eta, _w);
          } // within trigger pT range

          // closure plots for JEC
          h->pdpt ->Fill(pt, dpt, _w);
          h->pmpf ->Fill(pt, 1 + met  * cos(DPhi(metphi,  phi)) / pt, _w);
          h->pmpf1->Fill(pt, 1 + met1 * cos(DPhi(metphi1, phi)) / pt, _w);
          h->pmpf2->Fill(pt, 1 + met2 * cos(DPhi(metphi2, phi)) / pt, _w);

          // Histograms for quark/gluon study (Ozlem)
          double probg = 1. - qgl[jetidx]; // First approximation
          if (jp::doqglfile) { // If we loaded a previous file to _h3probg, use this for a better probg value
            assert(_h3probg);
            probg = _h3probg->GetBinContent(_h3probg->FindBin(eta,pt,qgl[jetidx]));
          }
          if (probg>=0 and probg<=1) {
            assert(h->hgpt);  h->hgpt->Fill(pt,_w*probg);
            assert(h->hgpt0); h->hgpt0->Fill(pt, _w*probg);
            assert(h->hqgl);  h->hqgl->Fill(qgl[jetidx], _w);
            assert(h->hqgl2); h->hqgl2->Fill(pt, qgl[jetidx], _w);
            if (jp::ismc) {
              assert(h->hqgl2_gen); h->hqgl2_gen->Fill(ptgen, qgl[jetidx], _w);//ben ekledim sorabilirsin
              assert(h->hqgl_g);
              assert(h->hqgl_q);
#ifdef NEWMODE
              bool isgluon = (fabs(partonflavorphys[jetidx]-21)<0.5); //qglokan
              bool isquark = (fabs(partonflavorphys[jetidx])<6 and partonflavorphys[jetidx]!=0); //qglokan why we are ingnoring t(6) b'(7) t'(8)
              bool isunmatch=((fabs(partonflavorphys[jetidx])>=6 and partonflavorphys[jetidx]!=21) || partonflavorphys[jetidx]==0);
#else
              bool isgluon = (fabs(partonflavor[jetidx]-21)<0.5); //qglokan
              bool isquark = (fabs(partonflavor[jetidx])<6 and partonflavor[jetidx]!=0); //qglokan why we are ingnoring t(6) b'(7) t'(8)
              bool isunmatch=(fabs(partonflavor[jetidx])<7); //qglokan
#endif
              assert(isgluon or isquark or isunmatch);

              // For data templates from scaling Pythia (wq & wg), see instructions at
              // https://twiki.cern.ch/twiki/bin/viewauth/CMS/QuarkGluonLikelihood#Systematics
              double x = qgl[jetidx];
              if (isgluon) {
                h->hqgl_g->Fill(x, _w);
                h->hqgl2_g->Fill(pt, x, _w);
                h->hqgl2_g_g->Fill(ptgen, x, _w);
              } else if (isquark) {
                h->hqgl_q->Fill(x, _w);
                h->hqgl2_q->Fill(pt, x, _w);
                h->hqgl2_q_g->Fill(ptgen, x, _w);
              } else if(isunmatch){
                h->hqgl_u->Fill(x, _w);
                h->hqgl2_u->Fill(pt, x, _w);
                h->hqgl2_u_g->Fill(ptgen, x, _w);
              } else {
                PrintInfo("Quark/Gluon status missing from partonflavor");
              }
            }
          } // probg quark/gluon

          if (h->ismcdir and mcgendr) { // MC extras
            if (jp::debug) PrintInfo(Form("genmatch %d ptg=%f yg=%f yr=%f",jetidx,ptgen,jtgeny[jetidx],y),true);

            double r = (ptgen ? pt/ptgen : 0);
            //double resp = (jtjesnew[jetidx] ? r / jtjesnew[jetidx] : 0);
            double dy = (r ? TMath::Sign(jty[jetidx]-jtgeny[jetidx], jtgeny[jetidx]) : 0.);
            h->hpt_r->Fill(pt, _w);
            h->hpt_g->Fill(ptgen, _w);

            h->hpt_gtw->Fill(ptgen, _w);
            // Ozlem: (gluon vs quark)
#ifdef NEWMODE
            if (partonflavorphys[jetidx]==21) h->hgpt_g->Fill(ptgen, _w); //qglokan we need unmatched ones
            else if (abs(partonflavorphys[jetidx])<6 and partonflavorphys[jetidx]!=0) h->hqpt_g->Fill(ptgen, _w);
            else h->hupt_g->Fill(ptgen, _w);
#else
            if (partonflavor[jetidx]==21) h->hgpt_g->Fill(ptgen, _w); //qglokan
            else if (abs(partonflavor[jetidx])<6 and partonflavor[jetidx]!=0) h->hqpt_g->Fill(ptgen, _w);
            else h->hupt_g->Fill(ptgen, _w);
#endif

            h->ppt_r->Fill(pt, pt, _w);
            h->ppt_g->Fill(ptgen, ptgen, _w);

            if (r) {
              // Response closure vs NPV
              h->p2rvsnpv->Fill(ptgen, npvgood, r, _w);

              h->h2r_r->Fill(pt, r, _w);
              h->h2r_g->Fill(ptgen, r, _w);
              h->p2r_r->Fill(pt, r, _w);
              h->p2r_g->Fill(ptgen, r, _w);
              h->p2r_ruw->Fill(pt, r); // unweighted!
              h->p2r_guw->Fill(ptgen, r); // unweighted!

              if(fabs(partonflavorphys[jetidx]-21)<0.5){
                  h->h2r_g_r->Fill(pt, r, _w);
                  h->h2r_g_g->Fill(ptgen, r, _w);
                  h->p2r_g_r->Fill(pt, r, _w);
                  h->p2r_g_g->Fill(ptgen, r, _w);
                  h->p2r_g_ruw->Fill(pt, r); // unweighted!
                  h->p2r_g_guw->Fill(ptgen, r); // unweighted!
              }
              if(fabs(partonflavorphys[jetidx])<6 && partonflavorphys[jetidx]!=0){
                  h->h2r_q_r->Fill(pt, r, _w);
                  h->h2r_q_g->Fill(ptgen, r, _w);
                  h->p2r_q_r->Fill(pt, r, _w);
                  h->p2r_q_g->Fill(ptgen, r, _w);
                  h->p2r_q_ruw->Fill(pt, r); // unweighted!
                  h->p2r_q_guw->Fill(ptgen, r); // unweighted!
              }
              if ((fabs(partonflavorphys[jetidx])>=6 && partonflavorphys[jetidx]!=21) || partonflavorphys[jetidx]==0){
                   /*h->h2r_u_r->Fill(pt, r, _w);
                   h->h2r_u_g->Fill(ptgen, r, _w);
                   h->p2r_u_r->Fill(pt, r, _w);
                   h->p2r_u_g->Fill(ptgen, r, _w);
                   h->p2r_u_ruw->Fill(pt, r); // unweighted!
                   h->p2r_u_guw->Fill(ptgen, r); // unweighted!*/
                   h->h2r_g_r->Fill(pt, r, _w);
                   h->h2r_g_g->Fill(ptgen, r, _w);
                   h->p2r_g_r->Fill(pt, r, _w);
                   h->p2r_g_g->Fill(ptgen, r, _w);
                   h->p2r_g_ruw->Fill(pt, r); // unweighted!
                   h->p2r_g_guw->Fill(ptgen, r); // unweighted!
              }

              // Rapidity closure
              h->h2dy_r->Fill(pt, dy, _w);
              h->h2dy_g->Fill(ptgen, dy, _w);
              h->p2dy_r->Fill(pt, dy, _w);
              h->p2dy_g->Fill(ptgen, dy, _w);
              h->p2dy_ruw->Fill(pt, dy); // unweighted
              h->p2dy_guw->Fill(ptgen, dy); // unweighted
              h->pdy_r->Fill(pt, fabs(y), dy, _w);
              h->pdy_g->Fill(ptgen, fabs(y), dy, _w);
            }
          } // is MC
        } // etabin
      } // if id
    } // pt visible
  } // for jetidx

  // Event statistics
  for (int xidx = 1; xidx != h->hpt_tmp->GetNbinsX()+1; ++xidx) {
    if (h->hpt_tmp->GetBinContent(xidx)!=0) {
      double pt = h->hpt_tmp->GetBinCenter(xidx);
      int njet = h->hpt_tmp->GetBinContent(xidx);
      h->hpt_evtcount->Fill(pt);
      h->hpt_evt->Fill(pt, _w);
      h->hpt_jet->Fill(pt, _w*njet);
    }
  } // for xidx

  if (jp::ismc) {
    if (jp::debug) PrintInfo("Truth loop:",true);
    for (int gjetidx = 0; gjetidx != gen_njt; ++gjetidx) { // Unbiased gen spectrum (for each trigger)
      double etagen = gen_jteta[gjetidx];
      double ptgen = gen_jtpt[gjetidx];

      if (fabs(etagen) >= h->etamin and fabs(etagen) < h->etamax) {
        if (jp::debug) PrintInfo(Form("genjet %d/%d ptg=%f etag=%f",gjetidx,gen_njt,ptgen,etagen),true);

        h->hpt_g0tw->Fill(ptgen, _w);
        // Ozlem: (gluon vs quark)
        if (gen_partonflavor[gjetidx]==21) h->hgpt_g0tw->Fill(ptgen, _w); //qglokan
        else if(abs(gen_partonflavor[gjetidx])<6 and gen_partonflavor[gjetidx]!=0) h->hqpt_g0tw->Fill(ptgen, _w);
        else h->hupt_g0tw->Fill(ptgen, _w);

        // unfolding studies (Mikael)
        if (h->ismcdir) { // Only the 'mc' trigger
          if (gen_jtpt[gjetidx]>jp::recopt) {
            h->mx->Fill(ptgen, _w);
            h->mxf->Fill(ptgen, _w);
            h->mxuw->Fill(ptgen);
            h->mxfuw->Fill(ptgen);
          } // recopt limit

          h->hpt_g0->Fill(gen_jtpt[gjetidx], _w);
          // Ozlem: (gluon vs quark)
          if (gen_partonflavor[gjetidx]==21) h->hgpt_g0->Fill(gen_jtpt[gjetidx], _w); //qglokan
          else if(abs(gen_partonflavor[gjetidx])<6 and gen_partonflavor[gjetidx]!=0) h->hqpt_g0->Fill(gen_jtpt[gjetidx], _w);
          else h->hupt_g0->Fill(gen_jtpt[gjetidx], _w);

          assert(h->hpt_g0_tmp); h->hpt_g0_tmp->Fill(gen_jtpt[gjetidx]);
        } // mcdir (a subset of jp::ismc)
      } // gen jet eta
    } // genjet loop
  } // MC
} // FillSingleBasic


// Write and delete histograms
void HistosFill::WriteBasic()
{
  // Report memory usage to avoid malloc problems when writing file
  PrintInfo("WriteBasic():");
  PrintMemInfo();

  for (auto &histrange : _histos) {
    for (auto &h : histrange.second) {
      // Luminosity information
      for (int j = 0; j != h->hlumi->GetNbinsX()+1; ++j) {
        h->hlumi->SetBinContent(j, jp::isdt ? h->lumsum : 1. );
        h->hlumi2->SetBinContent(j, jp::isdt ? h->lumsum2 : 1. );
      }
      h->Write();
      delete h;
    } // for histidx
  } // for histit

  PrintInfo(Form("\nOutput (HistosBasic) stored in %s",_outfile->GetName()),true);

  // Report memory usage to avoid malloc problems when writing file
  PrintInfo("writeBasic() finished:");
  PrintMemInfo();
} // writeBasic


// Initialize eta histograms for trigger bins
void HistosFill::InitEta(string name)
{
  // Report memory usage to avoid malloc problems when writing file
  PrintInfo(Form("InitEta(%s):",name.c_str()));
  PrintMemInfo();

  TDirectory *curdir = gDirectory;

  // open file for output
  TFile *f = _outfile;
  assert(f && !f->IsZombie());
  f->mkdir(name.c_str());
  bool enteroutdir = f->cd(name.c_str());
  assert(enteroutdir);
  TDirectory *topdir = f->GetDirectory(name.c_str()); assert(topdir);
  topdir->cd();

  // define triggers
  vector<string> triggers;
  // define efficient pT ranges for triggers for control plots
  map<string, pair<double, double> > pt;
  // define pT values for triggers
  map<string, double> pttrg;
  if (jp::ismc) {
    triggers.push_back("mc");
    pt["mc"] = pair<double, double>(jp::recopt, jp::emax);
    pttrg["mc"] = jp::recopt;
  }
  if (jp::isdt or jp::domctrigsim) {
    // This is done both for data and MC, because why not?
    for (unsigned itrg = 0; itrg != jp::notrigs; ++itrg) {
      string trg = jp::triggers[itrg];
      triggers.push_back(trg);
      double pt1 = jp::trigranges[itrg][0];
      double pt2 = jp::trigranges[itrg][1];
      pt[trg] = pair<double, double>(pt1, pt2);
      double pt0 = jp::trigthr[itrg];
      pttrg[trg] = pt0;
    }
  }

  assert(topdir);

  for (unsigned int j = 0; j != triggers.size(); ++j) {
    // subdirectory for trigger
    const char *trg = triggers[j].c_str();
    topdir->mkdir(trg);
    assert(topdir->cd(trg));
    TDirectory *dir = topdir->GetDirectory(trg);
    assert(dir);
    dir->cd();

    // Initialize and store
    assert(dir);
    HistosEta *h = new HistosEta(dir, trg,pttrg[trg],pt[trg].first, pt[trg].second, triggers[j]=="mc");
    _etahistos[name].push_back(h);
  } // for j

  curdir->cd();

  // Report memory usage to avoid malloc problems when writing file
  PrintInfo(Form("InitEta(%s) finished:",name.c_str()));
  PrintMemInfo();
} // InitEta


// Loop over basic histogram containers to fill all
void HistosFill::FillEta(string name, Float_t* _pt, Float_t* _eta, Float_t* _phi)
{
  for (auto histidx = 0u; histidx != _etahistos[name].size(); ++histidx)
    FillSingleEta(_etahistos[name][histidx], _pt, _eta, _phi);
}


// Fill basic histograms after applying pt, y cuts
void HistosFill::FillSingleEta(HistosEta *h, Float_t* _pt, Float_t* _eta, Float_t* _phi)
{
  assert(h);

  _w = _w0 * _wt[h->trigname];
  if (_w <= 0) return;

  bool fired = (_trigs.find(h->trigname)!=_trigs.end());

  if (jp::debug) {
    if (h == _etahistos.begin()->second[0]) {
      PrintInfo(Form("Triggers size: %lu",_trigs.size()),true);
      for (auto trgit = _trigs.begin(); trgit != _trigs.end(); ++trgit) cout << *trgit << ", ";
      PrintInfo(Form("(%s)",h->trigname.c_str()),true);
    }
  }

  // check if required trigger fired
  if (!fired or !_pass) return;

  // Tag & probe hoods
  int i0 = jt3leads[0];
  int i1 = jt3leads[1];
  int i2 = jt3leads[2];
  if (_pass_qcdmet) {
    if (i0>=0 and _jetids[i0] and jtpt[i0]>jp::recopt and i1>=0 and _jetids[i1] and jtpt[i1]>jp::recopt) { // Quality conditios for leading jets
      double dphi = DPhi(jtphi[i0], jtphi[i1]);
      if (dphi > 2.7) { // Back-to-back condition
        double pt3 = ((i2>=0 and jtpt[i2]>jp::recopt) ? jtpt[i2] : 0.);
        double ptave = 0.5 * (jtpt[i0] + jtpt[i1]);
        double alpha = pt3/ptave;

        for (auto itag_lead = 0u; itag_lead<2u; ++itag_lead) { // Look for both t&p combos for the leading jets
          int itag = jt3leads[itag_lead];
          int iprobe = jt3leads[(itag_lead==0 ? 1 : 0)];
          double etatag = jteta[itag];
          double etaprobe = jteta[iprobe];
          if (fabs(etatag) < 1.3) { // Tag required to be in the barrel region
            double pttag = jtpt[itag];
            double ptprobe = _pt[iprobe];
            // Special PU studies
            if (pttag>50 and pttag<60) {
              h->hnpvall_pt50to60->Fill(npv, _w);
              h->hnpv_pt50to60->Fill(npvgood, _w);
              h->hrho_pt50to60->Fill(rho, _w);
              h->htrpu_pt50to60->Fill(trpu, _w);
              h->hpuf_pt50to60->Fill(jtbetaprime[iprobe], _w);
              h->hchf_pt50to60->Fill(jtchf[iprobe], _w);
            }
            if (jp::do3dHistos) {
              double asymm = (ptprobe - pttag)/(2*ptave);
              double mpf = met1*cos(DPhi(metphi1,_phi[itag]))/(2*ptave);
              for (auto alphaidx = 0u; alphaidx < h->alpharange.size(); ++alphaidx) {
                float alphasel = h->alpharange[alphaidx];
                if (alpha<alphasel) {
                  // Val 10 = excluded, -10 = ok
                  h->hdjasymm[alphaidx]  ->Fill(ptave, etaprobe, asymm, _w);
                  h->hdjmpf[alphaidx]    ->Fill(ptave, etaprobe, mpf  , _w);
                  h->hdjasymmtp[alphaidx]->Fill(pttag, etaprobe, asymm, _w);
                  h->hdjmpftp[alphaidx]  ->Fill(pttag, etaprobe, mpf  , _w);
                  //h->hdjasymmpt[alphaidx]->Fill(ptprobe, etaprobe, asymm, _w);
                  //h->hdjmpfpt[alphaidx]  ->Fill(ptprobe, etaprobe, mpf  , _w);
                }
              }
            }

            // for composition vs eta
            if (alpha < 0.3 and pttag >= h->ptmin and pttag < h->ptmax) { // Alpha and trigger
              assert(h->pchftp_vseta); h->pchftp_vseta->Fill(etaprobe, jtchf[iprobe], _w);
              assert(h->pneftp_vseta); h->pneftp_vseta->Fill(etaprobe, jtnef[iprobe]-jthef[iprobe], _w);
              assert(h->pnhftp_vseta); h->pnhftp_vseta->Fill(etaprobe, jtnhf[iprobe]-jthhf[iprobe], _w);
              assert(h->pceftp_vseta); h->pceftp_vseta->Fill(etaprobe, jtcef[iprobe], _w);
              assert(h->pmuftp_vseta); h->pmuftp_vseta->Fill(etaprobe, jtmuf[iprobe], _w);
              assert(h->phhftp_vseta); h->phhftp_vseta->Fill(etaprobe, jthhf[iprobe], _w);
              assert(h->pheftp_vseta); h->pheftp_vseta->Fill(etaprobe, jthef[iprobe], _w);
              assert(h->ppuftp_vseta); h->ppuftp_vseta->Fill(etaprobe, jtbetaprime[iprobe], _w);
            } // select pt bin for profiles vseta
          } // etatag < 1.3
        } // tag & probe
      } // dphi > 2.7
    }
  } // ids

  for (int jetidx = 0; jetidx != njt; ++jetidx) {
    // adapt variable names from different trees
    double pt = jtpt[jetidx];
    double eta = jteta[jetidx];

    if (pt>jp::recopt and _pass_qcdmet and _jetids[jetidx] and pt >= h->ptmin and pt < h->ptmax) {
      double eta = jteta[jetidx];
      h->heta->Fill(eta, _w);
      double phi = jtphi[jetidx];
      h->hetaphi->Fill(eta, phi, _w);
      assert(h->hetaphi_chf); h->hetaphi_chf->Fill(eta, phi, jtchf[jetidx]*_w);
      assert(h->hetaphi_nef); h->hetaphi_nef->Fill(eta, phi, jtnef[jetidx]-jthef[jetidx]*_w);
      assert(h->hetaphi_nhf); h->hetaphi_nhf->Fill(eta, phi, jtnhf[jetidx]-jthhf[jetidx]*_w);
      assert(h->hetaphi_cef); h->hetaphi_cef->Fill(eta, phi, jtcef[jetidx]*_w);
      assert(h->hetaphi_muf); h->hetaphi_muf->Fill(eta, phi, jtmuf[jetidx]*_w);
      assert(h->hetaphi_hhf); h->hetaphi_hhf->Fill(eta, phi, jthhf[jetidx]*_w);
      assert(h->hetaphi_hef); h->hetaphi_hef->Fill(eta, phi, jthef[jetidx]*_w);
      assert(h->hetaphi_puf); h->hetaphi_puf->Fill(eta, phi, jtbetaprime[jetidx]*_w);
      assert(h->petaphi_chf); h->petaphi_chf->Fill(eta, phi, jtchf[jetidx], _w);
      assert(h->petaphi_nef); h->petaphi_nef->Fill(eta, phi, jtnef[jetidx]-jthef[jetidx], _w);
      assert(h->petaphi_nhf); h->petaphi_nhf->Fill(eta, phi, jtnhf[jetidx]-jthhf[jetidx], _w);
      assert(h->petaphi_cef); h->petaphi_cef->Fill(eta, phi, jtcef[jetidx], _w);
      assert(h->petaphi_muf); h->petaphi_muf->Fill(eta, phi, jtmuf[jetidx], _w);
      assert(h->petaphi_hhf); h->petaphi_hhf->Fill(eta, phi, jthhf[jetidx], _w);
      assert(h->petaphi_hef); h->petaphi_hef->Fill(eta, phi, jthef[jetidx], _w);
      assert(h->petaphi_puf); h->petaphi_puf->Fill(eta, phi, jtbetaprime[jetidx], _w);
    }
  }

  if (h->ismcdir and _pass_qcdmet) {
    for (int jetidx = 0; jetidx != njt; ++jetidx) {
      double pt = jtpt[jetidx];
      if (pt>jp::recopt) { // pt visible
        bool mcgendr = jtgenr[jetidx] < 0.25;
        bool id = _jetids[jetidx];
        if (mcgendr and id) { // id OK
          double ptgen = jtgenpt[jetidx];
          if (ptgen>0 and jtjesnew[jetidx]!=0) {
            // CAUTION: Do we lay jtjesnew over the old correction?
            double resp = pt / (ptgen*jtjesnew[jetidx]);
            // Response closure vs NPV
            if (resp) {
              double eta = jteta[jetidx];
              h->p3rvsnpv->Fill(ptgen, eta, npvgood, resp, _w);
              h->p3rvsnpvW->Fill(ptgen, fabs(eta), npvgood, resp, _w);
            }
          }
        } // if id
      } // pt visible
    } // for jetidx
  } // if MC
} // FillSingleEta


// Write and delete histograms
void HistosFill::WriteEta()
{
  // Report memory usage to avoid malloc problems when writing file
  PrintInfo("WriteEta():");
  PrintMemInfo();

  for (auto histit : _etahistos) {
    for (unsigned int histidx = 0; histidx != histit.second.size(); ++histidx) {
      HistosEta *h = histit.second[histidx];
      delete h;
    } // for histidx
  } // for histit

  PrintInfo(Form("\nOutput (HistosEta) stored in %s",_outfile->GetName()),true);

  // Report memory usage to avoid malloc problems when writing file
  PrintInfo("WriteEta() finished:");
  PrintMemInfo();
} // WriteEta


// Initialize eta histograms for trigger bins
void HistosFill::InitMC(string name)
{
  // Report memory usage to avoid malloc problems when writing file
  PrintInfo("InitMC(%s):",name.c_str());
  PrintMemInfo();

  TDirectory *curdir = gDirectory;

  // open file for output
  TFile *f = _outfile;
  assert(f && !f->IsZombie());
  f->mkdir(name.c_str());
  bool entertopdir = f->cd(name.c_str());
  assert(entertopdir);
  TDirectory *topdir = f->GetDirectory(name.c_str()); assert(topdir);
  topdir->cd();

  // define triggers
  vector<string> triggers;
  // define efficient pT ranges for triggers for control plots
  map<string, pair<double, double> > pt;
  // define pT values for triggers
  map<string, double> pttrg;
  if (jp::ismc) {
    triggers.push_back("mc");
    pt["mc"] = pair<double, double>(jp::recopt, jp::emax);
    pttrg["mc"] = jp::recopt;
  }
  if (jp::isdt or jp::domctrigsim) {
    // This is done both for data and MC, because why not?
    for (unsigned itrg = 0; itrg != jp::notrigs; ++itrg) {
      string trg = jp::triggers[itrg];
      triggers.push_back(trg);
      double pt1 = jp::trigranges[itrg][0];
      double pt2 = jp::trigranges[itrg][1];
      pt[trg] = pair<double, double>(pt1, pt2);
      double pt0 = jp::trigthr[itrg];
      pttrg[trg] = pt0;
    }
  }

  assert(topdir);

  for (unsigned int j = 0; j != triggers.size(); ++j) {
    // subdirectory for trigger
    const char *trg = triggers[j].c_str();
    topdir->mkdir(trg);
    bool entertrgdir = topdir->cd(trg);
    assert(entertrgdir);
    TDirectory *dir = topdir->GetDirectory(trg);
    assert(dir);
    dir->cd();

    // Initialize and store
    assert(dir);
    HistosMC *h = new HistosMC(dir, trg);
    _mchistos[name].push_back(h);
  } // for j

  curdir->cd();

  // Report memory usage to avoid malloc problems when writing file
  PrintInfo(Form("InitMC(%s) finished:",name.c_str()));
  PrintMemInfo();
} // InitMC


// Loop over basic histogram containers to fill all
void HistosFill::FillMC(string name,  Float_t* _recopt, Float_t* _genpt,
                              Float_t* _pt, Float_t* _eta,    Float_t* _phi)
{
  for (auto histidx = 0u; histidx != _mchistos[name].size(); ++histidx)
    FillSingleMC(_mchistos[name][histidx], _recopt, _genpt, _pt, _eta, _phi);
}


// Fill basic histograms after applying pt, y cuts
void HistosFill::FillSingleMC(HistosMC *h,  Float_t* _recopt,  Float_t* _genpt,
                             Float_t* _pt, Float_t* _eta,     Float_t* _phi)
{
  assert(h);

  _w = _w0 * _wt[h->trigname];
  if (_w <= 0) return;

  bool fired = (_trigs.find(h->trigname)!=_trigs.end());

  if (jp::debug) {
    if (h == _mchistos.begin()->second[0]) {
      PrintInfo(Form("Triggers size: %lu",_trigs.size()));
      for (auto trgit = _trigs.begin(); trgit != _trigs.end(); ++trgit) cout << *trgit << ", ";
      PrintInfo(Form("(%s)",h->trigname.c_str()));
    }
  }

  // check if required trigger fired
  if (!fired or !_pass) return;

  // Tag & probe hoods
  int i0 = jt3leads[0];
  int i1 = jt3leads[1];
  int i2 = jt3leads[2];
  if (_pass_qcdmet and i0>=0 and _jetids[i0] and jtpt[i0]>jp::recopt and i1>=0 and _jetids[i1] and jtpt[i1]>jp::recopt) { // Leading jets
    double dphi = DPhi(jtphi[i0], jtphi[i1]);
    if (dphi > 2.7) { // Back-to-back condition
      double pt3 = ((i2>=0 and jtpt[i2]>jp::recopt) ? jtpt[i2] : 0.);
      double ptave = 0.5 * (jtpt[i0] + jtpt[i1]);
      double alpha = pt3/ptave;

      for (auto itag_lead = 0u; itag_lead<2u; ++itag_lead) { // Look for both t&p combos for the leading jets
        int itag = jt3leads[itag_lead];
        int iprobe = jt3leads[(itag_lead==0 ? 1 : 0)];
        double etatag = jteta[itag];
        double etaprobe = jteta[iprobe];
        if (etatag < 1.3) {
          if (jp::do3dHistos) {
            double pttag = _pt[itag];
            double ptprobe = _pt[iprobe];
            double asymm = (ptprobe - pttag)/(2*ptave);
            double asymmtp = (ptprobe - pttag)/(2*pttag);
            double ptresp_tag = _recopt[itag]/_genpt[itag];
            double ptresp_probe = _recopt[iprobe]/_genpt[iprobe];
            for (unsigned alphaidx = 0; alphaidx < h->alpharange.size(); ++alphaidx) {
              float alphasel = h->alpharange[alphaidx];
              if (alpha<alphasel) {
                h->hdjasymmtp     [alphaidx]->Fill(pttag, etaprobe, asymmtp,      _w);
                h->hdjresptp_tag  [alphaidx]->Fill(pttag, etaprobe, ptresp_tag,   _w);
                h->hdjresptp_probe[alphaidx]->Fill(pttag, etaprobe, ptresp_probe, _w);
                h->hdjasymm     [alphaidx]->Fill(ptave, etaprobe, asymm,        _w);
                h->hdjresp_tag  [alphaidx]->Fill(ptave, etaprobe, ptresp_tag,   _w);
                h->hdjresp_probe[alphaidx]->Fill(ptave, etaprobe, ptresp_probe, _w);
              } // alpha sel.
            } // alpha loop
          } // 3D histos?
        } // etatag < 1.3
      } // itag
    } // dphi > 2.7
  } // two or more jets, phase space
} // FillSingleMC


// Write and delete histograms
void HistosFill::WriteMC()
{
  // Report memory usage to avoid malloc problems when writing file
  PrintInfo("WriteMC():");
  PrintMemInfo();

  for (auto histit : _mchistos) {
    for (auto histidx = 0u; histidx != histit.second.size(); ++histidx) {
      HistosMC *h = histit.second[histidx];
      delete h;
    } // for histidx
  } // for histit

  PrintInfo(Form("\nOutput (HistosMC) stored in %s",_outfile->GetName()),true);

  // Report memory usage to avoid malloc problems when writing file
  PrintInfo("WriteMC() finished:");
  PrintMemInfo();
} // WriteMC


// Initialize basic histograms for trigger and eta bins
void HistosFill::InitRun(string name, double etamin, double etamax) {

  // Report memory usage to avoid malloc problems when writing file
  PrintInfo(Form("InitRun(%s):",name.c_str()));
  PrintMemInfo();

  TDirectory *curdir = gDirectory;
  TFile *f = _outfile;
  assert(f && !f->IsZombie());
  f->mkdir(name.c_str());
  bool enteroutdir = f->cd(name.c_str());
  assert(enteroutdir);
  TDirectory *dir = f->GetDirectory(name.c_str()); assert(dir);
  dir->cd();

  HistosRun *h = new HistosRun(dir, etamin, etamax);
  _runhistos[name] = h;

  curdir->cd();

  // Report memory usage to avoid malloc problems when writing file
  PrintInfo("InitRun() finished:");
  PrintMemInfo();
} // InitRun


// Fill run histograms
void HistosFill::FillRun(string name)
{
  HistosRun *h = _runhistos[name];
  assert(h);

  // Luminosity information
  if (jp::isdt and h->lums[run][lbn]==0) {

    double lum = _lums[run][lbn];
    double lum2 = _lums2[run][lbn];
    // Let lum==0 pass, it can be a Poisson fluctuation for a valid LS

    h->lumsum += lum;
    h->lumsum2 += lum2;
    h->runlums[run] += lum;
    h->runlums2[run] += lum2;
    h->lums[run][lbn] = 1;

    for (auto trgidx = 0u; trgidx != h->trg.size(); ++trgidx) {
      string const& t = h->trg[trgidx];
      double prescale(0);
      if (_prescales[t].find(run)==_prescales[t].end()) {
        if (_trigs.find(t)!=_trigs.end()) {
          PrintInfo(Form("Prescale not found for trigger %s run %d",
                         t.c_str(),run));
          assert(false);
        }
      } else prescale = _prescales[t][run];
      h->runlums_trg[t][run] += (prescale ? lum / prescale : 0.);
    } // for trgidx

    // Initialize counters for a new run
    if (h->lums[run].size()==1) {
      for (auto trgidx = 0u; trgidx != h->trg.size(); ++trgidx) {
        string const& t = h->trg[trgidx];

        h->p_trg[t][run] = 0;
        h->t_trg[t][run] = 0;
        h->npv_trg[t][run] = 0;
        h->c_chf[t][run] = 0;
        h->c_nef[t][run] = 0;
        h->c_nhf[t][run] = 0;
        h->c_betaprime[t][run] = 0;
        h->t_trgtp[t][run] = 0;
        h->c_chftp[t][run] = 0;
        h->c_neftp[t][run] = 0;
        h->c_nhftp[t][run] = 0;
        h->c_betaprimetp[t][run] = 0;

        for (unsigned int j = 0; j != h->trg.size(); ++j) {
          string const& t2 = h->trg[j];
          h->p_trgpair[t+t2][run] = 0;
        } // for j
      } // for trgidx
    } // new run
  } // data, luminosity zero

  int i0 = jt3leads[0];
  int i1 = jt3leads[1];
  int i2 = jt3leads[2];
  double dphi = (njt>=2 ? DPhi(jtphi[i0], jtphi[i1]) : 0.);
  double pt3 = (njt>=3 ? jtpt[i2] : 0.);

  for (int jetidx = 0; jetidx != njt; ++jetidx) {
    double pt = jtpt[jetidx];
    double eta = jteta[jetidx];

    if (h->etamin <= fabs(eta) and fabs(eta) < h->etamax and _pass and _jetids[jetidx] and _pass_qcdmet) {
      for (auto trgit = _trigs.begin(); trgit != _trigs.end(); ++trgit) {
        string const& t = *trgit;

        if (pt > 18.) ++h->p_trg[t][run];
        if (pt > h->pt[t]) {
          ++h->t_trg[t][run]; // unweighted events
          h->tw_trg[t][run] += _prescales[t][run]; // prescale weighted events
          h->npv_trg[t][run] += npv;
          h->npvgood_trg[t][run] += npvgood;
          h->c_chf[t][run]       += jtchf[jetidx];
          h->c_nef[t][run]       += (jtnef[jetidx]-jthef[jetidx]);
          h->c_nhf[t][run]       += (jtnhf[jetidx]-jthhf[jetidx]);
          h->c_betaprime[t][run] += jtbetaprime[jetidx];
        }

        int itag = (jetidx==0 ? 1 : 0);
        if (jetidx<2 and dphi > 2.7 and pt3 < jtpt[itag] and fabs(jteta[itag]) < 1.3 and jtpt[itag] > h->pt[t] and _jetids[itag]) {
          ++h->t_trgtp[t][run];
          h->c_chftp[t][run]       += jtchf[jetidx];
          h->c_neftp[t][run]       += (jtnef[jetidx]-jthef[jetidx]);
          h->c_nhftp[t][run]       += (jtnhf[jetidx]-jthhf[jetidx]);
          h->c_betaprimetp[t][run] += jtbetaprime[jetidx];
        }

        for (auto trgit2 = _trigs.begin(); trgit2 != _trigs.end(); ++trgit2) {
          string const& t2 = *trgit2;
          if (t!=t2) ++h->p_trgpair[t+t2][run];
        } // for trgit2
      } // for trgit
    } // conditions
  } // for jetidx
} // FillRun


// Write and delete histograms
void HistosFill::WriteRun()
{
  // Report memory usage to avoid malloc problems when writing file
  PrintInfo("WriteRun():");
  PrintMemInfo();

  for (auto histit = _runhistos.begin(); histit != _runhistos.end(); ++histit) {
    HistosRun *h = histit->second;
    delete h;
  } // for histit

  PrintInfo(Form("\nOutput (HistosRun) stored in %s",_outfile->GetName()),true);

  // Report memory usage to avoid malloc problems when writing file
  PrintInfo("WriteRun() finished:");
  PrintMemInfo();
} // WriteRun


// Initialize basic histograms for trigger and eta bins
void HistosFill::InitAll(string name) {

  // Report memory usage to avoid malloc problems when writing file
  PrintInfo(Form("InitAll(%s):",name.c_str()));
  PrintMemInfo();

  TDirectory *curdir = gDirectory;
  TFile *f = _outfile;
  assert(f and !f->IsZombie());
  f->mkdir(name.c_str());
  bool enteroutdir = f->cd(name.c_str());
  assert(enteroutdir);
  TDirectory *dir = f->GetDirectory(name.c_str()); assert(dir);
  dir->cd();

  HistosAll *h = new HistosAll(dir);
  _allhistos[name] = h;

  curdir->cd();

  // Report memory usage to avoid malloc problems when writing file
  PrintInfo("InitAll() finished:");
  PrintMemInfo();
} // InitAll


// Fill all-trigs histograms
void HistosFill::FillAll(string name)
{
  HistosAll *h = _allhistos[name];
  assert(h);

  if (mCorrs.size()==0) { // DATA, MC, HW; halfscale, halfshift, fullshift, norm
    //mCorrs = {1.5881843032615166, 1.361884410568897, 1.2447321607649051, 1.175724989044774, 1.131913280004901, 1.0951827500224993, 1.0720885205684814, 1.0678881715251474, 1.0638893126768867, 1.069469615756942, 1.0700072065373922, 1.0737713502112098, 1.0680887463406228, 1.0627951512910385, 1.0628180273465946, 1.062877791920791, 1.0638770121849623, 1.0644226481573509, 1.0620787344737574, 1.0631648721053788, 0.00017023474153275188, 1.6329062588406502, 1.3730425913147788, 1.2436502118995467, 1.165377273606686, 1.1221672977506756, 1.0887157650441934, 1.0671642156826038, 1.0634576653848786, 1.0612735936730957, 1.0673575304486878, 1.068358739488697, 1.0713852353345252, 1.0670475744786696, 1.0621740578638394, 1.0620255711435342, 1.0624906114939867, 1.062626295108181, 1.0633188424180275, 1.0617434209504808, 1.0576940309171876, 6.928385182494995e-05, 1.6822141040134608, 1.3841439756484244, 1.2584377838357297, 1.1744028207505808, 1.1258006475546014, 1.0932362462704166, 1.07132291156919, 1.0596899318062742, 1.0598664006648792, 1.064685132028396, 1.0670408797414865, 1.071960655857443, 1.0641923290087492, 1.0575262017153655, 1.0570212121002476, 1.0567098206498406, 1.0563244896754587, 1.0585048025661679, 1.0542454686559766, 1.054775420694358, 2.585184449443649e-14, 1.6828025197371317, 1.3720879282376839, 1.247018779954876, 1.1643993866381885, 1.1150646295002031, 1.0864333899925924, 1.0644604688263573, 1.057762576262843, 1.0519794682117043, 1.0562254831475775, 1.0569476396074335, 1.0605452497646677, 1.0556637309015853, 1.0500555878534674, 1.0510111207880666, 1.052742929283868, 1.0507339302077288, 1.0528948811749725, 1.048006503398759, 3.563385771775403e-05, 1.5567920253933327e-14, 1.805291667779424, 1.4057781989348042, 1.2501331785412533, 1.154775186518558, 1.103372024232585, 1.071628891715457, 1.0555728210033584, 1.0447547666269728, 1.0430329622458443, 1.0495490176056776, 1.053118097339263, 1.0578130732149633, 1.05078121691245, 1.0426079938745283, 1.0420714008196463, 1.037839464216399, 1.0407374739515323, 0.00018511696568949908, 1.1988596404349834e-05, 1.0829734534768868e-14, -6.298339232228696e-15, 2.023312668121601, 1.4841105603860478, 1.256956047084795, 1.172271395163435, 1.1187091564864862, 1.0752920334120313, 1.0544727365616617, 1.0457072268780891, 1.0456794435155858, 1.0428702046643141, 1.033699495337932, 1.0328587979553039, 1.0337300773329887, 1.0295548204425604, 1.0340239781249905, 0.00018434992757481678, -1.395923499797197e-16, -6.088913829904976e-17, 2.252364280842302e-19, -2.610077447841682e-21, 2.1928369785664234e-24, 2.2717017421800314, 1.5249968551684738, 1.3269427973273829, 1.201260090864257, 1.1425630074009738, 1.0977792377389037, 1.0646632442209394, 1.0559001324234114, 1.0298921822803926, 0.9887271765206481, 0.0002726478659716026, 1.150145273104169e-05, -2.6262488875203464e-49, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.025675045772373697};
    //mCorrs = {1.1297491778234579, 1.169270012252267, 1.147645683950961, 1.1300896480877574, 1.1013616506781205, 1.0788493506331822, 1.0686865539115649, 1.060105771892873, 1.0575111997495628, 1.0559173812557636, 1.0531530911145404, 1.05260757044433, 1.0517932979737066, 1.0518837946602064, 1.0518488938360386, 1.0520626966851299, 1.0516300010289559, 1.051598700577618, 1.050799013302952, 1.0497304163230863, 1.0495653899341584, 1.1942921781513343, 1.2196133904316153, 1.1642291751136518, 1.1244861365985228, 1.0937002554956914, 1.0848114784240146, 1.0667890315038755, 1.0624082993826809, 1.0571458437569634, 1.054130594936643, 1.0525505442604302, 1.0504354839140342, 1.0504966829742857, 1.051015881819214, 1.0504125554525907, 1.0503331598675072, 1.0505526991546108, 1.0507770631762872, 1.0497047276264742, 1.048879955524842, 1.047550436682962, 1.2510474464173487, 1.2412461299387083, 1.1770162319264408, 1.144911145101289, 1.1057658389572536, 1.0845101532717614, 1.0740003592918625, 1.0603240645348837, 1.0564166081430164, 1.0514468165192377, 1.0529766035836063, 1.0520981460590297, 1.0505429516082707, 1.0512175027062942, 1.0512423750982334, 1.0508627482762136, 1.0500553320449204, 1.048917476096092, 1.049423233076728, 1.045906890620297, 1.0323282367287625, 1.3109612735589034, 1.2260599162652368, 1.1608031448827325, 1.122283475930843, 1.0952818580629302, 1.0742310840128002, 1.0588681502045958, 1.0526675472479423, 1.0474232711096538, 1.045866314354532, 1.0433862150382418, 1.0447062228127333, 1.044964602251212, 1.04376458954849, 1.0426197684634642, 1.042997710842602, 1.0417828312778201, 1.0406288225829907, 1.0416214349841202, 1.0413758689223223, -7.794049383477983e-14, 1.4056384453891961, 1.2617102733160928, 1.1994607643150128, 1.1294045503933416, 1.0863715518085637, 1.0595066931551727, 1.0510765966239688, 1.0450454704666157, 1.0405983945959927, 1.0390748139682395, 1.0408312381562403, 1.0377879437786057, 1.036664882367785, 1.036847011916031, 1.035830342675568, 1.0328695729586856, 1.0319383653802512, 1.0311715991545152, 1.0382097079553325, 5.199675206782083e-14, -6.280784300645584e-14, 1.7572894478872925, 1.2965506573905121, 1.1976338295058695, 1.1257876533765185, 1.0960728604624892, 1.064120755473755, 1.0528910684289243, 1.0435150127015878, 1.0385644847851463, 1.034252305366325, 1.0324499737394108, 1.031387688981519, 1.0281269966167117, 1.0258706336502579, 1.0280202633926194, 1.0239304495095465, 1.7428986911066616e-05, 1.2515803621196965e-15, 4.925104670951977e-15, 4.5261360194493796e-15, -8.921610339140404e-16, 1.8284327968806626, 1.4137590150301325, 1.251793724468246, 1.1415649860574877, 1.1163429598219985, 1.0760327127307348, 1.0539221429616092, 1.062175481760724, 1.0396802413032402, 1.0319376557370283, 0.9536394355536806, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.019848561436393488};
    //mCorrs = {1.239642537066349, 1.1921088551407846, 1.1313732606963063, 1.110229805131554, 1.0910074182129839, 1.076277311306353, 1.069868398217456, 1.061514245762337, 1.059823998912975, 1.0564168135210326, 1.057245444346939, 1.0561172055353851, 1.0562643108031493, 1.0560631929956006, 1.0559207833637505, 1.056072732735473, 1.055225818378951, 1.0563192249853728, 1.0553482887912582, 1.055493668061604, 1.0513981093690314, 1.3471208053089627, 1.1985748800273093, 1.1464508603302277, 1.1106280880053427, 1.088500159047667, 1.0754716635324395, 1.066084347086856, 1.0620868116364865, 1.0585111980211381, 1.0579011920291654, 1.054530985931413, 1.0539615910836184, 1.0560806343084477, 1.0550958431186723, 1.0561621163402044, 1.0550763310206188, 1.055791687117243, 1.0552471689908656, 1.055134649533671, 1.0548868164593053, 1.0538487920650512, 1.356895187150072, 1.2460121092303467, 1.16938868184433, 1.1269941000881836, 1.0899634596426377, 1.0817840127355332, 1.0709200159178867, 1.0584672626514406, 1.0573788455969386, 1.0555058078404238, 1.0550153714670127, 1.05587391180079, 1.0558573137301004, 1.0556914911232453, 1.0569788799641207, 1.0584824028234747, 1.0584588875910734, 1.0560150660897767, 1.055075101074345, 1.0515471793701499, 1.0586132599800309, 1.3452417437467838, 1.2481329247837971, 1.1737221879117412, 1.1050376937798727, 1.0900752587864455, 1.0690620037962202, 1.055236050697627, 1.0512928624643767, 1.0485062784502612, 1.0470582008946934, 1.0486907084263495, 1.0480972088490241, 1.0502163284048176, 1.0529644725678116, 1.0518532918634143, 1.0523725976120253, 1.0509255778398798, 1.0493497910216814, 1.0488525532600042, 1.0412958800905574, 8.571880917142213e-14, 1.485881392627248, 1.2980310274835893, 1.1806575254983804, 1.105313294889267, 1.0770305436331367, 1.0551472773584205, 1.0502407158494635, 1.0456073258458036, 1.0447059761962705, 1.0430424353822623, 1.044733523869333, 1.0473989511066744, 1.049175712602343, 1.0467420501559048, 1.0483572733221078, 1.0453940502238135, 1.0420839017692294, 1.0228526900766364, 6.481891327243726e-05, 1.5452601296283148e-14, 1.5201740501853884e-13, 1.7064734536488344, 1.3861481355879766, 1.1892497928129226, 1.1160641147656405, 1.0829488788506263, 1.0656769674188384, 1.055233652303149, 1.0461003425530797, 1.0458032215962398, 1.0406128438525462, 1.037696876531623, 1.0382891994797232, 1.036411862330938, 1.0360319029755776, 1.0225517873354575, 1.0290594079061939, 1.5678353731577746e-05, 1.4060201475838442e-16, 1.1930258734564169e-15, -4.3909067264033153e-16, -4.568690348487973e-16, 1.6389061820333115, 1.435098564380208, 1.228883272949715, 1.1578751051838205, 1.1071891904100695, 1.0811034162506092, 1.0497981514076522, 1.0405147522586111, 1.0169234253551307, 0.9875071628359475, 0.893953219913553, 4.525542833981491e-05, 1.825495694680141e-46, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.025536694130001026};
    //mCorrs = {1.137747535571464, 1.152501786149164, 1.1212295462754625, 1.1056859636511343, 1.0794923149851738, 1.0569238052231293, 1.0469495319433886, 1.0390782611872311, 1.0366476011109365, 1.035146851459015, 1.032498037653653, 1.0318905377977872, 1.0312107537411808, 1.031178708611077, 1.0313367169506455, 1.031398337990366, 1.031122725181685, 1.0310837362566987, 1.0304183007192766, 1.029324064041424, 1.029239095000324, 1.199027958026756, 1.1985724192676661, 1.1392989440621477, 1.1007756471626988, 1.0716500183070812, 1.0622022470493735, 1.045051514987479, 1.0414938824601356, 1.036122666678906, 1.0332646867789186, 1.031798223354502, 1.0299316013944921, 1.0299863608635496, 1.030146379986312, 1.0297634856881317, 1.029745794673131, 1.0298665113385088, 1.030261555716178, 1.0292215802072973, 1.0286063812234332, 1.026924618897556, 1.2677935815341657, 1.214097139724866, 1.1525705800135448, 1.1200607381810477, 1.0838280471587396, 1.062193704463722, 1.052273633337584, 1.038941290700088, 1.0354394461980532, 1.030354147623587, 1.0322070012264672, 1.0312847304396577, 1.0297311549375543, 1.0302382675018085, 1.0304123165730452, 1.029995797370485, 1.0294593051400542, 1.0283854427901384, 1.0286113307331488, 1.0250084086558116, 1.0124616146370236, 1.3117633743810266, 1.2010086574621295, 1.1335913082574216, 1.0988231812701086, 1.0731208001143604, 1.0527359837821701, 1.0374879700425426, 1.0314812238412001, 1.0267221050999935, 1.0249866477957825, 1.0224869889618033, 1.0240422526762067, 1.0242200951172746, 1.0233092470072596, 1.022021081893008, 1.0224969090991247, 1.0210773212215556, 1.020314008513725, 1.0217841026654972, 1.0206981518260985, -2.704348492402445e-13, 1.37183841458067, 1.2345604526331775, 1.1744916174329585, 1.1060064015736146, 1.0645519552693312, 1.0368866923427593, 1.030501572329205, 1.0245176373795197, 1.02032315747228, 1.018744600719401, 1.0204331162493623, 1.017092927272428, 1.0165624736336503, 1.016566915331367, 1.0155399384657209, 1.012921058412904, 1.0107879345642732, 1.0122968853351528, 1.018662080453108, 5.864197734151932e-13, 1.6122486237233534e-13, 1.6695907217456987, 1.2666285072911336, 1.1596773062950727, 1.0964283523131602, 1.0691922287096194, 1.0389109335743656, 1.0283887572197894, 1.0207456237084604, 1.0160599378927786, 1.0128347812391414, 1.0115208623097975, 1.0102395696127315, 1.0078311663796282, 1.0056767293572177, 1.0071101017053796, 1.0066832210222016, 1.0298057928511142, 4.590835157486196e-13, 3.483058842230714e-14, -2.0185971885033365e-14, -2.325723608636007e-15, 1.7066695683003423, 1.3739895936860413, 1.2110957858846176, 1.1066456271021452, 1.0844021911757107, 1.0466562047639307, 1.0240084802967504, 1.033792607710178, 1.01295548400675, 1.006980445827197, 0.9315242388694518, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    //mCorrs = {1.139795989039566, 1.1548307060825496, 1.1229640060203967, 1.1071249523619058, 1.0804333834515023, 1.0574358644269173, 1.0472719882150716, 1.0392510909174208, 1.0367742259433366, 1.0352449482446713, 1.0325457826141677, 1.0319267346734744, 1.0312340284673824, 1.0312013741850672, 1.0313623861363916, 1.031425178542635, 1.0311443265556013, 1.031104596482434, 1.0304265115493634, 1.029311474310235, 1.0292248900758394, 1.2022413031568648, 1.20177710497147, 1.141376888560872, 1.1021212946968595, 1.0724420110058928, 1.0628146451970757, 1.0453378914797204, 1.0417126312125289, 1.036239312928939, 1.033327005124177, 1.0318326654066918, 1.0299305604609221, 1.0299863608635496, 1.030149421821443, 1.0297592490098728, 1.0297412217028714, 1.0298642330952001, 1.0302667869495217, 1.02920704234065, 1.0285801489177129, 1.0268664176393973, 1.2723141059924672, 1.2175969379081697, 1.1549008076630949, 1.1217729798219103, 1.0848515344148761, 1.062805940223725, 1.0526972965049326, 1.0391115167312164, 1.0355431049749326, 1.0303611389547984, 1.0322492138180166, 1.0313094114035417, 1.0297263036776678, 1.0302430560448823, 1.0304204136493136, 1.0299959767509104, 1.0294492862336382, 1.0283550106220731, 1.0285851925136371, 1.0249137817784448, 1.0121284832723865, 1.3171197293218095, 1.2042596540992736, 1.135560755179068, 1.1001317139942288, 1.073940751195234, 1.0531684358604996, 1.0376305696140553, 1.0315096399869892, 1.0266600542170317, 1.0248916072619265, 1.022344431919136, 1.0239292599488712, 1.0241104830320504, 1.0231823204302906, 1.0218696683307873, 1.0223545406303083, 1.0209079675262052, 1.020130144856249, 1.021628184318304, 1.0205215904247702, -2.755755987385992e-13, 1.3783367478339283, 1.238449241975955, 1.1772385464153208, 1.1074514815521865, 1.0652090194849864, 1.0370178621076394, 1.0305113660857828, 1.0244136813339653, 1.0201394677291653, 1.0185309038789827, 1.0202515167343738, 1.016847833445017, 1.01630729630818, 1.0163118224390066, 1.015265323567148, 1.012596660705808, 1.0104229878843447, 1.0119606225986593, 1.018446814968638, 5.975671427889212e-13, 1.642896176458933e-13, 1.6817490874540673, 1.2711268846228823, 1.1621426271090924, 1.09769136126032, 1.0699375008002114, 1.0390805825408913, 1.0283583880563725, 1.0205699647095263, 1.0157952077660883, 1.0125087434721973, 1.0111698479982407, 1.0098641989550594, 1.0074100139088977, 1.005214622767322, 1.006675242373744, 1.0062402470312901, 1.0298023603980782, 4.678103250334358e-13, 3.5492689961596216e-14, -2.0569691014181548e-14, -2.369933748372051e-15, 1.7195327731311865, 1.380528819128076, 1.21453853073958, 1.1081028582451264, 1.0854365924489404, 1.046973085121286, 1.0238948455835788, 1.0338649614087578, 1.0126317407025713, 1.0065431218413476, 0.9296525529322653, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    //mCorrs = {1.1202167747378704, 1.135251491780854, 1.1033847917187012, 1.0875457380602103, 1.0608541691498068, 1.0378566501252218, 1.027692773913376, 1.0196718766157253, 1.017195011641641, 1.0156657339429758, 1.0129665683124722, 1.0123475203717789, 1.011654814165687, 1.0116221598833717, 1.0117831718346961, 1.0118459642409394, 1.0115651122539058, 1.0115253821807384, 1.0108472972476679, 1.0097322600085394, 1.0096456757741439, 1.1826620888551693, 1.1821978906697745, 1.1217976742591764, 1.082542080395164, 1.0528627967041972, 1.0432354308953802, 1.025758677178025, 1.0221334169108334, 1.0166600986272434, 1.0137477908224815, 1.0122534511049963, 1.0103513461592266, 1.010407146561854, 1.0105702075197476, 1.0101800347081773, 1.010162007401176, 1.0102850187935046, 1.0106875726478262, 1.0096278280389546, 1.0090009346160174, 1.0072872033377018, 1.2527348916907717, 1.1980177236064742, 1.1353215933613994, 1.1021937655202148, 1.0652723201131806, 1.0432267259220296, 1.033118082203237, 1.0195323024295209, 1.0159638906732371, 1.010781924653103, 1.012669999516321, 1.0117301971018462, 1.0101470893759723, 1.0106638417431868, 1.010841199347618, 1.0104167624492149, 1.0098700719319427, 1.0087757963203776, 1.0090059782119416, 1.0053345674767493, 0.992549268970691, 1.297540515020114, 1.184680439797578, 1.1159815408773726, 1.0805524996925333, 1.0543615368935384, 1.033589221558804, 1.0180513553123598, 1.0119304256852937, 1.0070808399153361, 1.005312392960231, 1.0027652176174404, 1.0043500456471757, 1.004531268730355, 1.003603106128595, 1.0022904540290918, 1.0027753263286128, 1.0013287532245096, 1.0005509305545535, 1.0020489700166084, 1.0009423761230747, -2.755755987385992e-13, 1.3587575335322328, 1.2188700276742594, 1.1576593321136253, 1.087872267250491, 1.0456298051832909, 1.0174386478059438, 1.0109321517840872, 1.0048344670322698, 1.0005602534274698, 0.9989516895772872, 1.0006723024326782, 0.9972686191433215, 0.9967280820064846, 0.9967326081373111, 0.9956861092654525, 0.9930174464041124, 0.9908437735826492, 0.9923814082969638, 0.9988676006669426, 5.975671427889212e-13, 1.642896176458933e-13, 1.6621698731523717, 1.2515476703211867, 1.142563412807397, 1.0781121469586246, 1.050358286498516, 1.0195013682391958, 1.008779173754677, 1.0009907504078308, 0.9962159934643928, 0.9929295291705018, 0.9915906336965452, 0.9902849846533639, 0.9878307996072022, 0.9856354084656265, 0.9870960280720484, 0.9866610327295946, 1.0102231460963826, 4.678103250334358e-13, 3.5492689961596216e-14, -2.0569691014181548e-14, -2.369933748372051e-15, 1.699953558829491, 1.3609496048263805, 1.1949593164378844, 1.088523643943431, 1.065857378147245, 1.0273938708195904, 1.0043156312818833, 1.0142857471070623, 0.9930525264008758, 0.9869639075396521, 0.9100733386305698, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    mCorrs = {1.1593752033412614, 1.174409920384245, 1.1425432203220922, 1.1267041666636013, 1.1000125977531978, 1.0770150787286128, 1.0668512025167671, 1.0588303052191164, 1.056353440245032, 1.0548241625463668, 1.0521249969158633, 1.05150594897517, 1.050813242769078, 1.0507805884867627, 1.0509416004380872, 1.0510043928443304, 1.0507235408572968, 1.0506838107841294, 1.050005725851059, 1.0488906886119305, 1.048804104377535, 1.2218205174585604, 1.2213563192731656, 1.1609561028625675, 1.121700508998555, 1.0920212253075883, 1.0823938594987712, 1.064917105781416, 1.0612918455142244, 1.0558185272306344, 1.0529062194258725, 1.0514118797083873, 1.0495097747626176, 1.049565575165245, 1.0497286361231386, 1.0493384633115683, 1.049320436004567, 1.0494434473968957, 1.0498460012512172, 1.0487862566423456, 1.0481593632194084, 1.0464456319410929, 1.2918933202941627, 1.2371761522098652, 1.1744800219647904, 1.1413521941236058, 1.1044307487165717, 1.0823851545254206, 1.0722765108066281, 1.058690731032912, 1.0551223192766281, 1.049940353256494, 1.0518284281197121, 1.0508886257052372, 1.0493055179793633, 1.0498222703465778, 1.049999627951009, 1.0495751910526059, 1.0490285005353337, 1.0479342249237686, 1.0481644068153326, 1.0444929960801403, 1.031707697574082, 1.336698943623505, 1.2238388684009691, 1.1551399694807636, 1.1197109282959243, 1.0935199654969294, 1.072747650162195, 1.0572097839157508, 1.0510888542886847, 1.0462392685187272, 1.044470821563622, 1.0419236462208314, 1.0435084742505667, 1.043689697333746, 1.042761534731986, 1.0414488826324828, 1.0419337549320038, 1.0404871818279007, 1.0397093591579445, 1.0412073986199994, 1.0401008047264657, -2.755755987385992e-13, 1.3979159621356239, 1.2580284562776505, 1.1968177607170163, 1.127030695853882, 1.0847882337866819, 1.0565970764093349, 1.0500905803874783, 1.0439928956356608, 1.0397186820308608, 1.0381101181806782, 1.0398307310360693, 1.0364270477467126, 1.0358865106098756, 1.0358910367407022, 1.0348445378688436, 1.0321758750075034, 1.0300022021860402, 1.0315398369003548, 1.0380260292703336, 5.975671427889212e-13, 1.642896176458933e-13, 1.7013283017557628, 1.2907060989245778, 1.181721841410788, 1.1172705755620156, 1.089516715101907, 1.0586597968425868, 1.047937602358068, 1.0401491790112218, 1.0353744220677839, 1.0320879577738928, 1.0307490622999362, 1.029443413256755, 1.0269892282105932, 1.0247938370690175, 1.0262544566754395, 1.0258194613329856, 1.0493815746997737, 4.678103250334358e-13, 3.5492689961596216e-14, -2.0569691014181548e-14, -2.369933748372051e-15, 1.739111987432882, 1.4001080334297715, 1.2341177450412755, 1.127682072546822, 1.105015806750636, 1.0665522994229815, 1.0434740598852743, 1.0534441757104533, 1.0322109550042669, 1.0261223361430432, 0.9492317672339609, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  }

  // Pass basic quality checks, at least two jets, jet ids ok for these two.
  if (_pass and _pass_qcdmet and njt>1 and _jetids[0] and _jetids[1]) {
    int idx = -1;
    vector<int> tfired;
    for (auto &trg : _trigs) {
      idx = -1;
      for (unsigned i = 0; i < jp::triggers.size(); ++i) if (string(jp::triggers[i])==trg) { idx = i; break; }
      if (idx!=-1) tfired.push_back(idx);
    }
    // Tag & probes method
    // We do implicit binning or histogramming by tag pt, so no need to look at firing triggers.
    double metfx = cos(metphi1), metfy = sin(metphi1);
    double metpx = met1*metfx, metpy = met1*metfy;
    bool trigfire = false;
    for (unsigned tagi = 0; tagi<2; ++tagi) {
      double tagpt = jtpt[tagi];
      // We do not bin below 28 GeV.
      if (tagpt<28) continue;
      // We want the tag trigger to have fired
      if (jp::isdt) {
        bool fired = false;
        for (auto &tf : tfired) {
          auto &rang = jp::trigranges[tf];
          if (tagpt>rang[0] and tagpt<rang[1]) {
            fired = true;
            break;
          }
        }
        if (!fired) continue;
      }
      trigfire = true;
      // We setup the probe
      unsigned pr1i = tagi==0 ? 1 : 0;
      double pr1pt = jtpt[pr1i];
      // We want a strong, independent tag.
      if (tagpt/pr1pt<0.7) continue;
      // Remaining tag info
      double tagphi = jtphi[tagi];
      double tagfx = cos(tagphi), tagfy = sin(tagphi);
      double tagpx = tagpt*tagfx, tagpy = tagpt*tagfy;
      double tageta = jteta[tagi];
      double atageta = fabs(tageta);
      // Remaining probe info
      double pr1phi = jtphi[pr1i];
      double pr1fx = cos(pr1phi), pr1fy = sin(pr1phi);
      double pr1px = pr1pt*pr1fx, pr1py = pr1pt*pr1fy;
      double pr1eta = jteta[pr1i];

      // We calculate and fill MPF values.
      double metproj = tagfx*metpx+tagfy*metpy;
      double MPF  = 1 + metproj/tagpt;
      double MPF2 = MPF*MPF;
      h->p2mpf->Fill(tagpt,atageta,MPF);
      h->p2mpf2->Fill(tagpt,atageta,MPF2);
      h->p2mpf_fe->Fill(tagpt,tageta,MPF);
      h->p2mpf2_fe->Fill(tagpt,tageta,MPF2);
      h->h3mpf->Fill(tagpt,tageta,MPF);

      // We calculate and fill HT sum values.
      // We need to add the leading probe manually.
      //double HTproj = 0.0;
      //HTproj += tagfx*pr1px + tagfy*pr1py;
      //for (int jidx = 2; jidx < njt; ++jidx) {
      //  double jpt = jtpt[jidx];
      //  double jfx = TMath::Cos(jtphi[jidx]), jfy = TMath::Sin(jtphi[jidx]);
      //  double jpx = jpt*jfx, jpy = jpt*jfy;
      //  HTproj += tagfx*jpx+tagfy*jpy;
      //}
      //double HTF  = -HTproj/tagpt;
      //double HTF2 = HTF*HTF;
      //h->p2htf->Fill(tagpt,atageta,HTF);
      //h->p2htf2->Fill(tagpt,atageta,HTF2);
      //h->p2htf_fe->Fill(tagpt,tageta,HTF);
      //h->p2htf2_fe->Fill(tagpt,tageta,HTF2);
      //h->h3htf->Fill(tagpt,tageta,HTF);
    }

    //if (trigfire) {
    if (true) {
      for (unsigned tagi = 0; tagi<1; ++tagi) {
        // We setup the tag
        double tagpt = jtpt[tagi];
        double tageta = jteta[tagi];
        double atageta = fabs(tageta);
        if (tagpt<28 or atageta>1.3) continue;
        // Require a trigger fire for tag.
        if (jp::isdt) {
          bool fired = false;
          for (auto &tf : tfired) {
            auto &rang = jp::trigranges[tf];
            if (tagpt>rang[0] and tagpt<rang[1]) {
              fired = true;
              break;
            }
          }
          if (!fired) continue;
        }
        // We setup the probe
        unsigned pr1i = tagi==0 ? 1 : 0;
        double pr1pt = jtpt[pr1i];
        double pr1eta = jteta[pr1i];
        double ptfrac = pr1pt/tagpt;
        if (tagpt/pr1pt<0.7) continue;
        int tagbin = h->PhaseBin(tagpt,tageta);
        int pr1bin = h->PhaseBin(pr1pt,pr1eta);
        if (tagbin<0 or pr1bin<0) continue;
        // We want a strong, independent tag.
        // Remaining tag info
        double tagphi = jtphi[tagi];
        double tagfx = cos(tagphi), tagfy = sin(tagphi);
        double tagpx = tagpt*tagfx, tagpy = tagpt*tagfy;
        // Remaining probe info
        double pr1phi = jtphi[pr1i];
        double pr1fx = cos(pr1phi), pr1fy = sin(pr1phi);
        double pr1px = pr1pt*pr1fx, pr1py = pr1pt*pr1fy;
          // We use the tag and probe inherited from above.
        double refpt = tagpt;

        unsigned ntot = (h->mNPts)*(h->mNEtas)+1;
        vector<double> tmpVec (ntot,0.0);
        vector<double> tmpVecT(ntot,0.0);
        //vector<double> tmpVecQG (3*ntot,0.0);
        //vector<double> tmpVecQGT(3*ntot,0.0);
        // Tag jet informatics
        double tagSum = 0.0;
        double tagSumU = 0.0;
        double collector = -1.0;
        double ctag = 1.0;
        //double ctag = mCorrs[tagbin];
        // Special handling of the missing hadronic recoil
        double htx = -met0*cos(metphi0)-jtptu[tagi]*tagfx-jtptu[pr1i]*pr1fx;
        double hty = -met0*sin(metphi0)-jtptu[tagi]*tagfy-jtptu[pr1i]*pr1fy;
        tmpVec [pr1bin] += (tagfx*pr1px+tagfy*pr1py)/refpt;
        tmpVecT[pr1bin] += (tagfy*pr1px-tagfx*pr1py)/refpt;
        // We need to start with the third jet, as we don't know which is the tag.
        for (int jidx = 2; jidx < njt; ++jidx) {
          // Only take the good jets.
          //if (!_jetids[jidx]) continue;
          double jpt = jtpt[jidx];
          double jpx = jpt*cos(jtphi[jidx]), jpy = jpt*sin(jtphi[jidx]);
          double jptu = jtptu[jidx];
          double proj  = (tagfx*jpx+tagfy*jpy)/refpt;
          double projT = (tagfy*jpx-tagfx*jpy)/refpt;

          collector -= proj;
          int pbin = h->PhaseBin(jpt,jteta[jidx]);
          if (pbin>=0) {
            tagSum  += proj*mCorrs[pbin];
            tagSumU += proj;
            tmpVec [pbin] += proj/ctag;
            tmpVecT[pbin] += projT/ctag;
            htx -= jptu*cos(jtphi[jidx]);
            hty -= jptu*sin(jtphi[jidx]);

            //double qglV = qgl[jidx];
            //if (qglV>0.85) {
            //  // Values for quarks
            //  tmpVecQG [pbin] += proj;
            //  tmpVecQGT[pbin] += projT;
            //} else if (qglV<0.3 and qglV>=0) {
            //  // Values for quarks
            //  tmpVecQG [pbin+ntot] += proj;
            //  tmpVecQGT[pbin+ntot] += projT;
            //} else {
            //  // Values for gluons and untagged
            //  tmpVecQG [pbin+2*ntot] += proj;
            //  tmpVecQGT[pbin+2*ntot] += projT;
            //}
          }
        }
        tmpVec .back() = (tagfx*htx+tagfy*hty)/refpt;
        tmpVecT.back() = (tagfy*htx-tagfx*hty)/refpt;
        if (tagbin>=0) {
          (*(h->mColumn)) [tagbin][2] += tagSum;
          (*(h->mColumn)) [tagbin][3] += tagSumU;
          (*(h->mColumn)) [tagbin][4] += 1;
          (*(h->mSquare)) [tagbin][tagbin+2*ntot] += 1;
        }
        (*(h->mSingle))[0][0] += 1.0;
        (*(h->mSingle))[0][1] += collector;
        for (unsigned i = 0; i < ntot; ++i) {
          double tval  = tmpVec [i];
          double tvalT = tmpVecT[i];
          (*(h->mColumn))[i][0]      += tval;
          (*(h->mColumn))[i][1]      += tval*collector;
          (*(h->mSquare))[i][i]      += tval*tval;
          (*(h->mColumn))[i][2     ] += tvalT;
          (*(h->mSquare))[i][i+ntot] += tvalT*tvalT;
          for (unsigned j = i+1; j < ntot; ++j) {
            double tval2  = tval *tmpVec [j];
            double tvalT2 = tvalT*tmpVecT[j];
            (*(h->mSquare))[i][j     ] += tval2;
            (*(h->mSquare))[j][i     ] += tval2;
            (*(h->mSquare))[i][j+ntot] += tvalT2;
            (*(h->mSquare))[j][i+ntot] += tvalT2;
          }
        }
        //for (unsigned i = 0; i < 3*ntot; ++i) {
        //  double tval  = tmpVecQG [i];
        //  double tvalT = tmpVecQGT[i];
        //  (*(h->mTColumn))[i][0]      += tval;
        //  (*(h->mTSquare))[i][i]      += tval*tval;
        //  (*(h->mTSquare))[i][i+3*ntot] += tvalT*tvalT;
        //  for (unsigned j = i+1; j < 3*ntot; ++j) {
        //    double tval2  = tval *tmpVecQG [j];
        //    double tvalT2 = tvalT*tmpVecQGT[j];
        //    (*(h->mTSquare))[i][j]        += tval2;
        //    (*(h->mTSquare))[j][i]        += tval2;
        //    (*(h->mTSquare))[i][j+3*ntot] += tvalT2;
        //    (*(h->mTSquare))[j][i+3*ntot] += tvalT2;
        //  }
        //}
      }
    }

    //if (fabs(etatag)<1.3 and pttag>jp::wwptrange[0]) {
      //int ptbin = -1;
      //for (unsigned ptloc = 0; ptloc < jp::nwwpts; ++ptloc) {
      //  if (pttag < jp::wwptrange[ptloc+1]) {
      //    ptbin = ptloc;
      //    break;
      //  }
      //}
      //// We want to be inside a supported bin and we check the quality of the four leading jets
      //if (ptbin>0 and (njt<2 or _jetids[1]) and (njt<3 or _jetids[2]) and (njt<4 or _jetids[3]) and (njt<5 or _jetids[4])) {
      //  // This code sector is separated from standard weighting strategies: for data, no weight is applied
      //  double wt = 1.0;
      //  if (!jp::isdt) wt = _w;

      //  // Let's create a universal tag unit vector
      //  double ju_px = cos(jtphi[0]);
      //  double ju_py = sin(jtphi[0]);

      //  // Let's fill MET histos and pttag histos
      //  double pttag_nol2l3 = jtpt_nol2l3[0];
      //  double projmet = ju_px * cos(metphi1) + ju_py * sin(metphi1);
      //  double projmet_nol2l3 = ju_px * cos(metphi1_nol2l3) + ju_py * sin(metphi1_nol2l3);
      //  h->pmetave         ->Fill(pttag,       projmet       *met1,       wt);
      //  h->pmetave_nol2l3  ->Fill(pttag_nol2l3,projmet_nol2l3*met1_nol2l3,wt);
      //  h->ppttagave       ->Fill(pttag,       pttag,                     wt);
      //  h->ppttagave_nol2l3->Fill(pttag_nol2l3,pttag_nol2l3,              wt);

      //  // First jet
      //  int pidx = 1;
      //  if (pidx<njt and jtpt[pidx]>jp::recopt) {
      //    double proj1 = ju_px*cos(jtphi[pidx]) + ju_py*sin(jtphi[pidx]);
      //    double etaj1       = jteta      [pidx];
      //    double ptj1        = jtpt       [pidx];
      //    double ptj1_nol2l3 = jtpt_nol2l3[pidx];
      //    h->h2njet1        [ptbin]->Fill(ptj1,       etaj1,                  wt);
      //    h->h2njet1_nol2l3 [ptbin]->Fill(ptj1_nol2l3,etaj1,                  wt);
      //    h->p2ptjet1       [ptbin]->Fill(ptj1,       etaj1,proj1*ptj1,       wt);
      //    h->p2ptjet1_nol2l3[ptbin]->Fill(ptj1_nol2l3,etaj1,proj1*ptj1_nol2l3,wt);
      //  }
      //  // Second jet
      //  pidx = 2;
      //  if (pidx<njt and jtpt[pidx]>jp::recopt) {
      //    double proj2 = ju_px*cos(jtphi[pidx]) + ju_py*sin(jtphi[pidx]);
      //    double etaj2       = jteta      [pidx];
      //    double ptj2        = jtpt       [pidx];
      //    double ptj2_nol2l3 = jtpt_nol2l3[pidx];
      //    h->h2njet2        [ptbin]->Fill(ptj2,       etaj2,                  wt);
      //    h->h2njet2_nol2l3 [ptbin]->Fill(ptj2_nol2l3,etaj2,                  wt);
      //    h->p2ptjet2       [ptbin]->Fill(ptj2,       etaj2,proj2*ptj2,       wt);
      //    h->p2ptjet2_nol2l3[ptbin]->Fill(ptj2_nol2l3,etaj2,proj2*ptj2_nol2l3,wt);
      //  }
      //  // Third jet
      //  pidx = 3;
      //  if (pidx<njt and jtpt[pidx]>jp::recopt) {
      //    double proj3 = ju_px*cos(jtphi[pidx]) + ju_py*sin(jtphi[pidx]);
      //    double etaj3       = jteta      [pidx];
      //    double ptj3        = jtpt       [pidx];
      //    double ptj3_nol2l3 = jtpt_nol2l3[pidx];
      //    h->h2njet3        [ptbin]->Fill(ptj3,       etaj3,                  wt);
      //    h->h2njet3_nol2l3 [ptbin]->Fill(ptj3_nol2l3,etaj3,                  wt);
      //    h->p2ptjet3       [ptbin]->Fill(ptj3,       etaj3,proj3*ptj3,       wt);
      //    h->p2ptjet3_nol2l3[ptbin]->Fill(ptj3_nol2l3,etaj3,proj3*ptj3_nol2l3,wt);
      //  }
      //  // Fourth jet & friends
      //  pidx = 4;
      //  if (pidx<njt and jtpt[pidx]>jp::recopt) {
      //    // First, we seek for the jet<->bin connections
      //    map<int,int> jet2bin;
      //    map<int,int> jet2bin_nol2l3;
      //    for (int jidx = 4; jidx < njt; ++jidx) {
      //      jet2bin       [jidx] = h->h2njet4plus[ptbin]->FindBin(jtpt       [jidx],jteta[jidx]);
      //      jet2bin_nol2l3[jidx] = h->h2njet4plus[ptbin]->FindBin(jtpt_nol2l3[jidx],jteta[jidx]);
      //    }
      //    // Loop over the remaining jets (we do not consider jetids here).
      //    // For each jet, we fill the bin that corresponds to it with a sum over the jets within that bin.
      //    for (pidx = 4; pidx < njt; ++pidx) {
      //      int assoc        = jet2bin       [pidx];
      //      int assoc_nol2l3 = jet2bin_nol2l3[pidx];
      //      // This is an indicator that this jet has already been considered
      //      if (assoc==-1 and assoc_nol2l3==-1) continue;

      //      double cumul        = 0;
      //      double cumul_nol2l3 = 0;
      //      // We loop over jets, including the current jet, and take the cumulative sum corresponding to the bin of this jet
      //      // A jet already considered is marked with the tag '-1'
      //      for (int jidx = pidx; jidx < njt; ++jidx) {
      //        bool match        = (assoc == jet2bin[jidx]);
      //        bool match_nol2l3 = (assoc_nol2l3 == jet2bin_nol2l3[jidx]);
      //        if (match or match_nol2l3) {
      //          double proj4p = ju_px*cos(jtphi[jidx]) + ju_py*sin(jtphi[jidx]);
      //          if (match) {
      //            cumul        += proj4p*jtpt       [jidx];
      //            jet2bin       [jidx] = -1;
      //          }
      //          if (match_nol2l3) {
      //            cumul_nol2l3 += proj4p*jtpt_nol2l3[jidx];
      //            jet2bin_nol2l3[jidx] = -1;
      //          }
      //        }
      //      }
      //      double etaj4 = jteta[pidx];
      //      if (assoc!=-1) {
      //        double ptj4 = jtpt[pidx];
      //        h->h2njet4plus [ptbin]->Fill(ptj4,etaj4,      wt);
      //        h->p2ptjet4plus[ptbin]->Fill(ptj4,etaj4,cumul,wt);
      //      }
      //      if (assoc_nol2l3!=-1) {
      //        double ptj4_nol2l3 = jtpt_nol2l3[pidx];
      //        h->h2njet4plus_nol2l3 [ptbin]->Fill(ptj4_nol2l3,etaj4,             wt);
      //        h->p2ptjet4plus_nol2l3[ptbin]->Fill(ptj4_nol2l3,etaj4,cumul_nol2l3,wt);
      //      }
      //    } // 4th jet & friends jet loop
      //  } // 4th jet & friends
      //}
    //} // Tag within barrel and the pt range
  } // Basic sanity checks
} // FillAll


// Write and delete histograms
void HistosFill::WriteAll()
{
  for (auto &hh : _allhistos) {
    TDirectory *curdir = gDirectory;
    string name = hh.first;
    TFile *f = _outfile;
    assert(f && !f->IsZombie());
    TDirectory *dir = f->GetDirectory(name.c_str()); assert(dir);
    dir->cd();
    HistosAll *h = hh.second;
    // We create correctly scaled end user histograms to reduce future error rate
    //for (unsigned idx = 0; idx < jp::nwwpts; ++idx) {
    //  // Total event counts are best fetched from the MET histos
    //  double sf        = h->pmetave       ->GetBinEntries(idx+1);
    //  double sf_nol2l3 = h->pmetave_nol2l3->GetBinEntries(idx+1);
    //  sf        = (sf>0        ? 1.0/sf        : 1.0);
    //  sf_nol2l3 = (sf_nol2l3>0 ? 1.0/sf_nol2l3 : 1.0);

    //  int num = jp::wwptrange[idx];
    //  string number = std::to_string(num);
    //  // Get the histos as projections
    //  h->h2ptjet1           [idx] = h->p2ptjet1           [idx]->ProjectionXY((string("h2ptj1_")        +number+string("GeV")).c_str(),"e");
    //  h->h2ptjet2           [idx] = h->p2ptjet2           [idx]->ProjectionXY((string("h2ptj2_")        +number+string("GeV")).c_str(),"e");
    //  h->h2ptjet3           [idx] = h->p2ptjet3           [idx]->ProjectionXY((string("h2ptj3_")        +number+string("GeV")).c_str(),"e");
    //  h->h2ptjet4plus       [idx] = h->p2ptjet4plus       [idx]->ProjectionXY((string("h2ptj4p_")       +number+string("GeV")).c_str(),"e");
    //  h->h2ptjet1_nol2l3    [idx] = h->p2ptjet1_nol2l3    [idx]->ProjectionXY((string("h2ptj1_nol2l3_") +number+string("GeV")).c_str(),"e");
    //  h->h2ptjet2_nol2l3    [idx] = h->p2ptjet2_nol2l3    [idx]->ProjectionXY((string("h2ptj2_nol2l3_") +number+string("GeV")).c_str(),"e");
    //  h->h2ptjet3_nol2l3    [idx] = h->p2ptjet3_nol2l3    [idx]->ProjectionXY((string("h2ptj3_nol2l3_") +number+string("GeV")).c_str(),"e");
    //  h->h2ptjet4plus_nol2l3[idx] = h->p2ptjet4plus_nol2l3[idx]->ProjectionXY((string("h2ptj4p_nol2l3_")+number+string("GeV")).c_str(),"e");
    //  // Multiply by bin event counts
    //  h->h2ptjet1           [idx]->Multiply(h->h2njet1           [idx]);
    //  h->h2ptjet2           [idx]->Multiply(h->h2njet2           [idx]);
    //  h->h2ptjet3           [idx]->Multiply(h->h2njet3           [idx]);
    //  h->h2ptjet4plus       [idx]->Multiply(h->h2njet4plus       [idx]);
    //  h->h2ptjet1_nol2l3    [idx]->Multiply(h->h2njet1_nol2l3    [idx]);
    //  h->h2ptjet2_nol2l3    [idx]->Multiply(h->h2njet2_nol2l3    [idx]);
    //  h->h2ptjet3_nol2l3    [idx]->Multiply(h->h2njet3_nol2l3    [idx]);
    //  h->h2ptjet4plus_nol2l3[idx]->Multiply(h->h2njet4plus_nol2l3[idx]);
    //  // Divide by total event counts
    //  h->h2ptjet1           [idx]->Scale(sf);
    //  h->h2ptjet2           [idx]->Scale(sf);
    //  h->h2ptjet3           [idx]->Scale(sf);
    //  h->h2ptjet4plus       [idx]->Scale(sf);
    //  h->h2ptjet1_nol2l3    [idx]->Scale(sf_nol2l3);
    //  h->h2ptjet2_nol2l3    [idx]->Scale(sf_nol2l3);
    //  h->h2ptjet3_nol2l3    [idx]->Scale(sf_nol2l3);
    //  h->h2ptjet4plus_nol2l3[idx]->Scale(sf_nol2l3);
    //}
    curdir->cd();
  }

  // Report memory usage to avoid malloc problems when writing file
  PrintInfo("WriteAll():");
  PrintMemInfo();

  for (auto histit = _allhistos.begin(); histit != _allhistos.end(); ++histit) {
    HistosAll *h = histit->second;
    delete h;
  } // for histit

  PrintInfo(Form("\nOutput (HistosAll) stored in %s",_outfile->GetName()),true);

  // Report memory usage to avoid malloc problems when writing file
  PrintInfo("WriteAll() finished:");
  PrintMemInfo();
} // WriteAll


void HistosFill::FillJetID(vector<bool> &id)
{
  assert(int(id.size())==njt);

  for (int jetidx = 0; jetidx != njt; ++jetidx) {
    id[jetidx] = jtidtight[jetidx];
    float etabs = fabs(jteta[jetidx]);
    if (jp::yid==0) { // 2016 specialities
      // Loose ID is still in use
      if (etabs>2.5) id[jetidx] = jtidloose[jetidx];
      // TightLepVeto
      if (etabs<=2.7 and (jtcef[jetidx]>=0.90 or jtmuf[jetidx]>=0.80)) id[jetidx] = false;
    } else if (jp::yid==1) { // 2017 specialities
      if (etabs<=2.7 and (jtcef[jetidx]>=0.80 or jtmuf[jetidx]>=0.80)) id[jetidx] = false;
    } else if (jp::yid==2) {
      if (etabs<=2.7 and (jtcef[jetidx]>=0.80 or jtmuf[jetidx]>=0.80)) id[jetidx] = false;
    }

    if (jp::doVetoHot) {
      // Abort if one of the leading jets is in a difficult zone
      assert(h2HotExcl);
      bool good = h2HotExcl->GetBinContent(h2HotExcl->FindBin(jteta[jetidx],jtphi[jetidx])) <= 0;
      id[jetidx] = (id[jetidx] and good);
    }
  }
} // FillJetID


// Load good run and LS information
bool HistosFill::LoadJSON()
{
  PrintInfo(string("Processing LoadJSON() with ") + jp::json + " ...",true);
  ifstream file(jp::json, ios::in);
  if (!file.is_open()) return false;
  char c;
  string s, s2;
  char s1[256];
  int rn(0), ls1(0), ls2(0), nrun(0), nls(0);
  file.get(c);
  if (c!='{') return false;
  while (file >> s and sscanf(s.c_str(),"\"%d\":",&rn)==1) {
    if (jp::debug) PrintInfo(Form("\"%d\": ",rn),true);

    while (file.get(c) and c==' ') {};
    if (jp::debug) { PrintInfo(Form("%c",c),true); assert(c=='['); }
    ++nrun;

    bool endrun = false;
    while (!endrun and file >> s >> s2 and sscanf((s+s2).c_str(),"[%d,%d]%s",&ls1,&ls2,s1)==3) {
      if (jp::debug) PrintInfo(Form("[%d,%d,%s]",ls1,ls2,s1),true);

      for (int ls = ls1; ls != ls2+1; ++ls) {
        _json[rn][ls] = 1;
        ++nls;
      }

      s2 = s1;
      endrun = (s2=="]," || s2=="]}");
      if (jp::debug and !endrun and s2!=",") { PrintInfo(string("s1: ")+s2,true); assert(s2==","); }
    } // while ls
    if (jp::debug) PrintInfo("",true);

    if (s2=="]}") continue;
    else if (jp::debug and s2!="],") PrintInfo(string("s2: ")+s2,true); assert(s2=="],");
  } // while run
  if (s2!="]}") { PrintInfo(string("s3: ")+s2,true); return false; }

  PrintInfo(string("Called LoadJSON() with ") + jp::json + ":",true);
  PrintInfo(Form("Loaded %d good runs and %d good lumi sections",nrun,nls));
  return true;
} // LoadJSON


// Load luminosity information
bool HistosFill::LoadLumi()
{
  PrintInfo(string("Processing LoadLumi() with ") + jp::lumifile + "...",true);

  // Check lumi against the list of good runs
  const int a_goodruns[] = {};
  const int ngoodruns = sizeof(a_goodruns)/sizeof(a_goodruns[0]);
  set<int> goodruns;
  if (ngoodruns>0) { // This is an old remnant
    for (int runidx = 0; runidx != ngoodruns; ++runidx)
      goodruns.insert(a_goodruns[runidx]);

    for (auto runit = goodruns.begin(); runit != goodruns.end(); ++runit) cout << *runit << ", ";
    cout << endl;
  }
  set<pair<int, int> > nolums;

  ifstream f(jp::lumifile, ios::in);
  if (!f.is_open()) return false;
  float secLS = 2.3310e+01;
  string s;
  int rn, fill, ls, ifoo;
  float del, rec, avgpu, energy;
  char sfoo[512];
  bool getsuccess1 = static_cast<bool>(getline(f, s, '\n'));
  if (!getsuccess1) return false;
  PrintInfo(string("\nstring: ") + s + " !",true);

  // HOX: the lumi file format has been changing. Change the conditions when needed.
  if (s!="#Data tag : 19v2 , Norm tag: None") return false;

  bool getsuccess2 = static_cast<bool>(getline(f, s, '\n'));
  if (!getsuccess2) return false;
  PrintInfo(string("\nstring: ") + s + " !",true);
  if (s!="#run:fill,ls,time,beamstatus,E(GeV),delivered(/ub),recorded(/ub),avgpu,source") return false;

  int nls(0);
  double lumsum(0);
  double lumsum_good(0);
  double lumsum_json(0);
  bool skip(false);
  while (getline(f, s, '\n')) {
    // Skip if not STABLE BEAMS or wrong number of arguments
    // STABLE BEAMS alts: ADJUST, BEAM DUMP, FLAT TOP, INJECTION PHYSICS BEAM, N/A, RAMP DOWN, SETUP, SQUEEZE
    if (sscanf(s.c_str(),"%d:%d,%d:%d,%d/%d/%d %d:%d:%d,STABLE BEAMS,%f,%f,%f,%f,%s",
        &rn,&fill,&ls,&ifoo,&ifoo,&ifoo,&ifoo,&ifoo,&ifoo,&ifoo,&energy,&del,&rec,&avgpu,sfoo)!=15)
      skip=true;

    if (jp::debug) PrintInfo(Form("Run %d ls %d lumi %f/pb",rn,ls,rec*1e-6),true);

    if (skip) { // The user should know if this happens, since we can choose to use only STABLE BEAMS
      if (skip) PrintInfo(string("Skipping line (effects the recorded lumi):\n")+s,true);
      skip = false;
      continue;
    }

    if (_lums[rn][ls]!=0) return false;
    if (_avgpu[rn][ls]!=0) return false;
    // lumiCalc.py returns lumi in units of mub-1 (=>nb-1=>pb-1)
    double lum = rec*1e-6;
    double lum2 = del*1e-6;
    if (lum==0 and goodruns.find(rn)!=goodruns.end() and (!jp::dojson or _json[rn][ls]==1))
      nolums.insert(pair<int, int>(rn,ls));

    _avgpu[rn][ls] = avgpu; // * 69000. / 78400.; // brilcalc --minBiasXsec patch
    _lums[rn][ls] = lum;
    _lums2[rn][ls] = lum2;
    lumsum += lum;
    if (goodruns.find(rn)!=goodruns.end()) // Apr 17
      lumsum_good += lum;
    if ((!jp::dojson || _json[rn][ls]))
      lumsum_json += lum;
    ++nls;
    if (nls>100000000) return false;
  }

  PrintInfo(Form("Called LoadLumi() with %s:\nLoaded %lu runs with %d lumi sections containing %f"
                 " pb-1 of data,\n of which %f pb-1 is in good runs (%f%%)\nThis corresponds to %f"
                 " hours of data-taking\nThe JSON file contains %f pb-1 (%f%%)",
                 jp::lumifile,_lums.size(),nls,lumsum,lumsum_good,
                 100.*lumsum_good/lumsum,nls*secLS/3600,lumsum_json,100.*lumsum_json/lumsum),true);

  // Report any empty lumi section
  if (nolums.size()!=0) {
    PrintInfo(Form("Warning, found %lu non-normalizable LS:",nolums.size()),true);
    for (auto lumit = nolums.begin(); lumit != nolums.end(); ++lumit) {
      cout << " ["<<lumit->first<<","<<lumit->second;
      auto lumit2 = lumit;
      ++lumit2;
      if (lumit2->first!=lumit->first or lumit2->second!=lumit->second+1) cout << "]";
      else {
        for (int lumadd = 0; lumit2!=nolums.end() and lumit2->first==lumit->first and
                             lumit2->second==lumit->second+lumadd+1; ++lumadd, ++lumit2) {};
        lumit = --lumit2;
        cout << "-" << lumit->second << "]";
      }
    } // for lumit
    cout << endl;
  } // nolums
  return true;
} // LoadLumi


bool HistosFill::LoadPuProfiles()
{
  string datafile = jp::pudtpath + jp::run + "/pileup_DT.root";
  string mcfile   = jp::pumcpath;
  if (jp::isnu)      mcfile += "pileup_NU.root";
  else if (jp::ishw) mcfile += jp::puhwfile;
  else if (jp::ispy) {
    if (jp::pthatbins)   mcfile += "pileup_Pthat.root";
    else if (jp::htbins) mcfile += "pileup_P8MG.root";
    else                 mcfile += jp::pup8file;
  } else {
    PrintInfo("Problems with PU file types!",true);
    return false;
  }

  PrintInfo(Form("Processing LoadPuProfiles() using %s and %s ...",datafile.c_str(),mcfile.c_str()),true);

  TDirectory *curdir = gDirectory;
  // Load pile-up files and hists from them
  TFile *f_pudist = new TFile(datafile.c_str(), "READ");
  if (!f_pudist or f_pudist->IsZombie()) { PrintInfo(string("Unable to read PU data file")+datafile,true); return false; }
  TFile *fpumc = new TFile(mcfile.c_str(),"READ");
  if (!fpumc or fpumc->IsZombie()) { PrintInfo(string("Unable to read PU MC file")+mcfile,true); return false; }

  _pumc = dynamic_cast<TH1D*>(fpumc->Get("pileupmc")->Clone("pumchelp"));
  if (!_pumc) return false;
  double maxmcpu = _pumc->GetMaximum();
  _pumc->Scale(1.0/maxmcpu);
  int lomclim = _pumc->FindFirstBinAbove(0.01);
  int upmclim = _pumc->FindLastBinAbove (0.01);
  int maxmcbin = _pumc->FindFirstBinAbove(0.999);
  PrintInfo(Form("Maximum bin: %d for MC",maxmcbin),true);
  PrintInfo(Form("Hazardous pu below & above: %f, %f",_pumc->GetBinLowEdge(lomclim),_pumc->GetBinLowEdge(upmclim+1)),true);
  int nbinsmc = _pumc->GetNbinsX();
  int kmc = _pumc->FindBin(33); // Check that pu=33 occurs at the same place as for data

  // For data, load each trigger separately
  for (auto &t : jp::triggers) {
    auto *tmpPU = dynamic_cast<TH1D*>(f_pudist->Get(t));
    if (!tmpPU) {
      PrintInfo(Form("The trigger %s was not found in the DT pileup file!",t),true);
      return false;
    }
    _pudist[t] = dynamic_cast<TH1D*>(tmpPU->Clone(Form("pu%s",t)));
    int nbinsdt = _pudist[t]->GetNbinsX();
    int kdt = _pudist[t]->FindBin(33);
    if (kdt!=kmc or nbinsdt!=nbinsmc) {
      PrintInfo("The pileup histogram dt vs mc binning or range do not match (dt left mc right):",true);
      PrintInfo(Form(" Bins: dt:%d mc:%d",nbinsdt,nbinsmc),true);
      PrintInfo(Form(" Pu=33 bin: dt:%d mc:%d",kdt,kmc),true);
      return false;
    }
    double maxdtpu = _pudist[t]->GetMaximum();
    int lodtlim    = _pudist[t]->FindFirstBinAbove(maxdtpu/100.0);
    int updtlim    = _pudist[t]->FindLastBinAbove (maxdtpu/100.0);
    int maxdtbin   = _pudist[t]->FindFirstBinAbove(0.999*maxdtpu);

    for (int bin = 0; bin < lomclim; ++bin) // Set fore-tail to zero
      _pudist[t]->SetBinContent(bin,0.0);
    for (int bin = upmclim+1; bin <= nbinsdt; ++bin) // Set aft-tail to zero
      _pudist[t]->SetBinContent(bin,0.0);

    PrintInfo(Form("Maximum bin: %d for DT trg %s",maxdtbin,t),true);
    PrintInfo(Form("Hazardous pu below & above: %f, %f",_pudist[t]->GetBinLowEdge(lodtlim),_pudist[t]->GetBinLowEdge(updtlim+1)),true);
    _pudist[t]->Divide(_pumc);
  }
  PrintInfo("Finished processing pileup histos!",true);

  curdir->cd();
  return true;
} // LoadPuProfiles


Int_t HistosFill::FindMCSliceIdx(string filename)
{
  int sliceIdx = 0;
  bool sliceFound = false;
  if (jp::pthatbins) {
    for (auto &fname : jp::pthatfiles) {
      regex rfile(fname);
      std::cmatch mfile;
      if (std::regex_search(filename.c_str(),mfile,rfile)) {
        sliceFound = true;
        break;
      }
      ++sliceIdx;
    }
  } else if (jp::htbins) {
    for (auto &fname : jp::htfiles) {
      regex rfile(fname);
      std::cmatch mfile;
      if (std::regex_search(filename.c_str(),mfile,rfile)) {
        sliceFound = true;
        break;
      }
      ++sliceIdx;
    }
  }
  if (sliceFound) return sliceIdx;
  return -1;
}


// Check that the correct tree is open in the chain
Long64_t HistosFill::LoadTree(Long64_t entry)
{
  if (!fChain) return -5;
  Long64_t centry = fChain->LoadTree(entry);
  if (centry < 0) return centry;

  // A new tree is opened
  if (fChain->GetTreeNumber() != fCurrent) {

    fCurrent = fChain->GetTreeNumber();
    PrintInfo(Form("Opening tree number %d", fChain->GetTreeNumber()));

    if (jp::fetchMETFilters) {
      // Reload the MET filters and print them
      if (!GetFilters()) {
        PrintInfo("Failed to load DT filters. Check that the SMPJ tuple has the required histograms. Aborting...");
        return -4;
      }
      PrintInfo(Form("Tree %d MET filters:",fCurrent),true);
      for (auto &flt : _availFlts) PrintInfo(flt,true);
    }

    if (jp::isdt) {
      // Reload the triggers and print them
      if (!GetTriggers()) {
        PrintInfo("Failed to load DT triggers. Check that the SMPJ tuple has the required histograms. Aborting...");
        return -4;
      }
      PrintInfo(Form("Tree %d triggers:",fCurrent));
      for (auto trigi = 0u; trigi < _availTrigs.size(); ++trigi) {
        auto str = _availTrigs[trigi];
        *ferr << str;
        auto trgplace = std::find(_goodTrigs.begin(),_goodTrigs.end(),trigi);
        if (trgplace!=_goodTrigs.end()) *ferr << "_y ";
        else *ferr << "_n ";
        if (trigi%(jp::notrigs+1)==jp::notrigs) *ferr << endl;
      }
      *ferr << endl << flush;
    } else if (jp::pthatbins or jp::htbins) {
      // If there are two pthat files with the same pthat range, we convey this information through "prevweight"
      bool htmode = !jp::pthatbins;
      const char* bintag = htmode ? "HT" : "Pthat";
      Long64_t noevts = fChain->GetTree()->GetEntries();
      if (_binnedmcrepeats<=0) {
        const char* fname = fChain->GetCurrentFile()->GetName();
        // Check the position of the current file in the list of file names
        int sliceIdx = FindMCSliceIdx(fname);
        if (sliceIdx<0) {
          PrintInfo(Form("%s slice file name contradictory %s. Aborting...",bintag,fname));
          return -3;
        }
        if ((!htmode and jp::pthatsigmas[sliceIdx]<=0) or (htmode and jp::htsigmas[sliceIdx]<=0)) {
          PrintInfo(Form("Suspicious %s information for file %s. Aborting...",bintag,fname));
          return -3;
        }
        PrintInfo(Form("%s bin changing.\nFile %d %s, position %lld with %lld events.",bintag,sliceIdx,fname,centry,noevts),true);

        _binnedmcrepeats = 0;
        // We have a look if the next tree in the chain has the same range
        Long64_t next = entry-centry;
        while (fChain->GetTreeNumber()<fChain->GetNtrees()-1) {
          next = next+fChain->GetTree()->GetEntries();
          if (next>=_ntot) break;
          Long64_t nentry = fChain->LoadTree(next);
          if (nentry < 0) break;
          Long64_t nextevts = fChain->GetTree()->GetEntries();
          const char* nextname = fChain->GetCurrentFile()->GetName();
          int nextIdx = FindMCSliceIdx(nextname);
          if (nextIdx<0) {
            PrintInfo(Form("%s slice file name contradictory %s. Aborting...",bintag,nextname));
            return -3;
          }
          if (sliceIdx!=nextIdx) break;
          PrintInfo(Form("File extension %d %s, position %lld with %lld events",nextIdx,nextname,nentry,nextevts),true);
          noevts += nextevts;
          ++_binnedmcrepeats;
        }
        // Normalization with the amount of entries within the current tree
        double sigmacurr = htmode ? jp::htsigmas[sliceIdx  ] : jp::pthatsigmas[sliceIdx  ];
        double slicecurr = htmode ? jp::htranges[sliceIdx  ] : jp::pthatranges[sliceIdx  ];
        double slicenext = htmode ? jp::htranges[sliceIdx+1] : jp::pthatranges[sliceIdx+1];
        _binnedmcweight  = sigmacurr/noevts;
        // This is a normalization procedure by the luminosity of the furthest PtHat/HT bin.
        // In practice, it does not hurt if the normalevts number is arbitrary.
        _binnedmcweight /= (htmode ? (jp::htsigmas.back()/jp::htnormalevts) : (jp::pthatsigmas.back()/jp::pthatnormalevts));
        PrintInfo(Form("The given slice has the %s range [%f,%f]\nWeight: %f, with a total x-sec %f pb and %lld events.",
                  bintag,slicecurr,slicenext,_binnedmcweight,sigmacurr,noevts),true);
      } else {
        --_binnedmcrepeats;
        PrintInfo(Form("%s bin remains the same while file is changing.\nFile %s\nWeight: %f",
                  bintag,fChain->GetCurrentFile()->GetName(),_binnedmcweight),true);
      }
    }
    // slices with PtHat/HT bins
  }
  return centry;
}


// Update the available MET filter types for each new tree
bool HistosFill::GetFilters()
{
  TH1F *filters = dynamic_cast<TH1F*>(fChain->GetCurrentFile()->Get("ak4/FilterActive")); assert(filters);
  TAxis *xax = filters->GetXaxis();
  regex filter("Flag_([a-zA-Z0-9]+)");

  _availFlts.clear();
  bool empty  = false;
  bool passall = false;
  for (int fltidx = xax->GetFirst(); fltidx <= xax->GetLast(); ++fltidx) {
    string fltName = xax->GetBinLabel(fltidx);

    if (fltName=="") {
      empty = true;
    } else if (fltName=="PassAll") {
      if (empty) {
        PrintInfo("PassAll MET filter in the wrong position!",true);
        return false;
      }
      passall = true;
    } else if (std::regex_match(fltName,filter)) {
      if (empty or passall) {
        PrintInfo(Form("MET filter %s in the wrong position!",fltName.c_str()),true);
        return false;
      }
      string stripName = std::regex_replace(fltName, filter, "$1", std::regex_constants::format_no_copy);
      _availFlts.push_back(stripName);
    } else {
      PrintInfo(Form("Unknown filter type %s",fltName.c_str()),true);
      return false;
    }
  }

  return _availFlts.size()>0;
}

// Update the available trigger types for each new tree
bool HistosFill::GetTriggers()
{
  TH1F *triggers = dynamic_cast<TH1F*>(fChain->GetCurrentFile()->Get("ak4/TriggerNames")); assert(triggers);
  TAxis *xax = triggers->GetXaxis();
  TH1F *usedtrigs = dynamic_cast<TH1F*>(fChain->GetCurrentFile()->Get("ak4/TriggerPass")); assert(usedtrigs);
  TAxis *uxax = usedtrigs->GetXaxis();

  regex zbs("HLT_ZeroBias_v([0-9]+)");
  regex pfjet("HLT_PFJet([0-9]+)_v([0-9]+)");
  regex ak8("HLT_AK8PFJet([0-9]+)_v[0-9]+");
  regex jetht("HLT_PFHT([0-9]+)_v[0-9]+");
  regex hipfjet("HLT_HI[AK4]*PFJet([0-9]+)_v[0-9]+");
  regex hipfjetfwd("HLT_HI[AK4]*PFJet[FWwDd]*([0-9]+)[FWwDd]*_v[0-9]+");
  regex hiak8("HLT_HIAK8PFJet([0-9]+)_v[0-9]+");
  regex hiak8fwd("HLT_HIAK8PFJet[FWwDd]*([0-9]+)[FWwDd]*_v[0-9]+");

  // List triggers with actual contents
  map<string,unsigned int> utrigs;
  bool zbcase = false;
  for (int trgidx = uxax->GetFirst(); trgidx <= uxax->GetLast(); ++trgidx) {
    string trgName = uxax->GetBinLabel(trgidx);
    if (trgName=="") continue;
    utrigs[trgName] = usedtrigs->GetBinContent(trgidx);
    if (std::regex_match(trgName,zbs)) zbcase = true;
  }

  _availTrigs.clear();
  _goodTrigs.clear();
  for (int trgidx = xax->GetFirst(); trgidx <= xax->GetLast(); ++trgidx) {
    string trgName = xax->GetBinLabel(trgidx);
    if (trgName.compare("")==0) continue; // Ignore empty places on x-axis

    string trigger = "x"; // HLT_PFJet are given non-empty trigger names
    if (std::regex_match(trgName,zbs)) {
      trigger = "jt0"; // Zero for ZeroBias
      if (zbcase and utrigs.find(trgName) != utrigs.end()) {
        unsigned thrplace = 0;
        if (thrplace < jp::notrigs) {
          _goodTrigs.push_back(_availTrigs.size());
          PrintInfo(Form("Trigger %s responding loud and clear with %u events!",trgName.c_str(),utrigs[trgName]),true);
        } else { // No trig era weighting: no relative weights
          PrintInfo(Form("The trigger %s is available, but not supported",trgName.c_str()),true);
        }
      }
    } else if (std::regex_match(trgName,hiak8)) {
      trigger=std::regex_replace(trgName, hiak8, "ak8jt$1", std::regex_constants::format_no_copy);
    } else if (std::regex_match(trgName,hiak8fwd)) {
      trigger=std::regex_replace(trgName, hiak8fwd, "ak8jt$1fwd", std::regex_constants::format_no_copy);
    } else if (std::regex_match(trgName,hipfjet)) {
      trigger=std::regex_replace(trgName, hipfjet, "jt$1", std::regex_constants::format_no_copy);
      if (_eraIdx==1 and jp::yid==2 and !zbcase and utrigs.find(trgName) != utrigs.end()) {
        double trigthr = std::stod(std::regex_replace(trgName, hipfjet, "$1", std::regex_constants::format_no_copy));
        unsigned thrplace = static_cast<unsigned>(std::find(jp::trigthr.begin()+1,jp::trigthr.end(),trigthr)-jp::trigthr.begin());
        if (thrplace < jp::notrigs) {
          _goodTrigs.push_back(_availTrigs.size());
          PrintInfo(Form("Trigger %s responding loud and clear with %u events!",trgName.c_str(),utrigs[trgName]),true);
        } else {
          PrintInfo(Form("The trigger %s is available, but not supported",trgName.c_str()),true);
        }
      }
    } else if (std::regex_match(trgName,hipfjetfwd)) {
      trigger=std::regex_replace(trgName, hipfjetfwd, "jt$1fwd", std::regex_constants::format_no_copy);
      if (_eraIdx==0 and jp::yid==2 and !zbcase and utrigs.find(trgName) != utrigs.end()) {
        double trigthr = std::stod(std::regex_replace(trgName, hipfjetfwd, "$1", std::regex_constants::format_no_copy));
        unsigned thrplace = static_cast<unsigned>(std::find(jp::trigthr.begin()+1,jp::trigthr.end(),trigthr)-jp::trigthr.begin());
        if (thrplace < jp::notrigs) {
          _goodTrigs.push_back(_availTrigs.size());
          PrintInfo(Form("Trigger %s responding loud and clear with %u events!",trgName.c_str(),utrigs[trgName]),true);
        } else {
          PrintInfo(Form("The trigger %s is available, but not supported",trgName.c_str()),true);
        }
      }
    } else if (std::regex_match(trgName,pfjet)) {
      trigger=std::regex_replace(trgName, pfjet, "jt$1", std::regex_constants::format_no_copy);
      if (!zbcase and utrigs.find(trgName) != utrigs.end()) {
        double trigthr = std::stod(std::regex_replace(trgName, pfjet, "$1", std::regex_constants::format_no_copy));
        unsigned thrplace = static_cast<unsigned>(std::find(jp::trigthr.begin()+1,jp::trigthr.end(),trigthr)-jp::trigthr.begin());
        if (thrplace < jp::notrigs) {
          _goodTrigs.push_back(_availTrigs.size());
          PrintInfo(Form("Trigger %s responding loud and clear with %u events!",trgName.c_str(),utrigs[trgName]),true);
        } else { // No trig era weighting: no relative weights
          PrintInfo(Form("The trigger %s is available, but not supported",trgName.c_str()),true);
        }
      }
    } else if (std::regex_match(trgName,ak8)) {
      trigger=std::regex_replace(trgName, ak8, "ak8jt$1", std::regex_constants::format_no_copy);
    } else if (std::regex_match(trgName,jetht)) {
      trigger=std::regex_replace(trgName, jetht, "jetht$1", std::regex_constants::format_no_copy);
    } else {
      PrintInfo(Form("Unknown trigger type %s",trgName.c_str()),true);
    }

    _availTrigs.push_back(trigger);
  }

  return _availTrigs.size()>0;
} // GetTriggers
