int checkherwig()
{
  TChain chainherwig("herwig");
  chainherwig.Add("$EOS/analysis_MC13TeV_TTJets/HADDmigration/migration_MC13TeV_TTJets_herwig.root/E_allconst_leading_jet_migration");
  chainherwig.Add("$EOS/analysis_MC13TeV_TTJets/HADDmigration/migration_MC13TeV_TTJets_herwig.root/M_allconst_leading_jet_migration");
  TH2F * hherwig = new TH2F("herwig", "herwig", 5, 0.0, 1.0, 5, 0.0, 1.0);
  Float_t reco;
  Float_t gen;
  vector<double> * weights = nullptr;
  const char * observable = "pull_angle";
  chainherwig.SetBranchAddress(TString(observable) + "_reco", & reco);
  chainherwig.SetBranchAddress(TString(observable) + "_gen",  & gen);
  chainherwig.SetBranchAddress("weight",          & weights);
  const unsigned int dbcut = 1;
  for (unsigned long event_ind = 0; event_ind < chainherwig.GetEntries()/dbcut; event_ind ++)
    {
      if (event_ind % 10000 == 0)
	printf("%u\r", event_ind);
      chainherwig.GetEntry(event_ind);
      if (string(observable).compare("pull_angle") == 0)
	hherwig -> Fill(reco == -10 ? reco : TMath::Abs(reco)/TMath::Pi(), gen == -10.0 ? gen : TMath::Abs(gen)/TMath::Pi(), (*weights)[0]);
    }

  TChain chainpythia("herwig");
  chainpythia.Add("$EOS/analysis_MC13TeV_TTJets/HADDmigration/migration_MC13TeV_TTJets.root/E_allconst_leading_jet_migration");
  chainpythia.Add("$EOS/analysis_MC13TeV_TTJets/HADDmigration/migration_MC13TeV_TTJets.root/M_allconst_leading_jet_migration");
  TH2F * hpythia = new TH2F("pythia", "pythia", 5, 0.0, 1.0, 5, 0.0, 1.0);
  chainpythia.SetBranchAddress(TString(observable) + "_reco", & reco);
  chainpythia.SetBranchAddress(TString(observable) + "_gen",  & gen);
  chainpythia.SetBranchAddress("weight",          & weights);
  for (unsigned long event_ind = 0; event_ind < chainpythia.GetEntries()/dbcut; event_ind ++)
    {
      if (event_ind % 10000 == 0)
	printf("%u\r", event_ind);
      chainpythia.GetEntry(event_ind);
      if (string(observable).compare("pull_angle") == 0)
	hpythia -> Fill(reco == -10 ? reco : TMath::Abs(reco)/TMath::Pi(), gen == -10.0 ? gen : TMath::Abs(gen)/TMath::Pi(), (*weights)[0]);
    }
  TCanvas * c = new TCanvas;
  THStack * stack = new THStack;
  TH1D * hherwiggen = hherwig -> ProjectionY();
  hherwiggen -> Scale(1.0/hherwiggen -> Integral());
  TH1D * hpythiagen = hpythia -> ProjectionY();
  hpythiagen -> Scale(1.0/hpythiagen -> Integral());
  hpythiagen -> SetLineColor(kGreen);
  stack -> Add(hherwiggen);
  stack -> Add(hpythiagen);
  stack -> Draw("nostack");
  return 0;
}
