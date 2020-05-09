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
float * optimise(TH2 * h2, unsigned long & size, float factor);
float * optimisev2(TH2 * h2, double * sigmas, unsigned long & size, float factor);
TH1 * stability(TH2 * h2);
TH1 * purity(TH2 * h2);
float * splitbins(float *bins, unsigned long size);
void writetofile(float *bins, unsigned long size, FILE * file);
using namespace std;
int main(int argc, char * argv[])
{
  assert(argc > 1);
  const string observable(argv[1]);
  TApplication app("myapp", 0, 0);
  TChain chain("migration");
  chain.Add("$EOS/analysis_MC13TeV_TTJets/HADDmigration/migration_MC13TeV_TTJets.root/E_allconst_leading_jet_migration");
  chain.Add("$EOS/analysis_MC13TeV_TTJets/HADDmigration/migration_MC13TeV_TTJets.root/M_allconst_leading_jet_migration");
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
  const unsigned char nbins(20);
  float max;
  if (observable.compare("pull_angle") == 0 )
    max = TMath::Pi();
  if (observable.compare("pvmag") == 0 )
    max = 0.015;
  TH2F * _th2 = new TH2F("m", "m", nbins, 0.0, max, nbins, 0.0, max);
  const unsigned char div(10);
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
      printf("size %lu weight %.12f\n", weights -> size(), (*weights)[0] * (*weights)[18]);
      _th2 -> Fill(reco == -10.0 ? reco : TMath::Abs(reco), gen == -10.0 ? gen : TMath::Abs(gen), (*weights)[0]);
      if (string(observable).compare("pull_angle") == 0)
	{
	  if (reco != -10.0 and gen != -10.0)
	    {
	      const unsigned char bind(_th2 -> GetXaxis() -> FindBin(TMath::Abs(reco)) - 1);
	      stdev_gen[bind] = TMath::Sqrt(TMath::Power(stdev_gen[bind], 2.0) + TMath::Power((TMath::Abs(reco) - TMath::Abs(gen)) * (*weights)[0] * (*weights)[18], 2.0));  
	      weights_gen[bind] = TMath::Sqrt(TMath::Power(weights_gen[bind], 2.0) + TMath::Power((* weights)[0], 2.0));
	    }
	}
      if (string(observable).compare("pvmag") == 0)
	{
	  if (reco >= 0.0 and reco < max and gen >= 0.0 and gen < max)
	    {
	      const unsigned char bind(_th2 -> GetXaxis() -> FindBin(reco)- 1);
	      stdev_gen[bind] = TMath::Sqrt(TMath::Power(stdev_gen[bind], 2.0) + TMath::Power((reco - gen) * (*weights)[0], 2.0));  
	      weights_gen[bind] = TMath::Sqrt(TMath::Power(weights_gen[bind], 2.0) + TMath::Power((* weights)[0], 2.0));
	      if (bind > nbins -1)
		{
		  printf("bind %u reco %f\n", bind, reco);
		  getchar();
		}
	    }
	  _th2 -> Fill(reco, gen, (*weights)[0]);
	}
    }
  for (unsigned char bind = 1; bind <= nbins; bind ++)
    {
      stdev_gen[bind -1 ] /= weights_gen[bind -1];
      printf("bind %u %.9f\n", bind, stdev_gen[bind - 1]);
    }
  
  TCanvas * c = new TCanvas;
  _th2 -> Draw("COLZ");

  TCanvas * c3 = new TCanvas;
  _th2 -> RebinX(2);
  TH1F * stabilityh((TH1F*) stability(_th2));
  stabilityh -> SetLineColor(kRed);
  TH1F * purityh((TH1F*) purity(_th2));
  purityh -> SetLineColor(kBlue);
  THStack * stack = new THStack;
  stack -> Add(stabilityh, "HIST");
  stack -> Add(purityh, "HIST");
  stack -> Draw("nostack");

  app.Run();
  app.Terminate(kTRUE);
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


float * optimisev2(TH2 * h2, double * sigmas, unsigned long & size, float factor)
{
  const unsigned char nbins(h2 -> GetNbinsX());
  const TAxis * xaxis = h2 -> GetXaxis(); 
  vector<float> xbins;
  float edge(xaxis -> GetBinLowEdge(1));
  float binfraction(0.0);
  xbins.push_back(edge);
  while (edge <= xaxis -> GetBinUpEdge(nbins))
    {
      printf("binfraction %f edge before %f\n", binfraction, edge);
      if (binfraction > 0.25)
	{
	  printf("pushing back %f\n", edge);
	  xbins.push_back(edge);
	  binfraction = 0.0;
	}
      
      const unsigned char bind1 = xaxis -> FindBin(edge);
      printf("factor %f bind 1 %u sigmas[bind1 - 1] %f\n", factor, bind1, sigmas[bind1 -1]);
      edge += factor*sigmas[bind1 - 1];
      
      const unsigned char bind2 = xaxis -> FindBin(edge);
      binfraction += h2 -> Integral(bind1, bind2, 1, h2 -> GetNbinsY())/h2 -> Integral();
      printf("edge after %f bind1 %u bind2 %u\n", edge, bind1, bind2);
      //      getchar();
    }
  if ( edge >= xaxis -> GetBinUpEdge(nbins) and xbins.back() != edge)
    {
      if (binfraction < 0.25)
	{
	  printf("changing end\n");
	  xbins.back() = xaxis -> GetBinUpEdge(nbins);
	}
      else
	{
	  printf("pushing back %f\n", xaxis -> GetBinUpEdge(nbins));
	  xbins.push_back(xaxis -> GetBinUpEdge(nbins));
	}
      
    }
  size = xbins.size();
  float * rbins = new float[size];
  for (unsigned char bind = 0; bind < size; bind ++)
    rbins[bind] = xbins.at(bind);
  return rbins;
}

float * optimise(TH2 * h2, unsigned long & size, float factor)
{
  TH2 & h = *h2;
  TAxis * yaxis = h.GetYaxis();
  TAxis * xaxis = h.GetXaxis();
  const char * _folder = "binningplots/"; 

  TF1 f1 = TF1("f1", "gaus", yaxis -> GetXmin(), yaxis -> GetXmax());
  f1.SetParameters(100, (yaxis -> GetXmax() - yaxis -> GetXmin())/2.0, 0.01);
  TObjArray slices;
  h.FitSlicesY(& f1, 1, h.GetNbinsX(), 0, "QNRLM", &slices);

  TCanvas c2("c2", "c2");
  ((TH1D*)slices[0]) -> Draw();
  c2.Print((string(_folder) + "/slices_N.png").c_str());
  ((TH1D*)slices[1]) -> GetYaxis() -> SetRangeUser(yaxis -> GetXmin(), yaxis -> GetXmax());
  ((TH1D*)slices[1]) -> Draw();
  c2.Print((string(_folder) + "/slices_mean.png").c_str());
  ((TH1D*)slices[2]) -> GetYaxis() -> SetRangeUser(0.0, yaxis -> GetXmax());
  ((TH1D*)slices[2]) -> Draw();
  c2.Print((string(_folder) + "/slices_sigma.png").c_str());

  vector<float> bins;
  bins.push_back(xaxis -> GetXmin());
  vector<float> exact;
  exact.push_back(xaxis -> GetXmin());
  const float integral = h.Integral();
  float binfraction = 0.0;
  for (unsigned char ind = 0; ind < h.GetNbinsY() + 1; ind ++)
    {
      TH1F * proj = (TH1F*) h.ProjectionY("py", ind, ind);
      binfraction += proj -> Integral()/integral;
      delete proj;
      const float mean      = ((TH1D *)slices[1]) -> GetBinContent(ind);
      const float meanError = ((TH1D *)slices[1]) -> GetBinError(ind);
      printf("mean %f meanError %f meanError/mean %f\n", mean, meanError, meanError/mean);
      if (mean == 0 or meanError/mean > 0.25)
        continue;
      printf("ind %u sigma %f\n", ind, ((TH1D *) slices[2]) -> GetBinContent(ind));
      const float sigma = ((TH1D *) slices[2]) -> GetBinContent(ind) * factor;
      if (mean - sigma > exact.back())
	{
	  if (mean + sigma < xaxis -> GetXmax())
	    {
	      exact.push_back(mean + sigma);
	      const float newbinedge = xaxis -> GetBinUpEdge(xaxis -> FindBin(mean + sigma));
	      if (newbinedge > bins.back() and binfraction > 0.01)
		{
		  bins.push_back(newbinedge);
		  binfraction = 0.0;
		}
	    }
	}
    }
  if (bins.size() > 1)
    {
      bins.back() = xaxis -> GetXmax();
    }
  size = bins.size();
  float * fbins = new float[size];
  for (unsigned char ind = 0; ind < size; ind ++)
    {
      fbins[ind] = bins[ind];
    }
  return fbins;

  
}
