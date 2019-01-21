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
  system((string("rm -r common_plots_") + method).c_str());
  system((string("mkdir common_plots_") + method).c_str());
  const char * observables[] = {"pull_angle", "pvmag"};
  const char * samples_nominal[] = {"MC13TeV_TTJets", "MC13TeV_TTJets_herwig", "MC13TeV_TTJets2l2nu_amcatnlo"};
  const char * samples_cflip[] = {"MC13TeV_TTJets", "MC13TeV_TTJets_cflip"};
  const char ** samples = nullptr;
  const char * sampletitles_nominal[3] = {"Powheg+Pythia8", "Powheg+Herwig", "aMC@NLO+Pythia8"};
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
		  TCanvas * c[2][2];
		  TString canvas_name[2][2];
		  for (RecoLevelCode_t recocode = 0; recocode < 2; recocode ++)
		    {
		      for (ResultLevelCode_t resultlevel = 0; resultlevel < 2; resultlevel ++)
			{
			  canvas_name[recocode][resultlevel] = TString(tag_jet_types_[jetcode]) + '_' + tag_charge_types_[charge_code] + '_' + observables[observable_ind] + '_' + tag_opt[opt_level] + "_" + tag_recolevel[recocode] + "_" + tag_resultlevel[resultlevel];
			  c[recocode][resultlevel] = new TCanvas(canvas_name[recocode][resultlevel], canvas_name[recocode][resultlevel]);
			}
		    }
		  THStack stack[2][2];
		  TH1F * hdmbckg;
		  TH1F * hfsrup[2][2];
		  TH1F * hfsrdown[2][2];
		  TGraphErrors * gfsrup[2][2];
		  TGraphErrors * gfsrdown[2][2];
		  TGraphErrors * gdmbckg;
		  TLegend * legend[2][2];
		  TH1F * h[nsamples][2][2];
		  for (unsigned char sample_ind = 0; sample_ind < nsamples; sample_ind ++)
		    {
		      const TString input_file_name = input_folder + tag_jet_types_[jetcode] + '_' + tag_charge_types_[charge_code] + '_' 
			+ samples[sample_ind] + '_' + observables[observable_ind] + '_' + tag_opt[opt_level] + '_' + method + "/save.root";
		      TFile  * input_file = TFile::Open(input_file_name);
		      TH2F * h2 = (TH2F * ) input_file -> Get((string(tag_opt[opt_level]) + "_" + samples[sample_ind] + "_aggr").c_str());
		      input_file -> ls();
		      for (RecoLevelCode_t recocode = 0; recocode < 2; recocode ++)
			{
			  for (ResultLevelCode_t resultlevel = 0; resultlevel < 2; resultlevel ++)
			    {
			      const TString name = TString(tag_jet_types_[jetcode]) + '_' + tag_charge_types_[charge_code] + '_' + samples[sample_ind] + '_' + observables[observable_ind] + '_' + tag_opt[opt_level] + "_" + tag_recolevel[recocode] + "_" + tag_resultlevel[resultlevel];
			      switch(resultlevel)
				{
				case IN:
				  switch(recocode)
				    {
				    case RECO:
				      h[sample_ind][recocode][resultlevel] = (TH1F *) h2 -> ProjectionX(name, 1, h2 -> GetNbinsX());
				      break;
				    case GEN:
				      h[sample_ind][recocode][resultlevel] = (TH1F *) h2 -> ProjectionY(name/*, 1, h2 -> GetNbinsY()*/);
				      break;
				    }
				  break;
				case OUT:
				  switch(recocode)
				    {
				    case RECO:
				      h[sample_ind][recocode][resultlevel] = (TH1F *) input_file -> Get((string(tag_opt[opt_level]) + "_folded_output").c_str());
				      break;
				    case GEN:
				      h[sample_ind][recocode][resultlevel] = (TH1F *) input_file -> Get((string(tag_opt[opt_level]) + "_output").c_str());
				      break;
				    }
				  break;
				}
			      if (sample_ind == 0)
				{
				  const TString namefsrup = TString(tag_jet_types_[jetcode]) + '_' + tag_charge_types_[charge_code] + '_' + samples[sample_ind] + '_' + observables[observable_ind] + '_' + tag_opt[opt_level] + "_" + tag_recolevel[recocode] + "_" + tag_resultlevel[resultlevel] + "fsrup";
				  const TString namefsrdown = TString(tag_jet_types_[jetcode]) + '_' + tag_charge_types_[charge_code] + '_' + samples[sample_ind] + '_' + observables[observable_ind] + '_' + tag_opt[opt_level] + "_" + tag_recolevel[recocode] + "_" + tag_resultlevel[resultlevel] + "fsrdown";
				  const TString namedata = TString(tag_jet_types_[jetcode]) + '_' + tag_charge_types_[charge_code] + '_' + samples[sample_ind] + '_' + observables[observable_ind] + '_' + tag_opt[opt_level] + "_" + tag_recolevel[recocode] + "_" + tag_resultlevel[resultlevel] + "data";
				  switch(resultlevel)
				    {
				    case IN:
				      switch(recocode)
					{
					case RECO:
					  hfsrup[recocode][resultlevel] = (TH1F *) ((TH2F*) input_file -> Get((string(tag_opt[opt_level]) + "_MC13TeV_TTJets_jec_RelativeFSR_up").c_str())) -> ProjectionX(namefsrup, 1, h2 -> GetNbinsX());
					  hfsrdown[recocode][resultlevel] = (TH1F *) ((TH2F*) input_file -> Get((string(tag_opt[opt_level]) + "_MC13TeV_TTJets_jec_RelativeFSR_down").c_str())) -> ProjectionX(namefsrdown, 1, h2 -> GetNbinsX());
					  hdmbckg = (TH1F *) input_file -> Get((string(tag_opt[opt_level]) + "_inputreco").c_str());
					  break;
					case GEN:
					  hfsrup[recocode][resultlevel] = (TH1F *) ((TH2F*) input_file -> Get((string(tag_opt[opt_level]) + "_MC13TeV_TTJets_jec_RelativeFSR_up").c_str())) -> ProjectionY(namefsrup/*,1, h2 -> GetNbinsY()*/);
					  hfsrdown[recocode][resultlevel] = (TH1F *) ((TH2F*) input_file -> Get((string(tag_opt[opt_level]) + "_MC13TeV_TTJets_jec_RelativeFSR_down").c_str())) -> ProjectionY(namefsrdown/*, 1, h2 -> GetNbinsY()*/);
					  break;
					}
				      break;
				    case OUT:
				      switch(recocode)
					{
					case RECO:
					  hfsrup[recocode][resultlevel] = (TH1F *) input_file -> Get((string(tag_opt[opt_level]) + "_MC13TeV_TTJets_jec_RelativeFSR_up_folded_output").c_str());
					  hfsrdown[recocode][resultlevel] = (TH1F *) input_file -> Get((string(tag_opt[opt_level]) + "_MC13TeV_TTJets_jec_RelativeFSR_down_folded_output").c_str());
					  break;
					case GEN:
					  hfsrup[recocode][resultlevel] = (TH1F *) input_file -> Get((string(tag_opt[opt_level]) + "_MC13TeV_TTJets_jec_RelativeFSR_up_output").c_str());
					  hfsrdown[recocode][resultlevel] = (TH1F *) input_file -> Get((string(tag_opt[opt_level]) + "_MC13TeV_TTJets_jec_RelativeFSR_down_output").c_str());
					  break;
					}
				      break;
				    }
				}
			    }
			}
		      for (RecoLevelCode_t recocode = 0; recocode < 2; recocode ++)
			{
			  if (sample_ind == 0 and recocode == RECO)
			    {
			      hdmbckg -> SetDirectory(nullptr);
			      //			      hdmbckg -> SetLineColor(kWhite);
			      NormaliseToBinWidth(hdmbckg, orig_bin_width);
			      hdmbckg -> Scale(1.0/hdmbckg -> Integral());
			      hdmbckg -> Sumw2();
			      hdmbckg -> Print("all");
			      hdmbckg -> SetMarkerStyle(kFullCircle);
			      
			    }
			  for (ResultLevelCode_t resultlevel = 0; resultlevel < 2; resultlevel ++)
			    {
			      h[sample_ind][recocode][resultlevel] -> SetDirectory(nullptr);
			      h[sample_ind][recocode][resultlevel] -> SetLineColor(samplecolors[sample_ind]);
			      h[sample_ind][recocode][resultlevel] -> SetTitle(sampletitles[sample_ind]);
			      NormaliseToBinWidth(h[sample_ind][recocode][resultlevel], orig_bin_width);
			      h[sample_ind][recocode][resultlevel] -> Scale(1.0/h[sample_ind][recocode][resultlevel] -> Integral());
     			      stack[recocode][resultlevel].Add(h[sample_ind][recocode][resultlevel], "HIST");
			      if (sample_ind == 0)
				{
				  legend[recocode][resultlevel] = new TLegend(0.6, 0.6, 1.0, 1.0);
				  hfsrup[recocode][resultlevel] -> SetDirectory(nullptr);
				  hfsrdown[recocode][resultlevel] -> SetDirectory(nullptr);
				  NormaliseToBinWidth(hfsrup[recocode][resultlevel], orig_bin_width);
				  NormaliseToBinWidth(hfsrdown[recocode][resultlevel], orig_bin_width);
				  hfsrup[recocode][resultlevel] -> Scale(1.0/ hfsrup[recocode][resultlevel] -> Integral());
				  hfsrdown[recocode][resultlevel] -> Scale(1.0/ hfsrdown[recocode][resultlevel] -> Integral());
				  //				  if (recocode == RECO and resultlevel == OUT)
				    {
				      gfsrup[recocode][resultlevel] = new TGraphErrors(hfsrup[recocode][resultlevel]);
				      /*hfsrup[recocode][resultlevel] -> SetLineColor(kBlack);
				      hfsrup[recocode][resultlevel] -> SetLineStyle(3);
				            hfsrdown[recocode][resultlevel] -> SetLineColor(kBlack);
				      hfsrdown[recocode][resultlevel] -> SetLineStyle(3);
				    }
				  else
				  {*/
				      gfsrup[recocode][resultlevel] -> SetLineColor(kBlack);
				      gfsrup[recocode][resultlevel] -> SetMarkerStyle(kFullTriangleUp);
				      gfsrup[recocode][resultlevel] -> SetMarkerColor(kBlack);
				      gfsrdown[recocode][resultlevel] = new TGraphErrors(hfsrdown[recocode][resultlevel]);
				      gfsrdown[recocode][resultlevel] -> SetLineColor(kBlack);
				      gfsrdown[recocode][resultlevel] -> SetMarkerStyle(kFullTriangleDown);
				      gfsrdown[recocode][resultlevel] -> SetMarkerColor(kBlack);
				    }
				  //				  hfsrup[recocode][resultlevel] -> SetLineColor(kBlack);
				  //hfsrup[recocode][resultlevel] -> Scale(1.2);
				    //				  stack[recocode][resultlevel].Add(hfsrup[recocode][resultlevel], recocode == RECO and resultlevel == OUT ? "HIST" : "E");
				    //stack[recocode][resultlevel].Add(hfsrdown[recocode][resultlevel], recocode == RECO and resultlevel == OUT ? "HIST" : "E");
				}
			      legend[recocode][resultlevel] -> AddEntry(h[sample_ind][recocode][resultlevel], sampletitles[sample_ind], "l");
			      if (sample_ind == 0 )
				{
				  legend[recocode][resultlevel] -> AddEntry(gfsrup[recocode][resultlevel], "fsrup", "lp");
				  legend[recocode][resultlevel] -> AddEntry(gfsrdown[recocode][resultlevel], "fsrdn", "lp");
				  if (recocode == RECO and resultlevel == IN)
				    {
				      //stack[recocode][resultlevel].Add(hdmbckg, "E");
				      legend[recocode][resultlevel] -> AddEntry(hdmbckg, "data - bckg", "lp");
				    }
				}
			    }
			}
		      input_file -> Close();
		    }		    
		  for (RecoLevelCode_t recocode = 0; recocode < 2; recocode ++)
		    {
		      for (ResultLevelCode_t resultlevel = 0; resultlevel < 2; resultlevel ++)
			{
			  c[recocode][resultlevel] -> cd();
			  TPad * pad1(nullptr), * pad2(nullptr);
			  if (recocode == RECO and resultlevel == IN)
			    {
			      pad2 = new TPad("pad2", "pad2", 0.0, 0.0, 1.0, 0.2);
			      c[recocode][resultlevel] -> cd();
			      pad2 -> Draw();
			      c[recocode][resultlevel] -> cd();
			      pad1 = new TPad("pad1", "pad1", 0.0, 0.2, 1.0, 1.0);
			      pad1 -> Draw();
			      pad1 -> cd();
			    }
			  stack[recocode][resultlevel].SetMinimum(0.0); 
			  stack[recocode][resultlevel].SetMaximum(1.3 * stack[recocode][resultlevel].GetMaximum("NOSTACK"));
			  stack[recocode][resultlevel].Draw("NOSTACK"); 
			  gfsrup[recocode][resultlevel] -> Draw("EPSAME");
			  gfsrdown[recocode][resultlevel] -> Draw("EPSAME");
			  if (recocode == RECO and resultlevel == IN)
			    {
			      hdmbckg -> Draw("EPSAME");
			    }
			  //			  stack[recocode][resultlevel].ls(); 
			  legend[recocode][resultlevel] -> Draw("SAME");
			  c[recocode][resultlevel] -> cd();
			  if (recocode == RECO and resultlevel == IN)
			    {
			      pad2 -> cd();
			      TH1F * ratioframe = (TH1F*) h[0][recocode][resultlevel] -> Clone((string(h[0][recocode][resultlevel] -> GetName()) + "_ratioframe").c_str());
			      ratioframe -> SetDirectory(nullptr);
			      ratioframe -> Reset("ICE");
			      ratioframe -> SetStats(kFALSE);
			      ratioframe -> GetYaxis() -> SetRangeUser(0.4, 1.6);
			      ratioframe -> GetYaxis() -> SetTitle("MC/data");
			      ratioframe -> Draw();
			      TH1F * hrf[nsamples];
			      for (unsigned char sample_ind = 0; sample_ind < nsamples; sample_ind ++)
				{
				  hrf[sample_ind] = (TH1F *) h[sample_ind][recocode][resultlevel] -> Clone((string(h[sample_ind][recocode][resultlevel] -> GetName()) + "_rf").c_str());
				  hrf[sample_ind] -> Divide(hdmbckg);
				  hrf[sample_ind] -> SetLineColor(samplecolors[sample_ind]);
				  hrf[sample_ind] -> Draw("SAME");
				}
			      TH1F * hfsruprf = (TH1F*) hfsrup[recocode][resultlevel] -> Clone((string(hfsrup[recocode][resultlevel] -> GetName()) + "_rf").c_str());
			      hfsruprf -> Divide(hdmbckg);
			      hfsruprf -> SetMarkerColor(kBlack);
			      hfsruprf -> SetMarkerStyle(kFullTriangleUp);
			      hfsruprf -> Draw("SAMEE");
			      TH1F * hfsrdownrf = (TH1F*) hfsrdown[recocode][resultlevel] -> Clone((string(hfsrdown[recocode][resultlevel] -> GetName()) + "_rf").c_str());
			      hfsrdownrf -> Divide(hdmbckg);
			      hfsrdownrf -> SetMarkerColor(kBlack);
			      hfsrdownrf -> SetMarkerStyle(kFullTriangleDown);
			      hfsrdownrf -> Draw("SAMEE");
			      //pad2 -> Draw("SAME");
			    }    
			  c[recocode][resultlevel] -> SaveAs(output_folder + "/" + canvas_name[recocode][resultlevel] + ".png");
			}
		    }
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
