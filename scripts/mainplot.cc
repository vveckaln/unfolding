//To run: 
//   root -l -q 'scripts/mainplot.cc("nominal", "pull_angle", "ATLAS3")'
// https://root-forum.cern.ch/t/start-root-with-a-root-file-and-script/28635/3
R__ADD_INCLUDE_PATH($COMPOUNDHISTOLIB/interface)
R__ADD_INCLUDE_PATH($ROOUNFOLD/src)
#include "CompoundHistoUnfolding.hh"
R__ADD_LIBRARY_PATH($ROOUNFOLD) // if needed
R__LOAD_LIBRARY(libRooUnfold.so)
R__ADD_LIBRARY_PATH($COMPOUNDHISTOLIB/lib) // if needed
R__LOAD_LIBRARY(libCompoundHisto.so)
const unsigned char npsamples(3);
const char * privatesamples[npsamples] = {"dire2002", "sherpa", "herwig7"};
const char * privatesamplesl[npsamples] = {"DIRE 2002", "SHERPA", "POWHEG+HERWIG7"};
const Style_t psamplesst[npsamples] = {2, 9, 10};
const Color_t psamplesc[npsamples] = {6, 46, 38};
const Style_t psamplesm[npsamples] = {27, 28, 29};//kOpenFourTrianglesPlus, kFullFourTrianglesPlus, kOpenCrossX};
int mainplot(const char * methodchar, const char * observable, const char * binmethod)
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
      if (charge_code != 1)
        continue;
      for (VectorCode_t jetcode = 0; jetcode < 4; jetcode ++)
	{
	  if (jetcode != 0)
	  	continue;
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
		}
	    }
	  for(ResultLevelCode_t resultcode = 0; resultcode < 2; resultcode ++)
	    //  chups -> GetLevel(OUT) -> GetHURef(SIGNALMO) = chups -> GetLevel(IN) -> GetHU(SIGNALMO);
	    //ResultLevelCode_t resultcode = IN;

	    {
	      for(RecoLevelCode_t recocode = 0; recocode < 2; recocode ++)
		//RecoLevelCode_t recocode = RECO;
		{ 
		  if (charge_code != 1 or jetcode !=0)// or recocode != GEN or resultcode != OUT)
		    continue;
		  unsigned char lpos;
		  printf("resultcode %u recocode %u\n", resultcode, recocode);
		  TCanvas *canvas = chups -> CreateCombinedPlot(recocode, resultcode);
		  printf("combined plot created\n");
		  canvas -> SetName(TString(tag_jet_types_[jetcode]) + '_' + 
				    tag_charge_types_[charge_code] + '_' + 
				    observable + '_' + 
				    tag_opt[opt_level] + "_" + 
				    tag_recolevel[recocode] + "_" + 
				    tag_resultlevel[resultcode]);
			  
		  TPad * p1 = (TPad *) canvas -> GetListOfPrimitives() -> At(0);
		  TIter nextp1(p1 -> GetListOfPrimitives());
		  TObject * objp1 = nullptr;
		  while ((objp1 = nextp1()) and not TString(objp1 -> GetName()).Contains("_hframe"))
		    {
		      printf("%s %s\n", objp1 -> ClassName(), objp1 -> GetName());
		    }
		  TH1D * hframe = (TH1D *) objp1;
		  hframe -> GetYaxis() -> SetTitle(TString(hframe -> GetYaxis() -> GetTitle()) + " #frac{1}{N}");
		  TPad * p2 = (TPad *) canvas -> GetListOfPrimitives() -> At(1); 
		  TLegend * legend = nullptr;
		  TObject * obj = nullptr;
		  TIter next(p1 -> GetListOfPrimitives());
		  while ((obj = next()) and TString(obj -> ClassName()) != "TLegend")
		    {
		    }
		  legend = (TLegend *)obj;
		  TLegendEntry * lentry = nullptr;
		  next = TIter(legend -> GetListOfPrimitives());

		  while ((lentry = (TLegendEntry*) next()) and not TString(lentry -> GetLabel()).Contains("data"))
		    {

		    }
		  printf("lentry %p\n", lentry);
		  if (lentry)
		    {
		      printf("lentry -> GetLabel %s\n", lentry -> GetLabel()); 

		      lentry -> SetOption("pe");
		      printf("lentry -> GetOption %s\n", lentry -> GetOption());
		    }
		  // p1 -> Modified();
                  // p1 -> Update();
		  legend -> SetX1(legend -> GetX1() - 0.1);
		  legend -> SetMargin(0.32);
		  legend -> SetY1(legend -> GetY1());
		  TH1 * data = nullptr;
		  if (resultcode == IN and recocode == RECO)
		    {
		      data = chups -> GetLevel(resultcode) -> GetHU(DATAMO) -> Project(recocode, "data");
		    }
		  if (resultcode == OUT )
		    {
		      data = chups -> GetLevel(resultcode) -> GetHU(DATAMO) -> Project(recocode, "data");
		      printf("data\n");
		      data -> Print("ALL");
		      //getchar();
		    }

		  TH1F * datacl = nullptr;
		  if (data)
		    {
		      datacl = (TH1F*) data -> Clone("dataclone");
		      chups -> NormaliseToBinWidth(datacl);
		      datacl -> Scale(1.0/calculatearea(datacl));
		    }

		  TH1 * bckgproj = ch -> GetBackgroundTotal() -> Project(recocode);
		  THStack * stack = new THStack("mcnostack", "mcnostack");
		  HistoUnfolding * hherwig = ch -> GetLevel(IN/*resultcode*/) -> GetHU(TOTALMCSIGNALPROXY, resultcode);
		  HistoUnfolding * cflip = nullptr; 
		  if (ch_cflip)
		    cflip = ch_cflip -> GetLevel(IN/*resultcode*/) -> GetHU(TOTALMCSIGNALPROXY, resultcode);
		  TH1 * hherwigreco = nullptr;
		  TH1 * cflipreco = nullptr;
		  if (true /*resultcode == IN*/)
		    {
		      // hherwig -> GetTH2() -> Add(ch -> GetBackgroundTotal() -> GetTH2());
		  hherwig = ch -> GetLevel(IN/*resultcode*/) -> GetHU(SIGNAL, resultcode);
		   
		  if (ch_cflip)
		    cflip = ch_cflip -> GetLevel(IN/*resultcode*/) -> GetHU(SIGNAL, resultcode);

		      hherwigreco = hherwig -> Project(recocode, "projherwig");
		      if (cflip)
			cflipreco = cflip -> Project(recocode, "projcflip");
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
		  // printf("hherwigreco\n");
		  // hherwigreco ->Print("ALL");
		  chups -> NormaliseToBinWidth(hherwigreco);
		  hherwigreco -> Scale(1.0/calculatearea(hherwigreco));
		  		      hherwigreco -> Print("ALL");
		  //getchar();
		  if (ch_cflip)
		    {
		      ch_cflip -> NormaliseToBinWidth(cflipreco);
		      cflipreco -> Scale(1.0/calculatearea(cflipreco));
		      //cflipreco -> Print("all");
		    }
		  p1 -> cd();
		  stack -> Draw("nostackSAME");
		  const unsigned char nfsr = 2;
		  HistoUnfolding * h_fsr[nfsr] = 
		    {
		      chups -> GetSys(IN/*resultcode*/, "MC13TeV_TTJets_fsrup", "MC13TeV_TTJets"), 
		      chups -> GetSys(IN/*resultcode*/, "MC13TeV_TTJets_fsrdn", "MC13TeV_TTJets")
		    };
		  Style_t fsrMarkerStyle[nfsr] = {kOpenTriangleUp, kOpenTriangleDown};
		  TH1 *hh_fsr[nfsr];
		  TH1 *hh_fsr_cl[nfsr] = {nullptr, nullptr};
		  TList * lentries = legend -> GetListOfPrimitives();
		  TIter nextcheck(lentries);
		  TLegendEntry * PP8entry = nullptr;
		  lpos = 0;
		  while ((PP8entry = (TLegendEntry*) nextcheck()) and not (TString(PP8entry -> GetLabel()) == "POWHEG+PYTHIA8" or TString(PP8entry -> GetLabel()) == "t#bar{t}"))
		    {
		      lpos ++;
		      printf("label %p\n", PP8entry);
		      printf("label %s\n", PP8entry -> GetLabel());
		    }  
		  printf("PP8 label [%s] lpos %u\n", PP8entry -> GetLabel(), lpos);
		  if (recocode == GEN and resultcode == OUT)   		 
		    PP8entry -> SetOption("l");
		  TIter nextcheck2(lentries);
		  while (TLegendEntry * entry = (TLegendEntry*) nextcheck2())
		    {
		      printf("entry %p\n", entry);
		      printf("label %s\n", entry -> GetLabel());
		    }
		  for (unsigned char fsr_ind = 0; fsr_ind < nfsr; fsr_ind ++)
		    {
		      printf("probe C resultcode %u\n", resultcode);
		      if (resultcode == IN)
			{
			  
			  h_fsr[fsr_ind] -> GetTH2() -> Add(ch -> GetBackgroundTotal() -> GetTH2());
			  printf("probe D\n");
			  //hh_fsr[fsr_ind] = h_fsr[fsr_ind] -> Project(recocode, TString(h_fsr[fsr_ind] -> GetTag()));
			}
		      else
			{
			  // hh_fsr[fsr_ind] = h_fsr[fsr_ind] -> GetTH1(recocode);
			  // hh_fsr[fsr_ind] -> Add(bckgproj);
			}
		      /*!*/
		      hh_fsr[fsr_ind] = h_fsr[fsr_ind] -> Project(recocode, TString(h_fsr[fsr_ind] -> GetTag()));
		      /*!*/
		      hh_fsr[fsr_ind] -> SetLineWidth(0.0);
		      hh_fsr[fsr_ind] -> SetMarkerColor(kBlack);
		      hh_fsr[fsr_ind] -> SetMarkerStyle(fsrMarkerStyle[fsr_ind]);
		      stack -> Add(hh_fsr[fsr_ind], "P");
		      //				legend -> AddEntry(hh_fsr[fsr_ind], h_fsr[fsr_ind] -> GetTitle(), "p");
		      //		      printf("adding %s\n", h_fsr[fsr_ind] -> GetTitle());
		      const char * fsrtitles[2] = {"PP8 FSR up", "PP8 FSR down"};
		      
		      lentries -> AddAt(new TLegendEntry(hh_fsr[fsr_ind], fsrtitles[fsr_ind], "p"), ++lpos);
		      // TIter nextcheck3(lentries);
		      // while (TLegendEntry * entry = (TLegendEntry*) nextcheck3())
		      // 	{
		      // 	  printf("label %s\n", entry -> GetLabel());
		      // 	}


		      if (data)
			{
			  hh_fsr_cl[fsr_ind] = (TH1*) hh_fsr[fsr_ind] -> Clone(TString(hh_fsr[fsr_ind] -> GetName()) + "_cl");
			  hh_fsr_cl[fsr_ind] -> Divide(data);
			  p2 -> cd();
			  hh_fsr_cl[fsr_ind] -> Draw("PSAME");
			}
		      chups -> NormaliseToBinWidth(hh_fsr[fsr_ind]);
		      hh_fsr[fsr_ind] -> Scale(1.0/calculatearea(hh_fsr[fsr_ind]));
		      // printf("probe C %p\n", data);
		      //			      legend -> AddEntry(hherwigreco, hherwigreco -> GetTitle(), "lp");
		      //  legend -> AddEntry(cflipreco, cflipreco -> GetTitle(), "lp");
		    }
		  
		  lentries -> AddAt(new TLegendEntry(hherwigreco, "POWHEG+HERWIG++", "lp"), ++lpos);
		  if (cflipreco)
		    {
		      
		      lentries -> AddAt(new TLegendEntry(cflipreco, "POWHEG+PYTHIA8 cflip", "lp"), ++lpos);
		    }
		  if (recocode == GEN)
		    {
		      for (unsigned char sind = 0; sind < npsamples; sind ++)
			{
			  TFile *f = TFile::Open(TString("$EOS/unfolding_") + 
						 privatesamples[sind] + "/" +
						 observable + "/" + 
						 bindir + "/" +
						 tag_jet_types_[jetcode] + "_" + 
						 tag_charge_types_[charge_code] + 
						 "_MC13TeV_TTJets_" + privatesamples[sind] + "_" + 
						 observable + "_" + 
						 tag_opt[opt_level] + "_" + 
						 privatesamples[sind] + bintag 
						 + "/save.root", "READ");
			  printf("%s\n", f -> GetName());
			  CompoundHistoUnfolding * chpr = nullptr;
			  if (f)
			    {
			      chpr = (CompoundHistoUnfolding * )((TKey * ) f -> GetListOfKeys() -> At(0)) -> ReadObj();
			      f -> Close();
			      printf("probe A %s\n", chpr -> GetSignalTag());
			      HistoUnfolding * hupr = chpr -> GetLevel(IN) -> GetInputHU(SYSMO, TString("MC13TeV_TTJets_") + privatesamples[sind], TString(chpr -> GetSignalTag()) + "_" + privatesamples[sind]); 
			      printf("probe B %p\n", hupr);
			      TH2 * h = hupr -> GetTH2();
			      TH1D * hproj = h -> ProjectionY();
			      hproj -> SetDirectory(nullptr);
			      hproj -> SetLineColor(psamplesc[sind]);
			      hproj -> SetLineWidth(2.0);
			      hproj -> SetLineStyle(psamplesst[sind]);
			      hproj -> SetMarkerStyle(psamplesm[sind]);
			      hproj -> SetMarkerColor(psamplesc[sind]);
			      chups -> NormaliseToBinWidth(hproj);
			      hproj -> Scale(1.0/calculatearea(hproj));
			      printf("%s integral %f\n", hproj -> Integral(), privatesamples[sind]);
			      float integral = 0.0;
			      for (unsigned char bind = 1; bind <= hproj -> GetNbinsX(); bind ++)
				{
				  const float w = hproj -> GetBinWidth(bind);
				  const float c = hproj -> GetBinContent(bind);
				  hproj -> SetBinError(bind, 0.0);
				  integral += w*c;
				  printf("%f %f\n", w, c);
				  
				}
			      // printf("integral check %f\n", integral);
			      // getchar();
			      TH1F * hratio = nullptr;
			      // printf("recocode %u\n", recocode);
			      if (datacl)
				{
				  hratio = (TH1F *) hproj -> Clone(TString(hproj -> GetName()) + "_ratio");
				  hratio -> SetDirectory(nullptr);
				  hratio -> Divide(datacl);
				  p2 -> cd();
				  hratio -> Draw("SAME");
				  // printf("printing sample ratio %s\n", privatesamples[sind]);
				  // hratio -> Print("all");
				  // getchar();
				  
				}
			      p1 -> cd();
			      hproj -> Draw("SAMEHIST");
			      hproj -> Draw("SAMEPHIST");
			      lentries -> AddAt(new TLegendEntry(hproj, privatesamplesl[sind], "lp"), ++ lpos);
			    }
			}
		    }
		  if (datacl)
		    delete datacl;

		  // if (data)
		  // 	{
		  // 	  chups -> NormaliseToBinWidth(data);
		  // 	  data -> Scale(1.0/calculatearea(data));
		  // 	}
		    
		  // printf("another check\n");
		  // TIter nextcheck2(lentries);
		  // while (TLegendEntry * entry = (TLegendEntry*) nextcheck2())
		  //   {
		  //     printf("label %s\n", entry -> GetLabel());
		  //   }

		  canvas -> SaveAs(dir + "/" + canvas -> GetName() + ".png");
		  canvas -> SaveAs(dir + "/" + canvas -> GetName() + ".eps");
		  //		  getchar();

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

  return 0;
}
