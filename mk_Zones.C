#include "TFile.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TMath.h"
#include "tdrstyle_mod18.C"
#include "TError.h"

#include "settings.h"

void ZeroBinsToVal(TH2D *h2, double val) {
  for (int idxeta = 1; idxeta < h2->GetNbinsX()+1; ++idxeta) { // Loop over HCAL eta strips.
    for (int idxphi = 1; idxphi < h2->GetNbinsY()+1; ++idxphi) {
      if (h2->GetBinContent(idxeta,idxphi)==0.0) {
        h2->SetBinContent(idxeta,idxphi,val);
      }
    }
  }
}

// Draw 2D plot of jet rates in (eta,phi) to spot issues
void mk_Zones(string overlayName = "../hotjets-17runBCDEF.root") {

  const double minsig = 3.0; // For a single trigger, how many sigmas are interpreted as a significant deviation?
  bool phidep = true;

  string yrtag = "16"; // default
  if (jp::yid==1) yrtag = "17";
  else if (jp::yid==2) yrtag = "17 LowPU";
  else if (jp::yid==3) yrtag = "18";
  const char *yeartag = yrtag.c_str();

  //string savedir = "rootfiles";
  string savedir = ".";

  vector<const char*> types = {"","_cef","_muf","_nef","_chf","_puf","_nhf"};

  // Eta ranges where different counts of triggers are considered
  const array<vector<double>,jp::yrs> etalims_ = {{
    {4.2,4.0,3.8,3.4,3.0,2.6},
    {4.899,4.538,4.191,3.489,3.139,2.65,2.5},
    {4.899,4.538,4.191,3.489,3.139,2.65,2.5},
    {5.0,4.0,3.4,3.2,2.6}
  }};
  // Counts of triggers in each eta range
  const vector<double> etalims = etalims_.at(jp::yid);
  const array<vector<int>,jp::yrs> etatrgs_ = {{
    {2,3,4,5,6,10},
    {1,2,4,6,8,9,11},
    {1,2,4,6,8,9,11},
    {2,4,6,9,11}
  }};
  const vector<int> etatrgs = etatrgs_.at(jp::yid);
  assert(etatrgs.size()==etalims.size());

  map<string,const char*> trgNames = {
    {"jt0","ZeroBias"},
    {"jt40","PFJet40"},
    {"jt60","PFJet60"},
    {"jt80","PFJet80"},
    {"jt140","PFJet140"},
    {"jt200","PFJet200"},
    {"jt260","PFJet260"},
    {"jt320","PFJet320"},
    {"jt400","PFJet400"},
    {"jt450","PFJet450"},
    {"jt500","PFJet500"},
    {"jt550","PFJet550"}
  };

  TDirectory *curdir = gDirectory;
  setTDRStyle();
  gStyle->SetOptTitle();
  gStyle->SetPalette(1);
  //gStyle->SetPalette(kLightTemperature);

  TFile *fd = new TFile("./output-DATA-2a.root","READ");
  assert(fd && !fd->IsZombie());

  TFile *fm = new TFile("./output-MCNU-2a.root","READ");
  assert(fm && !fm->IsZombie());

  TFile *fh = 0;
  //TFile *fh = new TFile("./output-HWNU-2a.root","READ");
  //assert(fh && !fh->IsZombie());

  TH2D *h2s[jp::notrigs], *h2as[jp::notrigs], *h2bs[jp::notrigs], *h2hots[jp::notrigs], *h2colds[jp::notrigs];
  TH2D *h2template = 0;
  TH2D *h2hotNew = 0;
  // Further overlay settings
  TFile *hotmap = new TFile(overlayName.c_str());
  if (!hotmap->IsZombie()) {
    h2hotNew = static_cast<TH2D*>(static_cast<TH2D*>(hotmap->Get("h2hotfilter"))->Clone("newmap"));
    assert(h2hotNew && !h2hotNew->IsZombie());
  } else {
    cout << "Overlay file " << overlayName << " not found!" << endl;
  }
  for (int dtmc = 0; dtmc <= 1; ++dtmc) {
    TFile *f = (dtmc==0) ? fd : ((dtmc==1) ? fm : fh);
    bool enterdir = f->cd("FullEta_Reco");
    assert(enterdir);
    TDirectory *din = gDirectory;
    string nametag = (dtmc==0) ? "data" : ((dtmc==1) ? "mc" : "hw");
    string roottag = (dtmc==0) ? "" : ((dtmc==1) ? "mc" : "hw");
    TFile *phifile = phidep ? new TFile("pdf/phifile.root","RECREATE") : 0;

    TH2D *h2hotComb(0), *h2hotComb2(0);

    TFile *fouthot = new TFile(Form("%s/hotjets%s-%srun%s.root",savedir.c_str(),roottag.c_str(),yeartag,jp::run.c_str()),"RECREATE");
    TFile *foutcold = new TFile(Form("%s/coldjets%s-%srun%s.root",savedir.c_str(),roottag.c_str(),yeartag,jp::run.c_str()),"RECREATE");
    for (auto &type : types) {

      for (int itrg = 0; itrg < jp::notrigs; ++itrg) {
        din->cd();
        const char *ctrg = jp::triggers[itrg];

        bool enterdir2 = gDirectory->cd(ctrg);
        assert(enterdir2);

        TDirectory *d = gDirectory;

        TH2D *h2 = (TH2D*)d->Get(Form("hetaphi%s",type));
        assert(h2);
        // Create an empty histogram for cloning purposes
        TH2D *h2empty = (TH2D*)h2->Clone(Form("h2empty%s",ctrg));
        for (int idxeta = 1; idxeta != h2->GetNbinsX()+1; ++idxeta) {
          for (int idxphi = 1; idxphi != h2->GetNbinsY()+1; ++idxphi) {
            h2empty->SetBinContent (idxeta, idxphi, 0);
            h2empty->SetBinError   (idxeta, idxphi, 0);
          } // for j
        } // for i
        if (h2hotComb==0)  h2hotComb  = static_cast<TH2D*>(h2empty->Clone("h2hot"));       // Combined filtered variations
        if (h2hotComb2==0) h2hotComb2 = static_cast<TH2D*>(h2empty->Clone("h2hotfilter")); // Combined filtered variations

        // Hot and cold mappings
        TH2D *h2a    = static_cast<TH2D*>(h2empty->Clone(Form("h2a%s",ctrg)));    // Fluctuation w.r.t. variance
        TH2D *h2b    = static_cast<TH2D*>(h2empty->Clone(Form("h2b%s",ctrg)));    // Fluctuation w.r.t. mean
        TH2D *h2hot  = static_cast<TH2D*>(h2empty->Clone(Form("h2hot%s",ctrg)));  // Filtered upwards fluctuations in h2a
        TH2D *h2cold = static_cast<TH2D*>(h2empty->Clone(Form("h2cold%s",ctrg))); // Filtered downwards fluctuations in h2a
        // Extract eta and phi binnning
        vector<double> xbins, ybins;
        for (unsigned idxeta = 1u; idxeta <= h2->GetNbinsX(); ++idxeta) xbins.push_back(h2->GetXaxis()->GetBinLowEdge(idxeta));
        xbins.push_back(h2->GetXaxis()->GetBinUpEdge(h2->GetNbinsX()));
        for (unsigned idxphi = 1u; idxphi <= h2->GetNbinsY(); ++idxphi) ybins.push_back(h2->GetYaxis()->GetBinLowEdge(idxphi));
        ybins.push_back(h2->GetYaxis()->GetBinUpEdge(h2->GetNbinsY()));
        // Eta histograms
        TH1D *hvar  = new TH1D(Form("hvar%s",type),"#sigma/#mu for #eta -strip;#eta_{jet};#sigma/#mu",xbins.size()-1,&xbins[0]);
        TH1D *hvar2 = new TH1D(Form("hvar2%s",type),"Expected #sigma/#mu for #eta -strip;#eta_{jet};Expected #sigma/#mu",xbins.size()-1,&xbins[0]);

        for (int idxeta = 1; idxeta < h2->GetNbinsX()+1; ++idxeta) { // Loop over HCAL eta strips.
          // 1. Calculate mean bin content and sample variance in the current eta strip
          int NPhi = h2->GetNbinsY();
          double Sum = 0, SumPow2 = 0;
          for (int idxphi = 1; idxphi <= NPhi; ++idxphi) {
            double val = h2->GetBinContent(idxeta,idxphi);
            Sum     += val;
            SumPow2 += pow(val,2);
          }
          double Mean = Sum/NPhi;
          double Var  = sqrt( (SumPow2/NPhi-pow(Mean,2))/(1.0-1.0/NPhi) );

          // 2. Recalculate mean bin content and sample variance in the current eta strip by excluding bins over three sigmas away from mean.
          int NPhiGood = 0, NPhiNonZero = 0;
          double SumGood = 0, SumPow2Good = 0;
          for (int idxphi = 1; idxphi < h2->GetNbinsY()+1; ++idxphi) {
            // Accept only bins where fluctuations are under three sigmas away
            double val = h2->GetBinContent(idxeta,idxphi);
            if (fabs(val-Mean)/Var < 3.0) {
              SumGood     += val;
              SumPow2Good += pow(val,2);
              ++NPhiGood;
              if (val>0) ++NPhiNonZero;
            }
          }
          double MeanGood = NPhiGood==0 ? 0.0 : SumGood/NPhiGood;
          double VarGood  = NPhiGood<=1 ? 0.0 : sqrt( (SumPow2Good/NPhiGood-pow(MeanGood,2))/(1.0-1.0/NPhiGood) );

          // Eta-histograms for relative variance.
          // For MC, the scale of hvar2 might be off due to event weighting
          hvar ->SetBinContent(idxeta, MeanGood>0 ? VarGood/MeanGood  : 0.0);
          hvar2->SetBinContent(idxeta, MeanGood>0 ? 1./sqrt(MeanGood) : 0.0);

          TH1D *hphi = phidep ? new TH1D(Form("hphi%s%s_%.2f",ctrg,type,h2->GetXaxis()->GetBinCenter(idxeta)),Form("Fluctuation w.r.t. #eta -strip mean (%s);#phi_{jet};Fluctuation/#mu",trgNames[ctrg]),ybins.size()-1,&ybins[0]) : 0;
          // Eta-phi -histograms for complete binwise (HCAL-towerwise) fluctuations
          for (int idxphi = 1; idxphi < h2->GetNbinsY()+1; ++idxphi) {
            bool GoodThreshold = (NPhi/5.0 < NPhiNonZero);
            double diff = GoodThreshold ? h2->GetBinContent(idxeta,idxphi)-MeanGood : 0.0;
            // Fluctuation w.r.t. variance
            double aval = (GoodThreshold and VarGood>0) ? diff/VarGood  : 0.0;
            h2a   ->SetBinContent(idxeta, idxphi,                  aval      );
            h2hot ->SetBinContent(idxeta, idxphi, aval>= minsig ?  aval : 0.0); // When it is over 3 sigma upwards
            h2cold->SetBinContent(idxeta, idxphi, aval<=-minsig ? -aval : 0.0); // When it is over 3 sigma downwards
            // Fluctuation w.r.t. mean
            double bval = (GoodThreshold and MeanGood>VarGood) ? diff/MeanGood : 0.0;
            h2b->SetBinContent(idxeta, idxphi, bval);
            if (hphi) hphi->SetBinContent(idxphi, bval);
          } // for idxphi
          if (hphi and phifile) {
            phifile->cd();
            TCanvas *cphi = new TCanvas(Form("cphi%s%s_%.2f",ctrg,type,h2->GetXaxis()->GetBinCenter(idxeta)),"",600,600);
            gPad->SetTopMargin(0.10);
            hphi->SetLineColor(kRed);
            hphi->Draw();
            hphi->Write();
            d->cd();
            //cphi->SaveAs(Form("pdf/phi/%s_DiffPerMean_%s_%.2f.pdf",nametag.c_str(),ctrg,h2->GetXaxis()->GetBinCenter(idxeta)));
          }
        } // for idxeta

        // Store current histograms
        h2template = h2empty; // Only one template needed - pick it whenever
        h2s    [itrg] = h2;
        h2as   [itrg] = h2a;
        h2bs   [itrg] = h2b;
        h2hots [itrg] = h2hot;
        h2colds[itrg] = h2cold;

        TCanvas *cvar = new TCanvas(Form("cvar%s%s",ctrg,type),"",600,600);
        gPad->SetTopMargin(0.10);
        hvar->SetLineColor(kRed);
        hvar->Draw();
        hvar2->SetLineColor(kBlue);
        hvar2->Draw("SAME");
        cvar->SaveAs(Form("pdf/%s_VarPerMean%s_%s.pdf",nametag.c_str(),type,ctrg));
      } // itrg
      assert(h2template);

      // Create sum histograms
      TH2D *h2cumul  = (TH2D*)h2template->Clone("h2cumul"); // Completely unfiltered sum of h2as
      TH2D *h2pcumul = (TH2D*)h2template->Clone("h2pcumul"); // Completely unfiltered sum of h2as
      TH2D *h2ncumul = (TH2D*)h2template->Clone("h2ncumul"); // Completely unfiltered sum of h2as
      TH2D *h2hot    = (TH2D*)h2template->Clone("h2hot");   // Unfiltered sum of h2hot
      TH2D *h2hot2   = (TH2D*)h2template->Clone("h2hot2");  // Filtered sum of h2hot
      TH2D *h2cold   = (TH2D*)h2template->Clone("h2cold");  // Unfiltered sum of h2cold
      TH2D *h2cold2  = (TH2D*)h2template->Clone("h2cold2"); // Filtered sum of h2cold
      for (int idxeta = 1; idxeta <= h2hot->GetNbinsX(); ++idxeta) { // Loop over HCAL eta strips.
        // We check which triggers are in a usable form in this eta region. Triggers ordered from the most inclusive (ZeroBias) to the least inclusive (JetHT500)
        double abseta = fabs(h2hot->GetXaxis()->GetBinCenter(idxeta));
        int breaker = etalims.size();
        for (auto ie = 0u; ie < etalims.size(); ++ie) {
          if (abseta>etalims[ie]) {
            breaker = ie;
            break;
          }
        }
        breaker -= 1;
        cout << "Eta: " << abseta;
        if (breaker>=0) { // Only do something if the eta strip is activated
          const int NTrgs = etatrgs[breaker];
          cout << " No trgs: " << NTrgs << endl;
          for (int idxphi = 1; idxphi < h2hot->GetNbinsY()+1; ++idxphi) {
            // Sum over all triggers, taking weight into account
            double cumulsum = 0, hotsum = 0, coldsum = 0;
            for (int itrg = 0; itrg < NTrgs; ++itrg) {
              cumulsum += h2as[itrg]->GetBinContent(idxeta,idxphi);    // This counter includes everything
              hotsum   += h2hots[itrg]->GetBinContent(idxeta,idxphi);  // A sum of over 3-sigma fluctuations upwards
              coldsum  += h2colds[itrg]->GetBinContent(idxeta,idxphi); // A sum of over 3-sigma fluctuations downwards
            } // for itrg
            double scale = 1.0/NTrgs;
            if      (NTrgs==1) scale /= 4.0;
            else if (NTrgs<4)  scale /= 3.0;
            else if (NTrgs<7)  scale /= 2.0;
            else if (NTrgs<9)  scale /= 1.5;

            cumulsum *= scale; hotsum *= scale; coldsum *= scale;

            // Unfiltered histograms
            h2cumul->SetBinContent(idxeta,idxphi, cumulsum);
            if      (cumulsum>1.0)  h2pcumul->SetBinContent(idxeta,idxphi, cumulsum);
            else if (cumulsum<-1.0) h2ncumul->SetBinContent(idxeta,idxphi, -cumulsum);
            h2hot  ->SetBinContent(idxeta,idxphi, hotsum);
            h2cold ->SetBinContent(idxeta,idxphi, coldsum);

            // Filter maps for hot and cold zones (saved in a .root file): required one sigma per trigger.
            h2hot2 ->SetBinContent(idxeta,idxphi,hotsum >=1.0 ? 10 : 0);
            h2cold2->SetBinContent(idxeta,idxphi,coldsum>=1.0 ? 10 : 0);
          } // for idxphi
        } else {
          cout << endl;
        }
      } // for idxeta

      if (h2hotNew==0) h2hotNew = static_cast<TH2D*>(h2hot2->Clone("newmap"));
      h2hotNew->SetFillStyle(0);
      h2hotNew->SetLineColor(kBlack);
      h2hotNew->GetZaxis()->SetRangeUser(0,10);
      h2hotNew->Scale(h2s[jp::notrigs-1]->Integral());

      TLine *l = new TLine();
      l->SetLineColor(kBlack);

      for (int itrg = 0; itrg != jp::notrigs; ++itrg) {
        h2hots[itrg]->SetFillStyle(0);
        h2hots[itrg]->SetLineColor(kBlack);
        h2hots[itrg]->GetZaxis()->SetRangeUser(0,10);
        h2hots[itrg]->Scale(h2s[jp::notrigs-1]->Integral());
        h2colds[itrg]->SetFillStyle(0);
        h2colds[itrg]->SetLineColor(kWhite);
        h2colds[itrg]->GetZaxis()->SetRangeUser(0,10);
        h2colds[itrg]->Scale(h2s[jp::notrigs-1]->Integral());
        const char *ctrg = jp::triggers[itrg];

        TCanvas *c1 = new TCanvas(Form("c1%s%s",ctrg,type),"",600,600);
        gPad->SetLeftMargin(0.10);
        gPad->SetRightMargin(0.15);
        gPad->SetTopMargin(0.10);
        gPad->SetBottomMargin(0.10);

        h2s[itrg]->SetTitle("Number of jets;#eta_{jet};#phi_{jet}");

        c1->SetLogz();
        h2s[itrg]->Draw("COLZ");
        h2hots[itrg]->Draw("SAMEBOX");
        h2colds[itrg]->Draw("SAMEBOX");
        l->DrawLine(1.305,-TMath::Pi(),1.305,TMath::Pi());
        l->DrawLine(2.964,-TMath::Pi(),2.964,TMath::Pi());
        l->DrawLine(-1.305,-TMath::Pi(),-1.305,TMath::Pi());
        l->DrawLine(-2.964,-TMath::Pi(),-2.964,TMath::Pi());
        gPad->Update();
        gErrorIgnoreLevel = kWarning;
        c1->SaveAs(Form("pdf/%s_njet%s_%s.pdf",nametag.c_str(),type,ctrg));

        TCanvas *c2a = new TCanvas(Form("c2a%s%s",ctrg,type),"",600,600);
        gPad->SetLeftMargin(0.10);
        gPad->SetRightMargin(0.15);
        gPad->SetTopMargin(0.10);
        gPad->SetBottomMargin(0.10);

        // For drawing, mark deficit <-8 as -8 (so blue box instead of white)
        h2as[itrg]->SetTitle(Form("Fluctuation w.r.t. #eta -strip #sigma (%s);#eta_{jet};#phi_{jet}",trgNames[ctrg]));

        double minmaxa = max(h2as[itrg]->GetMaximum(),fabs(h2as[itrg]->GetMaximum()));
        h2as[itrg]->SetMinimum(-minmaxa);
        h2as[itrg]->SetMaximum(+minmaxa);
        ZeroBinsToVal(h2as[itrg],-2*minmaxa);

        h2as[itrg]->Draw("COLZ");
        h2hotNew->DrawClone("SAMEBOX");
        l->DrawLine(1.305,-TMath::Pi(),1.305,TMath::Pi());
        l->DrawLine(2.964,-TMath::Pi(),2.964,TMath::Pi());
        l->DrawLine(-1.305,-TMath::Pi(),-1.305,TMath::Pi());
        l->DrawLine(-2.964,-TMath::Pi(),-2.964,TMath::Pi());

        TCanvas *c2b = new TCanvas(Form("c2b%s%s",ctrg,type),"",600,600);
        gPad->SetLeftMargin(0.10);
        gPad->SetRightMargin(0.15);
        gPad->SetTopMargin(0.10);
        gPad->SetBottomMargin(0.10);

        h2bs[itrg]->SetTitle(Form("Fluctuation w.r.t. #eta -strip mean (%s);#eta_{jet};#phi_{jet}",trgNames[ctrg]));
        h2bs[itrg]->SetMinimum(-1);
        h2bs[itrg]->SetMaximum(+1);
        ZeroBinsToVal(h2bs[itrg],-2);

        h2bs[itrg]->Draw("COLZ");
        h2hotNew->DrawClone("SAMEBOX");
        l->DrawLine(1.305,-TMath::Pi(),1.305,TMath::Pi());
        l->DrawLine(2.964,-TMath::Pi(),2.964,TMath::Pi());
        l->DrawLine(-1.305,-TMath::Pi(),-1.305,TMath::Pi());
        l->DrawLine(-2.964,-TMath::Pi(),-2.964,TMath::Pi());

        c2a->SaveAs(Form("pdf/%s_DiffPerVar%s_%s.pdf",nametag.c_str(),type,ctrg));
        c2b->SaveAs(Form("pdf/%s_DiffPerMean%s_%s.pdf",nametag.c_str(),type,ctrg));
      }
      TCanvas *c0 = new TCanvas(Form("c0%s",type),"",600,600);
      gPad->SetLeftMargin(0.10);
      gPad->SetRightMargin(0.15);
      gPad->SetTopMargin(0.10);
      gPad->SetBottomMargin(0.10);

      h2cumul->SetTitle("Cumulative sum of fluctuations;#eta_{jet};#phi_{jet}");
      double minmaxc = max(h2cumul->GetMaximum(),fabs(h2cumul->GetMaximum()));
      h2cumul->SetMinimum(-minmaxc);
      h2cumul->SetMaximum(+minmaxc);

      h2cumul->Draw("COLZ");

      l->DrawLine(1.305,-TMath::Pi(),1.305,TMath::Pi());
      l->DrawLine(2.964,-TMath::Pi(),2.964,TMath::Pi());
      l->DrawLine(-1.305,-TMath::Pi(),-1.305,TMath::Pi());
      l->DrawLine(-2.964,-TMath::Pi(),-2.964,TMath::Pi());
      h2hotNew->DrawClone("SAMEBOX");

      TCanvas *c0p = new TCanvas(Form("c0p%s",type),"",600,600);
      gPad->SetLeftMargin(0.10);
      gPad->SetRightMargin(0.15);
      gPad->SetTopMargin(0.10);
      gPad->SetBottomMargin(0.10);

      h2pcumul->SetTitle("Cumulative sum of positive fluctuations;#eta_{jet};#phi_{jet}");
      h2pcumul->SetMinimum(1.0);
      c0p->SetLogz();

      h2pcumul->Draw("COLZ");

      l->DrawLine(1.305,-TMath::Pi(),1.305,TMath::Pi());
      l->DrawLine(2.964,-TMath::Pi(),2.964,TMath::Pi());
      l->DrawLine(-1.305,-TMath::Pi(),-1.305,TMath::Pi());
      l->DrawLine(-2.964,-TMath::Pi(),-2.964,TMath::Pi());
      h2hotNew->DrawClone("SAMEBOX");

      TCanvas *c0n = new TCanvas(Form("c0n%s",type),"",600,600);
      gPad->SetLeftMargin(0.10);
      gPad->SetRightMargin(0.15);
      gPad->SetTopMargin(0.10);
      gPad->SetBottomMargin(0.10);

      h2ncumul->SetTitle("Cumulative sum of negative fluctuations;#eta_{jet};#phi_{jet}");
      c0n->SetLogz();

      h2ncumul->Draw("COLZ");

      l->DrawLine(1.305,-TMath::Pi(),1.305,TMath::Pi());
      l->DrawLine(2.964,-TMath::Pi(),2.964,TMath::Pi());
      l->DrawLine(-1.305,-TMath::Pi(),-1.305,TMath::Pi());
      l->DrawLine(-2.964,-TMath::Pi(),-2.964,TMath::Pi());
      h2hotNew->DrawClone("SAMEBOX");

      TCanvas *c0hot = new TCanvas(Form("c0hot%s",type),"",600,600);
      gPad->SetLeftMargin(0.10);
      gPad->SetRightMargin(0.15);
      gPad->SetTopMargin(0.10);
      gPad->SetBottomMargin(0.10);

      h2hot->SetTitle("Cumulative weighted sum of 3#sigma+ up fluctuations;#eta_{jet};#phi_{jet}");
      h2ncumul->SetMinimum(1.0);
      c0hot->SetLogz();

      h2hot->Draw("COLZ");

      l->DrawLine(1.305,-TMath::Pi(),1.305,TMath::Pi());
      l->DrawLine(2.964,-TMath::Pi(),2.964,TMath::Pi());
      l->DrawLine(-1.305,-TMath::Pi(),-1.305,TMath::Pi());
      l->DrawLine(-2.964,-TMath::Pi(),-2.964,TMath::Pi());
      h2hotNew->DrawClone("SAMEBOX");

      TCanvas *c0cold = new TCanvas(Form("c0cold%s",type),"",600,600);
      gPad->SetLeftMargin(0.10);
      gPad->SetRightMargin(0.15);
      gPad->SetTopMargin(0.10);
      gPad->SetBottomMargin(0.10);

      h2cold->SetTitle("Cumulative weighted sum of 3#sigma+ down fluctuations;#eta_{jet};#phi_{jet}");
      c0cold->SetLogz();

      h2cold->Draw("COLZ");

      l->DrawLine(1.305,-TMath::Pi(),1.305,TMath::Pi());
      l->DrawLine(2.964,-TMath::Pi(),2.964,TMath::Pi());
      l->DrawLine(-1.305,-TMath::Pi(),-1.305,TMath::Pi());
      l->DrawLine(-2.964,-TMath::Pi(),-2.964,TMath::Pi());
      h2hotNew->DrawClone("SAMEBOX");

      c0    ->SaveAs(Form("pdf/%s%s_cumulation.pdf",nametag.c_str(),type));
      c0p   ->SaveAs(Form("pdf/%s%s_hotcumulation.pdf",nametag.c_str(),type));
      c0n   ->SaveAs(Form("pdf/%s%s_coldcumulation.pdf",nametag.c_str(),type));
      c0hot ->SaveAs(Form("pdf/%s%s_hots.pdf",nametag.c_str(),type));
      c0cold->SaveAs(Form("pdf/%s%s_colds.pdf",nametag.c_str(),type));

      string typestring = type;
      const char* dirname = typestring=="" ? "all" : string(type).substr(1).c_str();
      fouthot->mkdir(dirname);
      fouthot->cd(dirname);
      h2hot->Write("h2hot");
      h2hot2->Write("h2hotfilter");
      foutcold->mkdir(dirname);
      foutcold->cd(dirname);
      h2cold->Write("h2cold");
      h2cold2->Write("h2hole");

      if (typestring=="" or typestring=="_chf" or typestring=="_nhf") {
        h2hotComb ->Add(h2hot);
        h2hotComb2->Add(h2hot2);
      }
    } // type
    if (phifile) phifile->Close();

    fouthot->cd();
    h2hotComb->Write("h2hot");
    h2hotComb2->Write("h2hotfilter");
    fouthot->Close();
    foutcold->Close();
  } // Sample (data/mc/herwig)
}
