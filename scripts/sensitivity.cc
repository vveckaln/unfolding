//To run: 
//   root -l -q 'scripts/sensitivity.cc("nominal")'
// https://root-forum.cern.ch/t/start-root-with-a-root-file-and-script/28635/3

R__ADD_INCLUDE_PATH($COMPOUNDHISTOLIB/interface)
R__ADD_INCLUDE_PATH($ERRORS/interface)

//#include "/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/interface/CompoundHistoUnfolding.hh"
#include "CompoundHistoUnfolding.hh"
#include "ERRORS.h"

R__ADD_LIBRARY_PATH($COMPOUNDHISTOLIB/lib) // if needed
R__LOAD_LIBRARY(libCompoundHisto.so)

R__ADD_LIBRARY_PATH($ERRORS/lib) // if needed
R__LOAD_LIBRARY(liberrors.so)

double stddev(TH1 *, void * params);
void prepare_header();
void prepare_footer();
FILE * file = nullptr;
string method("");
const char *titles[4] = {"$\\pullangle(j_{1}^{W}, j_{2}^{W})$", "$\\pullangle(j_{2}^{W}, j_{1}^{W})$", "$\\pullangle(j_{1}^{b}, j_{2}^{b})$", "$\\pullangle(j_{2}^{b}, j_{1}^{b})$"};
int sensitivity(const char * methodchar)
{
  method = string(methodchar);
  file = fopen(TString("sensitivities/sensitivities_") + method + ".txt", "w");
  prepare_header();
  const char * observables[] = {"pull_angle", "pvmag"};
  const char * samples_nominal = "MC13TeV_TTJets";
  const char * samples_cflip = "MC13TeV_TTJets_cflip";
  const char * binmethod = "optfixed";
  string bindir(binmethod);
  string observable("pull_angle");
  const TString dir = TString("common_plots_") + method + "/" + observable + "/" + bindir;
  unsigned char opt_level = ORIG;
  if (string(binmethod).compare("ORIG") != 0)
    opt_level = OPT;
  string bintag("");
  if (opt_level == OPT)
    bintag = bintag + "_" + bindir;
  const char * sample = nullptr;
  if (method.compare("nominal") == 0)
    {
      sample = samples_nominal;
    }
  if (method.compare("cflip") == 0)
    {
      sample = samples_cflip;
    }
  gStyle -> SetOptStat(0);
  gStyle -> SetOptTitle(0);
  gROOT -> SetBatch(kTRUE);
  /*  for (unsigned char observable_ind = 0; observable_ind < 2; observable_ind ++) 
      {*/

  // for (unsigned char opt_level = 0; opt_level < 2; opt_level ++)
  //   {
  for (VectorCode_t jetcode = 0; jetcode < 4; jetcode ++)
    {
      fprintf(file, "\t\t\\multicolumn{2}{c}{%s}\\\\\\hline\n", titles[jetcode]);
      Result res[2];
      for (ChargeCode_t charge_code = 0; charge_code < 2; charge_code ++)
	{
	  //      VectorCode_t jetcode = 0;
	  printf("Processing %s %s\n", tag_jet_types_[jetcode], tag_charge_types_[charge_code]);

	  TFile * fileps = TFile::Open(TString("$EOS/unfolding_") + 
				       method + "/" + 
				       observable + "/" + 
				       bindir + "/" +
				       tag_jet_types_[jetcode] + "_" + 
				       tag_charge_types_[charge_code] + "_" +
				       sample + "_" + 
				       observable + "_" + 
				       tag_opt[opt_level] + "_" + 
				       method + bintag + 
				       "/save.root", "READ");
	  printf("%s\n", fileps -> GetName());
	  CompoundHistoUnfolding * chups = (CompoundHistoUnfolding * )((TKey * ) fileps -> GetListOfKeys() -> At(0)) -> ReadObj();
	  TH1 * data = chups -> GetLevel(OUT) -> GetHU(DATAMO) -> GetTH1(GEN);
	  // if (charge_code == 0)
	  //   {
	  //     huncharged = data;
	  //   }
	  // else
	  //   {
	  //     huncharged -> Add(data, -1);
	  //     huncharged -> SetName(TString(data ->GetName()) + "_neutralprt");
	  //     TCanvas c;
	  //     huncharged -> Draw();
	  //     c . SaveAs(TString("neutralprt/") + tag_jet_types_[jetcode] + ".png");
	  //   }
	  //data -> Print("ALL");
	  //      data -> Scale(1.0/data -> Integral());
	  HistoUnfolding * hunfolding = chups -> GetLevel(OUT) -> GetHU(DATAMO, GEN);
	  TH1 * h = hunfolding -> GetTH1(GEN);
	  TH1 * hsyst = chups -> GetLevel(OUT) -> GetHU(TOTALMCUNC, GEN) -> GetTH1(GEN);

	  res[charge_code] = calculate_result(h, hsyst, stddev, (void *) chups, 3.0, 1.0);
	  fileps -> Close();
	 }
      fprintf(file, "\t\t %s & %s \\\\\\hline\n", res[0]. latex(".3f"). Data(), res[1].latex(".3f").Data());
    }
  prepare_footer();
  fclose(file);
  return 0;
}

double stddev(TH1 *h, void * params)
{
  //  const unsigned char N = h -> GetNbinsX();
  
  CompoundHistoUnfolding * chups = (CompoundHistoUnfolding *) params;
  //  chups -> Do();
  chups -> NormaliseToBinWidth(h);
  h -> Scale(1.0/calculatearea(h));
  double sum(0.0);
  double integral(0.0);
  for (unsigned char nbin = 1; nbin <= h -> GetNbinsX(); nbin ++)
    {
      sum += (TMath::Power(h -> GetXaxis() -> GetBinUpEdge(nbin), 3) - TMath::Power(h-> GetXaxis() -> GetBinLowEdge(nbin), 3))/3.0  * h -> GetBinContent(nbin);
      integral += (h -> GetXaxis() -> GetBinUpEdge(nbin) - h -> GetXaxis() -> GetBinLowEdge(nbin)) * h -> GetBinContent(nbin);
      //      printf("nbin %u content %f low %f up %f\n", nbin, h -> GetBinContent(nbin), h-> GetXaxis() -> GetBinLowEdge(nbin), h-> GetXaxis() -> GetBinUpEdge(nbin));
    }
  return sum;
  return TMath::Power(sum/ integral, 0.5);;
}

void prepare_header()
{
  fprintf(file, "\\begin{table}[htbp]\n");
  fprintf(file, "\t\\centering\n");
  fprintf(file, "\t\\begin{tabular}{cc}\n");
  fprintf(file, "\t\t all constituents & charged constituents \\\\\\hline\n");
  fprintf(file, "\t\t $\\sigma\\pm{\\rm(stat)}\\pm{\\rm(syst)}$ & $\\sigma\\pm{\\rm(stat)}\\pm{\\rm(syst)}$ \\\\\\hline\n");
}
void prepare_footer()
{
  fprintf(file, "\t\\end{tabular}\n");
  fprintf(file, "\t\\caption{Sensitivity of pull angle measured as the standard deviation for the \\protect\\methodtitle{%s} method}\n", method.c_str());
  fprintf(file, "\t\\label{tab:sensitivities_%s}\n", method.c_str());
  fprintf(file, "\\end{table}\n");

}
