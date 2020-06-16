// Purpose: Optimize high pT binning for inclusive jets
// Author:  mikko.voutilainen@cern.ch
// Created: sometime 2010
// Updated: June 9, 2015
#include "TFile.h"
#include "TDirectory.h"
#include "TList.h"
#include "TKey.h"
#include "TF1.h"
#include "TH1D.h"
#include "TMath.h"

#include "settings.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<double> optimizeBin(TDirectory *din, TDirectory *dnlo, int ibin,
			   string type, double lumi, double eff);

void optimizeBins(string type, double lumi, double eff=1.) {

  // Unfolded spectrum
  TFile *fin = new TFile(Form("output-%s-3.root",type.c_str()),"READ");
  assert(fin && !fin->IsZombie());

  // NLO theory predictions
  TFile *fnlo = new TFile(Form("output-%s-2c.root",type.c_str()),"READ");
  assert(fnlo && !fnlo->IsZombie());

  assert(fin->cd("Standard"));
  TDirectory *dtop = gDirectory;
  assert(fnlo->cd("Standard"));
  TDirectory *dtopnlo = gDirectory;

  // Automatically go through the list of keys (directories)
  TList *keys = dtop->GetListOfKeys();
  TListIter itkey(keys);
  TObject *key, *obj;

  int ibin = 0;
  vector<vector<double> > vx;
  vector<string> vs;
  while ( (key = itkey.Next()) ) {

    obj = ((TKey*)key)->ReadObj(); assert(obj);
    
    // Found a subdirectory
    if (obj->InheritsFrom("TDirectory")) {

      assert(dtop->cd(obj->GetName()));
      TDirectory *din = gDirectory;
      dtopnlo->cd(obj->GetName());
      TDirectory*dnlo = gDirectory;

      // Process subdirectory
      vx.push_back( optimizeBin(din, dnlo, ibin++, type, lumi, eff) );
      vs.push_back(din->GetName());
    } // inherits TDirectory
  } // while

  //fout->Write();

  cout << "// Optimized binning created by optimizeBins.C (\""
       << type << "\"; lumi "<<lumi<<"/pb, eff "<<eff*100<<"%)" << endl;
  //cout << "// Using unfolded data fit as input (not NLO theory)" << endl;
  cout << "// Using NLOxNP theory fit as input when available" << endl;
  cout << Form("double vx[%d][%d] =\n",(int)vx.size(),(int)vx[0].size()); 
  for (unsigned int i = 0; i != vx.size(); ++i) {
    cout << (i==0 ? "  {{" : "   {");
    for (unsigned int j = 0; j != vx[0].size(); ++j) {
      cout << Form("%s%d", j==0 ? "" : ", ",
		   j < vx[i].size() ? int(vx[i][j]+0.5) : 0); 
    } // for j
    cout << (i==vx.size()-1 ? "}};" : "},");// << endl;
    cout << " // " << vs[i] << endl;
  } // for i

} // optimizeBin


vector<double> optimizeBin(TDirectory *din, TDirectory *dnlo, int ibin,
			   string type, double lum, double eff) {

  if (ibin){};
  bool _dt = (type=="DATA");

  float y1, y2;
  if (!(sscanf(din->GetName(),"Eta_%f-%f",&y1,&y2)==2))
    cout << "din->GetName() == " << din->GetName() << endl;
  double y = 0.5*(y1+y2);
  double dy = fabs(y2-y1);

  cout << "Directory : " << din->GetName() << endl << flush;
  if (y > 3. || dy > 0.5) return vector<double>(0);
  TF1 *fus = (TF1*)din->Get("fus"); assert(fus);

  // Use theory curve instead, if available
  TF1 *fnlo = (dnlo ? (TF1*)dnlo->Get("fnlo") : 0);
  if (fnlo) {
    TF1 *fnlo_tmp = new TF1("fnlonew","[0]*exp([1]/x)*pow(x,[2])"
			    "*pow(1-x*cosh([4])/[5],[3])",_jp_recopt,_jp_emax);
    fnlo_tmp->SetParameters(fnlo->GetParameter(0), fnlo->GetParameter(1),
			    fnlo->GetParameter(2), fnlo->GetParameter(3),
			    fnlo->GetParameter(4), _jp_emax);
    fus = fnlo_tmp;
  }

  // Official binning to start from
  double x[] =
    //{1, 5, 6, 8, 
    {10, 12, 15, 18, 21, 24, 28, 32, 37, 43, 49, 56, 64, 74, 84,
     97, 114, 133, 153, 174, 196, 220, 245, 272, 300, 330, 362, 395, 430, 468,
     507, 548, 592, 638, 686, 737, 790, 846, 905, 967, 1032, 1101, 1172, 1248,
     1327, 1410, 1497, 1588, 1684, 1784, 1890, 2000,
     2116, 2238, 2366, 2500, 2640, 2787, 2941, 3103, 3273, 3450,
     3637, 3832, 4037, 4252, 4477, 4713, 4961, 5220, 5492, 5777,
     6076, 6389, 6717, 7000};
  int nx = sizeof(x)/sizeof(x[0]);
  
  // Phase space limits for NLO
  int k = TMath::BinarySearch(nx,&x[0],_jp_emax/cosh(y1));
  nx = min(nx, k)+1;
  
  // Working backwards, find the last bin that can be excluded
  // in order to miss less than 0.02 inclusive jets, then
  // merge bins that have less than 50 jets
  const double lumi = lum*eff*(_dt ? 1. : 1.);
  const double maxmiss = 0.02;
  const double nmin = 50.;
  vector<double> vx;
  vector<double> vy;

  bool foundlast = false;
  double binsum = 0;
  int iprev = nx-1;
  for (int i = nx-1; i != 0; --i) {

    double ptmin = x[i-1];
    double ptmax = x[i];
    double dn = fus->Integral(ptmin, ptmax)*lumi*2.*fabs(y2-y1);
    // For missed bin, need to peek ahead
    double dm = 0;
    if (!foundlast && i>1) {
      double ptmin0 = x[i-2];
      dm = fus->Integral(ptmin0, ptmin)*lumi;
    }

    binsum += dn;
    if (binsum > nmin || (binsum+dm > maxmiss && !foundlast)) {
      vx.push_back(x[iprev]);
      vy.push_back(binsum);
      foundlast = true;
      iprev = i-1;
      binsum = 0;
    }
  } // for i
  vx.push_back(x[0]);

  const int nskip = min((type=="BDATA" ? 34 : 41), int(vx.size())-1);
  for (int i = vx.size()-1-nskip; i != -1; --i) {
    cout << Form("%4d     ",int(vx[i]+0.5));
    assert(i>-1);
  }
  cout << endl;
  cout << " ";
  for (int i = vy.size()-1-nskip; i != -1; --i) {
    cout << Form(" %8.3g",vy[i]);
    assert(i>-1);
  }
  cout << endl;

  vector<double> vx2(vx.size());
  for (unsigned int i = 0; i != vx2.size(); ++i)
    vx2[i] = vx[vx.size()-1-i];


  return vx2;
} // optimizeBin
