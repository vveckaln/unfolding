#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TTree.h"
#include "TChain.h"
#include "TApplication.h"
#include "TMath.h"
#include <vector>
#include "TFitResult.h"
#include "THStack.h"
#include <assert.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
float * optimisev2(TH2 * h2, double * sigmas, unsigned long & size, float factor, float * binsi, unsigned char & depth, bool stretch);
TH1 * stability(TH2 * h2);
TH1 * purity(TH2 * h2);
TH1 * stabilityunsplit(TH2 * h2);
TH1 * purityunsplit(TH2 * h2);
float * splitbins(float *bins, unsigned long size);
void writetofile(float *bins, unsigned long size, FILE * file);
float gstep;
float gmax;
void printstabandpur(TH1 * stab, TH1 * pur, unsigned char depth);
void printbins(float *bins, unsigned long size, unsigned char depth);
void printstddev(double * stddev, unsigned char nbins);
using namespace std;
int main(int argc, char * argv[])
{
  assert(argc > 1);
  const string observable(argv[1]);
  system("mkdir -p binningresults");
  FILE * file = fopen(TString("binningresults/") + observable.c_str() + ".txt", "w");
  //  TApplication app("myapp", 0, 0);
  TChain chain("migration");
  chain.Add("$EOS/analysis_MC13TeV_TTJets/HADDmigration/migration_MC13TeV_TTJets.root/E_chconst_leading_jet_migration");
  chain.Add("$EOS/analysis_MC13TeV_TTJets/HADDmigration/migration_MC13TeV_TTJets.root/M_chconst_leading_jet_migration");
  Float_t reco;
  Float_t gen;
  using namespace std;
  vector<double> * weights = nullptr;
  if (observable.compare("pull_angle") == 0)
    {
      chain.SetBranchAddress("pull_angle_reco", & reco);
      chain.SetBranchAddress("pull_angle_gen",  & gen);
    }
  if (observable.compare("pvmag") == 0)
    {
      chain.SetBranchAddress("pvmag_reco", & reco);
      chain.SetBranchAddress("pvmag_gen",  & gen);
    }
  chain.SetBranchAddress("weight",          & weights);
  unsigned long coinc = 0;
  //  printf("GetTag() %s %s %s\n", GetTag(), string(GetTag()).c_str(), string(GetTag()).compare("MC13TeV_TTJets") == 0? "True" : " False");
  if (observable.compare("pull_angle") == 0 )
    gmax = 1.0;//TMath::Pi();
  if (observable.compare("pvmag") == 0 )
    gmax = 0.015;
  bool binned(false);
  unsigned char depth = 0;
  unsigned long size(21);
  unsigned char nbins = size - 1;
  float * binsi = new float[nbins + 1];
  float * bins = binsi;
  float step(gmax/nbins);
  gstep = step;
  binsi[0] = 0.0;
  for (unsigned char bind = 0; bind < nbins; bind ++)
    {
      binsi[bind + 1] = bins[bind] + step;
    }
  while (not binned)
    {
      printf("new iteration\n\n");
      printbins(bins, size, depth);
      unsigned char nbins = size - 1;
      TH2F * _th2 = new TH2F("m", "m", nbins, bins, nbins, bins);
      const unsigned char div(1);
      double stdev_gen[nbins];
      double weights_gen[nbins];
      for (unsigned char bind = 0; bind < nbins; bind ++)
	{
	  stdev_gen[bind] = 0.0;
	  weights_gen[bind] = 0.0;
	}
      for (unsigned long event_ind = 0; event_ind < chain.GetEntries()/div; event_ind ++)
	{
	  if (event_ind % 10000 == 0)
	    printf("%u\r", event_ind);
	  chain.GetEntry(event_ind);
	  _th2 -> Fill(reco == -10.0 ? reco : TMath::Abs(reco)/TMath::Pi(), gen == -10.0 ? gen : TMath::Abs(gen)/TMath::Pi(), (*weights)[0]);
	  if (string(observable).compare("pull_angle") == 0)
	    {
	      if (reco != -10.0 and gen != -10.0)
		{
		  const unsigned char bind(_th2 -> GetXaxis() -> FindBin(TMath::Abs(reco)/TMath::Pi()) - 1);
		  stdev_gen[bind] += TMath::Power((TMath::Abs(reco) - TMath::Abs(gen))/TMath::Pi() , 2.0)* (*weights)[0];  
		  weights_gen[bind] += (* weights)[0];//TMath::Sqrt(TMath::Power(, 2.0) + TMath::Power((* weights)[0], 2.0));
		}
	    }
	  if (string(observable).compare("pvmag") == 0)
	    {
	      if (reco >= 0.0 and reco < gmax and gen >= 0.0 and gen < gmax)
		{
		  const unsigned char bind(_th2 -> GetXaxis() -> FindBin(reco)- 1);
		  stdev_gen[bind] += TMath::Power(reco - gen, 2.0) * (*weights)[0];  
		  weights_gen[bind] += ( * weights)[0];
		  // if (bind > nbins -1)
		  //   {
		  //     printf("bind %u reco %f\n", bind, reco);
		  //     getchar();
		  //   }
		}
	      _th2 -> Fill(reco, gen, (*weights)[0]);
	}
	}
      for (unsigned char bind = 1; bind <= nbins; bind ++)
	{
	  stdev_gen[bind -1 ] /= weights_gen[bind -1];
	  stdev_gen[bind-1] = TMath::Sqrt(stdev_gen[bind-1]);
	}
      printstddev(stdev_gen, nbins);
      //  getchar();
      TH1 * stabilityh = stabilityunsplit(_th2);
      TH1 * purityh = purityunsplit(_th2);
      printstabandpur(stabilityh, purityh, depth);

      bool stretch(true);
      if (depth == 0 or depth == size  or depth > 0 and stabilityh -> GetBinContent(depth) > 0.5 and purityh -> GetBinContent(depth) > 0.5)
	{
	  stretch = false;
	}
      binned = true;
      if (depth == size -1 )
	{
	  for (unsigned char bind = 1; bind <= stabilityh -> GetNbinsX() and binned; bind ++)
	    {
	      if (stabilityh -> GetBinContent(bind) <= 0.5 and purityh -> GetBinContent(bind) <= 0.5)
		binned = false;
 	    }
	}
      else
	binned = false;
      delete stabilityh;
      delete purityh;
      if (not binned)
	bins = optimisev2(_th2, stdev_gen, size, 0.5, bins, depth, stretch);
      else
	printf("binning done\n");
      delete _th2;
      //getchar();
    }
  printbins(bins, size, depth);

  printf("unsplit bins\n");
  for (unsigned char bind = 0; bind < size; bind ++)
    printf("%u %f\n", bind, bins[bind]);
  writetofile(bins, size, file);
  // printf("split bins \n");
  // float * bins = splitbins(binsunsplit, size);
  // for (unsigned char bind = 0; bind < 2 * size - 1; bind ++)
  //   printf("%u %f\n", bind, bins[bind]);

  // TCanvas * c3 = new TCanvas;
  // _th2 -> RebinX(2);
  // TH1F * stabilityh((TH1F*) stability(_th2));
  // stabilityh -> SetLineColor(kRed);
  // TH1F * purityh((TH1F*) purity(_th2));
  // purityh -> SetLineColor(kBlue);
  // THStack * stack = new THStack;
  // stack -> Add(stabilityh, "HIST");
  // stack -> Add(purityh, "HIST");
  // stack -> Draw("nostack");
  // TH2F * th2opt = new TH2F("th2opt", "th2opt", 2 * size - 1, _th2 -> GetXaxis() -> GetXmin(), _th2 -> GetXaxis() -> GetXmax(), 2 * size - 1, _th2 -> GetYaxis() -> GetXmin(), _th2 -> GetYaxis() -> GetXmax()); 
  // for (unsigned long event_ind = 0; event_ind < chain.GetEntries()/div; event_ind ++)
  //   {
  //     if (event_ind % 10000 == 0)
  //       printf("%u\r", event_ind);
  //     chain.GetEntry(event_ind);
  //     if (string(observable).compare("pull_angle") == 0)
  // 	{
  // 	  th2opt -> Fill(reco == -10.0 ? reco : TMath::Abs(reco)/TMath::Pi(), gen == -10.0 ? gen : TMath::Abs(gen)/TMath::Pi(), (*weights)[0]);
  // 	}
  //     if (string(observable).compare("pvmag") == 0)
  // 	{
  // 	  th2opt -> Fill(reco,  gen, (*weights)[0]);  
  // 	}
  //     // if (reco != -10.0 and gen != -10.0)
  //     // 	{
  //     // 	  const unsigned char bind(_th2 -> GetXaxis() -> FindBin(TMath::Abs(reco)) - 1);
  //     // 	  stdev_gen[bind] = TMath::Sqrt(TMath::Power(stdev_gen[bind], 2.0) + TMath::Power((TMath::Abs(reco) - TMath::Abs(gen)) * (*weights)[0], 2.0));  
  //     // 	  weights_gen[bind] = TMath::Sqrt(TMath::Power(weights_gen[bind], 2.0) + TMath::Power((* weights)[0], 2.0));
  //     // 	}
  //   }
  // th2opt -> RebinX(2);
  // TCanvas  * c4 = new TCanvas;
  // th2opt -> Draw("COLZ");
  // TH1F * stabilityhopt((TH1F*) stability(th2opt));
  // stabilityhopt -> SetLineColor(kRed);
  // TH1F * purityhopt((TH1F*) purity(th2opt));
  // purityhopt -> SetLineColor(kBlue);
  // THStack * stackopt = new THStack;
  // stackopt -> Add(stabilityhopt, "HIST");
  // stackopt -> Add(purityhopt, "HIST");
  // stackopt -> Draw("nostack");
  // TCanvas * c4opt = new TCanvas;
  // stackopt -> Draw("nostack");

  // app.Run();
  // app.Terminate(kTRUE);
}

void writetofile(float *bins, unsigned long size, FILE * file)
{
  fprintf(file, "%lu\n", size);
  for (unsigned char bind = 0; bind < size; bind ++)
    {
      fprintf(file, "%f\n", bins[bind]); 
    }
}

float * splitbins(float *bins, unsigned long size)
{
  float *xbins = new float[2*size - 1];
  for (unsigned char bind = 0; bind < size - 1; bind ++)
    {
      xbins[2*bind] = bins[bind];
      xbins[2*bind + 1] = (bins[bind + 1] - bins[bind])*0.5;
    }
  xbins[2 * size - 2] = bins[size - 1];
  return xbins;
}

TH1 * stabilityunsplit(TH2 * h2)
{
  const unsigned char nbins(h2 -> GetNbinsX());
  TH1F * stability = new TH1F(TString(h2 -> GetName()) + "stability", TString(h2 -> GetName()) + "stability", nbins, h2 -> GetYaxis() -> GetXmin(), h2 -> GetYaxis() -> GetXmax());
  for (unsigned char bind = 1; bind <= nbins; bind ++)
    {
      const double content(h2 -> GetBinContent(bind, bind)/h2 -> Integral(1, nbins, bind, bind));
      stability -> SetBinContent(bind, content);
      
    }
  return stability;
}

TH1 * purityunsplit(TH2 * h2)
{
  const unsigned char nbins(h2 -> GetNbinsX());
  TH1F * purity = new TH1F(TString(h2 -> GetName()) + "_purity", TString(h2 -> GetName()) + "_purity", nbins, h2 -> GetYaxis() -> GetXmin(), h2 -> GetYaxis() -> GetXmax());
  for (unsigned char bind = 1; bind <= nbins; bind ++)
    {
      const double content(h2 -> GetBinContent(bind, bind)/h2 -> Integral(bind, bind, 1, nbins)); 
      purity -> SetBinContent(bind, content);
    }
  return purity;
}



TH1 * stability(TH2 * h2)
{
  const unsigned char nbins(h2 -> GetNbinsX());
  TH1F * stability = new TH1F(TString(h2 -> GetName()) + "stability", TString(h2 -> GetName()) + "stability", nbins, h2 -> GetYaxis() -> GetXmin(), h2 -> GetYaxis() -> GetXmax());
  for (unsigned char bind = 1; bind <= nbins; bind ++)
    {
      const double content(h2 -> Integral(bind, bind, 2 * bind - 1, 2 * bind)/h2 -> Integral(1, nbins, 2*bind - 1, 2 * bind));
      stability -> SetBinContent(bind, content);
      printf("stability bind %u content %f\n", bind, content);
      
    }
  return stability;
}

TH1 * purity(TH2 * h2)
{
  const unsigned char nbins(h2 -> GetNbinsX());
  TH1F * purity = new TH1F(TString(h2 -> GetName()) + "_purity", TString(h2 -> GetName()) + "_purity", nbins, h2 -> GetYaxis() -> GetXmin(), h2 -> GetYaxis() -> GetXmax());
  for (unsigned char bind = 1; bind <= nbins; bind ++)
    {
      const double content(h2 -> Integral(bind, bind, 2 * bind - 1, 2 * bind)/h2 -> Integral(bind, bind, 1, 2 * nbins)); 
      purity -> SetBinContent(bind, content);
      printf("purity bind %u content %f\n", bind, content); 
    }
  return purity;
}

 
float * optimisev2(TH2 * h2, double * sigmas, unsigned long & size, float factor, float * binsi, unsigned char & depth, bool stretch)
{
  if (stretch)
    {
      if (binsi[depth] != gmax)
	{
	  printf("stretching %u  %f \n", depth, binsi[depth]);
	  binsi[depth] *= 1.02;
	  printf("after stretch %f %f\n", binsi[depth], gmax);
	  if (binsi[depth] <=gmax )
	    {
	      printf("probe A\n");
	      unsigned char nbinsleft = TMath::Ceil((gmax - binsi[depth])/gstep);
	      float step = (gmax - binsi[depth])/nbinsleft;
	      unsigned char nbins = depth + nbinsleft;
	      size = nbins + 1;
	      float * bins = new float[size];
	      for (unsigned char bind = 0; bind <= depth; bind ++)
		{
		  bins[bind] = binsi[bind];
		}
	      for (unsigned char bind = depth + 1; bind <= nbins; bind ++)
		{
		  bins[bind] = bins[bind - 1] + step;
		}
	      delete [] binsi;
	      return bins;
	    }
	  else
	    {
	      	      printf("probe B\n");
	      size = depth + 1;
	      float *bins = new float[size];
	      for (unsigned char bind = 0; bind < size - 1; bind ++)
		{
		  bins[bind] = binsi[bind];
		}
	      bins[size - 1] = gmax;
	      delete [] binsi;
	      return bins;
	    }
	  
	}
      else
	{
	  printf("probe C\n");
	  depth --;
	  size = depth + 1;
	  float * bins = new float[size];
	  for (unsigned char bind = 0; bind < size - 1; bind ++)
	    {
	      bins[bind] = binsi[bind];
	    }
	  bins[size - 1] = gmax;
	  delete [] binsi;
	  return bins;
	}
    }
  printf("probe D\n");
  const unsigned char nbins(h2 -> GetNbinsX());
  const TAxis * xaxis = h2 -> GetXaxis(); 
  vector<float> xbins;
  for (unsigned char bind = 0; bind <= depth; bind ++)
    {
      xbins.push_back(binsi[bind]);
    }
  float newedge = binsi[depth] + factor*sigmas[depth];
  if (newedge > gmax)
    newedge = gmax;
  xbins.push_back(newedge);
  depth ++;
  float nbinsleft = TMath::Ceil((gmax - newedge)/gstep);
  if (nbinsleft != 0)
    {
      float step = (gmax - newedge)/nbinsleft;
      unsigned char i = 0;
      while (i < nbinsleft)
	{
	  float newedge = xbins.back() + step;
	  printf("newedge %f\n", newedge);
	  xbins.push_back(newedge);
	  i++;
	}
    }
  delete [] binsi;
  size = xbins.size();
  float * rbins = new float[size];
  for (unsigned char bind = 0; bind < size; bind ++)
    rbins[bind] = xbins.at(bind);
  return rbins;
}

void printstabandpur(TH1 * stab, TH1 * pur, unsigned char depth)
{
  printf("\t stab \t pur\n");
  for (unsigned char bind = 1; bind <= stab -> GetNbinsX(); bind ++)
    {
      if (bind - 1 == depth)
	printf("-------------------\n");
      printf("%u \t %f \t %f\n", bind, stab -> GetBinContent(bind), pur -> GetBinContent(bind)); 
    }
  printf("end printing stab and pur \n\n");
}

void printbins(float *bins, unsigned long size, unsigned char depth)
{
  printf("printing bins; size %lu depth %u\n", size, depth);
  for (unsigned char bind = 0; bind < size; bind ++)
    {
      printf("%u %f\n", bind, bins[bind]);
      if (bind == depth)
	printf("--------------------\n");
    }
  printf("end printing bins\n\n");
}

void printstddev(double * stddev, unsigned char nbins)
{
  printf("printing stddev\n");
  for (unsigned char bind = 0; bind < nbins; bind ++)
    printf("%u \t %f\n", bind, stddev[bind]);
  printf("end printing stddev\n\n");
}
