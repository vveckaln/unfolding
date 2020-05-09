#include "TChain.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include <vector>
#include "TCanvas.h"
#include "TApplication.h"
#include "TUnfoldDensity.h"
using namespace std;
TString input_folder = "$EOS//analysis_MC13TeV_TTJets/HADDmigration/";
const unsigned char ndataruns(6);
TString data_run[ndataruns] = {"B", "C", "D", "E", "F", "G"};//
//{"Hv2", "Hv3"};
TString leptons[2] = {"Electron", "Muon"};
TString observable("pull_angle");
const double xsec(832.0);
const double luminosity(35874.8);
const unsigned char nbins(6);
int main()
{
  TApplication app("myapp", 0, 0);
  TChain mcchain("mc");
  //mcchain.Add(input_folder + "migration_MC13TeV_TTJets.root/E_allconst_leading_jet_migration");
    mcchain.Add(input_folder + "migration_MC13TeV_TTJets.root/M_allconst_leading_jet_migration");
  Float_t reco;
  Float_t gen;
  vector<double> * weights = nullptr;
  mcchain.SetBranchAddress(TString(observable) + "_reco", & reco);
  mcchain.SetBranchAddress(TString(observable) + "_gen",  & gen);
  mcchain.SetBranchAddress("weight",          & weights);
  const unsigned int dbcut = 1;
  //      h -> FillFromTree(, tag_jet_types_[jetcode], tag_charge_types_[chargecode], observable);                                                                                                      
  TH2F * th2mc = new TH2F("mc", "mc", nbins, 0.0, 1.0, nbins, 0.0, 1.0);
  for (unsigned long event_ind = 0; event_ind < mcchain.GetEntries()/dbcut; event_ind ++)
    {
      if (event_ind % 10000 == 0)
	printf("%u\r", event_ind);
      mcchain.GetEntry(event_ind);
      //      if (string(observable).compare("pull_angle") == 0)
	th2mc -> Fill(reco == -10 ? reco : TMath::Abs(reco)/TMath::Pi(), gen == -10.0 ? gen : TMath::Abs(gen)/TMath::Pi(), (*weights)[0]);
    }
  th2mc -> Scale(xsec * luminosity);
  TH2F * th2data = new TH2F("data", "data", nbins, 0.0, 1.0, nbins, 0.0, 1.0);
  for (unsigned char lepton_ind = 0; lepton_ind < 2; lepton_ind ++)
    {
      for (unsigned char data_run_ind = 0; data_run_ind < ndataruns; data_run_ind ++)
	{
	  TChain datachain("data");
	  //datachain.Add(input_folder + "migration_Data13TeV_Single" + leptons[lepton_ind] + "_2016" + data_run[data_run_ind] + ".root/E_allconst_leading_jet_migration");
	  	  datachain.Add(input_folder + "migration_Data13TeV_Single" + leptons[lepton_ind] + "_2016" + data_run[data_run_ind] + ".root/M_allconst_leading_jet_migration");
	  //	  TH2F * th2mcsub = new TH2F("mc", "mc", 5, 0.0, 1.0, 0.0, 1.0);
	  Float_t reco;
	  Float_t gen;
	  vector<double> * weights = nullptr;
	  datachain.SetBranchAddress(TString(observable) + "_reco", & reco);
	  datachain.SetBranchAddress(TString(observable) + "_gen",  & gen);
	  datachain.SetBranchAddress("weight",          & weights);
	  for (unsigned long event_ind = 0; event_ind < datachain.GetEntries()/dbcut; event_ind ++)
	    {
	      if (event_ind % 10000 == 0)
		printf("%u\r", event_ind);
	      datachain.GetEntry(event_ind);
	      //      if (string(observable).compare("pull_angle") == 0)
	      th2data -> Fill(reco == -10 ? reco : TMath::Abs(reco)/TMath::Pi(), gen == -10.0 ? gen : TMath::Abs(gen)/TMath::Pi(), (*weights)[0]);
	    }

	}

    }
  TH1D * mcreco = th2mc -> ProjectionX();
  printf("listing mcreco\n");
  mcreco -> Print("all");
  
  TH1D * datareco = th2data -> ProjectionX();
  printf("listing datareco\n");
  datareco -> Print("all");
  datareco -> Divide(mcreco);
  printf("listing ratios\n");
  datareco -> Print("all");
  
  th2mc -> RebinY(2);
  TUnfoldDensity unfold(th2mc, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeBinWidthAndUser);
  unfold.SetInput(th2data -> ProjectionX());
  unfold.DoUnfold(0.0);
  TCanvas *c = new TCanvas;
  TH1 * out = unfold . GetOutput("output");
  // for (unsigned char bind = 1; bind <= out -> GetNbinsX(); bind ++);
  out -> Draw("HIST");
  out -> SetMinimum(0.0);
  TCanvas * c2 = new TCanvas();
  th2mc -> Draw("COLZ");
  app.Run();
  app.Terminate(kTRUE);

}
