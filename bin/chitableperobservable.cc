//  ./chitableperobservable pull_angle ATLAS4 lx bayesian
#include "Definitions.hh"
#include "CompoundHistoUnfolding.hh"
#include "TFile.h"
#include "TMatrixD.h"
#include "TKey.h"
#include <assert.h> 
#include <string> 
#include "TMath.h"
using namespace std;
TString insertnumber(float, const char *);
void prepare_header(FILE *, const char * caption, const char * label, unsigned char obs_ind);
void prepare_footer(FILE *);
string env("");
const char *titles[2][4] = {
  {"$\\pullangle(j_{1}^{W}, j_{2}^{W})$", "$\\pullangle(j_{2}^{W}, j_{1}^{W})$", "$\\pullangle(j_{1}^{b}, j_{2}^{b})$", "$\\pullangle(j_{2}^{b}, j_{1}^{b})$"},
  {"$\\left|\\vec{P}(j_{1}^{W})\\right|$", "$\\left|\\vec{P}(j_{2}^{W})\\right|$", "$\\left|\\vec{P}(j_{1}^{b})\\right|$", "$\\left|\\vec{P}(j_{2}^{b})\\right|$"}
  };
//const char *sampletitles[3] = {"Powheg + Pythia8", "Powheg + Herwig++$^{\\dagger}$", "aMC@NLO + Pythia8"};
const char * privatesamples[3] = {"sherpa", "dire2002", "herwig7"};
const char Nmethods = 3;
const char * methods[Nmethods] = {"nominal", "herwig", "cflip"};
const char * cflipsuffix[] = {"", "_cflip"};
int main(int argc, char * argv[])
{
  assert(argc == 5);
  const string observable(argv[1]);
  const string binningmethod(argv[2]);
  env = string(argv[3]);
  const string unfm(argv[4]);
  //  system((string("rm -r chi_perobservable/") + observable + "/" + binningmethod).c_str());
  system((string("mkdir -p chi_perobservable/") + observable + "/" + binningmethod).c_str());
  const char * observables[] = {"pull_angle", "pvmag"};
  const char * chargetags[2] = {"allconst", "chconst"};
  const char * observabletitles[] = {"pull angle \\pullangle", "magnitude of the pull vector \\pvmag"};
  const char ** samples[Nmethods] = 
    {
      new const char * [2] {"MC13TeV_TTJets", "MC13TeV_TTJets"},
      new const char * [1] {"MC13TeV_TTJets_herwig"},
      new const char * [1] {"MC13TeV_TTJets_cflip"}
    };
  const char ** sampletitles[Nmethods] = 
    {
      new const char * [2] {"Powheg + Pythia8", "Powheg + Pythia8 *"}, 
      new const char * [1] {"Powheg + Herwig++$^{\\dagger}$"}, 
      new const char * [1] {"Powheg + Pythia8 cf$^{\\dagger}$"}
    };
  unsigned char nsamples = 0;
  unsigned char observable_ind = 0;
  if (observable.compare("pull_angle") == 0) 
    {
      observable_ind = 0;
    }
  if (observable.compare("pvmag") == 0 )
    {
      observable_ind = 1;
    }
  
  for (ChargeCode_t charge_code = 0; charge_code < 2; charge_code ++)
    {
      const TString  latex_name = TString("chi_perobservable/") + 
	observable + "/" + 
	binningmethod + "/chi_table_perobservable_" + 
	observable + "_" +
	chargetags[charge_code] + "_" +
	unfm +
	".txt";
      FILE * file = fopen(latex_name, "w");
      unsigned char opt_level = 0;
      string bintag("");
      if (binningmethod.compare("ORIG") == 0) 
	{
	  opt_level = ORIG;
	}
      else
	{
	  bintag = bintag + "_" + binningmethod;
	  opt_level = OPT;
	}

      const TString  caption = TString("$\\chi^{2}$ and \\pval s of \\protect\\observabletitle{") + observable + "} including \\protect\\chargetitle{" + chargetags[charge_code] + "} using \\protect\\unfoldingmethodtitle{" + unfm + "}. For samples marked with dagger ($\\dagger$) its proper migration matrix has been used for unfolding, while for other samples unfolding has been done with the nominal unfolding matrix.";
      const TString  label = TString("chi_table_") + 
	observable + "_" + 
	chargetags[charge_code] + "_" + 
	binningmethod + "_" +
	unfm;
      prepare_header(file, caption.Data(), label.Data(), observable_ind);
      for (unsigned char methodind = 0; methodind < Nmethods; methodind ++)
	{
	  const string method(methods[methodind]);
      
	  if (method.compare("nominal") == 0)
	    {
	      nsamples = 2;
	    }
	  if (method.compare("cflip") == 0 or method.compare("herwig") == 0)
	    {
	      nsamples = 1;
	    }
	  const TString input_folder = "/eos/user/v/vveckaln/unfolding_" + method + "/" + observable + "/" + binningmethod;

	  //FILE * pFile[2][2][2];
    
	  for (unsigned char sample_ind = 0; sample_ind < nsamples; sample_ind ++)
	    {
	      fprintf(file, "\t %s", sampletitles[methodind][sample_ind]); 
	      for (VectorCode_t jetcode = 0; jetcode < 4; jetcode ++)
		{
		  // printf("binfilename %s ndf %u\n", binfilename.Data(), ndf);
		  // getchar();
		  const TString binfilename = input_folder + "/" + 
		    tag_jet_types_[jetcode] + '_' + 
		    tag_charge_types_[charge_code] + '_' + 
		    samples[methodind][0] + '_' + 
		    observables[observable_ind] + '_' + 
		    tag_opt[opt_level] + '_' + 
		    method + bintag + "/binfile.txt";
		  printf("binfile %s\n", binfilename.Data());
		  FILE * binfile = fopen(binfilename.Data(), "r");
		  int nbins;
		  fscanf(binfile, "%u", &nbins);
		  fclose(binfile);
		  const int ndf(nbins - 1);
		  const TString input_file_name = input_folder + "/" + 
		    tag_jet_types_[jetcode] + '_' + 
		    tag_charge_types_[charge_code] + '_' + 
		    samples[methodind][sample_ind] + '_' + 
		    observables[observable_ind] + '_' + 
		    tag_opt[opt_level] + '_' + 
		    method + bintag + "/" + 
		    unfm + 
		    cflipsuffix[sample_ind] + "/chi2.txt";
		  printf("input file name %s\n", input_file_name.Data());
		  //	      system(string("cat ") + input_file_name);
		  FILE  * input_file = fopen(input_file_name, "r");
		  char unc[64];
		  float chi;
		  float pvalue;
		  fscanf(input_file, "%64[^,]%*c%f%f\n", unc, &chi, &pvalue);
		  //		      if (opt_level == OPT and jetcode == 0)
		  //		getchar();
		  //printf("%s %f %f\n", unc, chi, pvalue);
		  fclose(input_file);

		  fprintf(file, " & %s & %u & %s", insertnumber(chi, ".2").Data(), ndf, insertnumber(pvalue, ".3").Data());
		}
	      fprintf(file, "\\\\\n");
	    }
	    
	  if (method == "nominal")
	    {
	      for (unsigned char psind = 0; psind < 3; psind ++)
		{
		  fprintf(file, "\t %s", (TString("")/*$t\\bar{t}$ ")*/ + privatesamples[psind]).Data()); 
			  
		  for (VectorCode_t jetcode = 0; jetcode < 4; jetcode ++)
		    {
		      const TString binfilename = input_folder + "/" + 
			tag_jet_types_[jetcode] + '_' + 
			tag_charge_types_[charge_code] + '_' + 
			samples[methodind][0] + '_' + 
			observables[observable_ind] + '_' + 
			tag_opt[opt_level] + '_' + 
			method + bintag + "/binfile.txt";
		      FILE * binfile = fopen(binfilename.Data(), "r");
		      int nbins;
		      fscanf(binfile, "%u", &nbins);
		      fclose(binfile);
		      const int ndf(nbins - 1);

		      TFile * filenom = TFile::Open(TString("$EOS/unfolding_") + 
						    method + "/" +
						    observable + "/" + 
						    binningmethod + "/" +
						    tag_jet_types_[jetcode] + "_" + 
						    tag_charge_types_[charge_code] + 
						    "_" + samples[methodind][0] + "_" + 
						    observable + "_" + 
						    tag_opt[opt_level] + "_" + 
						    method + bintag + "/" + 
						    unfm + "/save_" + unfm + ".root", "READ");
		      //printf("file %p %s\n", filenom, filenom -> GetName()); 
		      CompoundHistoUnfolding * chnominal = (CompoundHistoUnfolding * )((TKey * ) filenom -> GetListOfKeys() -> At(0)) -> ReadObj();
		      filenom -> Close();
		      //printf("chnominal %p\n", chnominal);
		      TH1 * hsys = chnominal -> GetLevel(OUT) -> GetHU(SIGNALPROXY, OUT) -> Project(GEN, "hsystesttest");
		      hsys -> Scale(1.0/hsys -> Integral());
		      chnominal -> GetLevel(OUT) -> GetCov() -> Print();
		      TMatrixD covinv(*chnominal -> GetLevel(OUT) -> GetCov());
		      //covinv.Print();
		      //getchar();
		      covinv.Invert();
		      TFile *fpsample = TFile::Open(TString("$EOS/unfolding_") + 
						    privatesamples[psind] + "/" +
						    observable + "/" + 
						    binningmethod + "/" +
						    tag_jet_types_[jetcode] + "_" + 
						    tag_charge_types_[charge_code] + 
						    "_MC13TeV_TTJets_" + privatesamples[psind] + "_" + 
						    observable + "_" + 
						    tag_opt[opt_level] + "_" + 
						    privatesamples[psind] + bintag 
						    + "/save.root", "READ");
		      //printf("%s\n", fpsample -> GetName());
		      CompoundHistoUnfolding * chpr = nullptr;
		      chpr = (CompoundHistoUnfolding * )((TKey * ) fpsample -> GetListOfKeys() -> At(0)) -> ReadObj();
		      HistoUnfolding * hupsample = chpr -> GetLevel(IN) -> GetSys(TString("MC13TeV_TTJets_") + privatesamples[psind], TString("MC13TeV_TTJets_") + privatesamples[psind]);
		      TH1 * hsig = chpr -> GetLevel(IN) -> GetSys(TString("MC13TeV_TTJets_") + privatesamples[psind], TString("MC13TeV_TTJets_") + privatesamples[psind]) -> Project(GEN, "psamplesig");
		      hsig -> Scale(1.0/hsig -> Integral());
		      TMatrixD Diff(1, nbins - 1);
		      for (unsigned char bind = 1; bind <= nbins - 1; bind ++)
			{
			  Diff(0, bind - 1) = hsys -> GetBinContent(bind) - hsig -> GetBinContent(bind);
			}
		      //getchar();                                                                                                                                                                                            
		      TMatrixD DiffT(Diff);
		      DiffT.T();
		      const float D = (Diff * covinv * DiffT)(0, 0);
		      if (jetcode == 1 and charge_code == 1)
			{
			  Diff.Print();
			  chnominal -> GetLevel(OUT) -> GetCov() -> Print();
			  covinv.Print();
			  printf("D = %f\n", D);
			  //getchar();
			}
		      const float pvalue = TMath::Prob(D, ndf);
		      // printf("%s %f %f\n", (TString("t\\bar{t} ") + hupsample -> GetTitle()).Data(), D, TMath::Prob(D, ndf));
		      // printf("chprf %p \n", chpr);
		      fprintf(file, " & %s & %u & %s", insertnumber(D, ".2").Data(), ndf, insertnumber(pvalue, ".3").Data());
		      delete hsig;
		      fpsample -> Close();
		      delete hsys;
		      //getchar();

		    }
		  fprintf(file, "\\\\\n");

	
		}
	    }
	  fprintf(file, "\t\\noalign{\\global\\arrayrulewidth=0.5mm}\\hline\\noalign{\\global\\arrayrulewidth=0.4pt}\n");

	}
      prepare_footer(file);
      fclose(file);

    }

}

void prepare_header(FILE * file, const char * caption, const char * label, unsigned char obs_ind)
{
  fprintf(file, "\\begin{longtable}{@{\\extracolsep{\\fill}}p{%.1fcm}|rcc|rcc|rcc|rcc}\n", env.compare("lx") == 0 ? 3.0 : 4.0);
  fprintf(file, "\t\\caption{%s}\n", caption);
  fprintf(file, "\t\\label{tab:%s}\\\\\n", label);
  fprintf(file, "\t\\noalign{\\global\\arrayrulewidth=0.5mm}\\hline\\noalign{\\global\\arrayrulewidth=0.4pt}\n");
  fprintf(file, 
	  "\t\\multicolumn{1}{c|}{\\multirow{2}{*}{\\bf Sample}} & \\multicolumn{3}{c|}{\\bf\\boldmath %s}&\\multicolumn{3}{c|}{\\bf\\boldmath %s}&\\multicolumn{3}{c|}{\\bf\\boldmath %s}&\\multicolumn{3}{c}{\\bf\\boldmath %s}\\\\\n", 
	  titles[obs_ind][0], 
	  titles[obs_ind][1], 
	  titles[obs_ind][2], 
	  titles[obs_ind][3]);
  fprintf(file, "\t\\cline{2-13}");
  if (env.compare("lx") != 0)
    fprintf(file, "\t & {\\boldmath\\makecell{$\\chi^{2}$}} & \\makecell{\\bf/Ndf} & {\\boldmath{\\makecell{\\bf \\pval}}} & {\\boldmath\\makecell{$\\chi^{2}$}} & \\makecell{\\bf/Ndf} & {\\boldmath\\makecell{\\bf\\pval}} & {\\boldmath\\makecell{$\\chi^{2}$}} & \\makecell{\\bf/Ndf} & {\\boldmath\\makecell{\\bf\\pval}} & {\\boldmath\\makecell{$\\chi^{2}$}} & \\makecell{\\bf/Ndf} & {\\boldmath\\makecell{\\bf\\pval}}\\\\\n");
  else
    fprintf(file, "\t & \\boldmath$\\chi^{2}$ & \\bf/Ndf & \\boldmath\\bf \\pval & \\boldmath$\\chi^{2}$ & \\bf/Ndf & \\boldmath\\bf\\pval & \\boldmath$\\chi^{2}$ & \\bf/Ndf & \\boldmath\\bf\\pval & \\boldmath$\\chi^{2}$ & \\bf/Ndf & \\boldmath\\bf\\pval\\\\\n");
    
  fprintf(file, "\t\\hline\n");
}
void prepare_footer(FILE *file)
{
  //  fprintf(file, "\t\\noalign{\\global\\arrayrulewidth=0.5mm}\\hline\n");
  fprintf(file, "\\end{longtable}\n");

}

TString insertnumber(float n, const char * format)
{
  return Form((string("%") + format + "f").c_str(), n); 
  return TString("\\num{") + Form((string("%") + format + "f}").c_str(), n); 
}
