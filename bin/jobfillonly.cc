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
  printf("1 [%s] 2 [%s] 3 [%s] 4 [%s] 5 [%s] 6 [%s] 7 [%s] 8 [%s] 9 [%s] 10 [%s] 11 [%s]\n", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], argv[11]); 
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
  const bool   reg              = string(argv[11]).compare("True") == 0 ? true : false;
  printf("regularisation %s\n", reg ? "True" : "False");  
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
  printf("binning method      [%s]\n", binning_method);
  printf("calculate bins       %s\n", calculate_bins ? "True" : "False");
  printf("sigma factor         %f\n", sfactor);
  printf("regularisation       %s\n", reg ? "True" : "False");
  const char * folder           = "output";
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
 if (levelind == ORIG)
   {
     subdir = subdir + "/" + tag_opt[levelind];
   }
 TString binfilename;
 if (levelind == OPT)
    {
      if (string(binning_method).compare("SIGMA") == 0)
	{
	  string sigmadirstr = string("SIGMA_") + sfactorstr;
	  bintag += "_" + sigmadirstr;

	  subdir += "/" + sigmadirstr;
	  binfilename = TString("root://eosuser.cern.ch//$EOS/bins/") + 
	    observable + "/" + sigmadirstr + "/" +
	    tag_jet_types_[jetind] + '_' + 
	    tag_charge_types_[chargeind] + '_' + 
	    "MC13TeV_TTJets" + '_' + 
	    observable + '_' + 
	    tag_opt[ORIG] + "_" + 
	    "nominal" + "_binSIGMA_" + 
	    sfactorstr +
	    "/bins" + sigmadirstr + ".txt";
	}
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
      else
	{
	  binfilename = TString("/afs/cern.ch/work/v/vveckaln/private/UnfoldingLIB/binfiles/bins") + binning_method + "_" + observable + ".txt";
	  bintag = bintag + "_" + binning_method;
	  subdir = subdir + "/" + binning_method;
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
      chisto = new CompoundHistoUnfolding(TString(tag_opt[levelind]) + "_" + sampletag + "_" + method, chutitle, 4*(size-1) , xbins, size - 1, ybins);
      chisto -> SetOrigBinWidth((bmax - bmin)/nbins);

      
    }
  if (levelind == 0)
    {
      chisto = new CompoundHistoUnfolding(TString(tag_opt[levelind]) + "_" + sampletag + "_" + method, chutitle, nbins, bmin, bmax, nbins, bmin, bmax);
    }
  if (not presampleTTJets)
    {
      const TString input_folder = TString("/eos/user/v/vveckaln/unfolding_") + method + "/";
      const TString input_file_name_MC13TeV_TTJets = input_folder + subdir + "/" + 
	tag_jet_types_[jetind] + '_' + 
	tag_charge_types_[chargeind] + '_' + 
	"MC13TeV_TTJets" + '_' + 
	observable + '_' + 
	tag_opt[levelind] + '_' + 
	method + bintag + "/save.root";
      TFile  * input_file_MC13TeV_TTJets = TFile::Open(input_file_name_MC13TeV_TTJets);
      CompoundHistoUnfolding * chistonominal = (CompoundHistoUnfolding * )((TKey*) input_file_MC13TeV_TTJets -> GetListOfKeys() -> At(0)) -> ReadObj(); 
      chisto -> SetCHUnominal(chistonominal);
      input_file_MC13TeV_TTJets -> Close();
    }

  //  chisto -> SetName(tag_opt[levelind]);
  chisto -> jetcode    = jetind;
  chisto -> chargecode = chargeind;
  chisto -> SetSignalTag(sampletag);
  chisto -> SetObservable(observable);
  chisto -> optcode    = levelind;
  chisto -> SetMethod(method);
  chisto -> SetFolder(folder);
  chisto -> SetLuminosity(35874.8);
  chisto -> SetCOM("13 TeV");
  chisto -> SetXaxisTitle(XaxisTitle);
  chisto -> SetYaxisTitle(YaxisTitle);
  TFile * f = TFile::Open("output/save.root", "RECREATE");
  chisto -> calculate_bins = calculate_bins;
  chisto -> LoadHistos("/afs/cern.ch/work/v/vveckaln/private/CMSSW_8_0_26_patch1/src/TopLJets2015/TopAnalysis/data/era2016/samples.json");
  // printf("stabpur after loading\n");
  // chisto -> stabpur();
  // getchar();
  chisto -> LoadHistos("/afs/cern.ch/work/v/vveckaln/private/CMSSW_8_0_26_patch1/src/TopLJets2015/TopAnalysis/data/era2016/qcd_samples.json");
  chisto -> AddXsecSystematics();
  chisto -> LoadHistos("/afs/cern.ch/work/v/vveckaln/private/CMSSW_8_0_26_patch1/src/TopLJets2015/TopAnalysis/data/era2016/expsyst_samples.json", EXPSYS, sampletag);
  chisto -> LoadHistos("/afs/cern.ch/work/v/vveckaln/private/CMSSW_8_0_26_patch1/src/TopLJets2015/TopAnalysis/data/era2016/syst_samples_pruned.json", THEORSYS, sampletag);
  
  chisto -> LoadHistos("/afs/cern.ch/work/v/vveckaln/private/CMSSW_8_0_26_patch1/src/TopLJets2015/TopAnalysis/data/era2016/syst_samples_MC13TeV_SingleT_tW.json", THEORSYS, "MC13TeV_SingleT_tW");
  chisto -> LoadHistos("/afs/cern.ch/work/v/vveckaln/private/CMSSW_8_0_26_patch1/src/TopLJets2015/TopAnalysis/data/era2016/syst_samples_MC13TeV_SingleTbar_tW.json", THEORSYS, "MC13TeV_SingleTbar_tW");

  if (string(method).compare("cflip") == 0 and string(sampletag).compare("MC13TeV_TTJets") == 0)
    {
      chisto -> MarkSysSample("MC13TeV_TTJets_cflip");
    }
  if (not calculate_bins)
    {
      chisto -> ApproximateTheorSys();
      //      chisto -> Process(reg);
      //      printf("stabpur after processing\n");
      // chisto -> stabpur();
      // getchar();
      //      chisto -> PullAnalysis();
      // TCanvas *crecoIN = chisto -> CreateCombinedPlot(RECO, IN);
      // crecoIN -> SaveAs(TString("output/") + crecoIN -> GetName() +  ".png");     
      // TPad * pad1 = (TPad *) crecoIN -> GetListOfPrimitives() -> At(0);
	    
      // TCanvas *cgenIN = chisto -> CreateCombinedPlot(GEN, IN);
      // cgenIN -> SaveAs(TString("output/") + cgenIN -> GetName() +  ".png");     
      
      // TCanvas *crecoOUT = chisto -> CreateCombinedPlot(RECO, OUT);
      // crecoOUT -> SaveAs(TString("output/") + crecoOUT -> GetName() +  ".png");     

      // TCanvas *cgenOUT = chisto -> CreateCombinedPlot(GEN, OUT);
      // cgenOUT -> SaveAs(TString("output/") + cgenOUT -> GetName() +  ".png");     

      // FILE * pFile = fopen("output/chi2.txt", "w");
      // const double chi2 = chisto -> GetChi();
      // const unsigned char ndf = chisto -> GetLevel(OUT) -> GetHU(SIGNALPROXY, OUT) -> GetTH1(GEN) -> GetNbinsX() - 1;
      // const float pvalue = TMath::Prob(chi2, ndf);
      // fprintf(pFile, "%s, %f %f\n", chisto -> GetLevel(IN) -> GetHU(SIGNALPROXY, OUT) -> GetTitle(), chi2, pvalue);
      HistoUnfolding *signaltemp = chisto -> GetLevel(IN) -> GetHU(SIGNALPROXY, IN);
      // for (vector<HistoUnfolding *> :: iterator it = chisto -> GetLevel(IN) -> GetV(SYSMO, "MC13TeV_TTJets")  -> begin(); it != chisto -> GetLevel(IN) -> GetV(SYSMO, "MC13TeV_TTJets") -> end(); it ++)
      // 	{
      // 	  chisto -> GetLevel(IN) -> GetHURef(SIGNALPROXY, IN) = * it;
      // 	  const double chi2 = chisto -> GetChi();
      // 	  const float pvalue = TMath::Prob(chi2, ndf);
      // 	  fprintf(pFile, "%s, %f %f\n", chisto -> GetLevel(IN) -> GetHU(SIGNALPROXY, IN) -> GetTitle(), chi2, pvalue);
	  
      // 	}
      chisto -> GetLevel(IN) -> GetHURef(SIGNALMO) = signaltemp;
      // fclose(pFile);
      // FILE * bfile = fopen("output/binfile.txt", "w");
      // fprintf(bfile, "%u\n", chisto -> GetLevel(OUT) -> GetHU(SIGNALPROXY, OUT) -> GetTH1(GEN) -> GetNbinsX());
      // for (unsigned char bind = 1; bind < chisto -> GetLevel(OUT) -> GetHU(SIGNALPROXY, OUT) -> GetTH1(GEN) -> GetNbinsX() + 2; bind ++)
      // 	{
      // 	  fprintf(bfile, "%f\n", chisto -> GetLevel(OUT) -> GetHU(SIGNALPROXY, OUT) -> GetTH1(GEN) -> GetBinLowEdge(bind));

      // 	}
      // fclose(bfile);
      /*f -> cd();
      chisto -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Write();
      printf("GEN IN\n");*/
      //chisto -> GetLevel(IN) -> GetHU(SIGNALPROXY, IN) -> GetTH2() -> ProjectionY() -> Print("all");
      //chisto -> GetLevel(OUT) -> GetHU(SIGNALMO) -> GetTH1(RECO) -> Write();
      //chisto -> GetLevel(OUT) -> GetHU(SIGNALMO) -> GetTH1(GEN) -> Write();
      //      chisto -> GetLevel(OUT) -> GetHU(SIGNALPROXY, OUT) -> GetTH1(GEN) -> Print("all");
      //chisto -> input -> Write();
      /*if (TString(sampletag) == "MC13TeV_TTJets")
	{
	 
	  chisto -> GetSys(IN, "MC13TeV_TTJets_jec_RelativeFSR_up") -> Write();
	  chisto -> GetSys(OUT, "MC13TeV_TTJets_jec_RelativeFSR_up") -> Write();
	  chisto -> GetSys(OUT, "MC13TeV_TTJets_jec_RelativeFSR_up") -> GetTH1(GEN) -> Write();
	  chisto -> GetSys(IN, "MC13TeV_TTJets_jec_RelativeFSR_down") -> GetTH2() -> Write();
	  chisto -> GetSys(OUT, "MC13TeV_TTJets_jec_RelativeFSR_down") -> GetTH1(RECO) -> Write();
	  chisto -> GetSys(OUT, "MC13TeV_TTJets_jec_RelativeFSR_down") -> GetTH1(GEN) -> Write();
	}*/
      /*if (string(presampleTTJets).compare("True") == 0)
	{
	  printf("saving hunominal\n");
	  HistoUnfolding *hunominalth1 = chisto -> GetLevel(OUT) -> GetHU(SIGNALMO);
	  hunominalth1 -> SetName("unfolded_data_nominal");
	  hunominalth1 -> ls();
	  hunominalth1 -> Write();
	  
	  }*/
    }
  //chisto.Do();
  // if (string(binning_method).compare("SIGMA") == 0)
  //   {
  //     unsigned long size;
  //     float * ybins = chisto -> splitForMinSigmaM(size, sfactor);
  //     string binfilename(folder); 
  //     binfilename = binfilename + "/binsSIGMA_" + sfactorstr + ".txt";
  //     printf("binfilename %s\n", binfilename.c_str());
  //     FILE * pFile_bins = fopen(binfilename.c_str(), "w");
  //     fprintf(pFile_bins, "%lu\n", size);

  //     for (unsigned long ind = 0; ind < size; ind ++)
  // 	{
  // 	  fprintf(pFile_bins, "%f\n", ybins[ind]);
  // 	}
  //     fclose(pFile_bins);
  //     delete [] ybins;
  //     //f -> cd();
  //     //      chisto -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Write();
  //   }
  f -> cd();
  chisto -> SetCHUnominal(nullptr);
  chisto -> Write();
  f -> Close();
  delete chisto;
}
