R__ADD_INCLUDE_PATH($COMPOUNDHISTOLIB/interface)
R__ADD_INCLUDE_PATH($ROOUNFOLD/src)
#include "CompoundHistoUnfolding.hh"
R__ADD_LIBRARY_PATH($ROOUNFOLD) // if needed
R__LOAD_LIBRARY(libRooUnfold.so)
R__ADD_LIBRARY_PATH($COMPOUNDHISTOLIB/lib) // if needed
R__LOAD_LIBRARY(libCompoundHisto.so)

int checkpvmag()
{
  gStyle -> SetOptTitle(kFALSE);
  gStyle -> SetOptStat(kFALSE);
    const TString file_name = "$EOS/unfolding_nominal/pvmag/optfixed/leading_jet_chconst_MC13TeV_TTJets_pvmag_OPT_nominal_optfixed/save.root";
  //  const TString file_name = "$EOS/unfolding_nominal/pull_angle/optfixed/leading_jet_chconst_MC13TeV_TTJets_pull_angle_OPT_nominal_optfixed/save.root";
  TFile * file = TFile::Open(file_name, "READ");
  CompoundHistoUnfolding * ch = (CompoundHistoUnfolding * )((TKey * ) file -> GetListOfKeys() -> At(0)) -> ReadObj();
  file -> Close();
  TCanvas * c = ch -> stabpur();
  c -> SaveAs(TString(c -> GetName()) + ".png");
  ch -> GetLevel(OUT) -> GetProjectionDeco(GEN) -> _ratioframe = nullptr;
  ch -> CreateRatioGraph(GEN, OUT);
  ch -> GetLevel(OUT) -> GetProjectionDeco(GEN) -> _ratiogr -> Print("all");
  TCanvas *canvas = ch -> CreateCombinedPlot(GEN, OUT);
  canvas -> SaveAs(TString(canvas -> GetName()) + ".png");
  return 0;
}
