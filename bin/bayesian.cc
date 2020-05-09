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
int main()
{
  gStyle -> SetOptStat(0);
  //  TApplication app("myapp", 0, 0);
  gROOT -> SetBatch(kTRUE);
  TFile * file = TFile::Open("root://eosuser.cern.ch//$EOS/unfolding_nominal/pull_angle/ATLAS4/leading_jet_chconst_MC13TeV_TTJets_pull_angle_OPT_nominal_ATLAS4//save.root", "UPDATE");
  printf("file %p %s\n", file, file -> GetName()); 
  CompoundHistoUnfolding * ch = (CompoundHistoUnfolding * )((TKey * ) file -> GetListOfKeys() -> At(0)) -> ReadObj();
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
  file -> cd();
  ch -> Write();
  file -> Close();
  // app.Run(kTRUE);
  // app.Terminate();

}
