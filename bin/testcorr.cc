#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "CompoundHistoUnfolding.hh"
#include "TChain.h"
#include <stdio.h>
#include "TChain.h"
#include "TFile.h"
#include "TKey.h"
#include "TApplication.h"
#include "THStack.h";
#include "TMath.h"
int main()
{
  //  TApplication app("myapp", 0, 0);
  const unsigned char kiter = 25;
  TFile * file = TFile::Open("root://eosuser.cern.ch//$EOS/unfolding_nominal/pull_angle/ATLAS4/leading_jet_chconst_MC13TeV_TTJets_pull_angle_OPT_nominal_ATLAS4//save.root", "READ");
  printf("file %p %s\n", file, file -> GetName()); 
  CompoundHistoUnfolding * ch = (CompoundHistoUnfolding * )((TKey * ) file -> GetListOfKeys() -> At(0)) -> ReadObj();
  TH2 * hsignal = (TH2 *) ch -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone("unfolding_signal");
  TH2 * hsignalorig = (TH2 *) ch -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone("unfolding_signal_orig");

  //hsignal -> RebinX(2);                                                                                                                                                                                   
  TH2 * datambackground = (TH2*) ch -> GetLevel(IN) -> GetHU(DATAMO) -> GetTH2() -> Clone("datambackground");

  datambackground -> Add(ch -> GetTotalBackground() -> GetTH2(), -1);
  //datambackground -> RebinX(2);                                                                                                                                                                          \
                                                                                                                                                                                                            
  TH1 * hmeas = (TH1*) datambackground -> ProjectionX("meas");
  for (unsigned char bind = 0; bind <= hsignal -> GetNbinsX() + 1; bind ++)
    {
      // if (hsignal -> Integral(bind, bind, 0, hsignal -> GetNbinsY() + 1) > 0.0)                                                                                                                          
      //   hmeas -> SetBinContent(bind, hmeas -> GetBinContent(bind) * (1.0 - hsignal -> GetBinContent(bind, 0)/hsignal -> Integral(bind, bind, 0, hsignal -> GetNbinsY() + 1)));                           
      hsignal -> SetBinContent(bind, 0, 0.0);
      //      hmeas -> SetBinError(bind, 0, 0.0);                                                                                                                                                           
    }
  for (unsigned char bind = 0; bind <= hsignal -> GetNbinsY() + 1; bind ++)
    {
      // if (hsignal -> Integral(bind, bind, 0, hsignal -> GetNbinsY() + 1) > 0.0)                                                                                                                          
      //   hmeas -> SetBinContent(bind, hmeas -> GetBinContent(bind) * (1.0 - hsignal -> GetBinContent(bind, 0)/hsignal -> Integral(bind, bind, 0, hsignal -> GetNbinsY() + 1)));                           
      //      hsignal -> SetBinContent(0, bind, 0.0);                                                                                                                                                       
    }

  ch -> ApplyScaleFactor(hmeas);
  TH1 * hreco = hsignal -> ProjectionX();
  TH1 * hmodel = hsignal -> ProjectionY("gen");
  TH1 * hgennorm = (TH1 *) hmodel -> Clone("hgennorm");
  hgennorm -> Scale(1.0/hgennorm -> Integral());
  RooUnfoldResponse response(hreco, hmodel, hsignal);
  RooUnfoldBayes   unfold (&response, hmeas, kiter);
  TH1* hunfoldeddmbckg = (TH1D*) unfold.Hreco();
  hunfoldeddmbckg -> Scale(1.0/hunfoldeddmbckg -> Integral());
  for (unsigned char bind = 0; bind < hunfoldeddmbckg -> GetNbinsX() + 1; bind ++)
    {
      const float coeff = hsignalorig -> Integral(1, hsignalorig -> GetNbinsX() + 1, bind, bind)/hsignalorig -> Integral(0, hsignalorig -> GetNbinsX(), bind, bind);
      //houtput -> SetBinContent(bind, houtput -> GetBinContent(bind) / coeff);                                                                                                                             
    }
  auto corr =[](TH1 * hunfolded, TH1 * hprediction) -> float
    {
      float D = 0.0;
      
      for (unsigned char bind = 1; bind <= hunfolded -> GetNbinsX(); bind ++)
	{
	  D += TMath::Power(hunfolded -> GetBinContent(bind) - hprediction -> GetBinContent(bind), 2);
	}
      return D;
    };
  const float Dnom = corr(hunfoldeddmbckg, hgennorm);
  printf("Dnom = %f\n", Dnom);
  for(map<TString, vector<HistoUnfolding *>>::iterator bit = ch -> GetLevel(IN) -> _msyshistos .begin(); bit != ch -> GetLevel(IN) -> _msyshistos . end(); bit ++)
    {
      const TString sample = bit -> first;
      if (sample != ch -> signaltag)                                                                                                                                                                           {       
	continue;                                                                                                                                                                                   
      }
      for (vector<HistoUnfolding *>::iterator it = ch -> GetLevel(IN) -> GetV(SYSMO, sample.Data()) -> begin(); it != ch -> GetLevel(IN) -> GetV(SYSMO, sample.Data()) -> end(); it ++)
	{
	  //printf("unfolding systematic %s systype\n", (*it) -> GetTag(), (*it) -> GetSysType());
	  if ((*it) -> GetSysType() != THEORSYS)
	    {
	      continue;
	    }
	  if (TString((*it) -> GetTag()) != "MC13TeV_TTJets_cflip")
	    {
	      continue;
	    }
	  (*it) -> ls();
	  getchar();
	  //              printf("unfolding %s\n", (*it) -> GetTag());                                                                                                                              
	  TH2 * inputsysh2 = (TH2*)(*it) -> GetTH2() -> Clone("inputsysh2");
	  //inputsysh2 -> RebinX(2);                                                                                                                                                                
	  TH1 * inputsys = inputsysh2 -> ProjectionX("inputsys");
	  TH1 * inputsysgen = inputsysh2 -> ProjectionY("inputsysgen");
	  ch -> ApplyScaleFactor(inputsys);
	  RooUnfoldResponse response(hreco, hmodel, hsignal);
	  RooUnfoldBayes   unfold (&response, inputsys, kiter);
	  //HistoUnfoldingTH1 * out = new HistoUnfoldingTH1((SampleDescriptor * ) * it);
	  //GetLevel(OUT) -> GetV(SYSMO, sample.Data()) -> push_back(out);
	  TH1 *  houtput  = unfold.Hreco();
	  for (unsigned char bind = 0; bind < houtput -> GetNbinsX() + 1; bind ++)
	    {
	      //const float coeff = hsignalorig -> Integral(1, hsignalorig -> GetNbinsX() + 1, bind, bind)/hsignalorig -> Integral(0, hsignalorig -> GetNbinsX(), bind, bind);
	      //  houtput -> SetBinContent(bind, houtput -> GetBinContent(bind) / coeff);                                                                                                           
	    }

	  //out -> GetTH1Ref(GEN) = houtput;
	  houtput -> Scale(1.0/houtput -> Integral());
	  inputsysgen -> Scale(1.0/inputsysgen -> Integral());
	  const float Dsys = corr(houtput, inputsysgen);
	  printf("%s %f %f\n", (*it) -> GetTag(), Dsys, Dnom/Dsys);
	  houtput -> SetName(ch -> CreateName(TString((*it) ->GetTag()) + "_output"));
	  delete inputsys;
	  delete inputsysh2;
	  delete inputsysgen;
	}
    }
  file -> Close();
}
