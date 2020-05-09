#include "JsonParser.hh"
#include "CompoundHistoUnfolding.hh"
#include "Utilities.hh"
#include "TFile.h"
#include <stdio.h>
#include "TApplication.h"
#include "TCanvas.h"
#include "TColor.h"
#include "TH1F.h"
#include "TMath.h"
#include "TH2F.h"
#include "TChain.h"
// READ READ READ READ READ READ READ READ
int main()
{
  //TApplication app("myapp", 0, 0);
    TFile *f = TFile::Open("$EOS/analysis_MC13TeV_TTJets/HADDChunks/MC13TeV_TTJets.root");
  //  TFile *f = TFile::Open("$EOS/analysis_MC13TeV_TTJets/Chunks/MC13TeV_TTJets/MC13TeV_TTJets_0.root");
  TH1F * h = (TH1F*) f -> Get("L_pull_angle_allconst_reco_leading_jet_scnd_leading_jet_DeltaRTotal");
  h -> Scale(35874.8*832);
  printf("%f\n", h -> Integral());
  exit(0);
  TChain chain("migration");
  // chain.Add("$EOS/analysis_MC13TeV_TTJets/migration/MC13TeV_TTJets/migration_MC13TeV_TTJets_0.root/E_allconst_leading_jet_migration");
  // chain.Add("$EOS/analysis_MC13TeV_TTJets/migration/MC13TeV_TTJets/migration_MC13TeV_TTJets_0.root/M_allconst_leading_jet_migration");

  chain.Add("$EOS/analysis_MC13TeV_TTJets/HADDmigration/migration_MC13TeV_TTJets.root/E_allconst_leading_jet_migration");
  chain.Add("$EOS/analysis_MC13TeV_TTJets/HADDmigration/migration_MC13TeV_TTJets.root/M_allconst_leading_jet_migration");
  Float_t reco;
  Float_t gen;
  vector<double> * weights = nullptr;
  chain.SetBranchAddress("weight",          & weights);

  const char * observable = "pull_angle";
  chain.SetBranchAddress(TString(observable) + "_reco", & reco);
  chain.SetBranchAddress(TString(observable) + "_gen",  & gen);
  TH2F migration("m", "m", 1, 0, TMath::Pi(), 1, 0, TMath::Pi()); 
  printf("nentries %lu\n", chain.GetEntries());
  for (unsigned long event_ind = 0; event_ind < chain.GetEntries(); event_ind ++)
    {
      chain.GetEntry(event_ind);
      migration .Fill(reco == -10 ? reco : TMath::Abs(reco), gen == -10.0 ? gen : TMath::Abs(gen), (*weights)[0]*35874.8*832);

    }
  // exit(0);
  // TFile * f = TFile::Open("$EOS/");
  // f -> ls ();
  // // exit(0);
  // CompoundHistoUnfolding  * chisto = (CompoundHistoUnfolding *) f -> Get("OPT_MC13TeV_TTJets_nominal");
  //  TH2F * migration = chisto -> GetLevel(IN) -> GetHU(SIGNALPROXY, IN) -> GetTH2(); 
  const unsigned char nx = migration . GetNbinsX();
  const unsigned char ny = migration . GetNbinsY();
  printf("%f\n %f\n %f\n %f\n %f\n %f\n %f\n %f\n", 
	 h -> Integral(),
	 migration . GetBinContent(0, 0), 
	 migration . Integral(1, nx, 0, 0), 
	 migration . Integral(0, 0, 1, ny), 
	 migration . GetBinContent(nx + 1, ny + 1),
	 migration . Integral(1, nx, 1, ny),
	 migration . Integral(),
	 migration . Integral() + migration . Integral(1, nx, 0, 0) );
  
  //chisto -> Do();
    /*chisto -> CreateCombinedPlot();
  app.Run(kTRUE);
  
  app.Terminate();*/
  printf("probe A\n");
  f -> Close();
}
