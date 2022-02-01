#include "CompoundHistoUnfolding.hh"
#include "Utilities.hh"
#include "TFile.h"
#include <stdio.h>
#include "TApplication.h"
#include "TCanvas.h"
#include "TColor.h"
#include "TH1F.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TSystem.h"
#include "TKey.h"
#include "TStyle.h"
#include <assert.h>
#include "TLine.h"
#include "TPaveText.h"
#include "TText.h"
unsigned char getbinnumber(float obs, unsigned char nbins, float * bins);
float transformval(float val, const char *observable );
int main(int argc, char * argv[])
{
  gStyle -> SetOptStat(0);
  gStyle -> SetOptTitle(0);
  const char * levels[2] = {"reco", "gen"};
  const unsigned char Nobs = 4;
  const unsigned char Nlevels = 2;
  const char * observables[Nobs] = {"pull_angle", "pull_angle", "pull_angle", "pvmag"};
  const char * jets[Nobs]        = {"leading_jet", "scnd_leading_jet", "leading_b", "leading_jet"};
  unsigned char jetindices[Nobs] = {LEADING_JET, SCND_LEADING_JET, LEADING_B, LEADING_JET};
  const char * titles[Nobs]      = {"#theta_{p}(j_{1}^{W}, j_{2}^{W})", "#theta_{p}(j_{2}^{W}, j_{1}^{W})", "#theta_{p}(j_{1}^{b}, j_{2}^{b})", "|#vec{P}(j_{1}^{W}, j_{2}^{W})|"};
  unsigned char sizes[Nobs];
  unsigned char multiplier[Nobs];
  float * ybins[Nobs];
  assert(argc == 2);
  const char * bmethod = argv[1];
  unsigned char nbins = 0;
  for (unsigned char obsind = 0; obsind < Nobs; obsind ++)
    {
      const TString binfilename = 
	TString("/eos/user/v/vveckaln/unfolding_nominal/") + 
	observables[obsind] + "/" + 
	bmethod + "/" + 
	jets[obsind] + "_" + 
	"chconst" + "_" + 
	"MC13TeV_TTJets" + "_" 
	+ observables[obsind] + "_" + 
	"OPT" + "_" + 
	"nominal" + "_" + 
	bmethod + "/binfile.txt";
      FILE * binfile = fopen(binfilename.Data(), "r");
      printf("%p, %s\n", binfile, binfilename.Data());
      fscanf(binfile, "%lu\n", & sizes[obsind]);
      // system((string("cat ") + binfilename.Data()).c_str());
      ybins[obsind] = new float[sizes[obsind] +1];
      multiplier[obsind] = nbins;
      nbins += sizes[obsind];
      int r = 0;
      unsigned char ind = 0;
      while (r >= 0)
	{
	  float store;
	  r = fscanf(binfile, "%f\n", & store);
	  if (r >= 0)
	    {
	      if (ind > sizes[obsind])
		throw "";
	      ybins[obsind][ind] = store;
	    }
	  ind ++;
	}

      fclose(binfile);
    }
  for (unsigned char obsind = 0; obsind < Nobs; obsind ++)
    {
      printf("sizes %lu\n", sizes[obsind]);
      for (unsigned char bind = 0; bind <= sizes[obsind]; bind ++)
	{

	  printf("%u %f\n", bind, ybins[obsind][bind]);
	}
      printf("-----\n");
    }
  getchar();
  TH2 * h2[Nlevels];
  TCanvas * canvas[Nlevels];
  for (unsigned char lind = 0; lind < 2; lind ++)
    {
      h2[lind] = new TH2F((TString("h_") + levels[lind]).Data(), levels[lind], nbins, 0.0, nbins, nbins, 0.0, nbins);
      h2[lind] -> GetXaxis() -> SetLabelSize(0.0);
      h2[lind] -> GetYaxis() -> SetLabelSize(0.0);
      h2[lind] -> GetYaxis() -> SetTitleOffset(0.4);
      h2[lind] -> GetXaxis() -> SetTickLength(0.0);
      h2[lind] -> GetYaxis() -> SetTickLength(0.0);
      h2[lind] -> GetXaxis() -> SetTitleOffset(0.4);
      canvas[lind] = new TCanvas((TString("corrmatrix_") + levels[lind]).Data(), (TString("corrmatrix_") + levels[lind]).Data());
      h2[lind] -> Draw("COLZ");
      unsigned char lticky = 0.0;
      unsigned char sticky = 0.0;
      unsigned char ltickx = 0.0;
      unsigned char stickx = 0.0;
      TLine * lline0y = new TLine(- 1.0, 0.0, 0.0, 0.0);
      lline0y -> SetLineColor(kRed);
      lline0y -> Draw();
      TLine * lline0x = new TLine(0.0, -1.0, 0.0, 0.0);
      lline0x -> SetLineColor(kRed);
      lline0x -> Draw();

      for (unsigned char obsind = 0; obsind < Nobs; obsind ++)
	{
	  TPaveText * labely = new TPaveText(-2.1, lticky, -1.0, lticky + sizes[obsind]);
	  labely -> AddText(titles[obsind]);
	  labely -> SetBorderSize(0.0);
	  labely -> SetFillStyle(0);
	  labely -> Draw();
	  TText *ltexty = labely -> GetLineWith(titles[obsind]);
	  ltexty -> SetTextAngle(90);
	  ltexty -> SetTextSize(0.04);//2.0 * ltext -> GetTextSize());
	  // TText * ttext = new TText(-1.0, lticky + 0.5 * sizes[obsind], titles[obsind]);
	  // ttext -> Draw();
	  // ttext -> SetTextAlign(22);
	  // ttext -> SetTextAngle(90);
	  // ttext -> SetTextSize(0.Nobs * ttext -> GetTextSize());
	  for (unsigned char slind = 0; slind < sizes[obsind]; slind ++)
	    {
	      sticky += 1;
	      TLine * sliney = new TLine(- 0.5, sticky, nbins, sticky);
	      sliney -> SetLineColor(kGreen);
	      sliney -> Draw("SAME");
	    }
	  lticky += sizes[obsind];
      
	  TLine * lliney = new TLine(- 1.0, lticky, nbins, lticky);
	  lliney -> SetLineColor(kRed);
	  lliney -> Draw("SAME");

	  TPaveText * labelx = new TPaveText(ltickx, -2.0, ltickx + sizes[obsind], -1.0);
	  labelx -> AddText(titles[obsind]);
	  labelx -> SetBorderSize(0.0);
	  labelx -> SetFillStyle(0);
	  labelx -> Draw();
	  TText *ltextx = labelx -> GetLineWith(titles[obsind]);
	  //      ltextx -> SetTextAngle(90);
	  ltextx -> SetTextSize(0.04);//2.0 * ltext -> GetTextSize());
	  for (unsigned char slind = 0; slind < sizes[obsind]; slind ++)
	    {
	      stickx += 1;
	      TLine * slinex = new TLine(stickx, - 0.5, stickx, nbins);
	      slinex -> SetLineColor(kGreen);
	      slinex -> Draw("SAME");
	    }
	  ltickx += sizes[obsind];
      
	  TLine * llinex = new TLine(ltickx, -1.0, ltickx, nbins);
	  llinex -> SetLineColor(kRed);
	  llinex -> Draw("SAME");
	}
    }
  //c -> SetLeftMargin(0.2);
  TFile * file = TFile::Open("$EOS/analysis_MC13TeV_TTJets/HADDmigration/migration_MC13TeV_TTJets.root", "READ"); 
  TTree * migration = (TTree*) file -> Get("migration");
  Float_t reco[Nobs];
  Float_t gen[Nobs];
  Float_t E_reco[Nobs], M_reco[Nobs];
  Float_t E_gen[Nobs], M_gen[Nobs];
  double * mean_reco[Nobs], * mean_gen[Nobs];
  float novalue_proxy[Nobs];
  double * wnevents_reco[Nobs], * wnevents_gen[Nobs];
  double * stddev_reco[Nobs], * stddev_gen[Nobs];
  double  cov_reco[nbins][nbins], cov_gen[nbins][nbins];
  for (unsigned char bindx = 0; bindx < nbins; bindx ++)
    {
      for (unsigned char bindy = 0; bindy < nbins; bindy ++)
	{
	  cov_reco[bindx][bindy] = 0.0;
	  cov_gen[bindx][bindy] = 0.0;
	}

    }
  printf("nbins %u \n", nbins);
  //  getchar();
  vector<double> * weights = nullptr;
  for (unsigned char obsind = 0; obsind < Nobs; obsind ++)
    {
      wnevents_reco[obsind] = new double[sizes[obsind]];
      wnevents_gen[obsind]  = new double[sizes[obsind]];
      mean_reco[obsind]     = new double[sizes[obsind]];
      mean_gen[obsind]      = new double[sizes[obsind]];
      stddev_reco[obsind]    = new double[sizes[obsind]];
      stddev_gen[obsind]     = new double[sizes[obsind]];
      for (unsigned char obssubind = 0; obssubind < sizes[obsind]; obssubind ++)
	{
	  wnevents_reco[obsind][obssubind] = 0.0;
	  wnevents_gen[obsind][obssubind]  = 0.0;
	  mean_reco[obsind][obssubind]     = 0.0;
	  mean_gen[obsind][obssubind]      = 0.0;
	  stddev_reco[obsind][obssubind]    = 0.0;
	  stddev_gen[obsind][obssubind]     = 0.0;
	  
	}
      if (TString(observables[obsind]) == "pull_angle")
	{
	  novalue_proxy[obsind] = - 10.0;
	}
      if (TString(observables[obsind]) == "pvmag")
	{
	  novalue_proxy[obsind] = - 1.0;
	}
      migration -> SetBranchAddress(TString("E") + "_" + observables[obsind] + "_" + tag_jet_types_[jetindices[obsind]] + "_" + tag_charge_types_[CHARGED] + "_" + "reco", & E_reco[obsind]);
      migration -> SetBranchAddress(TString("M") + "_" + observables[obsind] + "_" + tag_jet_types_[jetindices[obsind]] + "_" + tag_charge_types_[CHARGED] + "_" + "reco", & M_reco[obsind]);
      migration -> SetBranchAddress(TString("E") + "_" + observables[obsind] + "_" + tag_jet_types_[jetindices[obsind]] + "_" + tag_charge_types_[CHARGED] + "_" + "gen", & E_gen[obsind]);
      migration -> SetBranchAddress(TString("M") + "_" + observables[obsind] + "_" + tag_jet_types_[jetindices[obsind]] + "_" + tag_charge_types_[CHARGED] + "_" + "gen", & M_gen[obsind]);
      
    }
  migration -> SetBranchAddress("weight", & weights);

  const unsigned int dbcut = 5;
  //      h -> FillFromTree(, tag_jet_types_[jetcode], tag_charge_types_[chargecode], observable);                                                                                                      
  for (unsigned long event_ind = 0; event_ind < migration -> GetEntries()/dbcut; event_ind ++)
    {
      if (event_ind % 10000 == 0)
	printf("%u\r", event_ind);
      migration -> GetEntry(event_ind);
      // printf("observable %s novalue_proxy %f\n", observable, novalue_proxy);                                                                                                                         
      // printf("E_reco %f, M_reco %f E_gen %f M_gen %f\n", E_reco, M_reco, E_gen, M_gen);                                                                                                              
      for (unsigned char obsind = 0; obsind < Nobs; obsind ++)
	{
	  if (E_reco[obsind] != novalue_proxy[obsind] and M_reco[obsind] != novalue_proxy[obsind])
	    {
	      fprintf(stderr, "Values set in both E and M reco[obsind] channels\n");
	      throw "Values set in both E and M reco[obsind] channels\n";
	    }
	  if (E_gen[obsind] != novalue_proxy[obsind] and M_gen[obsind] != novalue_proxy[obsind])
	    {
	      fprintf(stderr, "Values set in both E and M gen[obsind] channels\n");
	      throw "Values set in both E and M gen[obsind] channels\n";
	    }
	  //printf("obsind %u %s %f\n", obsind, observables[obsind], novalue_proxy[obsind]);
	  if (E_reco[obsind] == novalue_proxy[obsind] and M_reco[obsind] == novalue_proxy[obsind])
	    reco[obsind] = novalue_proxy[obsind];
	  if (E_reco[obsind] != novalue_proxy[obsind])
	    {
	      reco[obsind] = transformval(E_reco[obsind], observables[obsind]);
	    }
	  if (M_reco[obsind] != novalue_proxy[obsind])
	    {
	      reco[obsind] = transformval(M_reco[obsind], observables[obsind]);
	    }
	  if (E_gen[obsind] == novalue_proxy[obsind] and M_gen[obsind] == novalue_proxy[obsind])
	    gen[obsind] = novalue_proxy[obsind];
	  if (E_gen[obsind] != novalue_proxy[obsind])
	    {
	      gen[obsind] = transformval(E_gen[obsind], observables[obsind]);
	    }
	  if (M_gen[obsind] != novalue_proxy[obsind])
	    {
	      gen[obsind] = transformval(M_gen[obsind], observables[obsind]);
	    }
	  if (reco[obsind] != novalue_proxy[obsind])
	    {
	      const unsigned char bind = getbinnumber(reco[obsind], sizes[obsind], ybins[obsind]);
	      if (bind < sizes[obsind])
		{
		  wnevents_reco[obsind][bind] += (* weights)[0];
		  mean_reco[obsind][bind] += reco[obsind] * (* weights)[0];
		}
	    }
	  if (gen[obsind] != novalue_proxy[obsind])
	    {
	      const unsigned char bind = getbinnumber(gen[obsind], sizes[obsind], ybins[obsind]);
	      if (bind < sizes[obsind])
		{
		  wnevents_gen[obsind][bind] += (* weights)[0];
		  mean_gen[obsind][bind] += gen[obsind] * (* weights)[0];
		}
	    }
	}
      // chain.SetBranchAddress(TString("E") + "_" + observables[obsind] + "_" + tag_jet_types_[jetcode] + "_" + tag_charge_types_[chargecode] + "_" + "gen",  & E_gen);
      // chain.SetBranchAddress(TString("M") + "_" + observables[obsind] + "_" + tag_jet_types_[jetcode] + "_" + tag_charge_types_[chargecode] + "_" + "gen",  & M_gen);
    }
  for (unsigned char obsind = 0; obsind < Nobs; obsind ++)
    {
      for (unsigned char obssubind = 0; obssubind < sizes[obsind]; obssubind ++)
	{
	  mean_reco[obsind][obssubind] = mean_reco[obsind][obssubind]/wnevents_reco[obsind][obssubind];
	  mean_gen[obsind][obssubind] = mean_gen[obsind][obssubind]/wnevents_gen[obsind][obssubind];
	  printf(" %f  %f\n", mean_reco[obsind][obssubind], mean_gen[obsind][obssubind]);
	}
      printf("-----\n");
    }
  for (unsigned long event_ind = 0; event_ind < migration -> GetEntries()/dbcut; event_ind ++)
    {
      if (event_ind % 10000 == 0)
	printf("%u\r", event_ind);
      migration -> GetEntry(event_ind);
      // printf("observable %s novalue_proxy %f\n", observable, novalue_proxy);                                                                                                                         
      // printf("E_reco %f, M_reco %f E_gen %f M_gen %f\n", E_reco, M_reco, E_gen, M_gen);                                                                                                              
      for (unsigned char obsind = 0; obsind < Nobs; obsind ++)
	{
	  if (E_reco[obsind] != novalue_proxy[obsind] and M_reco[obsind] != novalue_proxy[obsind])
	    {
	      fprintf(stderr, "Values set in both E and M reco[obsind] channels\n");
	      throw "Values set in both E and M reco[obsind] channels\n";
	    }
	  if (E_gen[obsind] != novalue_proxy[obsind] and M_gen[obsind] != novalue_proxy[obsind])
	    {
	      fprintf(stderr, "Values set in both E and M gen[obsind] channels\n");
	      throw "Values set in both E and M gen[obsind] channels\n";
	    }
	  //printf("obsind %u %s %f\n", obsind, observables[obsind], novalue_proxy[obsind]);
	  if (E_reco[obsind] == novalue_proxy[obsind] and M_reco[obsind] == novalue_proxy[obsind])
	    reco[obsind] = novalue_proxy[obsind];
	  if (E_reco[obsind] != novalue_proxy[obsind])
	    {
	      reco[obsind] = transformval(E_reco[obsind], observables[obsind]);
	    }
	  if (M_reco[obsind] != novalue_proxy[obsind])
	    {
	      reco[obsind] = transformval(M_reco[obsind], observables[obsind]);
	    }
	  if (E_gen[obsind] == novalue_proxy[obsind] and M_gen[obsind] == novalue_proxy[obsind])
	    gen[obsind] = novalue_proxy[obsind];
	  if (E_gen[obsind] != novalue_proxy[obsind])
	    {
	      gen[obsind] = transformval(E_gen[obsind], observables[obsind]);
	    }
	  if (M_gen[obsind] != novalue_proxy[obsind])
	    {
	      gen[obsind] = transformval(M_gen[obsind], observables[obsind]);
	    }
	  if (reco[obsind] != novalue_proxy[obsind])
	    {
	      const unsigned char bind = getbinnumber(reco[obsind], sizes[obsind], ybins[obsind]);
	      if (bind < sizes[obsind])
		{
		  stddev_reco[obsind][bind] += TMath::Power(reco[obsind] - mean_reco[obsind][bind], 2.0) * (* weights)[0];
		}
	    }
	  if (gen[obsind] != novalue_proxy[obsind])
	    {
	      const unsigned char bind = getbinnumber(gen[obsind], sizes[obsind], ybins[obsind]);
	      if (bind < sizes[obsind])
		{
		  stddev_gen[obsind][bind] += TMath::Power(gen[obsind] - mean_gen[obsind][bind], 2.0) * (* weights)[0];
		}
	    }
	  
	}
      for (unsigned char obsx = 0; obsx < Nobs; obsx ++)
	{
	  if (reco[obsx] == novalue_proxy[obsx])
	    continue;
	  const unsigned char obsxsub = getbinnumber(reco[obsx], sizes[obsx], ybins[obsx]);
	  if (obsxsub == sizes[obsx])
	    continue;
	  for (unsigned char obsy = 0; obsy < Nobs; obsy ++)
	    {
	      if (reco[obsy] == novalue_proxy[obsy])
		continue;
	      const unsigned char obsysub = getbinnumber(reco[obsy], sizes[obsy], ybins[obsy]);
	      if (obsysub == sizes[obsy])
		continue;
	      if (obsxsub + multiplier[obsx] == 0 and obsysub + multiplier[obsy] == 0)
		{
		  // printf("%u %u %u %u %u\n", obsx, obsxsub, obsy, obsysub, multiplier[obsx]);
		  // printf("%f %f %f %f %f %.9f\n", cov_reco[obsxsub + multiplier[obsx]][obsysub + multiplier[obsy]], reco[obsx], reco[obsy], mean_reco[obsx][obsxsub], mean_reco[obsy][obsysub], (*weights)[0]);
		  // printf("%f %.9f\n",  (reco[obsx] - mean_reco[obsx][obsxsub])*(reco[obsy] - mean_reco[obsy][obsysub]) , (reco[obsx] - mean_reco[obsx][obsxsub])*(reco[obsy] - mean_reco[obsy][obsysub])  * (*weights)[0]);
		  // getchar();
		}
	      
	      cov_reco[obsxsub + multiplier[obsx]][obsysub + multiplier[obsy]] += (reco[obsx] - mean_reco[obsx][obsxsub])*(reco[obsy] - mean_reco[obsy][obsysub]) * (*weights)[0];
	    }

	}
      for (unsigned char obsx = 0; obsx < Nobs; obsx ++)
	{
	  if (gen[obsx] == novalue_proxy[obsx])
	    continue;
	  const unsigned char obsxsub = getbinnumber(gen[obsx], sizes[obsx], ybins[obsx]);
	  if (obsxsub == sizes[obsx])
	    continue;
	  for (unsigned char obsy = 0; obsy < Nobs; obsy ++)
	    {
	      if (gen[obsy] == novalue_proxy[obsy])
		continue;
	      const unsigned char obsysub = getbinnumber(gen[obsy], sizes[obsy], ybins[obsy]);
	      if (obsysub == sizes[obsy])
		continue;
	      cov_gen[obsxsub + multiplier[obsx]][obsysub + multiplier[obsy]] += (gen[obsx] - mean_gen[obsx][obsxsub])*(gen[obsy] - mean_gen[obsy][obsysub]) * (*weights)[0];
	    }

	}

      // chain.SetBranchAddress(TString("E") + "_" + observables[obsind] + "_" + tag_jet_types_[jetcode] + "_" + tag_charge_types_[chargecode] + "_" + "gen",  & E_gen);
      // chain.SetBranchAddress(TString("M") + "_" + observables[obsind] + "_" + tag_jet_types_[jetcode] + "_" + tag_charge_types_[chargecode] + "_" + "gen",  & M_gen);
    }
  printf("\nSTDDEVS\n");
  for (unsigned char obsind = 0; obsind < Nobs; obsind ++)
    {
      for (unsigned char obssubind = 0; obssubind < sizes[obsind]; obssubind ++)
	{
	  stddev_reco[obsind][obssubind] = TMath::Power(stddev_reco[obsind][obssubind]/wnevents_reco[obsind][obssubind], 0.5);
	  stddev_gen[obsind][obssubind] = TMath::Power(stddev_gen[obsind][obssubind]/wnevents_gen[obsind][obssubind], 0.5);
	  printf(" %.8f  %.8f\n", stddev_reco[obsind][obssubind], stddev_gen[obsind][obssubind]);
	}
      printf("-----\n");
    }
  for (unsigned char obsx = 0; obsx < Nobs; obsx ++)
    {
      for (unsigned char obsxsub = 0; obsxsub < sizes[obsx]; obsxsub ++)
	{
	  for (unsigned char obsy = 0; obsy < Nobs; obsy ++)
	    {
	      for (unsigned char obsysub = 0; obsysub < sizes[obsy]; obsysub ++)
		{
		  if (obsxsub + multiplier[obsx] == obsysub + multiplier[obsy])
		    printf("%f %f %f %f\n", wnevents_reco[obsx][obsxsub], wnevents_reco[obsy][obsysub], stddev_reco[obsx][obsxsub], stddev_reco[obsy][obsysub]);
		  cov_reco[obsxsub + multiplier[obsx]][obsysub + multiplier[obsy]] = cov_reco[obsxsub + multiplier[obsx]][obsysub + multiplier[obsy]]
		    /(TMath::Power(wnevents_reco[obsx][obsxsub] * wnevents_reco[obsy][obsysub], 0.5) * stddev_reco[obsx][obsxsub] * stddev_reco[obsy][obsysub]);
		  if (obsxsub + multiplier[obsx] == obsysub + multiplier[obsy])
		    printf("%f\n", cov_reco[obsxsub + multiplier[obsx]][obsysub + multiplier[obsy]]);
		  h2[0] -> SetBinContent(obsxsub + multiplier[obsx] + 1, obsysub + multiplier[obsy] +1, cov_reco[obsxsub + multiplier[obsx]][obsysub + multiplier[obsy]]);
		  cov_gen[obsxsub + multiplier[obsx]][obsysub + multiplier[obsy]] = cov_gen[obsxsub + multiplier[obsx]][obsysub + multiplier[obsy]]
		    /(TMath::Power(wnevents_gen[obsx][obsxsub] * wnevents_gen[obsy][obsysub], 0.5) * stddev_gen[obsx][obsxsub] * stddev_gen[obsy][obsysub]);
		  h2[1] -> SetBinContent(obsxsub + multiplier[obsx] + 1, obsysub + multiplier[obsy] +1, cov_gen[obsxsub + multiplier[obsx]][obsysub + multiplier[obsy]]);
		  
		}
	    }
	}
    }
  for (unsigned char obsind = 0; obsind < Nobs; obsind ++)
    {
      delete [] mean_reco[obsind];
      delete [] mean_gen[obsind];
      delete [] wnevents_reco[obsind];
      delete [] wnevents_gen[obsind];
      delete [] stddev_reco[obsind];
      delete [] stddev_gen[obsind];
    }
  file -> Close();
  for (unsigned char lind = 0; lind < Nlevels; lind ++)
    {
      canvas[lind] -> SaveAs(TString(canvas[lind] -> GetName()) + ".png");
    }
  return 0;
}

unsigned char getbinnumber(float obs, unsigned char nbins, float * bins)
{
  unsigned char bind = 0;
  while (not (obs >= bins[bind] and obs < bins[bind + 1]) and bind < nbins)
    {
      bind ++;
     
    }
  if (bind == nbins)
    {
      // printf("bind %u\n", bind);
      // for (unsigned char ind = 0 ; ind < nbins + 1; ind ++)
      // 	printf("%u %f\n", ind, bins[ind]);
      // printf("%f %u\n", obs, bind);
      // printf("****\n");
    }
  // if (bind == nbins)
  //   throw "overflow value";
	 
  return bind;
}

float transformval(float val, const char *observable )
{
  float valret;
  if (TString(observable) == "pull_angle")
    valret = TMath::Abs(val)/TMath::Pi();
  if (TString(observable) == "pvmag")
    valret = val;
  //  printf("[%s] %f %f \n", observable, val, valret);
  return valret;
}
