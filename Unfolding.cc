#include "TUnfoldSys.h"
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
using namespace std;
vector<float> splitForMinSigma(TH2D & h, float factor = 0.5);
vector<float> splitForMinSigma_v2 (TH2D & h, const char * = "X", float factor = 0.5);

void FillFromTree(TTree &, TH2D &h);
const TString pic_folder("unfolding_31_08_2017_test");
static const unsigned int luminosity = 35922;
static const unsigned short xsec = 832;

int main()
{
  TApplication app("myapp", 0, 0);
  TChain chain("migration");
  const TString EOS = "root://eosuser.cern.ch///eos/user/v/vveckaln/analysis/";
  for (unsigned char ind = 0; ind < 136; ind ++)
    {
      char name[128];
      sprintf(name, "migration/migration_MC13TeV_TTJets_%u.root", ind);
      chain.Add(EOS + name);
    }
  const unsigned char nbins = 100;
  //  chain.Add("migration/migration_test_file.root");
  TH2D * migration_matrix = new TH2D("Migration", "Migration; reco; gen", nbins, -TMath::Pi(), TMath::Pi(), nbins, -TMath::Pi(), TMath::Pi());
  //TFile * file = TFile::Open("MC13TeV_TTJets.root");
  //TH2D * migration_matrix = (TH2D*)file -> Get("Migration");
  //  migration_matrix -> RebinY(2);
  //TH1D * pull_angle_reco = (TH1D*)file -> Get("pull_angle_allconst_reco_leading_jet_:_2nd_leading_jet_DeltaRTotal_4j2t");
  FillFromTree(chain, *migration_matrix);
  migration_matrix -> Scale(luminosity * xsec);
 
  migration_matrix -> SetMinimum(-10E-10);

  TH1D * gen_orig = migration_matrix -> ProjectionY();

  gen_orig -> SetMinimum(0.0);
  const float orig_bin_width = gen_orig -> GetBinWidth(2);

  TCanvas c_gen_orig("gen_orig", "gen_orig");
  gen_orig -> Draw();
  c_gen_orig.SaveAs(pic_folder + "/original_gen.png");
  TH1D * reco_orig = migration_matrix -> ProjectionX();
  reco_orig -> SetMinimum(0.0);

  TCanvas c_reco_orig("reco_orig", "reco_orig");
  reco_orig -> Draw();
  c_reco_orig.SaveAs(pic_folder + "/original_reco.png");

  const float sigma_supr_fact = 0.1;
  vector<float> bins_X = splitForMinSigma_v2(*migration_matrix, "X", sigma_supr_fact);
  float bins_fl_X[bins_X.size()];
  for (unsigned long ind = 0; ind < bins_X.size(); ind ++)
    {
      printf("bin %lu val %f\n", ind, bins_X.at(ind));
      bins_fl_X[ind] = bins_X.at(ind);
    }

  
  vector<float> bins_Y = splitForMinSigma_v2(*migration_matrix, "Y", sigma_supr_fact);
  float bins_fl_Y[bins_Y.size()];
  for (unsigned long ind = 0; ind < bins_Y.size(); ind ++)
    {
      printf("bin %lu val %f\n", ind, bins_Y.at(ind));
      bins_fl_Y[ind] = bins_Y.at(ind);
    }
  
  
  TH2D * matrix_opt = new TH2D("matrix_opt", "matrix_opt; reco; gen", bins_X.size() - 1, bins_fl_X, bins_X.size() - 1, bins_fl_X);

  FillFromTree(chain, *matrix_opt);
  matrix_opt -> Scale(luminosity * xsec);
  matrix_opt -> SetMinimum(-1E-10);
  /*for (unsigned char ind_x = 1; ind_x < matrix_opt -> GetNbinsX() + 1; ind_x ++)
    {
      for (unsigned char ind_y = 1; ind_y < matrix_opt -> GetNbinsY() + 1; ind_y ++)
	{
	  matrix_opt -> SetBinContent(ind_x, ind_y, matrix_opt -> GetBinContent(ind_x, ind_y)/(matrix_opt -> GetXaxis() -> GetBinWidth(ind_x) * matrix_opt -> GetYaxis() -> GetBinWidth(ind_y)));
	}
    }*/
  TH1D * gen_opt = matrix_opt -> ProjectionY();
  for (unsigned char ind = 1; ind < gen_opt -> GetNbinsX() + 1; ind ++)
    {
      gen_opt -> SetBinContent(ind, gen_opt -> GetBinContent(ind) * orig_bin_width/gen_opt -> GetBinWidth(ind));
    }
  gen_opt -> SetMinimum(0.0);
  TCanvas c_gen_opt("gen_opt", "gen_opt");
  gen_opt -> Draw();
  c_gen_opt.SaveAs(pic_folder + "/optimised_gen.png");
  TH1D * reco_opt = matrix_opt -> ProjectionX();
  for (unsigned char ind = 1; ind < reco_opt -> GetNbinsX() + 1; ind ++)
    {
      reco_opt -> SetBinContent(ind, reco_opt -> GetBinContent(ind) * orig_bin_width/reco_opt -> GetBinWidth(ind));
    }
  
  reco_opt -> SetMinimum(0.0);
  TCanvas c_reco_opt("reco_opt", "reco_opt");
  reco_opt -> Draw();
  c_reco_opt.SaveAs(pic_folder + "/optimised_reco.png");

  
  //exit(0);
  //TCanvas canvas2("pull_angle", "pull_angle");
  //pull_angle_reco -> Draw();*/

  
  printf("********** UNFOLD ORIGINAL *************\n");


  migration_matrix -> RebinY(2);
  TCanvas canvas1("migration", "migration");
  migration_matrix -> Draw("COLZ");
  canvas1.SaveAs(pic_folder + "/original_matrix.png");

  TUnfoldSys unfold(migration_matrix, TUnfold::kHistMapOutputVert/*, TUnfold::kRegModeSize*/);
  if (unfold.SetInput(migration_matrix -> ProjectionX()) >= 1000)
    {
      printf("Unfolding results may be wrong \n");
    }
  //  unfold.DoUnfold(pow(10, -0.411703));
  TGraph *LCurve = new TGraph;
  unfold.ScanLcurve(40, 0.0, 0.0, &LCurve);
  TH1D * output = new TH1D("output", "output", migration_matrix -> GetNbinsY(), -TMath::Pi(), TMath::Pi());
  unfold.GetOutput(output);
  output -> Scale(0.5);
 TCanvas c_output("output", "output");
  output -> SetMinimum(0.0);
  output -> Draw();
  c_output.SaveAs(pic_folder + "/unfolded_original_gen.png");
  TCanvas canvas4("graph", "graph");
  LCurve -> Draw("AP");
  canvas4.SaveAs(pic_folder + "/LCurve_original.png");

  printf("********** UNFOLD OPTIMAL *************\n");
  //matrix_opt -> RebinX(2);
  matrix_opt -> RebinY(2);
  TCanvas c_matrix_opt("migr_opt", "migr_opt");
  matrix_opt -> Draw("COLZ");
  c_matrix_opt.SaveAs(pic_folder + "/optimised_matrix.png");

  TUnfoldSys unfold_opt(matrix_opt, TUnfold::kHistMapOutputVert/*, TUnfold::kRegModeSize*/);
  if (unfold_opt.SetInput(matrix_opt -> ProjectionX()) >= 1000)
    {
      printf("Opt Unfolding result may be wrong \n");
    }  //  unfold.DoUnfold(pow(10, -0.411703));
  
  TGraph *LCurve_opt = new TGraph;
  unfold_opt.ScanLcurve(40, 0.0, 0.0, &LCurve_opt);
  const  double  * array = matrix_opt -> GetYaxis() -> GetXbins() -> GetArray();
  const unsigned short matrix_opt_size =  matrix_opt -> GetYaxis() -> GetXbins()-> GetSize();
  double array_fixed[matrix_opt_size + 1];
  unsigned char ind = 0;
  for (; ind < matrix_opt_size; ind ++)
    {
      array_fixed[ind] = array[ind];
    }
  array_fixed[ind] = TMath::Pi();
  TH1D * output_opt = new TH1D("output_opt", "output_opt", matrix_opt_size, array_fixed);
  
  unfold_opt.GetOutput(output_opt);
  for (unsigned char ind = 1; ind < output_opt -> GetNbinsX() + 1; ind ++)
    {
      output_opt -> SetBinContent(ind, output_opt -> GetBinContent(ind) * orig_bin_width / output_opt -> GetBinWidth(ind));
    }
  TCanvas c_output_opt("output_opt", "output_opt");
  output_opt -> SetMinimum(0.0);
  output_opt -> Draw();
  c_output_opt.SaveAs(pic_folder + "/unfolded_optimised_gen.png");
  TCanvas canvas4_opt("graph_opt", "graph_opt");
  LCurve_opt -> Draw("AP");
  canvas4_opt.SaveAs(pic_folder + "/LCurve_opt.png");
  
  app.Run(kTRUE);
  app.Terminate();
}

vector<float> splitForMinSigma(TH2D & h, float factor)
{
  TAxis * Xaxis = h.GetXaxis();
  //TAxis * Yaxix = h.GetYaxis();

  TF1 f1 = TF1("f1", "gaus", Xaxis -> GetXmin(), Xaxis -> GetXmax());
  f1.SetParameters(100, (Xaxis -> GetXmax() - Xaxis -> GetXmin())/2., 0.01);
  TObjArray slices;
  h.FitSlicesX(& f1, 1, h.GetNbinsX(), 0, "QNRLM", &slices);
    
  TCanvas c2("c2", "c2");
  ((TH1D*)slices[0]) -> Draw();
  c2.Print("migr_analysis/slices_N.eps");
  ((TH1D*)slices[1]) -> GetYaxis() -> SetRangeUser(Xaxis -> GetXmin(), Xaxis -> GetXmax());
  ((TH1D*)slices[1]) -> Draw();
  c2.Print("migr_analysis/slices_mean.eps");
  ((TH1D*)slices[2]) -> GetYaxis() -> SetRangeUser(0.0, Xaxis -> GetXmax());
  ((TH1D*)slices[2]) -> Draw();
  c2.Print("migr_analysis/slices_sigma.eps");
  
  vector<float> bins;
 
  bins.push_back(Xaxis -> GetXmin());
  vector<float> exact;
  exact.push_back(Xaxis -> GetXmin());
  
  for (unsigned short i = 1; i < h.GetNbinsX()+1; i++)
    {
      const float mean  = ((TH1D *)slices[1]) -> GetBinContent(i);
      const float sigma = ((TH1D *)slices[2]) -> GetBinContent(i) * factor;
      printf("i %u mean %f sigma %f mean - sigma %f exact.back() %f \n", i, mean, sigma, mean - sigma, exact.back());
      if (mean - sigma > exact.back())
      {
	if (mean + sigma < Xaxis -> GetXmax())
	  {
	    printf("mean + sigma %f\n", mean + sigma);
	    exact.push_back(mean + sigma);
	    const float newbinedge = Xaxis -> GetBinUpEdge((Xaxis -> FindBin(mean + sigma)));
	    if (newbinedge > bins.back()) 
	      {
		printf("newbinedge %f, bins.back() %f\n", newbinedge, bins.back());
		bins.push_back(newbinedge);
	      }			    
	
	  }
	else
	  {
	    printf("         !!!! mean + sigma > Xaxis -> GetXmax()\n");
	  }
      }
     
    }
  if (bins.size() > 1)
    {
      bins.back() = Xaxis -> GetXmax();
    }
  return bins;
}

vector<float> splitForMinSigma_v2(TH2D & h, const char * option, float factor)
{
  vector<float> bins;
  TAxis * axis;
  TObjArray slices;
  void (TH2D::*f_pointer)(TF1*, Int_t, Int_t, Int_t , Option_t*, TObjArray* );
  if (TString(option) == "X")
    {
      axis = h.GetXaxis();
      f_pointer = &TH2D::FitSlicesX;
    }
  else 
    {
      axis = h.GetYaxis();
      f_pointer = &TH2D::FitSlicesY;
    }
  const float up = axis -> GetXmax();
  const float down = axis -> GetXmin();
  TF1 f1 = TF1("f1", "gaus", down, up);
  f1.SetParameters(100, (up - down)/2., 1.0);
  (h .* f_pointer)(& f1, 1, h.GetNbinsX(), 0, "QNRLM", &slices);


  TCanvas c2("c2", "c2");
  ((TH1D*)slices[0]) -> Draw();
  c2.Print(pic_folder + "/slices_N_" + TString(option) + ".png");
  ((TH1D*)slices[1]) -> GetYaxis() -> SetRangeUser(axis -> GetXmin(), axis -> GetXmax());
  ((TH1D*)slices[1]) -> Draw();
  c2.Print(pic_folder + "/slices_mean_" + TString(option) + ".png");
  ((TH1D*)slices[2]) -> GetYaxis() -> SetRangeUser(0.0, axis -> GetXmax());
  ((TH1D*)slices[2]) -> Draw();
  c2.Print(pic_folder + "/slices_sigma_" + TString(option) + ".png");
  const unsigned short ind_start = axis -> GetNbins() % 2 == 0 ? axis -> GetNbins()/2 : (axis -> GetNbins() - 1)/2 + 1;
  
  vector<float> bins_up;
  {
    unsigned short ind_prev = ind_start;
    float mean_prev = ((TH1D *)slices[1]) -> GetBinContent(ind_prev);
    float sigma_prev = ((TH1D *)slices[2]) -> GetBinContent(ind_prev) * factor;
    for (unsigned short ind = ind_start; ind < axis -> GetNbins() + 1; ind ++)
      {
	const float mean  = ((TH1D *)slices[1]) -> GetBinContent(ind);
	const float sigma = ((TH1D *)slices[2]) -> GetBinContent(ind) * factor;
	//	printf("ind_down %u mean %f sigma %f mean - sigma %f mean_prev + sigma_prev %f mean + sigma %f\n", ind, mean, sigma, mean - sigma, mean_prev + sigma_prev, mean + sigma);
	if (mean - sigma > mean_prev + sigma_prev and mean + sigma < axis -> GetXmax())
      {
	const float distance = mean - mean_prev;
	const float split = mean - sigma - (mean_prev + sigma_prev);
	const float ratio = split/distance;
	const float segment1 = sigma_prev + split * sigma_prev/(sigma + sigma_prev);
	const float segment2 = sigma + split * sigma/(sigma + sigma_prev);
	bins_up.push_back(mean_prev + segment1);
	//	printf("    pushed back %f\n", bins_up.back());
	mean_prev = mean;
	sigma_prev = sigma;
      }
  }
  if (bins_up.size() > 1)
    {
      bins_up.back() = axis -> GetXmax();
    }
 
}
// *************************** DOWN *************************
  vector<float> bins_down;
  {
    unsigned short ind_prev = ind_start;
    float mean_prev = ((TH1D *)slices[1]) -> GetBinContent(ind_prev);
    float sigma_prev = ((TH1D *)slices[2]) -> GetBinContent(ind_prev) * factor;
    for (  unsigned short ind = ind_start; ind > 0; ind --)
      {
	const float mean  = ((TH1D *)slices[1]) -> GetBinContent(ind);
	const float sigma = ((TH1D *)slices[2]) -> GetBinContent(ind) * factor;
	//printf("ind_down %u mean %f sigma %f\n", ind, mean, sigma);
	if (mean + sigma < mean_prev - sigma_prev and mean - sigma > axis -> GetXmin())
	  {
	    const float distance = mean_prev - mean;
	    const float split = mean_prev - sigma_prev - (mean  + sigma);
	    const float ratio = split/distance;
	    const float segment1 = sigma_prev + split * sigma_prev/(sigma + sigma_prev);
	    const float segment2 = sigma + split * sigma/(sigma + sigma_prev);
	    bins_down.push_back(mean_prev - segment1);
	    mean_prev = mean;
	    sigma_prev = sigma;
	    //	    printf("pushed back %f\n", bins_down.back());
	  }
      }
    if (bins_down.size() > 1)
      {
	bins_down.back() = axis -> GetXmin();
      }

  }
  for (short ind = bins_down.size() - 1; ind > -1; ind --)
    {
      //printf("%u\n", ind);

      bins.push_back(bins_down.at(ind));
    }
  for (unsigned short ind = 0; ind < bins_up.size(); ind ++)
    {
      //printf("%u\n", ind);

      bins.push_back(bins_up.at(ind));
    }
  return bins;
}


void FillFromTree(TTree & tree, TH2D &h)
{
  Double_t pull_angle_reco;
  Double_t weight_reco;
  Double_t pull_angle_gen;
  Double_t weight_gen;
  tree.SetBranchAddress("pull_angle_reco", &pull_angle_reco);
  tree.SetBranchAddress("weight_reco",     &weight_reco);
  tree.SetBranchAddress("pull_angle_gen",  &pull_angle_gen);
  tree.SetBranchAddress("weight_gen",      &weight_gen);
  printf("Filling from  tree; %lu entries\n", tree. GetEntries());
  for (unsigned long event_ind = 0; event_ind < tree.GetEntries(); event_ind ++)
    {
      if (event_ind % 10000 == 0)
	printf("%u\r", event_ind);
      tree.GetEntry(event_ind);
      const double weight = 0.5*(weight_reco + weight_gen);
      h.Fill(pull_angle_reco, pull_angle_gen, weight);
    }
  printf("Filling from tree done \n");
}
