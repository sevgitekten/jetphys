// Ansatz functions for inclusive jets

// Ansatz Kernel 
int cnt_a = 0;
const int nk = 3; // number of kernel parameters (excluding pt, eta)

Double_t smearedAnsatzKernel(Double_t *x, Double_t *p) {

  if (++cnt_a%1000000==0) cout << "+" << flush;

  const double pt = x[0]; // true pT
  const double ptmeas = p[0]; // measured pT
  const double eta = p[1]; // rapidity

  double res = ptresolution(pt, eta+1e-3) * pt;
  
  const double s = TMath::Gaus(ptmeas, pt, res, kTRUE);
  const double f = p[2] * pow(pt, p[3]) * pow(1 - pt*cosh(eta) / jp::emax, p[4]);
  
  return (f * s);
}


// Smeared Ansatz

TF1 *_kernel = 0; // global variable, not pretty but works

Double_t smearedAnsatz(Double_t *x, Double_t *p) {

  double _epsilon = 1e-12;
  
  const double pt = x[0];
  const double eta = p[0];
  
  if (!_kernel) _kernel = new TF1("_kernel", smearedAnsatzKernel, 1., jp::emax/cosh(eta), nk+2);

  double res = ptresolution(pt, eta+1e-3); // * pt;
  //  const double sigma = max(0.10, min(res, 0.30)); // was max
  const double sigma = min(res, 0.30); // was max

  double ptmin = pt /(1. + 4.*sigma); // xmin*(1+4*sigma)=x
  ptmin = max(1.,ptmin); // safety check
  double ptmax = pt /(1. - 3.*sigma); // xmax*(1-3*sigma)=x
  
  //  cout << Form("1pt %10.5f sigma %10.5f ptmin %10.5f ptmax %10.5f eta %10.5f",pt, sigma, ptmin, ptmax, eta) << endl << flush;
  ptmax = min(jp::emax/cosh(eta), ptmax); // safety check
  //  cout << Form("2pt %10.5f sigma %10.5f ptmin %10.5f ptmax %10.5f eta %10.5f",pt, sigma, ptmin, ptmax, eta) << endl << flush;
  
  const double par[nk+2] = {pt, eta, p[1], p[2], p[3]};
  _kernel->SetParameters(&par[0]);

  // Set pT bin limits needed in smearing matrix generation
  if (p[4]>0 && p[4]<jp::emax/cosh(eta)) ptmin = p[4];
  if (p[5]>0 && p[5]<jp::emax/cosh(eta)) ptmax = p[5];

  return ( _kernel->Integral(ptmin, ptmax, _epsilon) );
}
