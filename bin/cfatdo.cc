#include "CompoundHistoUnfolding.hh"
#include "Utilities.hh"
#include "TFile.h"
#include <stdio.h>
#include "TApplication.h"
#include "TCanvas.h"
#include "TColor.h"
#include "TH1F.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TSystem.h"
#include "TKey.h"
#include "TStyle.h"
int main(int argc, char * argv[])
{
  const char * observable = "pull_angle";
  const char * binning_method = "NEWOPT";
  const unsigned char levelind = OPT;
  const unsigned char chargeind = CHARGED;
  const unsigned char jetind = LEADING_JET;
  const char * method = "cflip";
  const char * chutitlepa       = "migration; reco [rad]; gen [rad]";
  const char * chutitlepv       = "migration; reco [a.u.]; gen [a.u.]";
  const char * XaxisTitlepa     = "#theta_{p} [rad]";
  const char * YaxisTitlepa     = "#frac{#Delta N}{#Delta #theta_{p}}";  
  const char * XaxisTitlepv     = "#vec{P} [a.u.]";
  const char * YaxisTitlepv     = "#frac{#Delta N}{#Delta #vec{P}}";  
  const char * chutitle         = nullptr;
  const char * XaxisTitle       = nullptr;
  const char * YaxisTitle       = nullptr;
  CompoundHistoUnfolding * chisto(nullptr);
  float bmin;
  float bmax;
  if (string(observable).compare("pull_angle") == 0)
    {
      bmin = 0.0;
      bmax = TMath::Pi();
      //      sfactor = 0.1;
      chutitle = chutitlepa;
      XaxisTitle = XaxisTitlepa;
      YaxisTitle = YaxisTitlepa;
    }
 if (string(observable).compare("pvmag") == 0)
    {
      bmin = 0.0;
      bmax = 0.015;
      //sfactor = 0.6;
      chutitle = chutitlepv;
      XaxisTitle = XaxisTitlepv;
      YaxisTitle = YaxisTitlepv;
    }
 const unsigned char nbins = 20;
 string subdir = observable;
 string bintag = "";
 TString binfilename;
 if (levelind == OPT)
    {
      if (string(binning_method).compare("NEWOPT") == 0)
	{
	  //	  string sigmadirstr = string("SIGMA_") + sfactorstr;
	  bintag = bintag + "_" + binning_method;
	  subdir = subdir + "/" + binning_method;
	  binfilename = TString(/*root://eosuser.cern.ch//*//*"$EOS*/"/eos/user/v/vveckaln/binning_") + observable + "/" + 
	    observable + "_" +
	    tag_charge_types_[chargeind] + '_' + 
	    tag_jet_types_[jetind] +
	    "/bins_" + tag_charge_types_[chargeind] + '_' + 
	    tag_jet_types_[jetind]
	    + ".txt";
	}
      printf("binfilename %s\n", binfilename.Data());
      FILE * binfile = fopen(binfilename, "r");
      printf("binfile %p\n", binfile);
      unsigned long size;
      fscanf(binfile, "%lu\n", &size);
      system((string("cat ") + binfilename.Data()).c_str());
      float * ybins = new float[size];
      int r = 0;
      unsigned char ind = 0;
      while (r >= 0)
	{
	  float store;
	  r = fscanf(binfile, "%f\n", &store);
	  if (r >= 0)
	    {
	      ybins[ind] = store;
	    }
	  ind ++;
	}
      fclose(binfile);
      float * xbins = CompoundHistoUnfolding::split(CompoundHistoUnfolding::split(ybins, size), 2 * size - 1);
      for (unsigned char bin_ind = 0; bin_ind < 2 * ((2*size-1) - 1) + 1 ; bin_ind ++)
	{
	  printf("bin_ind %u xbins[bin_ind] %f\n", bin_ind, xbins[bin_ind]);
	}
      // getchar();
      /*for (unsigned char bin_ind = 0; bin_ind < size; bin_ind ++)
	{
	  printf("bin_ind %u ybins[ind] %f\n", bin_ind, ybins[bin_ind]);
	}
	getchar();*/
      chisto = new CompoundHistoUnfolding(TString(tag_opt[levelind]) + "_" + "MC13TeV_TTJets_cflip" + "_" + method, chutitle, 4*(size-1) , xbins, size - 1, ybins);
      chisto -> SetOrigBinWidth((bmax - bmin)/nbins);

      
    }


  const TString filename = "root://eosuser.cern.ch//eos/user/v/vveckaln/unfolding_nominal/pull_angle/NEWOPT/leading_jet_chconst_MC13TeV_TTJets_pull_angle_OPT_nominal_NEWOPT/save_fix.root";
  printf("opening file\n");
  TFile * file = TFile::Open(filename, "UPDATE");
  printf("file %p\n", file);
  file -> ls();
  CompoundHistoUnfolding * chistonominal = (CompoundHistoUnfolding * )((TKey*) file -> GetListOfKeys() -> At(0)) -> ReadObj(); 
  chisto -> SetCHUnominal(chistonominal);
  chisto -> SetSignalTag("MC13TeV_TTJets_cflip");
  chisto -> jetcode    = jetind;
  chisto -> chargecode = chargeind;
  chisto -> SetObservable(observable);
  chisto -> SetLuminosity(35874.8);
  chisto -> SetCOM("13 TeV");
  chisto -> SetXaxisTitle(XaxisTitle);
  chisto -> SetYaxisTitle(YaxisTitle);
  //chisto -> TransferHistos("/afs/cern.ch/work/v/vveckaln/private/CMSSW_9_4_11/src/TopLJets2015/TopAnalysis/data/era2016/samples.json");

  chisto -> LoadHistos_cflip("/afs/cern.ch/work/v/vveckaln/private/CMSSW_9_4_11/src/TopLJets2015/TopAnalysis/data/era2016/samples_cflip.json");
  chisto -> LoadHistos_cflip("/afs/cern.ch/work/v/vveckaln/private/CMSSW_9_4_11/src/TopLJets2015/TopAnalysis/data/era2016/expsyst_samples.json", EXPSYS, "MC13TeV_TTJets_cflip");
  // chisto -> TransferHistos("/afs/cern.ch/work/v/vveckaln/private/CMSSW_9_4_11/src/TopLJets2015/TopAnalysis/data/era2016/qcd_samples.json");
  //chunf.AddXsecSystematics();

  // chunf.TransferHistos("/afs/cern.ch/work/v/vveckaln/private/CMSSW_9_4_11/src/TopLJets2015/TopAnalysis/data/era2016/syst_samples_MC13TeV_SingleT_tW.json", THEORSYS, "MC13TeV_SingleT_tW");
  const char * sampletag = "MC13TeV_TTJets";

  //  chisto -> TransferHistos("/afs/cern.ch/work/v/vveckaln/private/CMSSW_9_4_11/src/TopLJets2015/TopAnalysis/data/era2016/syst_samples_pruned.json", THEORSYS, sampletag);
  // printf("ended histo transfer\n");
  // getchar();
  // chunf.Do();
  //  chisto -> Write();
  file -> Close();
}
