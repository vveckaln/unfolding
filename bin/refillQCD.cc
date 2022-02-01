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
  printf("Running executable \"refillQCD\"\n");

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
  string subdir = observable;

  string bintag = "";
  if (levelind == ORIG)
    {
      subdir = subdir + "/" + tag_opt[levelind];
    }
  if (levelind == OPT)
    {
      if (string(binning_method).compare("NEWOPT") == 0)
	{
	  //	  string sigmadirstr = string("SIGMA_") + sfactorstr;
	  bintag = bintag + "_" + binning_method;
	  subdir = subdir + "/" + binning_method;
	}
    }
  const TString input_folder = TString("root://eosuser.cern.ch//eos/user/v/vveckaln/unfolding_nominal/");
  const TString input_file_name_MC13TeV_TTJets_qcd = input_folder + subdir + "/" + 
    tag_jet_types_[jetind] + '_' + 
    tag_charge_types_[chargeind] + '_' + 
    "MC13TeV_TTJets" + '_' + 
    observable + '_' + 
    tag_opt[levelind] + '_' + 
    "nominal" + bintag + "/refillQCD/save_refillQCD.root";
  const TString input_file_name_MC13TeV_TTJets = input_folder + subdir + "/" + 
    tag_jet_types_[jetind] + '_' + 
    tag_charge_types_[chargeind] + '_' + 
    "MC13TeV_TTJets" + '_' + 
    observable + '_' + 
    tag_opt[levelind] + '_' + 
    "nominal" + bintag + "/save.root"; 
  system((TString("xrdcp -f ") + input_file_name_MC13TeV_TTJets + " " + input_file_name_MC13TeV_TTJets_qcd).Data());
  printf("opening nominal CHU %s\n", input_file_name_MC13TeV_TTJets_qcd.Data());

  TFile  * input_file_MC13TeV_TTJets_qcd = TFile::Open(input_file_name_MC13TeV_TTJets_qcd);
  CompoundHistoUnfolding * chisto = (CompoundHistoUnfolding * )((TKey*) input_file_MC13TeV_TTJets_qcd -> GetListOfKeys() -> At(0)) -> ReadObj(); 
  vector<HistoUnfolding *>  * backgrounds = chisto -> GetV(IN, BACKGROUNDMO);
  // printf("before \n");
  // for (vector<HistoUnfolding *>::iterator it = backgrounds -> begin(); it != backgrounds -> end(); it ++)
  //   {
  //     (*it) -> ls();
  //   }

  for (vector<HistoUnfolding *>::iterator it = backgrounds -> begin(); it != backgrounds -> end(); )
    {
      if (TString((*it) -> GetCategory()) == "QCD" or TString((*it) -> GetColor()) == "QCD")
	{
	  it = backgrounds -> erase(it);
	}
      else
	{
	  ++ it;
	}
    }
  // printf("checking \n");
  // for (vector<HistoUnfolding *>::iterator it = backgrounds -> begin(); it != backgrounds -> end(); it ++)
  //   {
  //     (*it) -> ls();
  //   }

  
  chisto -> LoadHistos("/afs/cern.ch/work/v/vveckaln/private/CMSSW_9_4_11/src/TopLJets2015/TopAnalysis/data/era2016/qcd_samples.json");

  chisto -> Process(reg);
  //      printf("stabpur after processing\n");
  // chisto -> stabpur();
  // getchar();
  //      chisto -> PullAnalysis();
  TCanvas *crecoIN = chisto -> CreateCombinedPlot(RECO, IN);
  crecoIN -> SaveAs(TString("output/") + crecoIN -> GetName() +  ".png");     
  TPad * pad1 = (TPad *) crecoIN -> GetListOfPrimitives() -> At(0);
	    
  TCanvas *cgenIN = chisto -> CreateCombinedPlot(GEN, IN);
  cgenIN -> SaveAs(TString("output/") + cgenIN -> GetName() +  ".png");     
      
  TCanvas *crecoOUT = chisto -> CreateCombinedPlot(RECO, OUT);
  crecoOUT -> SaveAs(TString("output/") + crecoOUT -> GetName() +  ".png");     

  TCanvas *cgenOUT = chisto -> CreateCombinedPlot(GEN, OUT);
  cgenOUT -> SaveAs(TString("output/") + cgenOUT -> GetName() +  ".png");     
  FILE * bfile = fopen("output/binfile.txt", "w");
  fprintf(bfile, "%u\n", chisto -> GetLevel(OUT) -> GetHU(SIGNALPROXY, OUT) -> GetTH1(GEN) -> GetNbinsX());
  for (unsigned char bind = 1; bind < chisto -> GetLevel(OUT) -> GetHU(SIGNALPROXY, OUT) -> GetTH1(GEN) -> GetNbinsX() + 2; bind ++)
    {
      fprintf(bfile, "%f\n", chisto -> GetLevel(OUT) -> GetHU(SIGNALPROXY, OUT) -> GetTH1(GEN) -> GetBinLowEdge(bind));

    }
  fclose(bfile);
  TCanvas * stabpur = chisto -> stabpur();
  stabpur -> SaveAs("output/stabpur.png");
  chisto -> Write(nullptr, TObject::kOverwrite);
  input_file_MC13TeV_TTJets_qcd -> Close();
  delete chisto;
}
