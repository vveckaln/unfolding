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
  TFile * file = TFile::Open("/eos/user/v/vveckaln/unfolding_nominal/pvmag/NEWOPT/leading_jet_allconst_MC13TeV_TTJets_herwig_pvmag_OPT_nominal_NEWOPT/save.root", "READ");
  CompoundHistoUnfolding * chisto = (CompoundHistoUnfolding * )((TKey * ) file -> GetListOfKeys() -> At(0)) -> ReadObj();

  chisto -> Process(false);
  file -> cd();
  chisto -> SetCHUnominal(nullptr);
  // chisto -> Write();
  // file -> Close();
  delete chisto;
}
