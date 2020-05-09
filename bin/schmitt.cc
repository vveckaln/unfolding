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
int main()
{
  TApplication app("myapp", 0, 0);
  TFile * file = TFile::Open("root://eosuser.cern.ch//$EOS/unfolding_nominal/pull_angle/ATLAS4/leading_jet_chconst_MC13TeV_TTJets_pull_angle_OPT_nominal_ATLAS4//save.root", "READ");
  printf("file %p %s\n", file, file -> GetName()); 
  CompoundHistoUnfolding * ch = (CompoundHistoUnfolding * )((TKey * ) file -> GetListOfKeys() -> At(0)) -> ReadObj();
  //    ch -> LoadHistos("/afs/cern.ch/work/v/vveckaln/private/CMSSW_8_0_26_patch1/src/TopLJets2015/TopAnalysis/data/era2016/samples_cflip.json", THEORSYS);
  TH2 * hsignal = ch -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2();
  TCanvas *c = new TCanvas;
  hsignal -> Draw("COLZ");
  file -> Close();
  ch -> unfold(false);
  ch -> GetLevel(OUT) -> SeparateSys();
  ch -> GetLevel(IN) -> SeparateSys();
  //  ch -> CreateMCTotal(OUT);
  if (ch -> GetLevel(IN) -> GetHU(TOTALMCUNC))
    {
      delete ch -> GetLevel(IN) -> GetHU(TOTALMCUNC);
      ch -> GetLevel(IN) -> GetHURef(TOTALMCUNC) = nullptr;
    }
  if (ch -> GetLevel(IN) -> GetHU(TOTALMCUNCSHAPE))
    {
      delete ch -> GetLevel(IN) -> GetHU(TOTALMCUNCSHAPE);
      ch -> GetLevel(IN) -> GetHURef(TOTALMCUNCSHAPE) = nullptr;
    }
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
  //  ch -> CreateTotalMCUnc(IN, RECO, true);

  // ch -> CreateTotalMCUnc(IN, GEN);

  // ch -> CreateTotalMCUnc(IN, GEN, true);

  // printf("%p\n", ch -> GetLevel(IN) -> GetHU(TOTALMCUNCSHAPE));
  // getchar();

  ch -> CreateTotalMCUnc(OUT, GEN);
  ch -> CreateTotalMCUnc(OUT, GEN, true);

  // ch -> CreateTotalMCUnc(OUT, RECO);
  // ch -> CreateTotalMCUnc(OUT, RECO, true);
      

  ch -> CreateDataGraph(IN, RECO);
  ch -> CreateDataGraph(OUT, GEN);
  ch -> CreateDataGraph(OUT, RECO);

  // TCanvas *crecoIN = ch -> CreateCombinedPlot(RECO, IN);
  // crecoIN -> SaveAs(TString("output/") + crecoIN -> GetName() +  ".png");     
  // TPad * pad1 = (TPad *) crecoIN -> GetListOfPrimitives() -> At(0);
	    
  // TCanvas *cgenIN = ch -> CreateCombinedPlot(GEN, IN);
  // cgenIN -> SaveAs(TString("output/") + cgenIN -> GetName() +  ".png");     
      
  TCanvas *cgenOUT = ch -> CreateCombinedPlot(GEN, OUT);
  cgenOUT -> SaveAs(TString("output/") + cgenOUT -> GetName() +  ".png");     

  // TCanvas *crecoOUT = ch -> CreateCombinedPlot(RECO, OUT);
  // crecoOUT -> SaveAs(TString("output/") + crecoOUT -> GetName() +  ".png");     


  app.Run(kTRUE);
  app.Terminate();

}
