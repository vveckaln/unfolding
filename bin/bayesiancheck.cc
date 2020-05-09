#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "CompoundHistoUnfolding.hh"
#include "TChain.h"
#include <stdio.h>
#include "TChain.h"
#include "TFile.h"
#include "TKey.h"
#include "TApplication.h"
#include "THStack.h";
int main()
{
  //  TApplication app("myapp", 0, 0);
  TFile * file = TFile::Open("root://eosuser.cern.ch//$EOS/unfolding_nominal/pull_angle/ATLAS4/leading_jet_chconst_MC13TeV_TTJets_pull_angle_OPT_nominal_ATLAS4//save.root", "READ");
  printf("file %p %s\n", file, file -> GetName()); 
  CompoundHistoUnfolding * ch = (CompoundHistoUnfolding * )((TKey * ) file -> GetListOfKeys() -> At(0)) -> ReadObj();
  file -> Close();
  TH2 * hsignal = (TH2 *) ch -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2();
  //  hsignal -> RebinX(2);
  TH2 * hsignalgen = (TH2*) hsignal -> Clone("hsignalgen");
  TH2 * datambackground = (TH2*) ch -> GetLevel(IN) -> GetHU(DATAMO) -> GetTH2() -> Clone("datambackground");
  
  datambackground -> Add(ch -> GetTotalBackground() -> GetTH2(), -1);
  //datambackground -> RebinX(2);
  TH1 * hmeas = (TH1*) datambackground -> ProjectionX("meas");
  for (unsigned char bind = 0; bind <= hsignal -> GetNbinsX() + 1; bind ++)
    {
      float f (0.0);
      float bef (0.0);
      if (hsignal -> Integral(bind, bind, 0, hsignal -> GetNbinsY() + 1) > 0.0)
	{
	  f = hsignal -> GetBinContent(bind, 0)/hsignal -> Integral(bind, bind, 0, hsignal -> GetNbinsY() + 1);
	  bef = hmeas -> GetBinContent(bind);
	  hmeas -> SetBinContent(bind, hmeas -> GetBinContent(bind) * (1.0 - f));
	  hmeas -> SetBinError(bind, hmeas -> GetBinError(bind) * (1.0 - f));

	}
     
      //      hsignal -> SetBinContent(bind, 0, 0.0);
      //hsignalgen -> SetBinContent(bind, 0, 0.0);
    }
  for (unsigned char ind = 0; ind < hsignal -> GetNbinsY() + 1; ind ++)
    {
      hsignal -> SetBinContent(0, ind, 0.0);
    }

  TH1 * hgen = hsignal -> ProjectionY("gen");
  TH1 * hgengen = hsignalgen -> ProjectionY("gengen");
  TH1 * hreco = hsignal -> ProjectionX();
  printf("hmeas\n");
  hmeas -> Print("all");
  printf("hreco\n");
  hreco -> Print("all");
  //getchar();
  RooUnfoldResponse response(hmeas, hgen, hsignal);
  RooUnfoldBayes   unfold (&response, hmeas, 3);
  TH1* output = (TH1D*) unfold.Hreco();
  output -> SetName("outputnogen");
  RooUnfoldResponse responsegen(hmeas, hgengen, hsignalgen);
  RooUnfoldBayes   unfoldgen (&responsegen, hmeas, 3);
  TH1* outputgen = (TH1D*) unfoldgen.Hreco();
  outputgen -> SetName("outputwgen");
// for (unsigned char bind = 1; bind <= output -> GetNbinsX(); bind ++)
//     {
//       const float coeff = hsignalgen -> Integral(1, hsignal -> GetNbinsX(), bind, bind) / hsignalgen -> Integral(0, hsignal -> GetNbinsX() + 1, bind, bind);
//       if (hsignal -> Integral(0, hsignal -> GetNbinsX() + 1, bind, bind) > 0.0)
// 	{
// 	  printf("%u %f %f %f\n", 
// 		 bind, 
// 		 TMath::Abs(output -> GetBinContent(bind) - hsignal -> Integral(0, hsignal -> GetNbinsX() + 1, bind, bind))/hsignal -> Integral(0, hsignal -> GetNbinsX() + 1, bind, bind), 
// 		 hsignal -> Integral(1, hsignal -> GetNbinsX(), bind, bind) / hsignal -> Integral(0, hsignal -> GetNbinsX() + 1, bind, bind),
// 		 coeff);
// 	}
//     }
  
  // output -> SetLineColor(kGreen);
// hgen -> SetLineColor(kRed);

// ch -> NormaliseToBinWidth(hmeas);
// ch -> NormaliseToBinWidth(output);
// ch -> NormaliseToBinWidth(hgen);
// hmeas -> Scale(1.0/calculatearea(hmeas));
  // output -> Scale(1.0/calculatearea(output));
  // hgen -> Scale(1.0/calculatearea(hgen));
 printf("unfolded signal %p\n", output);
  output -> Print("all");
  hsignal -> ProjectionY("hsignalprojy") -> Print("all");
  printf("unfolded signal w gen %p\n", outputgen);
  outputgen -> Print("all");
  hsignalgen -> ProjectionY("hsignalgenprojy") -> Print("all");
  for (unsigned char bind = 0; bind < output -> GetNbinsX() + 1; bind ++)
    {
      if (hsignalgen -> Integral(0, hsignalgen -> GetNbinsX(), bind, bind) == 0.0)
	continue;
      const float coeff = hsignalgen -> Integral(1, hsignalgen -> GetNbinsX() + 1, bind, bind)/ hsignalgen -> Integral(0, hsignalgen -> GetNbinsX(), bind, bind);
      output -> SetBinContent(bind, output -> GetBinContent(bind)/coeff);
    }
  printf("output again\n");
  output -> Print("all");
  return 0;

  printf("gen\n");
  hgen -> Print("all");
  printf("gen w reco\n");
  hsignal -> ProjectionY() -> Print("all");

  // THStack stack;
  // stack.Add(hmeas);
  // stack.Add(output);
  // stack.Add(hgen);
  // TCanvas *coutput = new TCanvas;
  // stack.Draw("nostack");
  return 0;
}
