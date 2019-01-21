#include "TUnfoldSys.h"
#include "TUnfoldDensity.h"
#include "Utilities.hh"
#include "TApplication.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TH2D.h"
#include "TFile.h"
#include "TGraph.h"
#include "TMath.h"
#include "TF1.h"
#include <iostream>
#include <vector>
#include "TChain.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "TROOT.h"
#include "THStack.h"
#include "TLegend.h"
int main()
{
  TApplication app("myapp", 0, 0);
  TChain chain("migration");
  chain.Add("$EOS/analysis/migration_MC13TeV_TTJets.root/E_migration");
  chain.Add("$EOS/analysis/migration_MC13TeV_TTJets.root/M_migration");
  const unsigned char nbins = 10;
  const float lower_edge = 0.0;//- TMath::Pi();
  const float upper_edge = TMath::Pi();
  const unsigned char ysize = 3;
  const unsigned char xsize = 5;
  const float ybins[ysize] = {0.000000, 1.413717, 3.141593};
  const float xbins[xsize] = {0.000000, 0.706858, 1.413717, 2.277655, 3.141593};

  TH2D * matrix = new TH2D("migration_matrix", "migration matrix; reco; gen", xsize - 1, xbins, ysize - 1, ybins);
  FillFromTreeCor(chain, *matrix, true);
  matrix -> Scale(luminosity * 832);
  for (unsigned char ind = 0; ind < matrix -> GetNbinsX(); ind ++)
    {
      printf("uf %f of %f \n", matrix -> GetBinContent(ind, 0), matrix -> GetBinContent(ind, matrix -> GetNbinsY() + 1));
    }
  TH1F * sigReco = (TH1F*)matrix -> ProjectionX("sigReco");
  TH1F * sigRecoNonGen = (TH1F*) matrix -> ProjectionX("sigRecoNonGen", 0, 0);
  const unsigned int nbinsm = matrix -> GetNbinsX() + 2;
  printf("nbinsm %u\n", nbinsm);
  float sf[nbinsm];
  for (unsigned int ind = 0; ind < nbinsm; ind ++)
    {
      float sfi = 1.0;
      if (sigReco -> GetBinContent(ind) > 0.0)
	{
	  sfi = sigRecoNonGen -> GetBinContent(ind)/sigReco -> GetBinContent(ind); 
	  printf("ind %u sf %f\n", ind, sfi);
	  sf[ind] = sfi;
	}
      matrix -> SetBinContent(ind, 0, 0.0);
    }
  TUnfoldDensity unfold(matrix, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeUser);
  TH1F * input_sf = (TH1F*) matrix -> ProjectionX();
  TFile *f = TFile::Open("test.root");
  TH1F * datambckg = (TH1F*) f -> Get("OPT_datambackground");
  unfold.SetInput(datambckg);
  //TGraph *LCurve = 0;
  //unfold.ScanLcurve(50, 0.0, 0.0, &LCurve);

  unfold.DoUnfold(0.0);
  TH1 * output = (TH1F*) unfold.GetOutput("output");

  TH1F * foldedback = (TH1F*)unfold.GetFoldedOutput("foldedoutput");
  //********************************************************
  TChain chain_cflip("migration_cflip");
  chain_cflip.Add("$EOS/analysis/migration_MC13TeV_TTJets_cflip.root/E_migration");
  chain_cflip.Add("$EOS/analysis/migration_MC13TeV_TTJets_cflip.root/M_migration");
  TH2D * matrix_cflip = new TH2D("migration_matrix_cflip", "migration matrix cflip; reco; gen", xsize - 1, xbins, ysize - 1, ybins);
  FillFromTreeCor(chain_cflip, *matrix_cflip, true);
  matrix_cflip -> Scale(luminosity * 832);
  for (unsigned int ind = 0; ind < nbinsm; ind ++)
    {
      matrix_cflip -> SetBinContent(ind, 0, 0.0);
    }
  TUnfoldDensity unfold_cflip(matrix_cflip, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeUser);
  unfold_cflip.SetInput(datambckg);
  //TGraph *LCurve = 0;
  //unfold.ScanLcurve(50, 0.0, 0.0, &LCurve);

  unfold_cflip.DoUnfold(0.0);
  TH1 * output_cflip = (TH1F*) unfold_cflip.GetOutput("output_cflip");
  THStack output_stack("output", "output");
  TCanvas coutput("coutput", "coutput");
  output_stack.Add(output_cflip);
  output_cflip -> SetMinimum(0.0);
  output_cflip -> SetLineColor(kGreen);
  TH1F * py_cflip = (TH1F*) matrix_cflip -> ProjectionY("py_cflip");
  output_stack.Add(output);
  output_stack.Draw("NOSTACK");
  TLegend loutput(0.6, 0.6, 1.0, 1.0);
  loutput.AddEntry(output);
  loutput.AddEntry(output_cflip);
  loutput.Draw("SAME");
  coutput.SaveAs("test_output/comp_output.png");
  TH1F * foldedback_cflip = (TH1F*)unfold_cflip.GetFoldedOutput("foldedoutput_cflip");
  THStack foldedback_stack("folded_back_stack", "folded back stack"); 
  TCanvas cfoldedback("foldedback", "foldedback_cflip");
  foldedback_stack.Add(foldedback_cflip);
  foldedback_stack.Add(foldedback);
  foldedback_cflip -> SetLineColor(kGreen);
  foldedback_cflip -> SetMinimum(0.0);
  input_sf -> SetMinimum(0.0);
  foldedback_stack.Add(datambckg);
  datambckg -> SetLineColor(kRed);
  foldedback_stack.Draw("NOSTACK");
  TLegend lfoldedback(0.6, 0.6, 1.0, 1.0);
  lfoldedback.AddEntry(foldedback_cflip);
  lfoldedback.AddEntry(foldedback);
  lfoldedback.Draw("SAME");
  cfoldedback.SaveAs("test_output/comp_foldedback.png");
  TCanvas proj("proj", "proj");
  THStack proj_stack("proj", "proj");
  TH1F * px = (TH1F*)matrix -> ProjectionX();
  TH1F * px_cflip = (TH1F*)matrix_cflip -> ProjectionX();
  proj_stack.Add(px);
  proj_stack.Add(px_cflip);
  px_cflip -> SetLineColor(kGreen);
  proj_stack.Draw("NOSTACK");
  app.Run(kTRUE);
  app.Terminate();
}
