#include "Definitions.hh"
#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "THStack.h"
#include "TPad.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include <assert.h> 
#include <string> 
using namespace std;
void NormaliseToBinWidth(TH1F *, float orig_bin_width);
int main(int argc, char * argv[])
{
  gStyle -> SetErrorX(0.);
  assert(argc == 2);
  const string method(argv[1]);
  //system((string("rm -r common_plots_") + method).c_str());
  system((string("mkdir -p common_plots_") + method).c_str());
  const char * observables[] = {"pull_angle", "pvmag"};
  const char * samples_nominal[] = {"MC13TeV_TTJets", "MC13TeV_TTJets_herwig"};
  const char * samples_cflip[] = {"MC13TeV_TTJets", "MC13TeV_TTJets_cflip"};
  const char ** samples = nullptr;
  const char * sampletitles_nominal[3] = {"Powheg+Pythia8", "Powheg+Herwig"};
  const char * sampletitles_cflip[3] = {"Powheg+Pythia8 *", "Powheg+Pythia8 cf"};
  const char ** sampletitles = nullptr;
  unsigned char nsamples = 0;
  if (method.compare("nominal") == 0)
    {
      samples = samples_nominal;
      sampletitles = sampletitles_nominal;
      nsamples = 2;
    }
  if (method.compare("cflip") == 0)
    {
      samples = samples_cflip;
      sampletitles = sampletitles_cflip;
      nsamples = 2;
    }
  const Color_t samplecolors[] = {kRed, kBlue, kGreen};
  const TString input_folder = "/eos/user/v/vveckaln/unfolding_" + method + "/";
  const TString output_folder = "common_plots_" + method;
  for (unsigned char observable_ind = 0; observable_ind < 2; observable_ind ++) 
    {
      float bmin(0.0);
      float bmax(0.0);
      if (string(observables[observable_ind]).compare("pull_angle") == 0)
	{
	  bmin = 0.0;
	  bmax = TMath::Pi();
	}
      if (string(observables[observable_ind]).compare("pvmag") == 0)
	{
	  bmin = 0.0;
	  bmax = 0.015;
	}
      const unsigned char nbins = 20;
      const float orig_bin_width = (bmax - bmin) / nbins;
      for (unsigned char opt_level = 0; opt_level < 2; opt_level ++)
	{
	  for (ChargeCode_t charge_code = 0; charge_code < 2; charge_code ++)
	    {
	      for (VectorCode_t jetcode = 0; jetcode < 4; jetcode ++)
		{
		  printf("********************************************************************\n");
	     
		  const TString canvas_name =  TString(tag_jet_types_[jetcode]) + '_' + tag_charge_types_[charge_code] + '_' + observables[observable_ind] + '_' + tag_opt[opt_level] + "_mainplot";
		  TCanvas * c = new TCanvas(canvas_name, canvas_name);
		  THStack stack;
		  TH1F * hdmbckg(nullptr);
		  TH1F * hfsrup(nullptr);
		  TH1F * hfsrdown(nullptr);
		  TGraphErrors * gfsrup(nullptr);
		  TGraphErrors * gfsrdown(nullptr);
		  TGraphErrors * gdmbckg(nullptr);
		  TLegend * legend(nullptr);
		  TH1F * h[nsamples];
		  for (unsigned char sample_ind = 0; sample_ind < nsamples; sample_ind ++)
		    {
		      const TString input_file_name = input_folder + tag_jet_types_[jetcode] + '_' + tag_charge_types_[charge_code] + '_' + samples[sample_ind] + '_' + observables[observable_ind] + '_' + tag_opt[opt_level] + '_' + method + "/save.root";
		      TFile  * input_file = TFile::Open(input_file_name);
		      TH2F * h2 = (TH2F * ) input_file -> Get((string(tag_opt[opt_level]) + "_" + samples[sample_ind] + "_aggr").c_str());
		      input_file -> ls();
		      const TString name = TString(tag_jet_types_[jetcode]) + '_' + tag_charge_types_[charge_code] + '_' + samples[sample_ind] + '_' + observables[observable_ind] + '_' + tag_opt[opt_level] + "_mainplot";
		      h[sample_ind] = (TH1F *) h2 -> ProjectionY(name/*, 1, h2 -> GetNbinsY()*/);
		      if (sample_ind == 0)
			{
			  const TString namefsrup = TString(tag_jet_types_[jetcode]) + '_' + tag_charge_types_[charge_code] + '_' + samples[sample_ind] + '_' + observables[observable_ind] + '_' + tag_opt[opt_level] + "_fsrup";
			  const TString namefsrdown = TString(tag_jet_types_[jetcode]) + '_' + tag_charge_types_[charge_code] + '_' + samples[sample_ind] + '_' + observables[observable_ind] + '_' + tag_opt[opt_level] + "_fsrdown";
			  const TString namedata = TString(tag_jet_types_[jetcode]) + '_' + tag_charge_types_[charge_code] + '_' + samples[sample_ind] + '_' + observables[observable_ind] + '_' + tag_opt[opt_level] + "_data";
			  hfsrup = (TH1F *) ((TH2F*) input_file -> Get((string(tag_opt[opt_level]) + "_MC13TeV_TTJets_jec_RelativeFSR_up").c_str())) -> ProjectionY(namefsrup/*, 1, h2 -> GetNbinsY()*/);
			  hfsrdown = (TH1F *) ((TH2F*) input_file -> Get((string(tag_opt[opt_level]) + "_MC13TeV_TTJets_jec_RelativeFSR_down").c_str())) -> ProjectionY(namefsrdown/*, 1, h2 -> GetNbinsY()*/);
			  hdmbckg = (TH1F *) input_file -> Get((string(tag_opt[opt_level]) + "_output").c_str());
			  hdmbckg -> SetDirectory(nullptr);
			  NormaliseToBinWidth(hdmbckg, orig_bin_width);
			  hdmbckg -> Scale(1.0/hdmbckg -> Integral());
			  hdmbckg -> SetMarkerStyle(kFullCircle);
			  legend = new TLegend(0.6, 0.6, 1.0, 1.0);
			  hfsrup -> SetDirectory(nullptr);
			  hfsrdown -> SetDirectory(nullptr);
			  NormaliseToBinWidth(hfsrup, orig_bin_width);
			  hfsrup -> Scale(1.0/hfsrup -> Integral());
			  NormaliseToBinWidth(hfsrdown, orig_bin_width);
			  hfsrdown -> Scale(1.0 / hfsrdown -> Integral());
			  hfsrup -> SetLineColor(kBlack);
			  hfsrup -> SetLineColor(kBlack);
			  hfsrup -> SetMarkerStyle(kFullTriangleUp);
			  hfsrup -> SetMarkerColor(kBlack);
			  hfsrdown -> SetLineColor(kBlack);
			  hfsrdown -> SetMarkerStyle(kFullTriangleDown);
			  hfsrdown -> SetMarkerColor(kBlack);
			  stack.Add(hfsrup, "EP");
			  stack.Add(hfsrdown, "EP");
			  stack.Add(hdmbckg, "EP");
			  legend -> AddEntry(hfsrup, "fsrup", "lp");
			  legend -> AddEntry(hfsrdown, "fsrdn", "lp");
			  legend -> AddEntry(hdmbckg, "unf data - bckg", "lp");
			}
		      h[sample_ind] -> SetDirectory(nullptr);
		      h[sample_ind] -> SetLineColor(samplecolors[sample_ind]);
		      h[sample_ind] -> SetTitle(sampletitles[sample_ind]);
		      NormaliseToBinWidth(h[sample_ind], orig_bin_width);
		      h[sample_ind] -> Scale(1.0 / h[sample_ind] -> Integral());
		      stack.Add(h[sample_ind], "HIST");
		      legend -> AddEntry(h[sample_ind], sampletitles[sample_ind], "l");
		      input_file -> Close();
		    }		    
		  TPad * pad1 = new TPad("pad1", "pad1", 0.0, 0.2, 1.0, 1.0);
		  pad1 -> Draw();
		  c -> cd();
		  TPad * pad2 = new TPad("pad2", "pad2", 0.0, 0.0, 1.0, 0.2);
		  pad2 -> Draw();
		  pad1 -> cd();
		  stack.SetMinimum(0.0); 
		  stack.SetMaximum(1.3 * stack.GetMaximum("NOSTACK"));
		  stack.Draw("NOSTACK"); 
		  legend -> Draw("SAME");
		  pad2 -> cd();
		  TH1F * ratioframe = (TH1F*) h[0] -> Clone((string(h[0] -> GetName()) + "_ratioframe").c_str());
		  ratioframe -> SetDirectory(nullptr);
		  ratioframe -> Reset("ICE");
		  ratioframe -> SetStats(kFALSE);
		  ratioframe -> GetYaxis() -> SetRangeUser(0.4, 1.6);
		  ratioframe -> GetYaxis() -> SetTitle("MC/data");
		  ratioframe -> Draw();
		  TH1F * hrf[nsamples];
		  for (unsigned char sample_ind = 0; sample_ind < nsamples; sample_ind ++)
		    {
		      hrf[sample_ind] = (TH1F *) h[sample_ind] -> Clone((string(h[sample_ind] -> GetName()) + "_rf").c_str());
		      hrf[sample_ind] -> Divide(hdmbckg);
		      hrf[sample_ind] -> SetLineColor(samplecolors[sample_ind]);
		      hrf[sample_ind] -> Draw("SAME");
		    }
		  TH1F * hfsruprf = (TH1F*) hfsrup -> Clone((string(hfsrup -> GetName()) + "_rf").c_str());
		  hfsruprf -> Divide(hdmbckg);
		  hfsruprf -> SetMarkerColor(kBlack);
		  hfsruprf -> SetMarkerStyle(kFullTriangleUp);
		  hfsruprf -> Draw("SAMEE");
		  TH1F * hfsrdownrf = (TH1F*) hfsrdown -> Clone((string(hfsrdown -> GetName()) + "_rf").c_str());
		  hfsrdownrf -> Divide(hdmbckg);
		  hfsrdownrf -> SetMarkerColor(kBlack);
		  hfsrdownrf -> SetMarkerStyle(kFullTriangleDown);
		  hfsrdownrf -> Draw("SAMEE");
		  //pad2 -> Draw("SAME");
		  c -> SaveAs(output_folder + "/" + canvas_name + ".png");
		}    
	    }
	}
    }
		
}

void NormaliseToBinWidth(TH1F * h, float orig_bin_width)
{
  for (unsigned char bin_ind = 1; bin_ind < h -> GetNbinsX() + 1; bin_ind ++)
    {
      h -> SetBinContent(bin_ind, h -> GetBinContent(bin_ind) * orig_bin_width / h -> GetBinWidth(bin_ind));
    }
}
