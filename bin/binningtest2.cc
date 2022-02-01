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
#include "WeightRecord.hh"
float * optimisev2(TH2 * h2, double * sigmas, unsigned long & size, float factor, float * binsi, unsigned char & depth, bool stretch);
TH1 * stability(TH2 * h2);
TH1 * purity(TH2 * h2);
TH1 * stabilityunsplit(TH2 * h2);
TH1 * purityunsplit(TH2 * h2);
float * splitbins(float *bins, unsigned long size);
void writetofile(float *bins, unsigned long size, FILE * file);
float gstep;
float gmax;
void printstats(TH1 * stab, TH1 * pur, TH2 *, double * stdev, unsigned char depth);
void printbins(float *bins, unsigned long size, unsigned char depth);
void printstddev(double * stddev, unsigned char nbins);
const char * jettags[]              = {"leading_jet", "scnd_leading_jet", "leading_b", "scnd_leading_b"};
const char * chargetags[]           = {"allconst", "chconst"};
float ceiling = 0.0;
using namespace std;
unsigned char target_nbins = 0;

int main(int argc, char * argv[])
{
  
  assert(argc > 3);
  
  const string jet(jettags[stoi(argv[1])]);
  const string charge(chargetags[stoi(argv[2])]);
  const string observable(argv[3]);
  unsigned long size;
  float factor;
  if (observable == "pull_angle")
    {
      if (stoi(argv[2]) == 0)
	{
	  ceiling = 0.5;
	  target_nbins = 3;
	}
      if (stoi(argv[2]) == 1)
	{
	  ceiling = 0.585;
	  target_nbins = 4;
	}
      size = 21;
      factor = 0.05;
    }
  if (observable == "pvmag") 
    {
      if (stoi(argv[2]) == 0)
	{
	  ceiling = 0.5;
	  target_nbins = 3;
	}
      if (stoi(argv[2]) == 1)
	{
	  ceiling = 0.595;
	  target_nbins = 4;
	}
      size = 21;
      factor = 0.05;//0.05;
    }
  if(observable == "pvmag_par")
    {
      ceiling = 0.58;
      if (stoi(argv[2]) == 0)
	{
	  ceiling = 0.5;
	}
      target_nbins = 3;
      size = 21;
      factor = 0.05;//0.05;
    }
  float novalue_proxy(0.0);
  if (observable == "pull_angle")
    {
      novalue_proxy = -10.0;
    }
  if (observable == "pvmag"  or observable == "pvmag_par")
    {
      novalue_proxy = - 1.0;
    }

  const TString job_tag = TString(charge) + "_" + jet;
  printf ("jet %s %s\n", jet.c_str(), job_tag.Data());
  // system("mkdir -p binningresults");
  // exit(1);
  FILE * file = fopen(TString("output") + "/bins_" + job_tag + ".txt", "w");
  //  TApplication app("myapp", 0, 0);
  TChain chain("migration");
  chain.Add("root://eosuser.cern.ch//$EOS/migration_MC13TeV_TTJets.root/migration");
  Float_t reco;
  Float_t gen;
  Float_t E_reco, M_reco;
  Float_t E_gen, M_gen;
  WeightMap * wmap = nullptr;
  //      const char * tag_jet_types_temp[4] = {"first_leading_jet", "scnd_leading_jet", "leading_b", "snd_leading_b"};                                                      
  chain.SetBranchAddress(TString("E") + "_" + observable + "_" + jet + "_" + charge + "_" + "reco", & E_reco);
  chain.SetBranchAddress(TString("M") + "_" + observable + "_" + jet + "_" + charge + "_" + "reco", & M_reco);
  chain.SetBranchAddress(TString("E") + "_" + observable + "_" + jet + "_" + charge + "_" + "gen",  & E_gen);
  chain.SetBranchAddress(TString("M") + "_" + observable + "_" + jet + "_" + charge + "_" + "gen",  & M_gen);
  chain.SetBranchAddress("weight", & wmap);


  unsigned long coinc = 0;
  //  printf("GetTag() %s %s %s\n", GetTag(), string(GetTag()).c_str(), string(GetTag()).compare("MC13TeV_TTJets") == 0? "True" : " False");
  if (observable == "pull_angle" )
    {
      gmax = 1.0;//TMath::Pi();
    }
  if (observable == "pvmag")
    {
      gmax = 0.015;
    }
  if (observable == "pvmag_par")
    {
      gmax = 0.005;
    }
  bool binned(false);
  unsigned char depth = 0;
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
      //      printbins(bins, size, depth);
      unsigned char nbins = size - 1;
      TH2 * _th2 = new TH2F("m", "m", nbins, bins, nbins, bins);
      // printf("projy\n");
      // TH1 * projy = _th2 -> ProjectionY();
      // for (unsigned char bind = 1; bind <= projy -> GetNbinsX(); bind ++)
      // 	printf("%u %f %f\n", projy -> GetXaxis() -> GetBinLowEdge(bind), projy -> GetXaxis() -> GetBinUpEdge(bind));
      // delete projy;
      const unsigned char div(1);
      double stdev_gen[nbins];
      double weights_gen[nbins];
      for (unsigned char bind = 0; bind < nbins; bind ++)
	{
	  stdev_gen[bind] = 0.0;
	  weights_gen[bind] = 0.0;
	}
      float reco_prev = 1.0; 
      float gen_prev = 1.0;
      unsigned long nevents = 0;
      printf("nentries %lu\n", chain.GetEntries());
      for (unsigned long event_ind = 0; event_ind < chain.GetEntries()/div; event_ind ++)
	{
	  
	  if (event_ind % 100000 == 0)
	    {
	      printf("event ind %u\n", event_ind);
	    }
	  chain.GetEntry(event_ind);
	  //	  printf("event_ind %lu\n", event_ind);
	  if (E_reco != novalue_proxy and M_reco != novalue_proxy)
	    {
	      fprintf(stderr, "Values set in both E and M reco channels\n");
	      throw "Values set in both E and M reco channels\n";
	    }
	  if (E_gen != novalue_proxy and M_gen != novalue_proxy)
	    {
	      fprintf(stderr, "Values set in both E and M gen channels\n");
	      throw "Values set in both E and M gen channels\n";
	    }
	  if (E_reco == novalue_proxy and M_reco == novalue_proxy)
	    {
	      reco = novalue_proxy;
	    }
	  if (E_reco != novalue_proxy)
	    {
	      reco = E_reco;
	    }
	  else
	    {
	      reco = M_reco;
	    }

	  if (E_gen == novalue_proxy and M_gen == novalue_proxy)
	    {
	      gen = novalue_proxy;
	    }
	  if (E_gen != novalue_proxy)
	    {
	      gen = E_gen;
	    }
	  else
	    {
	      gen = M_gen;
	    }
	  if (reco == novalue_proxy and gen == novalue_proxy)
	    {
	      continue;
	    }
	  nevents ++;
	  double weight0;
	  if (reco == novalue_proxy)
	    {
	      weight0 = wmap -> GetWeight0(WorkEnum_t::GEN);
	    }
	  else
	    {
	      weight0 = wmap -> GetWeight0(WorkEnum_t::RECO);
	    }
	  if (fabs(weight0) > 10000.0)
	    {
	      printf("weight > 10 000.0");
	      continue;
	    }
	  
	  // if (string(h -> GetTag()).compare("MC13TeV_TTJets_tracking_up") == 0)
	  //   {
	  //     printf("%lu %f %f\n", event_ind, reco, gen);
	  //     //     if (reco != -10 and (reco < - ;
	  //   }
	  // if (string(observable).compare("pvmag") == 0)
	  //   {
	  //     h -> GetTH2() -> Fill(reco, gen, weight0);
	  //   }



	  if (observable == "pull_angle")
	    {
	      _th2  -> Fill(reco == novalue_proxy ? reco : TMath::Abs(reco)/TMath::Pi(), gen == novalue_proxy ? gen : TMath::Abs(gen)/TMath::Pi(), weight0);
	      // _th2 -> Fill(reco == -10.0 ? reco : TMath::Abs(reco)/TMath::Pi(), gen == -10.0 ? gen : TMath::Abs(gen)/TMath::Pi(), (*weights)[0]);
	      if (reco != novalue_proxy and gen != novalue_proxy)
		{
		  const unsigned char bind(_th2 -> GetXaxis() -> FindBin(TMath::Abs(reco)/TMath::Pi()) - 1);
		  stdev_gen[bind] += TMath::Power((TMath::Abs(reco) - TMath::Abs(gen))/TMath::Pi() , 2.0)* weight0;  
		  weights_gen[bind] += weight0;//TMath::Sqrt(TMath::Power(, 2.0) + TMath::Power((* weights)[0], 2.0));
		}
	    }
	  if (observable == "pvmag" or observable == "pvmag_par")
	    {
	      
	      if (reco >= 0.0 and reco < gmax and gen >= 0.0 and gen < gmax)
		{
		  const unsigned char bind(_th2 -> GetXaxis() -> FindBin(reco)- 1);
		  stdev_gen[bind] += TMath::Power(reco - gen, 2.0) * weight0;  
		  weights_gen[bind] +=  weight0;
		  // if (bind > nbins -1)
		  //   {
		  //     printf("bind %u reco %f\n", bind, reco);
		  //     getchar();
		  //   }
		}


	      if (reco > 0.0 and reco< reco_prev)
		{
		  reco_prev = reco;
		}
	      if (gen > 0.0 and gen < gen_prev)
		{
		  gen_prev = gen;
		}
	      _th2 -> Fill(reco, gen, weight0);
	    }
	}
      // printf("reco_prev %f gen_prev %f\n", reco_prev, gen_prev);
      // getchar();
      // printf("nbins %u\n", nbins);
      for (unsigned char bind = 1; bind <= nbins; bind ++)
	{
	  stdev_gen[bind -1 ] /= weights_gen[bind -1];
	  stdev_gen[bind-1] = TMath::Sqrt(stdev_gen[bind-1]);
	}
      //printstddev(stdev_gen, nbins);
      //  getchar();
      TH1 * stabilityh = stabilityunsplit(_th2);
      printf("stabilityh\n");
      for (unsigned char bind = 1; bind <= stabilityh -> GetNbinsX(); bind ++)
      	printf("%u %f %f\n", stabilityh -> GetXaxis() -> GetBinLowEdge(bind), stabilityh -> GetXaxis() -> GetBinUpEdge(bind));
      TH1 * purityh = purityunsplit(_th2);
      printstats(stabilityh, purityh, _th2, stdev_gen, depth);
      //      getchar();

      bool stretch(true);
      if (depth == 0 or depth == size  or depth > 0 and stabilityh -> GetBinContent(depth) > ceiling and purityh -> GetBinContent(depth) > ceiling )
	{
	  stretch = false;
	  // TH1 * projy = _th2 -> ProjectionY("projy");
	  // if (depth > 0 and depth < size and _th2 -> Integral(1, _th2 -> GetNbinsX(), depth, depth)/_th2 -> Integral() + projy -> GetBinError(depth) < 1.0/(target_nbins))
	  //   stretch = true;
	  // if (depth > 0 and depth< size and projy -> GetXaxis() -> GetBinUpEdge(depth) - projy -> GetXaxis() -> GetBinLowEdge(depth) < 0.25 * (projy -> GetXaxis() -> GetXmax() - projy -> GetXaxis() -> GetXmin())/target_nbins)
	  //   {
	  //     printf("evading a too narrow bin\n");
	  //     stretch = true;
	  //   }
	  // delete projy;
	}
      
      binned = true;
      if (depth == size -1 )
	{
	  for (unsigned char bind = 1; bind <= stabilityh -> GetNbinsX() and binned; bind ++)
	    {
	      if (stabilityh -> GetBinContent(bind) <= ceiling and purityh -> GetBinContent(bind) <= ceiling)
		{
		  binned = false;
		}
 	    }
	}
      else
	{
	  binned = false;
	}
      delete stabilityh;
      delete purityh;
      if (not binned)
	{
	  bins = optimisev2(_th2, stdev_gen, size, factor, bins, depth, stretch);
	}
      else
	{
	  printf("binning done\n");
	}
      delete _th2;
      //getchar();
    }
  printbins(bins, size, depth);

  printf("unsplit bins\n");
  for (unsigned char bind = 0; bind < size; bind ++)
    {
      printf("%u %f\n", bind, bins[bind]);
    }
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
float * optimisev2(TH2 * h2, double * sigmas, unsigned long & size, float factor, float * binsi, unsigned char & depth, bool stretch)
{
  if (stretch)
    {
      if (binsi[depth] != gmax)
	{
	  printf("stretching: depth %u  binsi[depth] %f \n", depth, binsi[depth]);
	  binsi[depth] *= 1.02;
	  printf("after stretch binsi[depth] %f gmax %f\n", binsi[depth], gmax);
	  if (binsi[depth] <=gmax )
	    {
	      printf("probe A\n");
	      unsigned char nbinsleft = TMath::Ceil((gmax - binsi[depth])/gstep);
	      float step = (gmax - binsi[depth])/nbinsleft;
	      printf("step %f\n", step);
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
    {
      newedge = gmax;
    }
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
    {
      rbins[bind] = xbins.at(bind);
    }
  return rbins;
}

void printstats(TH1 * stab, TH1 * pur, TH2* h2, double *stdev, unsigned char depth)
{
  printf("  : \t low - up | stab \t pur \t content (threshold %f) \t stdev\n", 1.0/(target_nbins));
  for (unsigned char bind = 1; bind <= stab -> GetNbinsX(); bind ++)
    {
      if (bind - 1 == depth)
	{
	  printf("-------------------\n");
	}
      printf("%u : \t %f - %f | %f \t %f \t %f \t%f\n", 
	     bind, 
	     h2 -> GetYaxis() -> GetBinLowEdge(bind),
	     h2 -> GetYaxis() -> GetBinUpEdge(bind),
	     stab -> GetBinContent(bind), 
	     pur -> GetBinContent(bind), 
	     h2 -> Integral(1, h2 -> GetNbinsX(), bind, bind)/h2 -> Integral(1, h2 -> GetNbinsX(), 1, h2 -> GetNbinsY()),
	     stdev[bind - 1]); 
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
	{
	  printf("--------------------\n");
	}
    }
  printf("end printing bins\n\n");
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
  TH1 * stability = h2 -> ProjectionY("stability");//new TH1F(TString(h2 -> GetName()) + "stability", TString(h2 -> GetName()) + "stability", nbins, h2 -> GetYaxis() -> GetXmin(), h2 -> GetYaxis() -> GetXmax());
  
  stability -> Reset("ICE");
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
  TH1 * purity = h2 -> ProjectionY("purity");//new TH1F(TString(h2 -> GetName()) + "_purity", TString(h2 -> GetName()) + "_purity", nbins, h2 -> GetYaxis() -> GetXmin(), h2 -> GetYaxis() -> GetXmax());
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
  TH1 * stability = new TH1F(TString(h2 -> GetName()) + "stability", TString(h2 -> GetName()) + "stability", nbins, h2 -> GetYaxis() -> GetXmin(), h2 -> GetYaxis() -> GetXmax());
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
  TH1 * purity = new TH1F(TString(h2 -> GetName()) + "_purity", TString(h2 -> GetName()) + "_purity", nbins, h2 -> GetYaxis() -> GetXmin(), h2 -> GetYaxis() -> GetXmax());
  for (unsigned char bind = 1; bind <= nbins; bind ++)
    {
      const double content(h2 -> Integral(bind, bind, 2 * bind - 1, 2 * bind)/h2 -> Integral(bind, bind, 1, 2 * nbins)); 
      purity -> SetBinContent(bind, content);
      printf("purity bind %u content %f\n", bind, content); 
    }
  return purity;
}

 

void printstddev(double * stddev, unsigned char nbins)
{
  printf("printing stddev\n");
  for (unsigned char bind = 0; bind < nbins; bind ++)
    printf("%u \t %f\n", bind, stddev[bind]);
  printf("end printing stddev\n\n");
}
