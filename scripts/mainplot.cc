//To run: 
//   root -l -q 'scripts/mainplot.cc("nominal", "pvmag_par", "NEWOPT", "schmitt")'
// https://root-forum.cern.ch/t/start-root-with-a-root-file-and-script/28635/3
R__ADD_INCLUDE_PATH($CFAT/interface)
R__ADD_INCLUDE_PATH($COMPOUNDHISTOLIB/interface)
R__ADD_INCLUDE_PATH($ROOUNFOLD/src)
#include "CompoundHistoUnfolding.hh"
#include "Definitions.hh"
R__ADD_LIBRARY_PATH($ROOUNFOLD) // if needed
R__LOAD_LIBRARY(libRooUnfold.so)
R__ADD_LIBRARY_PATH($CFAT/lib) // if needed
R__LOAD_LIBRARY(libcfat.so)
R__ADD_LIBRARY_PATH($COMPOUNDHISTOLIB/lib) // if needed
R__LOAD_LIBRARY(libCompoundHisto.so)
#include "TStyle.h"
#include "TROOT.h"
#include "TFile.h"
#include "TLegendEntry.h"
#include "TMath.h"
#include "TKey.h"
#include "TLatex.h"
const unsigned char npsamples(3);
const char * privatesamples[npsamples] = {"dire2002", "sherpa", "herwig7"};
const char * privatesamplesl[npsamples] = {"DIRE 2002", "SHERPA", "POWHEG+HERWIG7"};
const Style_t psampleslinestyle[npsamples] = {2, 9, 10};
const Color_t psamplesc[npsamples] = {6, 46, 38};
const Style_t psamplesmarkerstyle[npsamples] = {27, 28, 29};//kOpenFourTrianglesPlus, kFullFourTrianglesPlus, kOpenCrossX};
string method;
const char * unfoldingmethod = nullptr;
const char * binmethod = nullptr;
void insertheader_values(FILE * file, TH1 * hsample, const char * observable, ChargeCode_t, VectorCode_t);
void insertfooter_values(FILE * file, const char * observable, ChargeCode_t, VectorCode_t);
void insert_principal_values(FILE * file, TH1 * h, TH1 * hunc);
void insert_values(FILE * file, TH1 * object);
const char Nmethods = 3;
bool includecflip;
const char * cflipsuffix[] = {"", "_cflip"};
unsigned char cflip_ind = 2;

int mainplot(const char * methodchar, const char * observable, const char * binmethod, const char * unfoldingmethod, const char * cflip = "false")
{
  method = string(methodchar);
  includecflip = string(cflip) == "true" ? true : false;
  if (includecflip)
    {
      cflip_ind = 1;
    }
  else
    {
      cflip_ind = 0;
    }
  ::unfoldingmethod = unfoldingmethod;
  const char * observables[]  = {"pull_angle", "pvmag", "pvmag_par"};
  unsigned char method_ind = 4;
  if (method == "nominal")
    {
      method_ind = 0;
    }
  if (method == "herwig")
    {
      method_ind = 1;
    }
  if (method == "cflip")
    {
      method_ind = 2;
    }

  const char ** samples[Nmethods] = 
    {
      new const char * [2] {"MC13TeV_TTJets", "MC13TeV_TTJets"},
      new const char * [1] {"MC13TeV_TTJets_herwig"},
      new const char * [1] {"MC13TeV_TTJets_cflip"}
    };

  const char * sampletitles[Nmethods] = {"t#bar{t}", "t#bar{t} herwig", "t#bar{t} cflip"};

  string bindir(binmethod);
  ::binmethod = binmethod;
  const TString dir = TString("common_plots_") + method + "/" + observable + "/" + bindir + "/" + unfoldingmethod + cflipsuffix[cflip_ind];
  system((TString("mkdir - p ") + dir).Data());
  unsigned char opt_level = ORIG;
  if (string(binmethod) != "ORIG")
    {
      opt_level = OPT;
    }
  string bintag("");
  if (opt_level == OPT)
    {
      bintag = bintag + "_" + bindir;
    }
  string unfmtag("");
  
  if (string(unfoldingmethod) != "")
    {
      unfmtag = string("_") + unfoldingmethod;
    }
  printf("method [%s], observable [%s], binmethod [%s], unfolding method [%s]\n", method.c_str(), observable, binmethod, unfoldingmethod);
  
  //  getchar();
  // unsigned char nsamples;
  // if (method.compare("nominal") == 0)
  //   {
  //     //      samples = samples_nominal;
  //     nsamples = 2;
  //   }
  // if (method.compare("cflip") == 0)
  //   {
  //     //samples = samples_cflip;
  //     nsamples = 2;
  //   }
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
      // if (charge_code != 1)
      //   continue;
      for (VectorCode_t jetcode = 0; jetcode < 4; jetcode ++)
	{
	  // if (jetcode != 0)
	  // 	continue;
	  printf("Processing %s %s %s\n", tag_jet_types_[jetcode], tag_charge_types_[charge_code], tag_opt[opt_level]);

	  TFile * fileps = TFile::Open(TString("$EOS/unfolding_") + 
				       method + "/" + 
				       observable + "/" + 
				       bindir + "/" +
				       tag_jet_types_[jetcode] + "_" + 
				       tag_charge_types_[charge_code] + 
				       "_" + samples[method_ind][cflip_ind] + "_" + 
				       observable + "_" + 
				       tag_opt[opt_level] + "_" + 
				       method + bintag + 
				       + "/" + unfoldingmethod + 
				       cflipsuffix[cflip_ind] + "/" + 
				       "save" + unfmtag + ".root", "READ");
	  CompoundHistoUnfolding * chups = (CompoundHistoUnfolding * )((TKey * ) fileps -> GetListOfKeys() -> At(0)) -> ReadObj(); //The principal ch
	  fileps -> Close();
	  TFile * file_herwig = TFile::Open(TString("$EOS/unfolding_") + 
				     "herwig" + "/" +
				     observable + "/" + 
				     bindir + "/" +
				     tag_jet_types_[jetcode] + "_" + 
				     tag_charge_types_[charge_code] + 
				     "_" + samples[1][0] + "_" + 
				     observable + "_" + 
				     tag_opt[opt_level] + "_" + 
				     "herwig" + bintag + 
				     "/" + unfoldingmethod + 
				     + "/save" + unfmtag + ".root", "READ");
	  //	  printf("file %p %s\n", file, file -> GetName()); 
	  CompoundHistoUnfolding * ch_herwig = (CompoundHistoUnfolding * )((TKey * ) file_herwig -> GetListOfKeys() -> At(0)) -> ReadObj();
	  
	  file_herwig -> Close();
	  CompoundHistoUnfolding * ch_cflip = nullptr;
	  if (method == "nominal")
	    {
	      TFile * file_cflip = TFile::Open(TString("$EOS/unfolding_cflip/") + 
					       observable + "/" + 
					       bindir + "/" +
					       tag_jet_types_[jetcode] + "_" + 
					       tag_charge_types_[charge_code] + 
					       "_" +  samples[2][0] + "_" + 
					       observable + "_" + 
					       tag_opt[opt_level] + "_cflip" + 
					       bintag + "/" + unfoldingmethod + 
					       "/save" + unfmtag + ".root", "READ");
	      if (file_cflip)
		{
		  ch_cflip = (CompoundHistoUnfolding * )((TKey * ) file_cflip -> GetListOfKeys() -> At(0)) -> ReadObj();
		  file_cflip -> Close();
		}
	    }

	  if (method == "cflip")
	    {
	      TFile * file_cflip = TFile::Open(TString("$EOS/unfolding_nominal/") + 
					       observable + "/" + 
					       bindir + "/" +
					       tag_jet_types_[jetcode] + "_" + 
					       tag_charge_types_[charge_code] + 
					       "_" +  samples[0][0] + "_" + 
					       observable + "_" + 
					       tag_opt[opt_level] + "_nominal" + 
					       bintag + "/" + unfoldingmethod + 
					       "/save" + unfmtag + ".root", "READ");
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
		  // if (recocode != RECO or resultcode != IN)
		  //    continue;
		  // if (recocode != GEN or resultcode != OUT)
		  //    continue;
		  if (string(unfoldingmethod) == "bayesian" and resultcode == OUT and recocode == RECO)
		    {
		      continue;
		    }
		  unsigned char lpos;
		  printf("---> resultcode %u recocode %u\n", resultcode, recocode);
		  TCanvas *canvas = chups -> CreateCombinedPlot(recocode, resultcode);
		  TH1 * data = nullptr;
		  if (resultcode == IN and recocode == RECO)
		    {
		      data = chups -> GetLevel(resultcode) -> GetHU(DATAMO) -> Project(recocode, "data");
		    }
		  if (resultcode == OUT )
		    {
		      data = chups -> GetLevel(resultcode) -> GetHU(DATAMO) -> Project(recocode, "data");
		      // printf("data\n");
		      // data -> Print("ALL");
		      //getchar();
		    }

		  TH1 * datacl = nullptr;
		  if (data)
		    {
		      datacl = (TH1*) data -> Clone("dataclone");
		      chups -> NormaliseToBinWidth(datacl);
		      datacl -> Scale(1.0/calculatearea(datacl));
		    }
		  canvas -> SetName(TString(tag_jet_types_[jetcode]) + '_' + 
				    tag_charge_types_[charge_code] + '_' + 
				    observable + '_' + 
				    tag_opt[opt_level] + "_" + 
				    tag_recolevel[recocode] + "_" + 
				    tag_resultlevel[resultcode] +
				    unfmtag);
		  TH1 * huncshape = chups -> GetLevel(resultcode) -> GetHU(TOTALMCUNCSHAPE) -> Project(recocode, "totaluncshape");

		  chups -> NormaliseToBinWidth(huncshape);
		  huncshape -> Scale(1.0/calculatearea(huncshape));
		  FILE * file_values = fopen(dir + "/" + canvas -> GetName() + "_values.tex", "w");
		  TH1 * hsignal = chups -> GetLevel(IN)/*level*/ -> GetHU(SIGNAL/*SIGNALPROXY*/, resultcode) -> Project(recocode, "signal");
		  insertheader_values(file_values, hsignal, observable, charge_code, jetcode);
		  insert_principal_values(file_values, datacl, huncshape);

		  hsignal -> SetTitle("POWHEG+PYTHIA 8");
		  chups -> NormaliseToBinWidth(hsignal);
		  hsignal -> Scale(1.0/calculatearea(hsignal));
		  insert_values(file_values, hsignal);		   
		  TPad * p1 = (TPad *) canvas -> GetListOfPrimitives() -> At(0);
		  const double addgap = 0.00;
		  const double hoffset = 0.1;
		  double p1_xlow_old, p1_ylow_old, p1_xup_old, p1_yup_old;
		  p1 -> GetPadPar(p1_xlow_old, p1_ylow_old, p1_xup_old, p1_yup_old);
		  const double p1_height_old = p1_yup_old - p1_ylow_old;
		  const double p1_ylow_new = p1_ylow_old + hoffset + addgap;
		  const double p1_height_new = p1_yup_old - p1_ylow_new;
		  p1 -> SetPad(p1_xlow_old, p1_ylow_new, p1_xup_old, p1_yup_old);
		  p1 -> SetBottomMargin(0.04);
		  TIter next2(p1 -> GetListOfPrimitives());
		  TObject * obj2 = nullptr;
		  TH1 * shape = nullptr;
		  TObject * noshape = nullptr;
		  TGraphErrors * datagraph = nullptr;
		  while (obj2 = next2())
		    {
		      // printf("obj2 %p %s\n", obj2, obj2->GetName());
		      if (TString(obj2 -> GetName()) == TString("OPT_MC13TeV_TTJets_") + method + /*"_" + method*/ + "_totalMCUncgendrawingclone")
			{
			  noshape = obj2;
			}
		      if (TString(obj2 -> GetName()) == TString("OPT_MC13TeV_TTJets_") + method + /*"_" + method*/ + "_totalMCUncShapegendrawingclone")
			{
			  shape = (TH1 *) obj2;
			}
		      if (TString(obj2 -> GetName()) == "Graph_from_OPT_MC13TeV_TTJets_nominal_datadggen")
			{
			  datagraph = (TGraphErrors *) obj2;
			}
		    }
		  //		  printf("%p %p\n", shape, datagraph);
		  if (datagraph)
		    for (unsigned char bind = 1; bind <= shape -> GetNbinsX(); bind ++)
		      {
			datagraph -> SetPointError(bind - 1, 0.0, shape -> GetBinError(bind));
		      }
		  delete shape;
		  delete noshape;
		  //getchar();
		  TIter nextp1(p1 -> GetListOfPrimitives());
		  TObject * objp1 = nullptr;
		  while ((objp1 = nextp1()) and not TString(objp1 -> GetName()).Contains("_hframe"))
		    {
		      //		      printf("%s %s\n", objp1 -> ClassName(), objp1 -> GetName());
		    }
		  TH1 * hframe = (TH1D *) objp1;
		  hframe -> GetYaxis() -> SetTitle(TString(hframe -> GetYaxis() -> GetTitle()) + " #frac{1}{N}");
		  hframe -> GetYaxis() -> SetLabelSize(hframe -> GetYaxis() -> GetLabelSize()*p1_height_old/p1_height_new);
		  hframe -> GetYaxis() -> SetTitleSize(hframe -> GetYaxis() -> GetTitleSize()*p1_height_old/p1_height_new);
		  hframe -> GetYaxis() -> SetTitleOffset(hframe -> GetYaxis() -> GetTitleOffset()*p1_height_new/p1_height_old);
		  
		  const float tlatextextsize = 0.05;
		  TLatex tlatex;
		  tlatex.SetTextSize(tlatextextsize);
		  p1 -> cd();
		  tlatex.DrawLatex(0.05, hframe -> GetMaximum() * 0.9, "work in progress");
		  TPad * p2 = (TPad *) canvas -> GetListOfPrimitives() -> At(1); 
		  double p2_xlow_old, p2_ylow_old, p2_xup_old, p2_yup_old;
		  p2 -> GetPadPar(p2_xlow_old, p2_ylow_old, p2_xup_old, p2_yup_old);
		  const double p2_height_old = p2_yup_old - p2_ylow_old;
		  const double p2_yup_new = p2_yup_old + hoffset;
		  const double p2_height_new = p2_yup_new - p2_ylow_old;
		  p2 -> SetPad(p2_xlow_old, p2_ylow_old, p2_xup_old, p2_yup_new);
		  p2 -> SetTopMargin(0.04);
		  TIter p2iter(p2 -> GetListOfPrimitives());
		  TObject * objp2 = nullptr;
		  TH1 * p2frame = nullptr;
		  TH1 * p2frameshape = nullptr;
		  while (objp2 = p2iter())
		    {
		      if (TString(objp2 -> GetName()) == TString("OPT_") + samples[method_ind][cflip_ind] + "_" + method + "_ratioframe")
			{
			  p2frame = (TH1*) objp2;
			}
		      if (TString(objp2 -> GetName()) == TString("OPT_") + samples[method_ind][cflip_ind] + "_" + method + "_ratioframeshape")
			{
			  p2frameshape = (TH1*) objp2;
			}
		    }
		  p2frame -> GetYaxis() -> SetLabelSize(p2frame -> GetYaxis() -> GetLabelSize() * p2_height_old/p2_height_new);
		  p2frame -> GetYaxis() -> SetTitleSize(p2frame -> GetYaxis() -> GetTitleSize() * p2_height_old/p2_height_new);
		  p2frame -> GetYaxis() -> SetTitleOffset(p2frame -> GetYaxis() -> GetTitleOffset() * p2_height_new/p2_height_old);
  		  p2frame -> GetXaxis() -> SetLabelSize(p2frame -> GetXaxis() -> GetLabelSize() * p2_height_old/p2_height_new);
		  p2frame -> GetXaxis() -> SetTitleSize(p2frame -> GetXaxis() -> GetTitleSize() * p2_height_old/p2_height_new);
		  p2frame -> Reset("ICE");
		  p2frame -> GetYaxis() -> SetRangeUser(0.8, 1.2);
		  //p2frameshape -> Reset("ICE");
		  delete p2frameshape;
		  p2 -> SetBottomMargin(0.3);
		    
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
		  // printf("lentry %p\n", lentry);
		  if (lentry)
		    {
		      // printf("lentry -> GetLabel %s\n", lentry -> GetLabel()); 

		      lentry -> SetOption("pe");
		      // printf("lentry -> GetOption %s\n", lentry -> GetOption());
		    }
		  // p1 -> Modified();
                  // p1 -> Update();
		  legend -> SetX1(legend -> GetX1() - 0.1);
		  legend -> SetMargin(0.32);
		  legend -> SetY1(legend -> GetY1());

		  TH1 * bckgproj = chups -> GetBackgroundTotal() -> Project(recocode);
		  THStack * stack = new THStack("mcnostack", "mcnostack");
		  HistoUnfolding * hherwig = nullptr;//GetLevel(IN/*resultcode*/) -> GetHU(TOTALMCSIGNALPROXY, resultcode);
		  HistoUnfolding * cflip = nullptr; 
		  if (ch_cflip)
		    {
		      cflip = ch_cflip -> GetLevel(IN/*resultcode*/) -> GetHU(TOTALMCSIGNALPROXY, resultcode);
		    }
		  TH1 * hherwigreco = nullptr;
		  TH1 * cflipreco = nullptr;
		  if (true /*resultcode == IN*/)
		    {
		      // hherwig -> GetTH2() -> Add(ch -> GetBackgroundTotal() -> GetTH2());
		      hherwig = ch_herwig -> GetLevel(IN/*resultcode*/) -> GetHU(SIGNAL, resultcode);
		   
		      if (ch_cflip)
			{
			  cflip = ch_cflip -> GetLevel(IN/*resultcode*/) -> GetHU(SIGNAL, resultcode);
			}

		      if (resultcode == OUT and recocode == GEN)
			{
			  hherwigreco = ch_herwig -> GetSignalProxy(OUT, GEN, "MC13TeV_TTJets", THEORSYS, "MC13TeV_TTJets_herwig", false);
			}
		      else
			{
			  hherwigreco = hherwig -> Project(recocode, "projherwig");
			}
		      //		      hherwigreco -> Print("all");
		      if (cflip)
			{
			  cflipreco = cflip -> Project(recocode, "projcflip");
			}
		    }
		  else
		    {
		      hherwigreco = hherwig -> GetTH1(recocode);
		      if (cflip)
			{
			  cflipreco = cflip -> GetTH1(recocode);
			}
		      //hherwigreco -> Add(bckgproj);
		    }
		  hherwigreco -> SetMarkerStyle(kOpenSquare);
		  hherwigreco -> SetMarkerColor(kGreen);
		  hherwigreco -> SetLineColor(kGreen);
		  hherwigreco -> SetLineWidth(0);
		  hherwigreco -> SetLineStyle(kDashed);
		  hherwigreco -> SetTitle("POWHEG+HERWIG++");
		  if (cflipreco)
		    {
		      cflipreco -> SetMarkerStyle(kOpenCircle);
		      cflipreco -> SetMarkerColor(kRed);
		      cflipreco -> SetLineColor(kRed);
		      cflipreco -> SetLineWidth(0);
		      cflipreco -> SetLineStyle(kDashed);
		      cflipreco -> SetTitle("POWHEG+PYTHIA8 cflip");
		    }
		  TH1 * hherwigrecocl = nullptr;
		  TH1 * cfliprecocl = nullptr;
		  if (datacl)
		    {
		      chups -> NormaliseToBinWidth(hherwigreco);
		      hherwigreco -> Scale(1.0/calculatearea(hherwigreco));
		      hherwigrecocl = (TH1 *) hherwigreco -> Clone(TString("hherwigrecocl_") + tag_recolevel[recocode]);
		      //hherwigreco -> Print("ALL");
		      //getchar();
		      printf("probe D0\n");
		      hherwigrecocl -> Divide(datacl);
		      printf("probe D1\n");
		      p2 -> cd();
		      hherwigrecocl -> Draw("PSAME");
		      if (cflipreco)
			{
			  if (ch_cflip)
			    {
			      ch_cflip -> NormaliseToBinWidth(cflipreco);
			      cflipreco -> Scale(1.0/calculatearea(cflipreco));
			      //cflipreco -> Print("all");
			    }
			  cfliprecocl = (TH1 *) cflipreco -> Clone(TString("cfliprecocl_") + tag_recolevel[recocode]);
			  cfliprecocl -> Divide(datacl);
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

		  p1 -> cd();
		  stack -> Draw("nostackSAME");
		  const unsigned char nfsr = 2;
		  HistoUnfolding * h_fsr[nfsr] = 
		    {
		      chups -> GetSys(IN/*resultcode*/, TString(samples[method_ind][cflip_ind]) + "_fsrup", samples[method_ind][cflip_ind]), 
		      chups -> GetSys(IN/*resultcode*/, TString(samples[method_ind][cflip_ind]) + "_fsrdn", samples[method_ind][cflip_ind])
		    };
		  Style_t fsrMarkerStyle[nfsr] = {kOpenTriangleUp, kOpenTriangleDown};
		  TH1 *hh_fsr[nfsr];
		  TH1 *hh_fsr_cl[nfsr] = {nullptr, nullptr};
		  TList * lentries = legend -> GetListOfPrimitives();
		  TIter nextcheck(lentries);
		  TLegendEntry * PP8entry = nullptr;
		  lpos = 0;
		  while ((PP8entry = (TLegendEntry*) nextcheck()) and not (TString(PP8entry -> GetLabel()) == "POWHEG+PYTHIA8" or TString(PP8entry -> GetLabel()) == sampletitles[method_ind]))
		    {
		      lpos ++;
		      printf("label %p\n", PP8entry);
		      printf("label %s\n", PP8entry -> GetLabel());
		    }  
		  if (recocode == GEN and resultcode == OUT)   		 
		    {
		      PP8entry -> SetOption("l");
		    }
		  TIter nextcheck2(lentries);
		  while (TLegendEntry * entry = (TLegendEntry*) nextcheck2())
		    {
		      // printf("entry %p\n", entry);
		      // printf("label %s\n", entry -> GetLabel());
		    }
		  for (unsigned char fsr_ind = 0; fsr_ind < nfsr; fsr_ind ++)
		    {
		      if (resultcode == IN)
			{
			  
			  h_fsr[fsr_ind] -> GetTH2() -> Add(chups -> GetBackgroundTotal() -> GetTH2());
			  // printf("probe D\n");
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
		      hh_fsr[fsr_ind] -> SetTitle(fsrtitles[fsr_ind]);
		      
		      lentries -> AddAt(new TLegendEntry(hh_fsr[fsr_ind], fsrtitles[fsr_ind], "p"), ++lpos);
		      // TIter nextcheck3(lentries);
		      // while (TLegendEntry * entry = (TLegendEntry*) nextcheck3())
		      // 	{
		      // 	  printf("label %s\n", entry -> GetLabel());
		      // 	}

		      chups -> NormaliseToBinWidth(hh_fsr[fsr_ind]);
		      hh_fsr[fsr_ind] -> Scale(1.0/calculatearea(hh_fsr[fsr_ind]));

		      if (datacl)
			{
			  hh_fsr_cl[fsr_ind] = (TH1*) hh_fsr[fsr_ind] -> Clone(TString(hh_fsr[fsr_ind] -> GetName()) + "_cl");
			  hh_fsr_cl[fsr_ind] -> Divide(datacl);
			  p2 -> cd();
			  hh_fsr_cl[fsr_ind] -> Draw("PSAME");
			}
		      insert_values(file_values, hh_fsr[fsr_ind]);		   
		      // printf("probe C %p\n", data);
		      //			      legend -> AddEntry(hherwigreco, hherwigreco -> GetTitle(), "lp");
		      //  legend -> AddEntry(cflipreco, cflipreco -> GetTitle(), "lp");
		    }
		  insert_values(file_values, hherwigreco);		   
		  insert_values(file_values, cflipreco);		   
		  
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
			  CompoundHistoUnfolding * chpr = nullptr;
			  if (f)
			    {
			      chpr = (CompoundHistoUnfolding * )((TKey * ) f -> GetListOfKeys() -> At(0)) -> ReadObj();
			      f -> Close();
			      HistoUnfolding * hupr = chpr -> GetLevel(IN) -> GetInputHU(SYSMO, TString("MC13TeV_TTJets_") + privatesamples[sind], TString(chpr -> GetSignalTag()) + "_" + privatesamples[sind]); 
			      TH2 * h = hupr -> GetTH2();
			      TH1 * hproj = h -> ProjectionY();
			      hproj -> SetDirectory(nullptr);
			      hproj -> SetLineColor(psamplesc[sind]);
			      hproj -> SetLineWidth(2.0);
			      hproj -> SetLineStyle(psampleslinestyle[sind]);
			      hproj -> SetMarkerStyle(psamplesmarkerstyle[sind]);
			      hproj -> SetMarkerColor(psamplesc[sind]);
			      chups -> NormaliseToBinWidth(hproj);
			      hproj -> Scale(1.0/calculatearea(hproj));
			      float integral = 0.0;
			      for (unsigned char bind = 1; bind <= hproj -> GetNbinsX(); bind ++)
				{
				  const float w = hproj -> GetBinWidth(bind);
				  const float c = hproj -> GetBinContent(bind);
				  hproj -> SetBinError(bind, 0.0);
				  integral += w*c;
				  
				}
			      // printf("integral check %f\n", integral);
			      // getchar();
			      TH1 * hratio = nullptr;
			      // printf("recocode %u\n", recocode);
			      if (datacl)
				{
				  hratio = (TH1 *) hproj -> Clone(TString(hproj -> GetName()) + "_ratio");
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
			      hproj -> SetTitle(privatesamplesl[sind]);
			      insert_values(file_values, hproj);		   
			      
			      lentries -> AddAt(new TLegendEntry(hproj, privatesamplesl[sind], "lp"), ++ lpos);
			    }
			}
		    }
		  
		  if (datacl)
		    delete datacl;
		  delete hsignal;
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
		  insertfooter_values(file_values, observable, charge_code, jetcode);
		  fclose(file_values);
		  canvas -> SaveAs(dir + "/" + canvas -> GetName() + ".png");
		  canvas -> SaveAs(dir + "/" + canvas -> GetName() + ".eps");
		  //TCanvas * stabpur = chups -> stabpur();
		  
		  //		  stabpur -> SaveAs(dir + "/stabpur_" + TString(canvas -> GetName()).ReplaceAll("_gen_out", "") + ".png");
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

void insertheader_values(FILE * file, TH1 * hsample, const char * observable, ChargeCode_t charge_code, VectorCode_t jet_code)
{
  fprintf(file, "\\begin{table}[h]\n");
  fprintf(file, "\\centering\\small\n");
  fprintf(file, "\\footnotesize\n");
  fprintf(file, "\t\\begin{tabular}{l");
  for (unsigned char bind = 0; bind < hsample -> GetNbinsX(); bind ++)
    {
      fprintf(file, "%s", "l");
    }
  fprintf(file, "}\n");
  fprintf(file, "\t\t\\multirow{2}{*}{Sample} & \\multicolumn{%u}{c}{Bins}\\\\\n", hsample -> GetNbinsX());
  for (unsigned char bind = 1; bind <= hsample -> GetNbinsX(); bind ++)
    {
      fprintf(file, "\t\t & %.2f \\textendash\\ %.2f", hsample -> GetBinLowEdge(bind), hsample -> GetXaxis() -> GetBinUpEdge(bind));
    }
  fprintf(file, "\\\\\n");
  fprintf(file, "\t\t\\hline\n");

}

void insertfooter_values(FILE * file, const char * observable, ChargeCode_t charge_code, VectorCode_t jet_code)
{
  fprintf(file, "\t\t\\hline\n");
  fprintf(file, "\t\\end{tabular}\n");
  fprintf(file, "\t\\label{tab:value_table_%s_%s_%s_%s_%s%s}\n", 
	  method.c_str(), 
	  observable, 
	  tag_charge_types_[charge_code], 
	  tag_jet_types_[jet_code], 
	  unfoldingmethod,
	  cflipsuffix[cflip_ind]);
  const TString caption = TString("Unfolding results") + (includecflip ? " including the colour flip systematic " : " ") + 
    "of \\protect\\observabletitle{" + observable + 
    "} (\\protect\\jettitle{" + tag_jet_types_[jet_code] + 
    "}, \\protect\\countertitle{" + tag_jet_types_[jet_code] + 
    "}) including \\protect\\chargetitle{" + tag_charge_types_[charge_code] + 
    "}. The binning scheme is \\protect\\binningtitle{" + ::binmethod + 
    "}. The results are for the \\protect\\modeltitle{" + method.c_str() + 
    "} model using \\protect\\unfoldingmethodtitle{" + ::unfoldingmethod + "}.";
  fprintf(file, "\t\\caption{%s}\n", caption.Data());
  fprintf(file, "\\end{table}");
}

void insert_principal_values(FILE * file, TH1 * h, TH1 *hunc)
{
  if (h == nullptr)
    return;
  fprintf(file, "\t\t%s", (TString(h -> GetTitle()).ReplaceAll("t#bar{t}", "\\ttbar") + " $\\pm$ stat $\\pm$ syst").Data());
  for (unsigned char bind = 1; bind <= h -> GetNbinsX(); bind ++)
    {
      fprintf(file, " & %.2f $\\pm$ %.2f $\\pm$ %.2f", h -> GetBinContent(bind), h -> GetBinError(bind), TMath::Sqrt(TMath::Power(hunc -> GetBinError(bind), 2) - TMath::Power(h -> GetBinError(bind), 2)));
    }
  fprintf(file, "\\\\\n");
	   
}

void insert_values(FILE * file, TH1 * h)
{
  fprintf(file, "\t\t%s", TString(h -> GetTitle()).ReplaceAll("t#bar{t}", "\\ttbar").Data());
  for (unsigned char bind = 1; bind <= h -> GetNbinsX(); bind ++)
    {
      fprintf(file, " & %.2f", h -> GetBinContent(bind));
    }
  fprintf(file, "\\\\\n");

}
