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
const char * privatesamples[3] = {"sherpa", "dire2002", "herwig7"};
const unsigned char npsamples = 3;
//no RECO for OUT
int main(int argc, char * argv[])
{
  printf("Running executable \"bayesian2\"\n");
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
  printf("signaltag %s\n", ch -> signaltag);
  //  ch -> SetSignalTag(TString(ch -> signaltag) + "_bayesian");
  //  TFile * fileout = TFile::Open("root://eosuser.cern.ch//$EOS/unfolding_nominal/pull_angle/ATLAS4/leading_jet_chconst_MC13TeV_TTJets_pull_angle_OPT_nominal_ATLAS4//save.root", "UPDATE");
  //    ch -> LoadHistos("/afs/cern.ch/work/v/vveckaln/private/CMSSW_8_0_26_patch1/src/TopLJets2015/TopAnalysis/data/era2016/samples_cflip.json", THEORSYS);
  //  TH2 * hsignal = ch -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2();
  // TCanvas *c = new TCanvas;
  // hsignal -> Draw("COLZ");

  // ch -> PrintNonRecoCoeff();
  // //  getchar();
  // const unsigned char N = 6;
  // float iterv[N];
  // float band[N];
  // for (unsigned char iter = 1; iter <= N; iter ++)
  //   {
  //     ch -> unfoldBayesian(iter);
  //     ch -> GetLevel(OUT) -> SeparateSys();
  //     ch -> GetLevel(IN) -> SeparateSys();
  //     //  ch -> CreateMCTotal(OUT);
  //     if (ch -> GetLevel(IN) -> GetHU(TOTALMCUNC))
  // 	{
  // 	  delete ch -> GetLevel(IN) -> GetHU(TOTALMCUNC);
  // 	  ch -> GetLevel(IN) -> GetHURef(TOTALMCUNC) = nullptr;
  // 	}
  //     if (ch -> GetLevel(IN) -> GetHU(TOTALMCUNCSHAPE))
  // 	{
  // 	  delete ch -> GetLevel(IN) -> GetHU(TOTALMCUNCSHAPE);
  // 	  ch -> GetLevel(IN) -> GetHURef(TOTALMCUNCSHAPE) = nullptr;
  // 	}
  //     if (ch -> GetLevel(OUT) -> GetHU(TOTALMCUNC))
  // 	{
  // 	  delete ch -> GetLevel(OUT) -> GetHU(TOTALMCUNC);
  // 	  ch -> GetLevel(OUT) -> GetHURef(TOTALMCUNC) = nullptr;
  // 	}
  //     if (ch -> GetLevel(OUT) -> GetHU(TOTALMCUNCSHAPE))
  // 	{
  // 	  delete ch -> GetLevel(OUT) -> GetHU(TOTALMCUNCSHAPE);
  // 	  ch -> GetLevel(OUT) -> GetHURef(TOTALMCUNCSHAPE) = nullptr;
  // 	}


  //     // ch -> CreateTotalMCUnc(IN, RECO);
  //     // ch -> CreateTotalMCUnc(IN, RECO, true);
  //     // printf("probe 1 %p\n", ch -> GetLevel(IN) -> GetHU(TOTALMCUNC) -> GetTH1(RECO));

  //     // ch -> CreateTotalMCUnc(IN, GEN);
  //     // printf("probe 2 %p\n", ch -> GetLevel(IN) -> GetHU(TOTALMCUNC) -> GetTH1(RECO));
  //     // ch -> CreateTotalMCUnc(IN, GEN, true);
  //     ch -> CreateTotalMCUnc(OUT, GEN);
  //     ch -> CreateTotalMCUnc(OUT, GEN, true);
  //     iterv[iter - 1] = iter;
  //     band[iter - 1] = ch -> GetLevel(OUT) -> GetHU(TOTALMCUNCSHAPE) -> GetTH1(GEN) -> GetBinError(1);
      
  //   }
  // TGraph* gr = new TGraph(N, iterv, band);
  // gr -> GetXaxis() -> SetTitle("Number of iterations");
  // gr -> GetYaxis() -> SetTitle("width of uncertainty band");
  // gr -> GetYaxis() -> SetTitleOffset(1.3);
  // TCanvas *cgraph = new TCanvas;
  // gr -> Draw("AC*");
  // gr -> Print("all");
  // printf("left margin %f\n", cgraph -> GetLeftMargin());
  // getchar();
  // cgraph -> SaveAs("output/itergraph.png");
  for(map<TString, vector<HistoUnfolding *>>::iterator bit = ch -> GetLevel(IN) -> _msyshistos .begin(); bit != ch -> GetLevel(IN) -> _msyshistos . end(); bit ++)
    {
      const TString sample = bit -> first;
      for (vector<HistoUnfolding *>::iterator it = ch -> GetLevel(IN) -> GetV(SYSMO, sample.Data()) -> begin(); it != ch -> GetLevel(IN) -> GetV(SYSMO, sample.Data()) -> end(); it ++)
	{
	  (*it) -> SetSysType(EXPSYS);
	  printf("%u\n", (*it) -> GetTH2() -> GetNbinsX());
	}
    }  
  getchar();
  string bindir(binning_method);

  for (unsigned char sind = 0; sind < npsamples; sind ++)
    {
      TFile *f = TFile::Open(TString("$EOS/unfolding_") + 
			     privatesamples[sind] + "/" +
			     observable + "/" + 
			     bindir + "/" +
			     tag_jet_types_[jetind] + "_" + 
			     tag_charge_types_[chargeind] + 
			     "_MC13TeV_TTJets_" + privatesamples[sind] + "_" + 
			     observable + "_" + 
			     tag_opt[levelind] + "_" + 
			     privatesamples[sind] + bintag 
			     + "/save.root", "READ");
      printf("%s\n", f -> GetName());
      CompoundHistoUnfolding * chpr = nullptr;
      if (f)
      	{
      	  chpr = (CompoundHistoUnfolding * )((TKey * ) f -> GetListOfKeys() -> At(0)) -> ReadObj();
      	  f -> Close();
      	  printf("probe A %s\n", chpr -> GetSignalTag());
      	  HistoUnfolding * hupr = chpr -> GetLevel(IN) -> GetInputHU(SYSMO, TString("MC13TeV_TTJets_") + privatesamples[sind], TString(chpr -> GetSignalTag()) + "_" + privatesamples[sind]); 
	  hupr -> SetSysType(THEORSYSTEST);
	  printf("%u\n", hupr -> GetTH2() -> GetNbinsX());
	  hupr -> ls();
	  ch -> GetLevel(IN) -> GetV(SYSMO, "MC13TeV_TTJets") -> push_back(hupr);
	}
    }  
  getchar();
  ch -> unfoldBayesian(3);
  ch -> GetLevel(OUT) -> SeparateSys();
  ch -> GetLevel(IN) -> SeparateSys();
  //  ch -> CreateDataMinusBackground(OUT);
  //ch -> CreateMCTotal(OUT);
  // if (ch -> GetLevel(IN) -> GetHU(TOTALMCUNC))
  //   {
  //     delete ch -> GetLevel(IN) -> GetHU(TOTALMCUNC);
  //     ch -> GetLevel(IN) -> GetHURef(TOTALMCUNC) = nullptr;
  //   }
  // if (ch -> GetLevel(IN) -> GetHU(TOTALMCUNCSHAPE))
  //   {
  //     delete ch -> GetLevel(IN) -> GetHU(TOTALMCUNCSHAPE);
  //     ch -> GetLevel(IN) -> GetHURef(TOTALMCUNCSHAPE) = nullptr;
  //   }
   printf("matrix before \n");
   //ch -> GetLevel(OUT) -> cov -> Print();
  //getchar();
  //  ch -> createCov_new();
  ch -> CreateChiTable();
  //  ch -> CreateChiTableFull();
  if (ch -> GetLevel(OUT) -> GetHU(TOTALMCUNC))
    {
      delete ch -> GetLevel(OUT) -> GetHU(TOTALMCUNC);
      ch -> GetLevel(OUT) -> GetHURef(TOTALMCUNC) = nullptr;
    }
  if (ch -> GetLevel(OUT) -> GetHU(TOTALMCUNCSHAPE))
    {
      delete ch -> GetLevel(OUT) -> GetHU(TOTALMCUNCSHAPE);
      ch -> GetLevel(OUT) -> GetHURef(TOTALMCUNCSHAPE) = nullptr;
    }


  // ch -> CreateTotalMCUnc(IN, RECO);
  // ch -> CreateTotalMCUnc(IN, RECO, true);
  // printf("probe 1 %p\n", ch -> GetLevel(IN) -> GetHU(TOTALMCUNC) -> GetTH1(RECO));

  // ch -> CreateTotalMCUnc(IN, GEN);
  // printf("probe 2 %p\n", ch -> GetLevel(IN) -> GetHU(TOTALMCUNC) -> GetTH1(RECO));
  // ch -> CreateTotalMCUnc(IN, GEN, true);
  ch -> CreateTotalMCUnc(OUT, GEN);
  ch -> CreateTotalMCUnc(OUT, GEN, true);
      

  // ch -> CreateDataGraph(IN, RECO);
     ch -> CreateDataGraph(OUT, GEN);

  // TCanvas *crecoIN = ch -> CreateCombinedPlot(RECO, IN);
  // crecoIN -> SaveAs(TString("output/") + crecoIN -> GetName() +  ".png");     
  // TPad * pad1 = (TPad *) crecoIN -> GetListOfPrimitives() -> At(0);
	    
  // TCanvas *cgenIN = ch -> CreateCombinedPlot(GEN, IN);
  // cgenIN -> SaveAs(TString("output/") + cgenIN -> GetName() +  ".png");     
      
  TCanvas *cgenOUT = ch -> CreateCombinedPlot(GEN, OUT);
  cgenOUT -> SaveAs(TString("output/") + cgenOUT -> GetName() +  ".png");     
  //file -> cd();
  TFile * fbayesian = TFile::Open("output/save_bayesian2.root", "RECREATE");
  ch -> Write();
  fbayesian -> Close();
  // app.Run(kTRUE);
  // app.Terminate();

}
