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
  //  TApplication app("myapp", 0, 0);
  TFile * file = TFile::Open("root://eosuser.cern.ch//$EOS/unfolding_nominal/pull_angle/ATLAS4/leading_jet_chconst_MC13TeV_TTJets_pull_angle_OPT_nominal_ATLAS4//save.root", "READ");
  printf("file %p %s\n", file, file -> GetName()); 
  CompoundHistoUnfolding * ch = (CompoundHistoUnfolding * )((TKey * ) file -> GetListOfKeys() -> At(0)) -> ReadObj();
  //    ch -> LoadHistos("/afs/cern.ch/work/v/vveckaln/private/CMSSW_8_0_26_patch1/src/TopLJets2015/TopAnalysis/data/era2016/samples_cflip.json", THEORSYS);
  //TH2 * hsignal = ch -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2();
  TCanvas *c = new TCanvas;
  //hsignal -> Draw("COLZ");
  file -> Close();
  ch -> CreateTotalMCUnc(OUT, GEN, true);


}
