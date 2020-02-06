// Derive x2 reco binning for inclusive jet unfolding
// Idea is to sub-divide existing bins into two
#include "TGraph.h"
// #include "tdrstyle_mod18.C"

vector<double> deriveSubBins(double minx = 0, double maxx = 7000) {

  //  setTDRStyle();

  vector<double> newbins;
  
  double xorig[] =
    {/*1,*/ 5, 6, 8, 10, 12, 15, 18, 21, 24, 28, 32, 37, 43, 49, 56, 64, 74, 84, 97, 114, 133, 153, 174, 196, 220, 245, 272, 300, 330, 362, 395, 430, 468, 507, 548, 592, 638, 686, 737, 790, 846, 905, 967, 1032, 1101, 1172, 1248, 1327, 1410, 1497, 1588, 1684, 1784, 1890, 2000, 2116, 2238, 2366, 2500, 2640, 2787, 2941, 3103, 3273, 3450, 3637, 3832, 4037, 4252, 4477, 4713, 4961, 5220, 5492, 5777, 6076, 6389, 6717, 7000};
  const int nx = sizeof(xorig)/sizeof(xorig[0])-1;

  TGraph *g = new TGraph();
  for (int i = 0; i != nx; ++i) {
    g->SetPoint(i, xorig[i], xorig[i+1]/xorig[i]);
  }

  /*  g->SetMarkerStyle(kFullCircle);
  g->Draw("AP");
  
  gPad->SetLogx(); */

  TF1 *f1 = new TF1("f1","1+[4]*(pow(x,[5])-pow(20,[5]))*(x<20)+[3]*(x<90)+sqrt([0]*fabs([0])/(x*x)+[1]*[1]/x+[2]*[2])*(x>=90)",6,7000);
  f1->SetParameters(1,1,0.05,0.15,0.2,-1);
  g->Fit(f1,"R");


  // f1->Draw("SAMEP"); 

  TF1 *f1x = new TF1("f1x","x*(1+[4]*(pow(x,[5])-pow(20,[5]))*(x<20)+[3]*(x<90)+sqrt([0]*fabs([0])/(x*x)+[1]*[1]/x+[2]*[2])*(x>=90))",6,7000);
  f1x->SetParameters(f1->GetParameter(0), f1->GetParameter(1),
		     f1->GetParameter(2), f1->GetParameter(3),
		     f1->GetParameter(4), f1->GetParameter(5));

  double xnew2[2*nx+1];

  // Generate new pT binning
  TGraph *g2 = new TGraph();
  TGraph *g2x2 = new TGraph();
  for (int i = 0; i != nx; ++i) {

    xnew2[2*i] = xorig[i];
    double xmean(0);
    if ((xorig[i+1]-xorig[i])<=2.) { 
      xmean= 0.5*(xorig[i+1]+xorig[i]);
    }
    else {
      
      double xup = f1x->Eval(xorig[i]);
      double xdw = f1x->GetX(xorig[i+1],xorig[i],xorig[i+1]);
      xmean = sqrt(xup*xdw);
    }
    xnew2[2*i+1] = xmean;

    g2->SetPoint(2*i, xorig[i], xmean/xorig[i]);
    g2->SetPoint(2*i+1, xmean, xorig[i+1]/xmean);
    g2x2->SetPoint(2*i, xorig[i], 1+2*(xmean/xorig[i]-1));
    g2x2->SetPoint(2*i+1, xmean, 1+2*(xorig[i+1]/xmean-1));
  }

  /*  g2->SetMarkerStyle(kOpenCircle);
  g2->Draw("SAMEP");
  g2x2->SetMarkerStyle(kOpenSquare);
  g2x2->Draw("SAMEP");

  
  TH1D *h = new TH1D("h",";p_{T} (GeV);Bin x_{i+1} / x_{i} (i.e. width+1)",
		     6995,5,7000);
  h->SetMinimum(1.00);
  h->SetMaximum(1.35);
  h->GetXaxis()->SetMoreLogLabels();  
  h->GetXaxis()->SetNoExponent();

  TCanvas *c1 = tdrCanvas("c1",h,11,kSquare);
  c1->SetLogx();

  tdrDraw(g2x2,"P",kOpenSquare);
  tdrDraw(g2,"P",kOpenCircle);
  tdrDraw(g,"P",kFullCircle);

  TLegend *leg = tdrLeg(0.6,0.6,0.8,0.8);
  leg->AddEntry(g,"Official SMP-J","PL");
  leg->AddEntry(g2,"New SMP-Jx2","PL");
  leg->AddEntry(g2x2,"SMP-Jx2 scaled","PL");

  c1->SaveAs("pdf/deriveSubBins.pdf"); */

  double x = 0;
  vector<double> vx; // true, gen
  for (int i = 0; i != 2*nx; ++i) {
    x = xnew2[i];
    
    if (x>=minx && x <= maxx) newbins.push_back(x);
   
  }



  return newbins;
}
