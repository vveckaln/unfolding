R__ADD_INCLUDE_PATH($COMPOUNDHISTOLIB/interface)
//#include "/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/interface/CompoundHistoUnfolding.hh"
#include "CompoundHistoUnfolding.hh"
R__ADD_LIBRARY_PATH($COMPOUNDHISTOLIB/lib) // if needed
R__LOAD_LIBRARY(libCompoundHisto.so)
int fileread()
{
  TFile * f = TFile::Open("/eos/user/v/vveckaln/unfolding_nominal/pull_angle/optfixed/leading_jet_allconst_MC13TeV_TTJets_pull_angle_OPT_nominal_optfixed/save.root");
  CompoundHistoUnfolding * chups = (CompoundHistoUnfolding * )((TKey * ) f -> GetListOfKeys() -> At(0)) -> ReadObj();
  printf("chups %p\n", chups);
  f -> Close();

  return 0;
}
