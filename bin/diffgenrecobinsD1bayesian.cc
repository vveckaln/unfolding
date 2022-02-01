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
#include "TUnfoldDensity.h"
#include "TRandom3.h"
#include "TRandom2.h"
#include <assert.h>
#include "TROOT.h"
#include <ctime>
#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include <random>
std::default_random_engine generator;
std::uniform_real_distribution<double> distribution(0.0, 1.0);

//TRandom3 random3;
unsigned char getbinnumber(float obs, unsigned char nbins, float * bins);
TH1 * bootstrapreplica(TH1 * input, double & probability, TRandom *);
float transformval(float val, const char *observable );
float * coeffbins(float *, unsigned char sizes, unsigned char coeff);
TH2 * createcorrmatrix();
const unsigned char Nobs       = 4;
const char * titles[Nobs]      = {"#theta_{p}(j_{1}^{W}, j_{2}^{W})", "#theta_{p}(j_{2}^{W}, j_{1}^{W})", "#theta_{p}(j_{1}^{b}, j_{2}^{b})", "|#vec{P}(j_{1}^{W}, j_{2}^{W})|"};
const char * observables[Nobs]    = {"pull_angle", "pull_angle", "pull_angle", "pvmag"};
const unsigned char jets[Nobs]     = {LEADING_JET, SCND_LEADING_JET, LEADING_B, LEADING_JET};
const unsigned char Nobsact    = 4;
const unsigned char Nit = 5000;
const unsigned char kiter = 3;
TH1 * unfold(const char* , TH2 * migration, TH1 * input);
int main(int argc, char * argv[])
{
  TRandom3 random3;
  random3.Rndm();

  gStyle -> SetOptStat(0);
  //gStyle -> SetOptTitle(0);
  //  float novalue_proxy[Nobsact];
  // TFile * file = TFile::Open("$EOS/analysis_MC13TeV_TTJets/HADDmigration/migration_MC13TeV_TTJets.root", "READ"); 
  // TTree * migration = (TTree*) file -> Get("migration");
  // const unsigned char obsindreco = 0;
  // const unsigned char obsindgen = 0;
  assert(argc == 3);
  const char * bmethod = argv[1];
  const char * unfmethod = argv[2];
  const char * method = "nominal";
  const unsigned char charge_code = CHARGED;
  const unsigned char opt_level = OPT;
  TFile * filech[Nobsact];
  CompoundHistoUnfolding *ch[Nobsact];
  TH1 * data[Nobsact];
  TH1 * background[Nobsact];
  unsigned char chargecode = CHARGED;
  TH2 * h2migration[Nobsact];
  unsigned char bincount = 0;
  unsigned char multipliers[Nobsact];
  unsigned char ngen[Nobsact];
  for (unsigned char obsind = 0; obsind < Nobsact; obsind ++)
    {
      filech[obsind] = TFile::Open(TString("$EOS/unfolding_") + 
				   method + "/" + 
				   observables[obsind] + "/" + 
				   bmethod + "/" +
				   tag_jet_types_[jets[obsind]] + "_" + 
				   tag_charge_types_[charge_code] + 
				   "_MC13TeV_TTJets_" + 
				   observables[obsind] + "_" + 
				   tag_opt[opt_level] + "_" + 
				   method + "_" + 
				   bmethod + "/" + 
				   "save" + ".root", "READ");
      ch[obsind] = (CompoundHistoUnfolding * )((TKey * ) filech[obsind] -> GetListOfKeys() -> At(0)) -> ReadObj(); //The principal ch
      h2migration[obsind] = (TH2 *) ch[obsind] -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> 
	Clone((TString("unfolding_signal_") + tag_jet_types_[jets[obsind]] + "_" + observables[obsind]).Data());
      //      h2migration[obsind] -> RebinX(2);
      h2migration[obsind] -> SetDirectory(nullptr);
      multipliers[obsind] = bincount;
      bincount += h2migration[obsind] -> GetNbinsY();
      ngen[obsind] = h2migration[obsind] -> GetNbinsY();
      data[obsind]        = (TH2*) ch[obsind] ->  GetLevel(IN) -> GetHU(DATAMO) -> GetTH2() -> ProjectionX((TString("data_") + tag_jet_types_[jets[obsind]] + "_" + observables[obsind]).Data());
      data[obsind] -> SetDirectory(nullptr);
      //data[obsind] -> RebinX(2);
      background[obsind]  = ch[obsind] -> GetBackgroundTotal() -> GetTH2() -> ProjectionX((TString("background_") + tag_jet_types_[jets[obsind]] + "_" + observables[obsind]).Data());
      background[obsind] -> SetDirectory(nullptr);
      //   background[obsind] -> RebinX(2);
      // background[obsind] -> Print("all");
      // getchar();
    }
  vector<double> results[bincount];
  vector<double> probabilities[bincount];
  TH1 * houtputnominal[Nobsact];
  for (unsigned char obsind = 0; obsind < Nobsact; obsind ++)
    {
      TH1 * hinputnominal = (TH1*) data[obsind] -> Clone((TString("nominal_input_") + tag_jet_types_[jets[obsind]] + "_" + observables[obsind]).Data());
      hinputnominal -> Add(background[obsind], -1.0);
      for (unsigned char bindy = 0; bindy <= h2migration[obsind] -> GetNbinsY() + 1; bindy ++)
	{
	  h2migration[obsind] -> SetBinContent(0, bindy, 0.0);
	}
      float corrcoeff[h2migration[obsind] -> GetNbinsX() + 2];
      for (unsigned char bindx = 0; bindx <= h2migration[obsind] -> GetNbinsX() + 1; bindx ++)
	{
	  corrcoeff[bindx] = h2migration[obsind] -> Integral(bindx, bindx, 1, h2migration[obsind] -> GetNbinsY())/
	    h2migration[obsind] -> Integral(bindx, bindx, 0, h2migration[obsind] -> GetNbinsY() + 1); 
	  h2migration[obsind] -> SetBinContent(bindx, 0, 0.0);
	  hinputnominal -> SetBinContent(bindx, corrcoeff[bindx] * hinputnominal -> GetBinContent(bindx));
	}


      // hinputnominal -> Print("all");
      // getchar();
      // TH1 * hreco = h2migration[obsind] -> ProjectionX();
      // TH1 * hgen = h2migration[obsind] -> ProjectionY("gen");
      // RooUnfoldResponse response(hreco, hgen, h2migration[obsind]);
      // RooUnfoldBayes   unfold (&response, hinputnominal, kiter);
      houtputnominal[obsind] = unfold(unfmethod, h2migration[obsind], hinputnominal);
      houtputnominal[obsind] -> SetName((TString("nominal_") + tag_jet_types_[jets[obsind]] + "_" + observables[obsind]).Data());
      houtputnominal[obsind] -> Scale(1.0/houtputnominal[obsind] -> Integral());
      for (unsigned char bind = 1; bind <= h2migration[obsind] -> GetNbinsY(); bind ++)
	{
	  results[multipliers[obsind] + bind - 1].reserve(Nit);
	}
      // TRandom3 random3;
      // random3.SetSeed(time(0));
      random3.SetSeed(100 * obsind);
      for (unsigned char it = 0; it < Nit; it ++)
	{
	  double probability;
	  TH1 * hinputreplica = bootstrapreplica(data[obsind], probability, & random3);
	  //hinputreplica -> Print("all");
	  //getchar();
	  hinputreplica -> Add(background[obsind], - 1.0);
	  for (unsigned char bindx = 0; bindx <= h2migration[obsind] -> GetNbinsX() + 1; bindx ++)
	    {
	      hinputreplica -> SetBinContent(bindx, hinputreplica -> GetBinContent(bindx) * corrcoeff[bindx]); 
	    }
	    hinputreplica -> Scale(hinputnominal -> Integral()/hinputreplica -> Integral());
	    TH1 * houtput = unfold(unfmethod, h2migration[obsind], hinputreplica);
	  //	  houtput -> Scale(houtputnominal[obsind] -> Integral()/houtput -> Integral());
	  houtput -> Scale(1.0/houtput -> Integral());
	  //houtput -> Print("all");
	  delete hinputreplica;
	  for (unsigned char bind = 1; bind <= houtput -> GetNbinsX(); bind ++)
	    {
	      //	      printf("%u ", multipliers[obsind] + bind - 1);
	      results[multipliers[obsind] + bind - 1].push_back(houtput -> GetBinContent(bind));
	    }
	  //printf("\n");
	  probabilities[obsind].push_back(probability);
	  delete houtput;
	}
      delete hinputnominal;
    }
      
  // for (unsigned char itind = 0; itind < Nit; itind ++)
  //   {
  //     printf("%u ", itind);
  //     for (unsigned char binind = 0; binind < bincount; binind ++)
  // 	{
  // 	  printf("%.0f\t", results[binind][itind]);
  // 	}
  //     printf("\n");
  //   }
  vector<double> probabilitiesit;
  double mean[bincount];

  double probsum = 0.0;
  double stdev[bincount];
  for (unsigned char itind = 0; itind < Nit; itind ++)
    {
      double probabilityit = 1.0;
      for (unsigned char obsind = 0; obsind < Nobsact; obsind ++)
	{
	  probabilityit *= probabilities[obsind][itind];
	}
      probabilitiesit.push_back(probabilityit);
      //      printf("%f\n", probabilityit);
      probsum += probabilityit;
    }
  // printf("probsum %f\n", probsum);
  // getchar();
  //probsum = 1.0;

  for (unsigned char obsind = 0; obsind < Nobsact; obsind ++)
    {
      for (unsigned char genind = 0; genind < ngen[obsind]; genind ++)
	{
	  mean[multipliers[obsind] + genind] = houtputnominal[obsind] -> GetBinContent(genind + 1);//, mean[multipliers[obsind] + genind] - houtputnominal[obsind] -> GetBinContent(genind + 1));
	  stdev[multipliers[obsind] + genind] = houtputnominal[obsind] -> GetBinError(genind + 1);//, mean[multipliers[obsind] + genind] - houtputnominal[obsind] -> GetBinContent(genind + 1));
	}
      //      printf("\n");
    }

  for (unsigned char binind = 0; binind < bincount; binind ++)
    {
      mean[binind] = 0.0;
      stdev[binind] = 0.0;

    
      for (unsigned char itind = 0; itind < Nit; itind ++)
      	{
      	  mean[binind] += results[binind][itind] * probabilitiesit[itind];
      	}
      mean[binind] = mean[binind] / probsum;
      
      for (unsigned char itind = 0; itind < Nit; itind ++)
  	{
  	  stdev[binind] += TMath::Power(results[binind][itind] - mean[binind], 2)* probabilitiesit[itind];
  	}
      stdev[binind] = TMath::Power(stdev[binind]/probsum, 0.5);
    }
  for (unsigned char obsind = 0; obsind < Nobsact; obsind ++)
    {
      for (unsigned char genind = 0; genind < ngen[obsind]; genind ++)
  	{
  	  printf("%f  %f %f\n", mean[multipliers[obsind] + genind], houtputnominal[obsind] -> GetBinContent(genind + 1), mean[multipliers[obsind] + genind] - houtputnominal[obsind] -> GetBinContent(genind + 1));
  	}
      printf("\n");
    }
  //getchar();
  double corr[bincount][bincount];
  for (unsigned char binindx = 0; binindx < bincount; binindx ++)
    {
      for (unsigned char binindy = 0; binindy < bincount; binindy ++)
	{
	  corr[binindx][binindy] = 0.0;
	  for (unsigned char itind = 0; itind < Nit; itind ++)
	    {
	      corr[binindx][binindy] += (results[binindx][itind] - mean[binindx])*(results[binindy][itind] - mean[binindy])* probabilitiesit[itind];
	    }
	  corr[binindx][binindy] = corr[binindx][binindy]/probsum;
	  corr[binindx][binindy] = corr[binindx][binindy]/(stdev[binindx]*stdev[binindy]);
	}
      
    }
  TString corrtitle;
  if (string(unfmethod).compare("bayesian") == 0)
    {
      corrtitle = TString("Correlation matrix (RooUnfold::RooUnfoldBayes, iter ") + Form("%u", kiter) + ")";
    }
  if (string(unfmethod).compare("schmitt") == 0)
    {
      corrtitle = TString("Correlation matrix (ROOT::TUnfold)");
    }
  TH2 * hcorr = new TH2I("corr", corrtitle, bincount, 0.0, bincount, bincount, 0.0, bincount);
  for (unsigned char genindx = 0; genindx < bincount; genindx ++)
    {
      for (unsigned char genindy = 0; genindy < bincount; genindy ++)
	{
	  printf("[%.2f] \t", corr[genindx][genindy]);

	  //if (genindy == genindx)
	    //	    printf("%f %f\n", corr[genindx][genindy], stdev[genindx]);
	  hcorr -> SetBinContent(genindx + 1, genindy + 1, (int) 100 * corr[genindx][genindy]);
	}	  
      printf("\n");

    }  
  // TCanvas *coutput = new TCanvas;
  // houtput -> Draw("HIST");
  // coutput -> SaveAs("output.png");
  //  TH1 * hgen= h2 -> ProjectionY("gen");
  // for (unsigned char bind = 1; bind <= houtput -> GetNbinsX(); bind ++)
  //   {
  //     printf("%u %f %f\n", bind, houtput -> GetBinError(bind), hgen -> GetBinContent(bind));
  //   }
  // TH2 * ematrix = nullptr;
  // ematrix = unfold.GetEmatrixInput("ematrixinput");
  // //  unfold.GetEmatrix(ematrix);
  // // for (unsigned char bindx = 1; bindx -> GetBinContent
  hcorr -> SetMinimum(-100.0);
  hcorr -> GetXaxis() -> SetLabelSize(0.0);
  hcorr -> GetYaxis() -> SetLabelSize(0.0);
  hcorr -> GetYaxis() -> SetTitleOffset(0.1);
  hcorr -> GetXaxis() -> SetTickLength(0.0);
  hcorr -> GetYaxis() -> SetTickLength(0.0);
  hcorr -> GetXaxis() -> SetTitleOffset(0.1);
  TCanvas * cematrix = new TCanvas;
  hcorr -> Draw("COLZTEXT");
  cematrix -> SetLeftMargin(0.15);
  cematrix -> SetBottomMargin(0.15);
  cematrix -> SetRightMargin(0.12);
  unsigned char lticky = 0.0;
  unsigned char sticky = 0.0;
  unsigned char ltickx = 0.0;
  unsigned char stickx = 0.0;
  const float ltickstart    = -0.7;
  const float stickstart    = -0.3;
  const float pavetextstart = -1.4;
  const float pavetextend   = -1.0;
  const float binlabelstart = ltickstart;
  const float binlabelend   = -0.1;
  const float llinewidth    = 2.0;
  TLine * lline0y = new TLine(ltickstart, 0.0, 0.0, 0.0);
  lline0y -> SetLineWidth(llinewidth);
  //lline0y -> SetLineColor(kRed);
  lline0y -> Draw();
  TLine * lline0x = new TLine(0.0, ltickstart, 0.0, 0.0);
  //lline0x -> SetLineColor(kRed);
  lline0x -> SetLineWidth(llinewidth);
  lline0x -> Draw();
  for (unsigned char obsind = 0; obsind < Nobsact; obsind ++)
    {
      TPaveText * labely = new TPaveText(pavetextstart, lticky, pavetextend, lticky + ngen[obsind]);
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
      for (unsigned char slind = 0; slind < ngen[obsind]; slind ++)
	{
	  sticky += 1;
	  TLine * sliney = new TLine(stickstart, sticky, bincount, sticky);
	  //sliney -> SetLineColor(kGreen);
	  sliney -> Draw("SAME");
	  TPaveText * binlabely = new TPaveText(binlabelstart, sticky -1,  binlabelend, sticky);
	  const TString binlabeltext(Form("%u\n", slind + 1));
	  binlabely -> AddText(binlabeltext.Data());
	  binlabely -> SetBorderSize(0.0);
	  binlabely -> SetFillStyle(0);
	  binlabely -> Draw();
	  TText *binlabeltexty = binlabely -> GetLineWith(binlabeltext.Data());
	  binlabeltexty -> SetTextAngle(90);
	  binlabeltexty -> SetTextSize(0.02);//2.0 * ltext -> GetTextSize());
	}
      lticky += ngen[obsind];
      
      TLine * lliney = new TLine(ltickstart, lticky, bincount, lticky);
      lliney -> SetLineWidth(llinewidth);
      //lliney -> SetLineColor(kRed);
      lliney -> Draw("SAME");

      TPaveText * labelx = new TPaveText(ltickx, pavetextstart, ltickx + ngen[obsind], pavetextend);
      labelx -> AddText(titles[obsind]);
      labelx -> SetBorderSize(0.0);
      labelx -> SetFillStyle(0);
      labelx -> Draw();
      TText *ltextx = labelx -> GetLineWith(titles[obsind]);
      //      ltextx -> SetTextAngle(90);
      ltextx -> SetTextSize(0.04);//2.0 * ltext -> GetTextSize());
      for (unsigned char slind = 0; slind < ngen[obsind]; slind ++)
	{
	  stickx += 1;
	  TLine * slinex = new TLine(stickx, stickstart, stickx, bincount);
	  //	  slinex -> SetLineColor(kGreen);
	  slinex -> Draw("SAME");
	  TPaveText * binlabelx = new TPaveText(stickx - 1,  binlabelstart, stickx, binlabelend);
	  const TString binlabeltext(Form("%u\n", slind + 1));
	  binlabelx -> AddText(binlabeltext.Data());
	  binlabelx -> SetBorderSize(0.0);
	  binlabelx-> SetFillStyle(0);
	  binlabelx -> Draw();
	  TText *binlabeltextx = binlabelx -> GetLineWith(binlabeltext.Data());
	  binlabeltextx -> SetTextSize(0.02);//2.0 * ltext -> GetTextSize());
	}
      ltickx += ngen[obsind];
      
      TLine * llinex = new TLine(ltickx, ltickstart, ltickx, bincount);
      llinex -> SetLineWidth(llinewidth);
      //llinex -> SetLineColor(kRed);
      llinex -> Draw("SAME");
    }
  const Int_t Number = 3;
  TColor * col1 = gROOT -> GetColor(kBlue);
  TColor * colm = gROOT -> GetColor(kWhite);
  TColor * col2 = gROOT -> GetColor(kRed);
  Double_t Red[Number]    = { col1 -> GetRed()   , colm -> GetRed(),   col2 -> GetRed()};
  Double_t Green[Number]  = { col1 -> GetGreen() , colm -> GetGreen(), col2 -> GetGreen()};
  Double_t Blue[Number]   = { col1 -> GetBlue()  , colm -> GetBlue(),  col2 -> GetBlue()};
  Double_t Length[Number] = { 0.00, 0.5, 1.00};
  const Int_t nb = 50;
  TColor::CreateGradientColorTable(Number, Length, Red, Green, Blue, nb);
  //  gStyle -> SetPaintTextFormat("");
  TString canvasname;
  if (string(unfmethod).compare("bayesian") == 0)
    {
      canvasname = TString("corr_bayesian_kiter") + Form("%u", kiter);
    }
  if (string(unfmethod).compare("schmitt") == 0)
    {
      canvasname = TString("corr_schmitt");
    }
  cematrix -> SaveAs(canvasname + ".png");
  for (unsigned char obsind = 0; obsind < Nobsact; obsind ++)
    {
      // delete [] xbins;
      // delete [] ybins;
      filech[obsind] -> Close();
      delete h2migration[obsind];
      delete background[obsind];
      delete data[obsind];
    }
  

}

TH1 * bootstrapreplica(TH1 * input, double & probability, TRandom  * random)
{
  TH1* inputret = (TH1 *) input -> Clone();
  probability = 1.0;
  //  random3.Rndm();
  for (unsigned char bind = 1; bind <= input -> GetNbinsX(); bind ++)
    {
      //printf("%f %f  %f\n", input -> GetBinContent(bind), input -> GetBinError(bind), TMath::Sqrt(input -> GetBinContent(bind)));
      //      const unsigned long nevents =  input -> GetBinContent(bind) - input -> GetBinError(bind) + random -> Integer(2 * input -> GetBinError(bind)) ;
      const unsigned long nevents =  random -> Poisson(input -> GetBinContent(bind)) ;
      //      const unsigned long nevents =  input -> GetBinContent(bind) - input -> GetBinError(bind) + 2 * input -> GetBinError(bind) * distribution(generator);//random -> Integer(2 * input -> GetBinError(bind)) ;
      //printf("%lu\n", nevents);
      inputret -> SetBinContent(bind, nevents);
      //      printf("%f\n", TMath::PoissonI(nevents, input -> GetBinContent(bind)));
      probability *= Nobsact * 100 *TMath::PoissonI(nevents, input -> GetBinContent(bind));
    }
  probability = 1;

  return inputret;
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

TH1 * unfold(const char* unfm, TH2 * migration, TH1 * input)
{
  if (string(unfm).compare("bayesian") == 0)
    {
      TH1 * hreco = migration -> ProjectionX();
      TH1 * hgen = migration -> ProjectionY("gen");
      RooUnfoldResponse response(hreco, hgen, migration);
      RooUnfoldBayes   unfold (&response, input, kiter);
      return unfold.Hreco();
    }

  if (string(unfm).compare("schmitt") == 0)
    {
      TUnfoldDensity unfoldit(migration, TUnfold::kHistMapOutputVert/*, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeBinWidthAndUser*/);                
      unfoldit.SetInput(input);
      unfoldit.DoUnfold(0.0);
      return unfoldit.GetOutput("output_schmitt");
    }
  return nullptr;
}
// float * doublebins(float * ybins, unsigned char sizes, unsigned char coeff)
// {
//   float * ret = new float[coeff * sizes + 1];
//   for (unsigned char ind = 0; ind < sizes; ind ++)
//     {
//       const float step = (ybins[ind + 1] - ybins [ind])/coeff;
//       for (unsigned char k = 0; k < coeff; k ++)
// 	{
// 	  ret[ind * coeff + k] = ybins[ind] + * k * step; 
// 	}
//     }
    
// }
