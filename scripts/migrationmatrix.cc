//To run: 
//   root -l -q 'scripts/migrationmatrix.cc("nominal", "pull_angle", "ATLAS3")'
// https://root-forum.cern.ch/t/start-root-with-a-root-file-and-script/28635/3
R__ADD_INCLUDE_PATH($COMPOUNDHISTOLIB/interface)
#include "CompoundHistoUnfolding.hh"
R__ADD_LIBRARY_PATH($COMPOUNDHISTOLIB/lib) // if needed
R__LOAD_LIBRARY(libCompoundHisto.so)
TString periods[2] = {"BCDEF", "GH"};
const char * leptons[2] = {"E", "M"};
void processth2(TH2 * th2);
int migrationmatrix()
{
  system("mkdir -p $UNFOLDINGLIB/migrationmatrices");
  const unsigned char n = 3;
  Int_t colours[n] = {kGreen -10, kGreen -6, kGreen + 1};
  gStyle -> SetPalette(n, colours);
  gStyle -> SetPaintTextFormat("2.2f");
  gStyle -> SetOptStat(0);
  const char *binfilename = "binfiles/binsATLAS4_pull_angle.txt";
  FILE * binfile = fopen(binfilename, "r");
  unsigned long size;
  fscanf(binfile, "%lu\n", &size);
  float * ybins = new float[size];
  int r = 0;
  unsigned char ind = 0;
  while (r >= 0)
    {
      float store;
      r = fscanf(binfile, "%f\n", &store);
      printf("%f\n", store);
      if (r >= 0)
	{
	  ybins[ind] = store;
	}
      ind ++;
    }
  fclose(binfile);
  for (unsigned char bind = 0; bind < size; bind ++)
    {
      printf("bind %u %f\n", bind , ybins[bind]);
    }
  TH2F * mtotal = new TH2F(TString("mtotal"), TString("mtotal; reco; gen"), size - 1, ybins, size - 1, ybins);
  for (unsigned char leptind = 0; leptind < 2; leptind ++)
    {
      TH2F * mltotal = new TH2F(TString("mtotal_") + leptons[leptind], TString("mtotal_" ) + leptons[leptind] + "; reco; gen", size - 1, ybins, size - 1, ybins);

      map<TString, TH2 *> th2map;
      th2map[periods[0]] = new TH2F(TString("m") + "_" + leptons[leptind] + "_" + periods[0], TString("m") + "_" + leptons[leptind] + "_" + periods[0] + "; reco; gen", size - 1, ybins, size - 1, ybins);
      th2map[periods[1]] = new TH2F(TString("m") + "_" + leptons[leptind] + "_" + periods[1], TString("m") + "_" + leptons[leptind] + "_" + periods[1] + "; reco; gen", size - 1, ybins, size - 1, ybins);
      

      TChain chain("migration");
      //      chain.Add(TString("$EOS/analysis_MC13TeV_TTJets/migration/MC13TeV_TTJets/migration_MC13TeV_TTJets_0.root/") + leptons[leptind] + "_chconst_leading_jet_migration");
            chain.Add(TString("$EOS/analysis_MC13TeV_TTJets/HADDmigration/migration_MC13TeV_TTJets.root/") + leptons[leptind] + "_chconst_leading_jet_migration");
      Float_t reco;
      Float_t gen;
      TString * period = nullptr;
      using namespace std;
      vector<double> * weights = nullptr;
      chain.SetBranchAddress("pull_angle_reco", & reco);
      chain.SetBranchAddress("pull_angle_gen",  & gen);
      chain.SetBranchAddress("period",  & period);
      chain.SetBranchAddress("weight",          & weights);
      const unsigned char div(1);
      for (unsigned long event_ind = 0; event_ind < chain.GetEntries()/div; event_ind ++)
	{
	  if (event_ind % 10000 == 0)
	    printf("%lu\r", event_ind);
	  chain.GetEntry(event_ind);
	  //	  printf("%lu %f %f %.9f [%s]\n", event_ind, reco, gen, (*weights)[0], period -> Data());
	  // getchar();
	  th2map[*period] -> Fill(reco == -10.0 ? reco : TMath::Abs(reco)/TMath::Pi(), gen == -10.0 ? gen : TMath::Abs(gen)/TMath::Pi(), (*weights)[0]);
	  mltotal         -> Fill(reco == -10.0 ? reco : TMath::Abs(reco)/TMath::Pi(), gen == -10.0 ? gen : TMath::Abs(gen)/TMath::Pi(), (*weights)[0]);
	  mtotal          -> Fill(reco == -10.0 ? reco : TMath::Abs(reco)/TMath::Pi(), gen == -10.0 ? gen : TMath::Abs(gen)/TMath::Pi(), (*weights)[0]);
	}
      for (unsigned char pind = 0; pind < 2; pind ++)
	{
	  TH2 * _th2 = th2map[periods[pind]];
	  processth2(_th2);
	}
      processth2(mltotal);
    }
  processth2(mtotal);
  return 0;
}

void processth2(TH2 * _th2)
{
  for (unsigned char bindy = 1; bindy <= _th2 -> GetNbinsY(); bindy ++)
    {
      const double integral = _th2 -> Integral(1, _th2 -> GetNbinsX(), bindy, bindy);
      for (unsigned char bindx = 1; bindx <= _th2 -> GetNbinsX(); bindx ++)
	{
	  _th2 -> SetBinContent(bindx, bindy, 100 * _th2 -> GetBinContent(bindx, bindy)/integral);
	}
	
    }
  TCanvas * c = new TCanvas;
  c -> SetRightMargin(0.1);
  _th2 -> Draw("COLZTEXT");
  _th2 -> SetMarkerSize(3.0);
  c -> Update();
  c -> Modified();
  c -> SaveAs(TString("migrationmatrices/") + _th2 -> GetName() + ".png");
}
