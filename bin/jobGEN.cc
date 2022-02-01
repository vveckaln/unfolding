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
  gStyle -> SetOptStat(0);
  gStyle -> SetOptTitle(0);
  printf("running executable jobGEN\n");
  printf("1 [%s] 2 [%s] 3 [%s] 4 [%s] 5 [%s] 6 [%s] 7 [%s] 8 [%s] 9 [%s] 10 [%s] 11 [%]\n", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10]); 
  const unsigned char jetind    = stoi(argv[1]);
  const unsigned char chargeind = stoi(argv[2]);
  const char * sampletag        = argv[3];
  const char * observable       = argv[4];
  const unsigned char levelind  = stoi(argv[5]);
  const char * method           = argv[6];
  const bool   presampleTTJets  = string(argv[7]).compare("True") == 0 ? true : false;
  const char * binning_method   = argv[8];
  const bool   calculate_bins   = string(argv[9]).compare("True") == 0 ? true : false;
  const float  sfactor          = stof(argv[10]);
  string sfactorstr(argv[10]);
  const size_t start_pos = sfactorstr.find(".");
  if(start_pos != std::string::npos)
    {
      sfactorstr.replace(start_pos, 1, "p");
    }

  printf("jet                  %s\n", tag_jet_types_[jetind]);
  printf("charge               %s\n", tag_charge_types_[chargeind]); 
  printf("sampletag            %s\n", sampletag);
  printf("observable           %s\n", observable);
  printf("level                %s\n", tag_opt[levelind]);
  printf("method               %s\n", method);
  printf("presample TTjets     %s\n", presampleTTJets ? "True" : "False");
  printf("binning method       %s\n", binning_method);
  printf("calculate bins       %s\n", calculate_bins ? "True" : "False");
  printf("sigma factor         %f\n", sfactor);
  const char * folder           = "output";
  const char * chutitlepa       = "migration; reco #left[#frac{rad}{#pi}#right]; gen #left[#frac{rad}{#pi}#right]";
  const char * chutitlepv       = "migration; reco [a.u.]; gen [a.u.]";
  const char * chutitlepvpar       = "migration; reco [a.u.]; gen [a.u.]";
  const char * XaxisTitlepa     = "#theta_{p} #left[#frac{rad}{#pi}#right]";
  const char * YaxisTitlepa     = "#frac{#Delta N}{#Delta #theta_{p}}";  
  const char * XaxisTitlepv     = "#vec{P} [a.u.]";
  const char * YaxisTitlepv     = "#frac{#Delta N}{#Delta #vec{P}}";  
  const char * XaxisTitlepvpar     = "#vec{P}|| [a.u.]";
  const char * YaxisTitlepvpar     = "#frac{#Delta N}{#Delta #vec{P}||}";  

  const char * chutitle         = nullptr;
  const char * XaxisTitle       = nullptr;
  const char * YaxisTitle       = nullptr;
  CompoundHistoUnfolding * chisto(nullptr);
  float bmin;
  float bmax;
  if (string(observable) == "pull_angle")
    {
      bmin = 0.0;
      bmax = 1.0;//TMath::Pi();
      //      sfactor = 0.1;
      chutitle = chutitlepa;
      XaxisTitle = XaxisTitlepa;
      YaxisTitle = YaxisTitlepa;
    }
 if (string(observable) == "pvmag")
    {
      bmin = 0.0;
      bmax = 0.015;
      //sfactor = 0.6;
      chutitle = chutitlepv;
      XaxisTitle = XaxisTitlepv;
      YaxisTitle = YaxisTitlepv;
    }

 if (string(observable) == "pvmag_par")
    {
      bmin = 0.0;
      bmax = 0.005;
      //sfactor = 0.6;
      chutitle = chutitlepv;
      XaxisTitle = XaxisTitlepv;
      YaxisTitle = YaxisTitlepv;
    }

 const unsigned char nbins = 20;
 string subdir = observable;
 string bintag = "";
 if (levelind == ORIG)
   {
     subdir = subdir + "/" + tag_opt[levelind];
   }
 TString binfilename;
 if (levelind == OPT)
    {
      if (string(binning_method) == "SIGMA")
	{
	  string sigmadirstr = string("SIGMA_") + sfactorstr;
	  bintag += "_" + sigmadirstr;

	  subdir += "/" + sigmadirstr;
	  binfilename = TString("root://eosuser.cern.ch//eos/user/v/vveckaln/bins/") + 
	    observable + "/" + sigmadirstr + "/" +
	    tag_jet_types_[jetind] + '_' + 
	    tag_charge_types_[chargeind] + '_' + 
	    "MC13TeV_TTJets" + '_' + 
	    observable + '_' + 
	    tag_opt[ORIG] + "_" + 
	    "nominal" + "_binSIGMA_" + 
	    sfactorstr +
	    "/bins" + sigmadirstr + ".txt";
	  char command[256];
	  sprintf( command, "xrdcp %s .", binfilename.Data());
	  system(command);
	  binfilename = TString("bins") + sigmadirstr + ".txt";
	}
      if (string(binning_method) == "NEWOPT")
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
      else
	{
	  binfilename = TString("/afs/cern.ch/work/v/vveckaln/private/UnfoldingLIB/binfiles/bins") + binning_method + "_" + observable + ".txt";
	  bintag = bintag + "_" + binning_method;
	  subdir = subdir + "/" + binning_method;
	}
      FILE * binfile = fopen(binfilename, "r");
      printf("binfile %p\n", binfile);
      printf("binfilename %s\n", binfilename.Data());
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
      float * xbins = CompoundHistoUnfolding::split(ybins, size);
      /*      for (unsigned char bin_ind = 0; bin_ind < 2 * (size - 1) + 1; bin_ind ++)
	{
	  printf("bin_ind %u xbins[bin_ind] %f\n", bin_ind, xbins[bin_ind]);
	}
      for (unsigned char bin_ind = 0; bin_ind < size; bin_ind ++)
	{
	  printf("bin_ind %u ybins[ind] %f\n", bin_ind, ybins[bin_ind]);
	}
	getchar();*/
      chisto = new CompoundHistoUnfolding(TString(tag_opt[levelind]) + "_" + sampletag, chutitle, 2 * (size - 1) , xbins, size - 1, ybins);
      printf("name %s\n", chisto -> GetName());
      //getchar();
      chisto -> SetOrigBinWidth((bmax - bmin)/nbins);

      
    }
  if (levelind == 0)
    {
      chisto = new CompoundHistoUnfolding(TString(tag_opt[levelind]) + "_" + sampletag + "_" + method, chutitle, nbins, bmin, bmax, nbins, bmin, bmax);
    }
  //  chisto -> SetName(tag_opt[levelind]);
  chisto -> jetcode    = jetind;
  chisto -> chargecode = chargeind;
  chisto -> SetSignalTag("MC13TeV_TTJets");
  chisto -> SetObservable(observable);
  chisto -> optcode    = levelind;
  chisto -> SetMethod(method);
  chisto -> SetFolder(folder);
  chisto -> SetLuminosity(1.0);
  chisto -> SetCOM("13 TeV");
  chisto -> SetXaxisTitle(XaxisTitle);
  chisto -> SetYaxisTitle(YaxisTitle);
  chisto -> calculate_bins = calculate_bins;
  chisto -> LoadHistos(TString("/afs/cern.ch/work/v/vveckaln/private/CMSSW_9_4_11/src/TopLJets2015/TopAnalysis/data/era2016/samples_") + method + ".json", THEORSYS, sampletag);

  chisto -> SetCHUnominal(nullptr);
  TFile * f = TFile::Open("output/save.root", "RECREATE");
  chisto -> Write();
  f -> Close();
  delete chisto;
}
