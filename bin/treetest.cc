#include "TChain.h"
int main()
{
  TChain chain("migration");
  chain.Add("$EOS/analysis/migration_MC13TeV_TTJets.root/E_migration");
  chain.Add("$EOS/analysis/migration_MC13TeV_TTJets.root/M_migration");
  Float_t pull_angle_reco;
  Float_t weight;
  Float_t pull_angle_gen;
  chain.SetBranchAddress("pull_angle_reco", &pull_angle_reco);
  chain.SetBranchAddress("weight",          &weight);
  chain.SetBranchAddress("pull_angle_gen",  &pull_angle_gen);

  printf("entries %lu\n", chain.GetEntries());
}
