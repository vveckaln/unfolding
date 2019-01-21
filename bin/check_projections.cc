#include "TUnfoldSys.h"
#include "TUnfoldDensity.h"

#include "TApplication.h"
#include "TCanvas.h"
#include "TH1D.h"
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
#include "Utilities.hh"
#include "CompoundHisto.hh"
using namespace std;
//TString pic_folder;
static const unsigned short xsec = 832;
int main(int argc, char* argv[])
{
  pic_folder = "projections";
  TApplication app("myapp", 0, 0);
  TChain chain("migration");
  chain.Add("$PROJECT/migration_testrun.root/E_migration");
  chain.Add("$PROJECT/migration_testrun.root/M_migration");
  const unsigned int nbins = 80;
  TH1D * hreco = new TH1D(CreateName("Reco"), CreateTitle("Reco ") +  "; pa; count", nbins , -TMath::Pi(), TMath::Pi());
  FillFromTreeCor(chain, * hreco, RECO);
  hreco -> Scale(luminosity * xsec);
  hreco -> SetMinimum(0.0);
  TCanvas creco;
  hreco -> Draw();
  TFile *f = TFile::Open("$PROJECT/testrun.root");
  TH1D * hrecocomp = (TH1D *) f -> Get("L_pull_angle_allconst_reco_leading_jet_2nd_leading_jet_DeltaRTotal");
  hrecocomp -> Scale(luminosity * xsec);
  TCanvas crecocomp;
  hrecocomp -> Draw();
  hrecocomp -> SetMinimum(0.0);
  printf("Integrals: hreco %f hrecocomp %f nbins %u\n", hreco -> Integral(), hrecocomp -> Integral(), hrecocomp -> GetNbinsX());
  TH1D * hgen = new TH1D(CreateName("Gen"), CreateTitle("Gen ") +  "; pa; count", nbins , -TMath::Pi(), TMath::Pi());
  FillFromTreeCor(chain, * hgen, GEN);
  hgen -> Scale(luminosity * xsec);
  hgen -> SetMinimum(0.0);
  TCanvas cgen;
  hgen -> Draw();
  TH1D * hgencomp = (TH1D *) f -> Get("L_pull_angle_allconst_gen_leading_jet_2nd_leading_jet_DeltaRTotal");
  hgencomp -> Scale(luminosity * xsec);
  TCanvas cgencomp;
  hgencomp -> Draw();
  hgencomp -> SetMinimum(0.0);
  printf("Integrals: hgen %f hgencomp %f\n", hgen -> Integral(), hgencomp -> Integral());
  if (gROOT -> GetListOfCanvases() -> GetSize() != 0)
    {
      app.Run(kTRUE);
      app.Terminate();
    }


  return 0;
  const float lower_edge = 0.0;
  const float upper_edge = TMath::Pi();
  orig_bin_width = (upper_edge - lower_edge)/nbins;
  TH2D * matrix = new TH2D(CreateName("migration_matrix"), CreateTitle("migration matrix") + "; reco; gen", nbins, lower_edge, upper_edge, nbins, lower_edge, upper_edge/*Nbinsx, binsx, Nbinsx, binsx*/);
  matrix -> RebinY(2);
  FillFromTree(chain, *matrix);
  for (unsigned char xind = 0; xind < matrix -> GetNbinsX() + 1; xind ++)
    {
      printf("xind %u x uf %f of %f\n", xind, matrix -> GetBinContent(xind, 0), matrix -> GetBinContent(xind, matrix -> GetNbinsY() + 1));  
    }

  for (unsigned char yind = 0; yind < matrix -> GetNbinsY() + 1; yind ++)
    {
      printf("yind %u y uf %f of %f\n", yind, matrix -> GetBinContent(yind, 0), matrix -> GetBinContent(yind, matrix -> GetNbinsX() + 1));  
    }
  
  matrix -> Scale(luminosity * xsec);
  f -> cd();
  matrix -> Write();
  Print(matrix, "COLZ");
  TH1D * matrixpx = matrix -> ProjectionX(CreateName("px"));
  ProcessHistogram(matrixpx);

  TH1D * matrixpy = matrix -> ProjectionY(CreateName("py"));
  ProcessHistogram(matrixpy);
  printf("Integrals: px %f py %f\n", matrixpx -> Integral(), matrixpy -> Integral());
  
  TH1D * sigRecoNonGen = matrix -> ProjectionX(CreateName("sigRecoNonGen"), 0, 0);
  TH1D * sigReco       = matrix -> ProjectionX(CreateName("sigReco"));
  TH1D * input_nosf    = matrix -> ProjectionX(CreateName("input_nosf"));
  TH1D * input_sf      = matrix -> ProjectionX(CreateName("input_sf"));

  
  if (sf_level_ind == SF)
    {
      for (unsigned char bin_ind = 0; bin_ind < input_sf -> GetNbinsX() + 1; bin_ind ++)
	{
	  float sf = 1.0;
	  if (sigReco -> GetBinContent(bin_ind) > 0)
	    {
	      sf = sigRecoNonGen -> GetBinContent(bin_ind) / sigReco -> GetBinContent(bin_ind);
	      printf("bin_ind %u sf %f\n", bin_ind, sf);

	      input_sf -> SetBinContent(bin_ind, (1.0 - sf) * input_sf -> GetBinContent(bin_ind));
	      input_sf -> SetBinError  (bin_ind, (1.0 - sf) * input_sf -> GetBinError(bin_ind));
	      matrix -> SetBinContent(bin_ind, 0, 0.0);
	    } 
	}
    }
  ProcessHistogram(input_sf);
  ProcessHistogram(input_nosf);

  TUnfoldDensity unfold(matrix, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeUser);
  unfold.SetInput(input_sf);
  TGraph *LCurve = 0;
  //unfold.DoUnfold(0.0);
  unfold.ScanLcurve(50, 0.0, 0.0, &LCurve);
  LCurve -> SetName(CreateName("LCurve"));

  printf("tau %.9f\n", unfold.GetTau());
  TH1 * houtput = unfold.GetOutput(CreateName("output"));
  ProcessHistogram(houtput);
  TH1 * hfolded_output = unfold.GetFoldedOutput(CreateName("folded_output"));
  ProcessHistogram(hfolded_output);
  TH2 * ematrix = unfold.GetEmatrixTotal(CreateName("ematrix"));
  Print(ematrix, "COLZ");
  /*
  TUnfoldSys unfold(matrix, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea);
  unfold.SetInput(matrix -> ProjectionX());
  TGraph *LCurve;
  unfold.ScanLcurve(50, 0.0, 0.0, &LCurve);
  printf("tau %.9f\n", unfold.GetTau());
  //  unfold.DoUnfold(unfold.GetTau());
  TH1D * houtput = (TH1D*) matrix -> ProjectionY() -> Clone("output");
  houtput -> Reset("ICE");
  unfold.GetOutput(houtput);
  TCanvas coutput;
  houtput -> Draw();
  coutput.SaveAs(TString("unfoldsysplots/") + houtput -> GetName() + ".png");
  TH1D * hfolded_output = (TH1D*) matrix -> ProjectionX() -> Clone("folded_output");
  hfolded_output -> Reset("ICE");
  unfold.GetFoldedOutput(hfolded_output);
  TCanvas cfolded_output;
  hfolded_output -> Draw();
  cfolded_output . SaveAs(TString("unfoldsysplots/") + hfolded_output -> GetName() + ".png");
  */
  opt_level_ind = OPT;
  vector<float> opt_bins = splitForMinSigmaM(*matrix, 0.2);
  const unsigned int sizegen = opt_bins.size();
  float array_bins_gen[sizegen];
  for (unsigned char bin_ind = 0; bin_ind < sizegen; bin_ind ++)
    {
      array_bins_gen[bin_ind] = opt_bins[bin_ind];
      printf("ind %u array_bins[bin_ind] %f\n", bin_ind, array_bins_gen[bin_ind]);
    }
  unsigned int sizereco = 2 *(sizegen - 1) + 1;
  float array_bins_reco[sizereco];
  unsigned char bin_indx;
  array_bins_reco[0] = array_bins_gen[0];
  for (bin_indx = 1; bin_indx < sizereco - 1; bin_indx +=2)
    {
      const float half = 0.5 * (array_bins_gen[bin_indx /2 + 1] - array_bins_gen[bin_indx / 2]);
      array_bins_reco[bin_indx] = array_bins_reco[bin_indx - 1] + half;
      array_bins_reco[bin_indx + 1] = array_bins_reco[bin_indx - 1] + 2*half;

    }
  for (bin_indx = 0; bin_indx < sizereco; bin_indx ++)
    {
      printf("bin_indx %u array_binsx[bin_indx] %f\n", bin_indx, array_bins_reco[bin_indx]);
    }
  TH2D matrix_opt(CreateName("migration_matrix"), CreateTitle("migration matrix") + "; reco; gen", sizereco - 1, array_bins_reco, sizegen - 1, array_bins_gen);
  FillFromTree(chain, matrix_opt);
  matrix_opt . Scale(luminosity * xsec);
  Print(&matrix_opt, "COLZ");
  
  f -> cd();
  matrix_opt . Write();

  TH1D * sigRecoNonGen_opt = matrix_opt . ProjectionX(CreateName("sigRecoNonGen"), 0, 0);
  TH1D * sigReco_opt       = matrix_opt . ProjectionX(CreateName("sigReco"));
  TH1D * input_nosf_opt    = matrix_opt . ProjectionX(CreateName("input_nosf"));
  TH1D * input_sf_opt      = matrix_opt . ProjectionX(CreateName("input_sf"));
  
  if (sf_level_ind == SF)
    { 
      for (unsigned char bin_ind = 0; bin_ind < input_sf -> GetNbinsX() + 1; bin_ind ++)
	{
	  float sf = 1.0;
	  if (sigReco_opt -> GetBinContent(bin_ind) > 0)
	    {
	      sf = sigRecoNonGen_opt -> GetBinContent(bin_ind) / sigReco_opt -> GetBinContent(bin_ind);
	      input_sf_opt -> SetBinContent(bin_ind, (1.0 - sf) * input_sf_opt -> GetBinContent(bin_ind));
	      input_sf_opt -> SetBinError  (bin_ind, (1.0 - sf) * input_sf_opt -> GetBinError(bin_ind));
	      matrix_opt. SetBinContent(bin_ind, 0, 0.0);
	    } 
	}
    }
  

  TUnfoldDensity unfold_opt(&matrix_opt, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeUser);
  unfold_opt.SetInput(input_sf_opt);
  TGraph * LCurve_opt = 0;

  //unfold_opt.DoUnfold(0.0);
  unfold_opt.ScanLcurve(50, 0.0, 0.0, &LCurve_opt);
  LCurve_opt -> SetName(CreateName("LCurve"));

  Print(LCurve_opt, "AP");
  printf("tau opt %.9f\n", unfold_opt.GetTau());
  TH2 * ematrix_opt = unfold_opt.GetEmatrixTotal(CreateName("ematrix"));
  Print(ematrix_opt, "COLZ");

  TH1 * houtput_opt = unfold_opt.GetOutput(CreateName("output"));
  ProcessHistogram(houtput_opt);
  houtput_opt -> SetMinimum(0.0);
  
  TH1 * hfolded_output_opt = unfold_opt.GetFoldedOutput(CreateName("folded_output"));
  ProcessHistogram(hfolded_output_opt);
  system(TString("sh scripts/copy_plots.sh ") + pic_folder);
}

