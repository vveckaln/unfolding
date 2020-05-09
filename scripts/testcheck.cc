R__ADD_INCLUDE_PATH($COMPOUNDHISTOLIB/interface)
//#include "/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/interface/CompoundHistoUnfolding.hh"
#include "CompoundHistoUnfolding.hh"
R__ADD_LIBRARY_PATH($COMPOUNDHISTOLIB/lib) // if needed
R__LOAD_LIBRARY(libCompoundHisto.so)
int testcheck(const char * methodchar, const char * observable, const char * binmethod)
{
  string method(methodchar);
  const char * observables[] = {"pull_angle", "pvmag"};
  const char * samples_nominal[] = {"MC13TeV_TTJets", "MC13TeV_TTJets_herwig", "MC13TeV_TTJets2l2nu_amcatnlo"};
  const char * samples_cflip[] = {"MC13TeV_TTJets", "MC13TeV_TTJets_cflip"};
  string bindir(binmethod);
  const TString dir = TString("common_plots_") + method + "/" + observable + "/" + bindir;
  unsigned char opt_level = ORIG;
  if (string(binmethod).compare("ORIG") != 0)
    opt_level = OPT;
  string bintag("");
  if (opt_level == OPT)
    bintag = bintag + "_" + bindir;
  const char ** samples;
  unsigned char nsamples;
  if (method.compare("nominal") == 0)
    {
      samples = samples_nominal;
      nsamples = 2;
    }
  if (method.compare("cflip") == 0)
    {
      samples = samples_cflip;
      nsamples = 2;
    }
  gStyle -> SetOptStat(0);
  gStyle -> SetOptTitle(0);
  gROOT -> SetBatch(kTRUE);
  /*  for (unsigned char observable_ind = 0; observable_ind < 2; observable_ind ++) 
      {*/
  system(TString("mkdir -p ") + dir);
  system(TString("rm ") + dir + "/*");

  // for (unsigned char opt_level = 0; opt_level < 2; opt_level ++)
  //   {
      for (ChargeCode_t charge_code = 0; charge_code < 2; charge_code ++)
	{
	  for (VectorCode_t jetcode = 0; jetcode < 4; jetcode ++)
	    {
	      printf("Processing %s %s %s\n", tag_jet_types_[jetcode], tag_charge_types_[charge_code], tag_opt[opt_level]);

	      TFile * fileps = TFile::Open(TString("$EOS/unfolding_") + 
					   method + "/" + 
					   observable + "/" + 
					   bindir + "/" +
					   tag_jet_types_[jetcode] + "_" + 
					   tag_charge_types_[charge_code] + 
					   "_MC13TeV_TTJets_" + 
					   observable + "_" + 
					   tag_opt[opt_level] + "_" + 
					   method + bintag + 
					   "/save.root", "READ");
	      CompoundHistoUnfolding * chups = (CompoundHistoUnfolding * )((TKey * ) fileps -> GetListOfKeys() -> At(0)) -> ReadObj();
	      printf("chups %p\n", chups);
	      fileps -> Close();
	      TFile * file = TFile::Open(TString("$EOS/unfolding_") + 
					 method + "/" +
					 observable + "/" + 
					 bindir + "/" +
					 tag_jet_types_[jetcode] + "_" + 
					 tag_charge_types_[charge_code] + 
					 "_" + samples[1] + "_" + 
					 observable + "_" + 
					 tag_opt[opt_level] + "_" + 
					 method + bintag 
					 + "/save.root", "READ");
	      printf("file %p %s\n", file, file -> GetName()); 
	      CompoundHistoUnfolding * ch = (CompoundHistoUnfolding * )((TKey * ) file -> GetListOfKeys() -> At(0)) -> ReadObj();
	      printf("ch %p\n");
	      file -> Close();
	      CompoundHistoUnfolding * ch_cflip = nullptr;
	      if (method.compare("nominal") == 0)
		{
		  TFile * file_cflip = TFile::Open(TString("$EOS/unfolding_cflip/") + 
						   observable + "/" + 
						   bindir + "/" +
						   tag_jet_types_[jetcode] + "_" + 
						   tag_charge_types_[charge_code] + 
						   "_" + samples_cflip[1] + "_" + 
						   observable + "_" + 
						   tag_opt[opt_level] + "_cflip" + 
						   bintag +
						   "/save.root", "READ");
		  if (file_cflip)
		    {
		      ch_cflip = (CompoundHistoUnfolding * )((TKey * ) file_cflip -> GetListOfKeys() -> At(0)) -> ReadObj();
		      file_cflip -> Close();
		      printf("ch_cflip %p\n", ch_cflip);
		    }
		}
	      for(ResultLevelCode_t resultcode = 0; resultcode < 2; resultcode ++)
		//  chups -> GetLevel(OUT) -> GetHURef(SIGNALMO) = chups -> GetLevel(IN) -> GetHU(SIGNALMO);
		//ResultLevelCode_t resultcode = IN;
		{
		  for(RecoLevelCode_t recocode = 0; recocode < 2; recocode ++)
		    //RecoLevelCode_t recocode = RECO;
		    {
		      TCanvas *canvas = chups -> CreateCombinedPlot(recocode, resultcode);
		      canvas -> SetName(TString(tag_jet_types_[jetcode]) + '_' + 
					tag_charge_types_[charge_code] + '_' + 
					observable + '_' + 
					tag_opt[opt_level] + "_" + 
					tag_recolevel[recocode] + "_" + 
					tag_resultlevel[resultcode]);
			  
		      TPad * p1 = (TPad *) canvas -> GetListOfPrimitives() -> At(0); 
		      TPad * p2 = (TPad *) canvas -> GetListOfPrimitives() -> At(1); 
		      TLegend * legend = nullptr;
			  
		      TObject * obj = nullptr;
		      TIter next(p1 -> GetListOfPrimitives());
		      while ((obj = next()) and TString(obj -> ClassName()) != "TLegend")
			{
			}
		      legend = (TLegend *)obj;
		      TH1 * data = nullptr;
		      if (resultcode == IN and recocode == RECO)
			{
			  data = chups -> GetLevel(resultcode) -> GetHU(DATAMO) -> Project(recocode, "data");
			}
		      if (resultcode == OUT )
			{
			  data = chups -> GetLevel(resultcode) -> GetHU(DATAMO) -> GetTH1(recocode);
			}
		      TH1 * bckgproj = ch -> GetBackgroundTotal() -> Project(recocode);
		      THStack * stack = new THStack("mcnostack", "mcnostack");
		      HistoUnfolding * hherwig = ch -> GetLevel(resultcode) -> GetHU(TOTALMCSIGNALPROXY, resultcode);
		      HistoUnfolding * cflip = nullptr; 
		      if (ch_cflip)
			cflip = ch_cflip -> GetLevel(resultcode) -> GetHU(TOTALMCSIGNALPROXY, resultcode);
		      TH1 * hherwigreco = nullptr;
		      TH1 * cflipreco = nullptr;
		      if (resultcode == IN)
			{
			  // hherwig -> GetTH2() -> Add(ch -> GetBackgroundTotal() -> GetTH2());
			  hherwigreco = hherwig -> Project(recocode);
			  if (cflip)
			    cflipreco = cflip -> Project(recocode);
			}
		      else
			{
			  hherwigreco = hherwig -> GetTH1(recocode);
			  if (cflip)
			    cflipreco = cflip -> GetTH1(recocode);
			  //hherwigreco -> Add(bckgproj);
			}
		      hherwigreco -> SetMarkerStyle(kOpenSquare);
		      hherwigreco -> SetMarkerColor(kGreen);
		      hherwigreco -> SetLineColor(kGreen);
		      hherwigreco -> SetLineWidth(0);
		      hherwigreco -> SetLineStyle(kDashed);
		      if (cflipreco)
			{
			  cflipreco -> SetMarkerStyle(kOpenCircle);
			  cflipreco -> SetMarkerColor(kRed);
			  cflipreco -> SetLineColor(kRed);
			  cflipreco -> SetLineWidth(0);
			  cflipreco -> SetLineStyle(kDashed);
			}
		      TH1 * hherwigrecocl = nullptr;
		      TH1 * cfliprecocl = nullptr;
		      if (data)
			{
			  hherwigrecocl = (TH1 *) hherwigreco -> Clone(TString("hherwigrecocl_") + tag_recolevel[recocode]);
			  hherwigrecocl -> Divide(data);
			  p2 -> cd();
			  hherwigrecocl -> Draw("PSAME");
			  if (cflipreco)
			    {
			      cfliprecocl = (TH1 *) cflipreco -> Clone(TString("cfliprecocl_") + tag_recolevel[recocode]);
			      cfliprecocl -> Divide(data);
			      p2 -> cd();
			      cfliprecocl -> Draw("PSAME");
			    }
			}
		      stack -> Add(hherwigreco, "P");
		      stack -> Add(cflipreco, "P");
		      /*	  TString title;
				  switch (resultcode)
				  {
				  case IN:
				  title = "t#bar{t} Herwig++";
				  break;
				  case OUT:
				  switch(recocode)
				  {
				  case RECO:
				  title = "data (t#bar{t} Herwig++) folded back";
				  break;
				  case GEN:
				  title = "data (t#bar{t} Herwig++) unfolded";
				  break;
				  }
				  }*/
		      chups -> NormaliseToBinWidth(hherwigreco);
		      if (ch_cflip)
			ch_cflip -> NormaliseToBinWidth(cflipreco);
		      p1 -> cd();
		      stack -> Draw("nostackSAME");
		      const unsigned char nfsr = 2;
		      HistoUnfolding * h_fsr[nfsr] = {
			chups -> GetSys(IN/*resultcode*/, "MC13TeV_TTJets_fsrup"), 
			chups -> GetSys(IN/*resultcode*/, "MC13TeV_TTJets_fsrdn")
		      };
		      Style_t fsrMarkerStyle[nfsr] = {kOpenTriangleUp, kOpenTriangleDown};
		      TH1 *hh_fsr[nfsr];
		      TH1 *hh_fsr_cl[nfsr] = {nullptr, nullptr};
		      TList * lentries = legend -> GetListOfPrimitives();
		      for (unsigned char fsr_ind = 0; fsr_ind < nfsr; fsr_ind ++)
			{
			  if (resultcode == IN)
			    {
			      h_fsr[fsr_ind] -> GetTH2() -> Add(ch -> GetBackgroundTotal() -> GetTH2());
			      //hh_fsr[fsr_ind] = h_fsr[fsr_ind] -> Project(recocode, TString(h_fsr[fsr_ind] -> GetTag()));
			    }
			  else
			    {
			      //				  hh_fsr[fsr_ind] = h_fsr[fsr_ind] -> GetTH1(recocode);
			      //hh_fsr[fsr_ind] -> Add(bckgproj);
			    }
			  /*!*/
			  hh_fsr[fsr_ind] = h_fsr[fsr_ind] -> Project(recocode, TString(h_fsr[fsr_ind] -> GetTag()));
			  /*!*/
			  hh_fsr[fsr_ind] -> SetLineWidth(0.0);
			  hh_fsr[fsr_ind] -> SetMarkerColor(kBlack);
			  hh_fsr[fsr_ind] -> SetMarkerStyle(fsrMarkerStyle[fsr_ind]);
			  stack -> Add(hh_fsr[fsr_ind], "P");
			  //				legend -> AddEntry(hh_fsr[fsr_ind], h_fsr[fsr_ind] -> GetTitle(), "p");
			  lentries -> AddAt(new TLegendEntry(hh_fsr[fsr_ind], h_fsr[fsr_ind] -> GetTitle(), "p"), 1);

			  if (data)
			    {
			      hh_fsr_cl[fsr_ind] = (TH1*) hh_fsr[fsr_ind] -> Clone(TString(hh_fsr[fsr_ind] -> GetName()) + "_cl");
			      hh_fsr_cl[fsr_ind] -> Divide(data);
			      p2 -> cd();
			      hh_fsr_cl[fsr_ind] -> Draw("PSAME");
			    }
			  chups -> NormaliseToBinWidth(hh_fsr[fsr_ind]);

			}
		      //			      legend -> AddEntry(hherwigreco, hherwigreco -> GetTitle(), "lp");
		      //  legend -> AddEntry(cflipreco, cflipreco -> GetTitle(), "lp");
		      lentries -> AddAt(new TLegendEntry(hherwigreco, hherwigreco -> GetTitle(), "lp"), 1);
		      if (cflipreco)
			lentries -> AddAt(new TLegendEntry(cflipreco, cflipreco -> GetTitle(), "lp"), 1);
		      canvas -> SaveAs(dir + "/" + canvas -> GetName() + ".png");
		      canvas -> SaveAs(dir + "/" + canvas -> GetName() + ".eps");
		      // if (resultcode == OUT and recocode == GEN)
		      //   {
		      //     TH1 * hsignalGEN = chups -> GetLevel(IN) -> Project(GEN, "signal");
		      //     hsignalGEN -> SetLineColor(kRed);
		      //     p2 -> cd();
		      //     hsignalGEN -> Draw("SAMEHIST");
		      //   }

		    }
		}
	    }
	}
      //    }
  //    }

  return 0;
}
