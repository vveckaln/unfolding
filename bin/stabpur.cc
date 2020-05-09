#include "CompoundHistoUnfolding.hh"
#include "TStyle.h"
#include "TFile.h"
#include "TMath.h"
#include "TUnfoldDensity.h"
#include "TKey.h"

int main()
{
  gStyle -> SetOptTitle(kFALSE);
  gStyle -> SetOptStat(kFALSE);
  const TString file_name = "$EOS/unfolding_nominal/pull_angle/ATLAS4/leading_jet_chconst_MC13TeV_TTJets_pull_angle_OPT_nominal_ATLAS4/save.root";
  TFile * file = TFile::Open(file_name, "READ");
  CompoundHistoUnfolding * ch = (CompoundHistoUnfolding * )((TKey * ) file -> GetListOfKeys() -> At(0)) -> ReadObj();
  TH2 * hsignal = (TH2 * ) ch -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone("unfolding_signal");
  //  hsignal -> RebinX(2);
  printf("x %u y %u\n", hsignal -> GetNbinsY(), hsignal -> GetNbinsX());
  unsigned char k = (unsigned char) hsignal -> GetNbinsX() /hsignal -> GetNbinsY();
  TH1 * hpurity = hsignal -> ProjectionY();
  hpurity -> SetDirectory(nullptr);
  hpurity -> Reset("ICE");
  TH1 * hstability = hsignal -> ProjectionY();
  hstability -> SetDirectory(nullptr);
  hstability -> Reset("ICE");
  for (unsigned char bindy = 1; bindy < hsignal -> GetNbinsY() + 1; bindy ++)
    {
      float diag = 0.0;
      for (unsigned char bindx = k * (bindy - 1) + 1; bindx < k * bindy + 1; bindx ++)
	{
	  diag += hsignal -> GetBinContent(bindx, bindy);
	}
      
      printf("%f %f %f\n", 
	     diag, 
	     hsignal -> Integral(k * (bindy - 1) + 1, k * bindy, 1, hsignal -> GetNbinsY()), 
	     hsignal -> Integral(1, hsignal -> GetNbinsX(), bindy, bindy));
      hpurity -> SetBinContent(bindy, diag/hsignal -> Integral(k * (bindy - 1) + 1, k * bindy, 1, hsignal -> GetNbinsY()));
      hstability -> SetBinContent(bindy, diag/hsignal -> Integral(1, hsignal -> GetNbinsX(), bindy, bindy));
    }
  hpurity -> Print("all");
  hstability -> Print("all");
  file -> Close();
  return 0;
}
