//To run: 
//   root -l -q 'scripts/mainplot.cc("nominal", "pull_angle", "ATLAS3")'
// https://root-forum.cern.ch/t/start-root-with-a-root-file-and-script/28635/3
R__ADD_INCLUDE_PATH($COMPOUNDHISTOLIB/interface)
//#include "/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/interface/CompoundHistoUnfolding.hh"
#include "CompoundHistoUnfolding.hh"
R__ADD_LIBRARY_PATH($COMPOUNDHISTOLIB/lib) // if needed
R__LOAD_LIBRARY(libCompoundHisto.so)
int readdire()
{
  TFile * f = TFile::Open("/eos/user/v/vveckaln/unfolding_dire2002/pull_angle/optfixed//leading_jet_allconst_MC13TeV_TTJets_dire2002_pull_angle_OPT_dire2002_optfixed/save.root");
  CompoundHistoUnfolding * chups = (CompoundHistoUnfolding * )((TKey * ) f -> GetListOfKeys() -> At(0)) -> ReadObj();
  f -> Close();
  chups -> GetLevel(IN) -> lsInputHU(SYSMO);
  HistoUnfolding * hu = chups -> GetLevel(IN) -> GetInputHU(SYSMO, "MC13TeV_TTJets_dire2002");
  TH2 * h = hu -> GetTH2();
  
  TCanvas * c = new TCanvas();
  TH1D * hpr = h -> ProjectionY();
  
  hpr -> Print("all");
  hpr -> Draw("HIST");
  return 0;
}
