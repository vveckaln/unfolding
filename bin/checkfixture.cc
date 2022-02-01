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
  const TString filename = "root://eosuser.cern.ch//eos/user/v/vveckaln/unfolding_nominal/pull_angle/NEWOPT/leading_jet_chconst_MC13TeV_TTJets_pull_angle_OPT_nominal_NEWOPT/save_fix.root";
  TFile * file = TFile::Open(filename, "UPDATE");
  CompoundHistoUnfolding * chisto = (CompoundHistoUnfolding * )((TKey*) file -> GetListOfKeys() -> At(0)) -> ReadObj(); 
  printf("chisto %p\n", chisto);
  chisto -> Check();
  //  chisto -> Write();
  file -> Close();
}
