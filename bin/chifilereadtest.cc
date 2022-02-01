#include <stdio.h>
#include "TString.h"
#include "TFile.h"
#include "TKey.h"
#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "CompoundHistoUnfolding.hh"
int main()
{
  //  const TString filename = "root://eosuser.cern.ch//eos/user/v/vveckaln/unfolding_nominal/pull_angle/ATLAS4/leading_b_chconst_MC13TeV_TTJets_pull_angle_OPT_nominal_ATLAS4/save_test.root";
  //    const TString filename = "root://eosuser.cern.ch//eos/user/v/vveckaln/unfolding_nominal/pull_angle/ATLAS4/leading_b_chconst_MC13TeV_TTJets_pull_angle_OPT_nominal_ATLAS4/save.root";
      const TString filename = "/eos/user/v/vveckaln/unfolding_nominal/pull_angle/ATLAS4/leading_b_allconst_MC13TeV_TTJets_pull_angle_OPT_nominal_ATLAS4/save.root";
  //  const TString filename = "root://eosuser.cern.ch//eos/user/v/vveckaln/save.root";
  //const TString filename = "save.root"; //on lxplus
  TFile * file = TFile::Open(filename.Data(), "UPDATE");
  CompoundHistoUnfolding * ch = (CompoundHistoUnfolding * )((TKey*) file -> GetListOfKeys() -> At(0)) -> ReadObj(); 
  printf("signaltag %s\n", ch -> signaltag);
  ch -> SetSignalTag(TString(ch -> signaltag).ReplaceAll("_add", "").Data());
  ch -> Write(0, TObject::kOverwrite);
  file -> ls();
  file -> Close();
}
