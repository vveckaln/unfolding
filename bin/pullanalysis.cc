#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "CompoundHistoUnfolding.hh"
#include "TChain.h"
#include <stdio.h>
#include "TChain.h"
#include "TFile.h"
#include "TKey.h"
#include "TApplication.h"
#include "THStack.h";
#include "TStyle.h"
#include "TROOT.h"
//no RECO for OUT
int main(int argc, char * argv[])
{
  printf("Running executable \"pullanalysis\"\n");
  gStyle -> SetOptStat(0);
  //  TApplication app("myapp", 0, 0);
  gROOT -> SetBatch(kTRUE);
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
 TString binfilename;
 if (levelind == OPT)
   {
     //	  string sigmadirstr = string("SIGMA_") + sfactorstr;
     bintag = bintag + "_" + binning_method;
     subdir = subdir + "/" + binning_method;
   }

  const TString input_folder = TString("/eos/user/v/vveckaln/unfolding_") + method + "/";
  const TString input_file_name = input_folder + subdir + "/" + 
    tag_jet_types_[jetind] + '_' + 
    tag_charge_types_[chargeind] + '_' + 
    sampletag + '_' + 
    observable + '_' + 
    tag_opt[levelind] + '_' + 
    method + bintag + "/save.root";
  TFile  * input_file = TFile::Open(input_file_name);
  CompoundHistoUnfolding * ch = (CompoundHistoUnfolding * )((TKey*) input_file -> GetListOfKeys() -> At(0)) -> ReadObj(); 
  //  chisto -> SetCHUnominal(ch);

  input_file -> Close();
  ch -> PullAnalysis();
  TCanvas *c = ch -> stabpur();
  c -> SaveAs("output/stabpur.png");
  ch -> PrintMigrationMatrix();
}
