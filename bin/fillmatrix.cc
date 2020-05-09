#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TTree.h"
#include "TChain.h"
#include "TApplication.h"
#include "TMath.h"
#include <vector>
#include "TFitResult.h"
#include "THStack.h"
#include <assert.h>
#include <string>
#include <stdio.h>
using namespace std;
int main(int argc, char * argv[])
{
  const string observable("pull_angle");
  TChain chain("migration");
  chain.Add("$EOS/analysis_MC13TeV_TTJets/HADDmigration/migration_MC13TeV_TTJets.root/E_allconst_leading_jet_migration");
  chain.Add("$EOS/analysis_MC13TeV_TTJets/HADDmigration/migration_MC13TeV_TTJets.root/M_allconst_leading_jet_migration");
  Float_t reco;
  Float_t gen;
  using namespace std;
  vector<double> * weights = nullptr;
  if (observable.compare("pull_angle") == 0)
    {
      chain.SetBranchAddress("pull_angle_reco", & reco);
      chain.SetBranchAddress("pull_angle_gen",  & gen);
    }
  if (observable.compare("pvmag") == 0)
    {
      chain.SetBranchAddress("pvmag_reco", & reco);
      chain.SetBranchAddress("pvmag_gen",  & gen);
    }
  chain.SetBranchAddress("weight",          & weights);
  unsigned long coinc = 0;
  //  printf("GetTag() %s %s %s\n", GetTag(), string(GetTag()).c_str(), string(GetTag()).compare("MC13TeV_TTJets") == 0? "True" : " False");
  const unsigned char nbins(20);
  float max;
  if (observable.compare("pull_angle") == 0 )
    max = 1.0;//TMath::Pi();
  if (observable.compare("pvmag") == 0 )
    max = 0.015;
  TFile *froot = new TFile("migrationfile.root", "recreate");
  TH2F * _th2 = new TH2F("m", "m", nbins, 0.0, max, nbins, 0.0, max);
  const unsigned char div(1);
  for (unsigned long event_ind = 0; event_ind < chain.GetEntries()/div; event_ind ++)
    {
      if (event_ind % 10000 == 0)
        printf("%u\r", event_ind);
      chain.GetEntry(event_ind);
      _th2 -> Fill(reco == -10.0 ? reco : TMath::Abs(reco)/TMath::Pi(), gen == -10.0 ? gen : TMath::Abs(gen)/TMath::Pi(), (*weights)[0]);
    }
  _th2 -> Write();
  froot -> Close();
}
