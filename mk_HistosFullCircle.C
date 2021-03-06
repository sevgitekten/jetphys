// Purpose: Run the creation of output-*2*.root files and plotting
// Author:  hannu.siikonen@cern.ch
// Created: July 4, 2019

// Normalization: compile code
R__LOAD_LIBRARY(HistosNormalize.C+g)
// Fuse: compile code
R__LOAD_LIBRARY(HistosFuse.C+g)
// Combination: compile code
R__LOAD_LIBRARY(HistosCombine.C+g)
// DrawFracs: plot the results
#include "DrawFracs.h"
R__LOAD_LIBRARY(DrawFracs.C+g)

void mk_HistosFullCircle() {
  // Quick configuration settings:
  bool dooutput = true;
  bool doplots  = true;
  bool dodt     = true;
  bool dopy     = true;
  bool dohw     = false;
  bool donu     = true;
  bool doht     = false;
  bool doall    = true;
  // Choose one: 0:16BCD,1:16EF,2:16GH,3:16BCDEFGH
  //             4:17B,5:17C,6:17D,7:17E,8:17F,9:17BCDEF
  //             10:18A,11:18B,12:18C,13:18D,14:18ABCD
  int lumidx = 9;

  // Output-*-2*.root files:
  if (dooutput) {
    if (dodt) {
      //HistosNormalize("DATA");
      HistosCombine("DATA");
    }
    if (donu) {
      HistosNormalize("NU");
      HistosCombine("NU");
    }
    if (dopy) {
      HistosNormalize("MC");
      HistosCombine("MC");
      if (donu) {
        HistosFuse("MC");
        HistosCombine("MCNU");
      }
    }
    if (dohw) {
      HistosNormalize("HW");
      HistosCombine("HW");
      if (donu) {
        HistosFuse("HW");
        HistosCombine("HWNU");
      }
    }
    if (doht) {
      HistosNormalize("HT");
      HistosCombine("HT");
      if (donu) {
        HistosFuse("HT");
        HistosCombine("HTNU");
      }
    }
  }

  // Plotting:
  if (doplots) {
    vector<const char*> savedirs;
    vector<const char*> mctypes;
    vector<bool> dotfracs;
    if (donu) {
      if (doall) {
        savedirs.push_back("nupdf");
        mctypes.push_back("NU");
        dotfracs.push_back(true);
      }
      if (dopy) {
        savedirs.push_back("pynupdf");
        mctypes.push_back("MCNU");
        dotfracs.push_back(false);
      }
      if (dohw) {
        savedirs.push_back("hwnupdf");
        mctypes.push_back("HWNU");
        dotfracs.push_back(false);
      }
      if (doht) {
        savedirs.push_back("htnupdf");
        mctypes.push_back("HTNU");
        dotfracs.push_back(false);
      }
    }
    if (!donu or doall) {
      if (dopy) {
        savedirs.push_back("pypdf");
        mctypes.push_back("MC");
        dotfracs.push_back(true);
      }
      if (dohw) {
        savedirs.push_back("hwpdf");
        mctypes.push_back("HW");
        dotfracs.push_back(true);
      }
      if (dohw) {
        savedirs.push_back("htpdf");
        mctypes.push_back("HT");
        dotfracs.push_back(true);
      }
    }

    // Picking luminosity info for the plots.
    string title, lumit;
    if (lumidx==0) { // 2016 IOV's: tot. 35.86 fb^{-1}
      title="2016BCD";
      lumit="12.6 fb^{-1}";
    } else if (lumidx==1) {
      title="2016EF";
      lumit="6.7 fb^{-1}";
    } else if (lumidx==2) {
      title="2016GH";
      lumit="16.5 fb^{-1}";
    } else if (lumidx==3) {
      title="2016BCDEFGH";
      lumit="35.86 fb^{-1}";
    } else if (lumidx==4) { // 2017 IOV's: tot. 41.52 fb^{-1}
      title="2017B";
      lumit="4.8 fb^{-1}";
    } else if (lumidx==5) {
      title="2017C";
      lumit="9.6 fb^{-1}";
    } else if (lumidx==6) {
      title="2017D";
      lumit="4.2 fb^{-1}";
    } else if (lumidx==7) {
      title="2017E";
      lumit="9.3 fb^{-1}";
    //} else if (lumidx==5) {
    //  title="2017DE";
    //  lumit="13.6 fb^{-1}";
    } else if (lumidx==8) {
      title="2017F";
      lumit="13.5 fb^{-1}";
    } else if (lumidx==9) {
      title="2017BCDEF";
      lumit="41.52 fb^{-1}";
    } else if (lumidx==10) { // 2018 IOV's: tot. 53.27 fb^{-1}
      title="2018A";
      lumit="14.1 fb^{-1}";
    } else if (lumidx==11) {
      title="2018B";
      lumit="7.1 fb^{-1}";
    } else if (lumidx==12) {
      title="2018C";
      lumit="6.9 fb^{-1}";
    } else if (lumidx==13) {
      title="2018D";
      lumit="29.2 fb^{-1}";
    } else if (lumidx==14) {
      title="2018ABCD";
      lumit="53.27 fb^{-1}";
    } else {
      cout << "Lumi index erroneous, check your value: " << lumidx << endl;
      return;
    }

    for (unsigned idx = 0; idx < mctypes.size(); ++idx) {
      const char *mctype = mctypes[idx];
      const char *savedir = savedirs[idx];
      bool do_t = dotfracs[idx];
      cout << mctype << endl;
      Fracs fracs("./","./",title,lumit,savedir,false,mctype,"DT",3);
      Fracs dfracs("./","./",title,lumit,savedir,false,mctype,"DT",3,"");
      vector<unsigned int> order = {0,3,1,2};//,4,5};
      for (auto &i : order)
        fracs.DrawFracs(i);
      dfracs.DrawFracs(0);
      if (do_t) {
        Fracs tfracs("./","./",title,lumit,savedir,true,mctype,"DT",3);
        for (auto &i : order)
          tfracs.DrawFracs(i);
      }
    }
  }
}
