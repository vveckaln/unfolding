//To run: 
//   root -l -q 'scripts/migrationmatrix.cc("nominal", "pull_angle", "ATLAS3")'
// https://root-forum.cern.ch/t/start-root-with-a-root-file-and-script/28635/3
R__ADD_INCLUDE_PATH($COMPOUNDHISTOLIB/interface)
#include "CompoundHistoUnfolding.hh"
R__ADD_LIBRARY_PATH($COMPOUNDHISTOLIB/lib) // if needed
R__LOAD_LIBRARY(libCompoundHisto.so)
TString periods[2] = {"BCDEF", "GH"};
const char * leptons[2] = {"E", "M"};
void processth2(TH2 * th2);
int migrationmatrix()
{
  system("mkdir -p $UNFOLDINGLIB/migrationmatrices");



  const unsigned char Number = 8;                                                                                                                                               
  
  Int_t colours[4] = {kGreen -10, kGreen -6, kGreen + 1, kGreen - 2}; 
  Double_t Red[Number];//   =  {0.7,  0.7,   0.6,       0.6,   0.4,     0.4,   0.3,       0.3};                                                                                     
  Double_t Green[Number];// =  {1.0,  1.0,   0.9,       0.9,   0.7,     0.7,   0.6,       0.6};                                                                                     
  Double_t Blue[Number];//  =  {0.7,  0.7,   0.6,       0.6,   0.4,     0.4,   0.3,       0.3};                                                                                     
  float red[4], green[4], blue[4];
  for (unsigned char cind = 0; cind < 4; cind ++)
    {
      TColor *color = gROOT -> GetColor(colours[cind]);
      color -> GetRGB(red[cind], green[cind], blue[cind]);
      Red[2 * cind] = red[cind];
      Red[2 * cind + 1] = red[cind];
      Blue[2*cind] = blue[cind];
      Blue[2*cind + 1] = blue[cind];
      Green[2 * cind] = green[cind];
      Green[2 * cind + 1] = green[cind];

    }
			 
 
  Double_t Stops[Number] =  {0.0 , 0.200, 0.200001,  0.400, 0.40001,  0.600,   0.6000001,  1.000};     
  //  gStyle -> SetPalette(n, colours);
  TColor::CreateGradientColorTable(Number, Stops, Red, Green, Blue, 2*Number-1);
  gStyle -> SetPaintTextFormat("2.2f");
  gStyle -> SetOptStat(0);
  gStyle -> SetOptTitle(0);
  // TH2 * test = new TH2F("test", "test", 2, -0.5, 1.5, 2, -0.5, 1.5);
  // test -> Fill(0.0, 0.0, 75.0);
  // test -> Fill(0.0, 1.0, 15.0);
  // test -> Fill(1.0, 0.0, 55.0);
  // test -> Fill(1.0, 1.0, 25.0);
  // TCanvas * ctest = new TCanvas;
  // test -> Draw("COLZ");
  // printf("test drawn\n");
  // getchar();
  const char *binfilename = "binfiles/binsATLAS4_pull_angle.txt";
  FILE * binfile = fopen(binfilename, "r");
  unsigned long size;
  fscanf(binfile, "%lu\n", &size);
  float * ybins = new float[size];
  int r = 0;
  unsigned char ind = 0;
  while (r >= 0)
    {
      float store;
      r = fscanf(binfile, "%f\n", &store);
      printf("%f\n", store);
      if (r >= 0)
	{
	  ybins[ind] = store;
	}
      ind ++;
    }
  fclose(binfile);
  for (unsigned char bind = 0; bind < size; bind ++)
    {
      printf("bind %u %f\n", bind , ybins[bind]);
    }
  const char * observable = "pull_angle";
  float novalue_proxy(0.0);
  if (string(observable).compare("pull_angle") == 0)
    {
      novalue_proxy = -10.0;
    }
  if (string(observable).compare("pvmag") == 0)
    {
      novalue_proxy = - 1.0;
    }

  TH2 * mtotal[2];
  const unsigned char jetcode = LEADING_JET;
  // for (unsigned char leptind = 0; leptind < 2; leptind ++)
  //   {
  //     //TH2F * mltotal = new TH2F(TString("mtotal_") + leptons[leptind], TString("mtotal_" ) + leptons[leptind] + "; reco; gen", size - 1, ybins, size - 1, ybins);
  //     TH2F * mltotal = new TH2F(TString("mtotal_") + leptons[leptind], TString("mtotal_" ) + leptons[leptind] + "; reco; gen", 4, 0.0, 1.0, 4, 0.0, 1.0);
      
  //     map<TString, TH2 *> th2map;
  //     // th2map[periods[0]] = new TH2F(TString("m") + "_" + leptons[leptind] + "_" + periods[0], TString("m") + "_" + leptons[leptind] + "_" + periods[0] + "; reco; gen", size - 1, ybins, size - 1, ybins);
  //     // th2map[periods[1]] = new TH2F(TString("m") + "_" + leptons[leptind] + "_" + periods[1], TString("m") + "_" + leptons[leptind] + "_" + periods[1] + "; reco; gen", size - 1, ybins, size - 1, ybins);
  //     th2map[periods[0]] = new TH2F(TString("m") + "_" + leptons[leptind] + "_" + periods[0], TString("m") + "_" + leptons[leptind] + "_" + periods[0] + "; reco; gen", 4, 0.0, 1.0, 4, 0.0, 1.0);
  //     th2map[periods[1]] = new TH2F(TString("m") + "_" + leptons[leptind] + "_" + periods[1], TString("m") + "_" + leptons[leptind] + "_" + periods[1] + "; reco; gen", 4, 0.0, 1.0, 4, 0.0, 1.0);
  //   }
  Float_t E_reco[2], M_reco[2];
  Float_t E_gen[2], M_gen[2];
  TString * period = nullptr;
  WeightMap * wmap = nullptr;
      

  TChain chain("migration");
  //      chain.Add(TString("$EOS/analysis_MC13TeV_TTJets/migration/MC13TeV_TTJets/migration_MC13TeV_TTJets_0.root/") + leptons[leptind] + "_chconst_leading_jet_migration");
  chain.Add("$EOS/analysis_MC13TeV_TTJets/HADDmigration/migration_MC13TeV_TTJets.root/migration");
  for (unsigned char chargecode = 0; chargecode < 2; chargecode ++)
    {
      chain.SetBranchAddress(TString("E") + "_" + observable + "_" + tag_jet_types_[jetcode] + "_" + tag_charge_types_[chargecode] + "_" + "reco", & E_reco[chargecode]);
      chain.SetBranchAddress(TString("M") + "_" + observable + "_" + tag_jet_types_[jetcode] + "_" + tag_charge_types_[chargecode] + "_" + "reco", & M_reco[chargecode]);
      chain.SetBranchAddress(TString("E") + "_" + observable + "_" + tag_jet_types_[jetcode] + "_" + tag_charge_types_[chargecode] + "_" + "gen",  & E_gen[chargecode]);
      chain.SetBranchAddress(TString("M") + "_" + observable + "_" + tag_jet_types_[jetcode] + "_" + tag_charge_types_[chargecode] + "_" + "gen",  & M_gen[chargecode]);
      mtotal[chargecode] = new TH2F(TString("mtotal_") + tag_charge_types_[chargecode], TString("mtotal; reco; gen"), size - 1, ybins, size - 1, ybins);
      mtotal[chargecode] -> SetMinimum(0.0);
      mtotal[chargecode] -> SetMaximum(0.0);
      mtotal[chargecode] -> GetZaxis() -> SetTitle("%");
      mtotal[chargecode] -> GetZaxis() -> SetTitleOffset(1.2);
      
    }
  chain.SetBranchAddress("weight", & wmap);
  chain.SetBranchAddress("period", & period);


  using namespace std;
  const unsigned char dbcut(1);
  
  for (unsigned long event_ind = 0; event_ind < chain.GetEntries()/dbcut; event_ind ++)
    {
      if (event_ind % 10000 == 0)
	{
	  printf("%u\r", event_ind);
	}
      chain.GetEntry(event_ind);
	  
      if ((*wmap) -> size() == 0)
	{
	  printf("weights -> size() == 0 !!!!\n");
	}
      // printf("%p size %lu ", weights, weights -> size() );
      // printf("%lu %.9f\n", event_ind,  (*weights)[0]);
      // printf("observable %s novalue_proxy %f\n", observable, novalue_proxy);
      // printf("E_reco %f, M_reco %f E_gen %f M_gen %f\n", E_reco, M_reco, E_gen, M_gen);
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
      if (string(observable).compare("pull_angle") == 0)
	{
	  h -> GetTH2() -> Fill(reco == novalue_proxy ? reco : TMath::Abs(reco)/TMath::Pi(), gen == novalue_proxy ? gen : TMath::Abs(gen)/TMath::Pi(), weight0);
	}
	  
      // if (string(h -> GetTag()).compare("MC13TeV_TTJets_tracking_up") == 0)
      //   {
      //     printf("%lu %f %f\n", event_ind, reco, gen);
      //     //     if (reco != -10 and (reco < - ;
      //   }
      if (string(observable).compare("pvmag") == 0)
	{
	  h -> GetTH2() -> Fill(reco, gen, weight0);
	}
      //      _th2 -> Fill(pull_angle_reco == -10 ? pull_angle_reco : pull_angle_reco, pull_angle_gen == -10.0 ? pull_angle_gen : pull_angle_gen, weight);
	  
    }
  printf("Filled %lu events\n", nevents);

  AddHisto(h);


  // if (TString(h -> GetTag()) == "MC13TeV_TTJets_tracking_up")
  // 	{
  // 	  printf("printing nongen of MC13TeV_TTJets_tracking_up\n");
  // 	  TH1 *htest = h -> GetTH2() -> ProjectionX("proj", 0, 0);
  // 	  htest -> Print("all");
  // 	  delete htest;
  // 	  getchar();
  // 	}

  // if (TString(parser.GetSample(sind) -> GetTag()) == "MC13TeV_TTJets_herwig")
  // 	{
  // 	  printf("~~~~~~~~~~~~ after loading MC13TeV_TTJets_herwig\n");
  // 	  h -> GetTH2() -> ProjectionX("test") -> Print ("all");
  // 	  getchar();
  // 	}
  bool nondedicated_end =  false;
  // if (string(parser.GetSample(sind)->GetTag()).compare(signaltag) == 0)
  // 	{
  // 	  TCanvas * cstabpur = stabpur();
  // 	  cstabpur -> SaveAs("output/stabpurtest.png");
  // 	  delete cstabpur;
  // 	  getchar();
  // 	}
  if (string(parser.GetSample(sind) -> GetTag()).compare(signaltag) == 0 and not calculate_bins)
    {
      //	  	  continue;
      vector<HistoUnfoldingTH2*> nondedicatedth2d;
      static const unsigned char Nnondedicated = 21;
      static const char * nondedicated_titles[Nnondedicated] = 
	{
	  "pileup up", "pileup down",
	  "trig. efficiency correction up", "trig. efficiency correction down",
	  "sel. efficiency correction up", "sel. efficiency correction down",
	  "b fragmentation up", "b fragmentation down",
	  "Peterson frag.", 
	  "semilep BR up", "semilep BR down", 
	  "top $p_{T}$",
	  "id1002 $#mu_{R}=1.0$ $#mu_{F}=2.0$ $h_{damp}$ $m_{t}=272.7225$", 
	  "id1003 $#mu_{R}=1.0$ $#mu_{F}=0.5$ $h_{damp}$ $m_{t}=272.7225$", 
	  "id1004 $#mu_{R}=2.0$ $#mu_{F}=1.0$ $h_{damp}$ $m_{t}=272.7225$", 
	  "id1005 $#mu_{R}=2.0$ $#mu_{F}=2.0$ $h_{damp}$ $m_{t}=272.7225$", 
	  "id1007 $#mu_{R}=0.5$ $#mu_{F}=1.0$ $h_{damp}$ $m_{t}=272.7225$", 
	  "id1009 $#mu_{R}=0.5$ $#mu_{F}=0.5$ $h_{damp}$ $m_{t}=272.7225$", 
	  "id3001PDFset13100", "id4001PDFset25200id1010",  ""
	};
      static const char * nondedicated_names[Nnondedicated] = 
	{
	  "pileup_up", "pileup_down",
	  "trig_efficiency_correction_up", "trig_efficiency_correction_down",
	  "sel_efficiency_correction_up", "sel_efficiency_correction_down",
	  "b_fragmentation_up", "b_fragmentation_down",
	  "Peterson_frag", 
	  "semilep_BR_up", "semilep_BR_down", 
	  "top_pt",
	  "id1002muR1muF2hdampmt272.7225", "id1003muR1muF0.5hdampmt272.7225", "id1004muR2muF1hdampmt272.7225", "id1005muR2muF2hdampmt272.7225", "id1007muR0.5muF1hdampmt272.7225", "id1009muR0.5muF0.5hdampmt272.7225", 
	  "id3001PDFset13100", "id4001PDFset25200", ""
	};
      static const char * nondedicated_categories[Nnondedicated] = 
	{
	  "Pile-up", "Pile-up",
	  "Trigger efficiency", "Trigger efficiency",
	  "Selection efficiency", "Selection efficiency",
	  "b-fragmentation", "b-fragmentation",
	  "Peterson fragmentation", 
	  "Semilepton branching ratios", "Semilepton branching ratios", 
	  "top pt",
	  "QCD scales", "QCD scales", "QCD scales", "QCD scales", "QCD scales", "QCD scales", 
	  "PDF", "PDF", ""
	};

      for (unsigned int ind = 1; ind <= windmap.size(); ++ ind)
	{
	  unsigned char nondedicated_ind = ind + 1;
	  printf("%s\n", nondedicated_names[nondedicated_ind]);
	  HistoUnfoldingTH2 *h = nullptr;
	  if (IsRegular())
	    {
	      h = new HistoUnfoldingTH2(GetName(), _XaxisTitle, _nbinsx, _xlow, _xup, _nbinsy, _ylow, _yup, parser.GetSample(sind)); 
	      h -> GetTH2() -> GetZaxis() -> SetTitle("N");
	      h -> GetTH2() -> RebinY(2);
	    }
	  else
	    {
	      h = new HistoUnfoldingTH2(GetName(), _XaxisTitle, _nentriesx - 1, _xbins, _nentriesy - 1, _ybins, parser.GetSample(sind));
	    }
	  h -> SetTag(TString(h -> GetTag()) + "_" + nondedicated_names[nondedicated_ind]);
	  h -> GetTH2() -> SetName(nondedicated_names[nondedicated_ind]);
	  h -> GetTH2() -> SetTitle(nondedicated_titles[nondedicated_ind]);
	  h -> SetTitle((TString("t#bar{t} ") + nondedicated_titles[nondedicated_ind]).Data());
	  h -> SetCategory(TString(nondedicated_categories[nondedicated_ind]).Data());
	  h -> SetSysType(EXPSYS);
	  h -> SetSample(signaltag);
	  unsigned long nevents = 0;
	  for (unsigned long event_ind = 0; event_ind < chain.GetEntries()/dbcut; event_ind ++)
	    {
	      if (event_ind % 10000 == 0)
		{
		  printf("%u\r", event_ind);
		}
	      chain.GetEntry(event_ind);
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
	      if (nondedicated_ind + 1 > (*wmap) -> size() - 1)
		{
		  nondedicated_end = true;
		  delete h;
		  h = nullptr;
		  printf("breaking\n");
		      
		  break;
		}
	      if (string(observable).compare("pull_angle") == 0)
		{
		  //		      printf("%f nondedicated_ind %u\n",  (*weights)[nondedicated_ind + 1], nondedicated_ind);
		  h -> GetTH2() -> Fill(reco == novalue_proxy ? reco : TMath::Abs(reco)/TMath::Pi(), 
					gen == novalue_proxy ? gen : TMath::Abs(gen)/TMath::Pi(), 
					weight0 * wmap -> GetWeight(windmap.at(ind).first, windmap.at(ind).second, WorkEnum_t::RECO));
		}
	      /*      if (string(GetTag()).compare("MC13TeV_TTJets") == 0)
		      {
		      printf("%lu %f\n", event_ind, reco);
		      getchar();
		      }*/
	      if (string(observable).compare("pvmag") == 0)
		{
		  h -> GetTH2() -> Fill(reco == novalue_proxy ? reco : TMath::Abs(reco)/TMath::Pi(), 
					gen == novalue_proxy ? gen : TMath::Abs(gen)/TMath::Pi(), 
					weight0 * wmap -> GetWeight(windmap.at(ind).first, windmap.at(ind).second, WorkEnum_t::RECO));
		}
	      // }
	      //      _th2 -> Fill(pull_angle_reco == -10 ? pull_angle_reco : pull_angle_reco, pull_angle_gen == -10.0 ? pull_angle_gen : pull_angle_gen, weight);

	    }
  
	}
  // for (unsigned char pind = 0; pind < 2; pind ++)
  //   {
  //     TH2 * _th2 = th2map[periods[pind]];
  //     processth2(_th2);
  //   }
  // processth2(mltotal);
  for (unsigned char chargecode = 0; chargecode < 2; chargecode ++)
    {
      processth2(mtotal[chargecode]);
    }
  return 0;
}

void processth2(TH2 * _th2)
{
  for (unsigned char bindy = 1; bindy <= _th2 -> GetNbinsY(); bindy ++)
    {
      const double integral = _th2 -> Integral(1, _th2 -> GetNbinsX(), bindy, bindy);
      for (unsigned char bindx = 1; bindx <= _th2 -> GetNbinsX(); bindx ++)
	{
	  _th2 -> SetBinContent(bindx, bindy, 100 * _th2 -> GetBinContent(bindx, bindy)/integral);
	}
	
    }
  _th2 -> SetMinimum(0.0);
  _th2 -> SetMaximum(100.0);
  TCanvas * c = new TCanvas;
  c -> SetRightMargin(0.17);
  _th2 -> Draw("COLZTEXT");
  _th2 -> SetMarkerSize(3.0);


  // c -> Update();
  // c -> Modified();
  c -> SaveAs(TString("migrationmatrices/") + _th2 -> GetName() + ".png");
}

