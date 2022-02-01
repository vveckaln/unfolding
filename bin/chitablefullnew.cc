// ./chitablefullnew nominal pull_angle ATLAS4 nonlx bayesian cflip
#include "Definitions.hh"
#include <assert.h> 
#include <string> 
#include <map>
using namespace std;
TString insertnumber(float, const char *);
void prepare_header(FILE *, const char * caption, const char * label, unsigned char obs_ind);
void prepare_footer(FILE *);
const unsigned int njets = 4;
string env ("");
const char *titles[2][4] = {
  {"$\\pullangle(j_{1}^{W}, j_{2}^{W})$", "$\\pullangle(j_{2}^{W}, j_{1}^{W})$", "$\\pullangle(j_{1}^{b}, j_{2}^{b})$", "$\\pullangle(j_{2}^{b}, j_{2}^{b})$"},
  {"$\\left|\\vec{P}(j_{1}^{W})\\right|$", "$\\left|\\vec{P}(j_{2}^{W})\\right|$", "$\\left|\\vec{P}(j_{1}^{b})\\right|$", "$\\left|\\vec{P}(j_{2}^{b})\\right|$"}
  };
const char *sampletitles[3] = {"Powheg + Pythia8", "Powheg + Herwig++", "aMC@NLO + Pythia8"};
const char * cflipsuffix[] = {"", "_cflip"};

int main(int argc, char * argv[])
{
  assert(argc == 7);
  const string method(argv[1]);
  const string observable(argv[2]);
  const string binningmethod(argv[3]);
  env = string(argv[4]);
  const string unfm(argv[5]);
  const bool includecflip = string(argv[6]).compare("cflip") == 0 ? true : false;
  //  system((string("rm -r chi_full_") + method + "/" + observable + "/" + binningmethod).c_str());
  system((string("mkdir -p chi_full_") + method + "/" + observable + "/" + binningmethod).c_str());
  const char * observables[] = {"pull_angle", "pvmag"};
  const char * chargetags[2] = {"allconst", "chconst"};
  const char * observabletitles[] = {"pull angle \\pullangle", "magnitude of the pull vector \\pvmag"};
  const unsigned char Nmethods = 3;
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
  unsigned char methodind = 4;
  unsigned char cflipind = 3;
  if (method.compare("nominal") == 0)
    methodind = 0;
  if (method.compare("herwig") == 0)
    methodind = 1;
  if (method.compare("cflip") == 0)
    methodind = 2;
  if (not includecflip)
    cflipind = 0;
  if (includecflip)
    cflipind = 1;
  unsigned char nsamples = 0;
  if (method.compare("nominal") == 0)
    {
      nsamples = 2;
    }
  if (method.compare("cflip") == 0 or method.compare("herwig") == 0)
    {
      nsamples = 1;
    }
  FILE * pFile[2][2][2];
  const TString input_folder = "/eos/user/v/vveckaln/unfolding_" + method + "/" + observable + "/" + binningmethod;
  unsigned char observable_ind = 0;
  unsigned char opt_level = 0;
  if (observable.compare("pull_angle") == 0) 
    {
      observable_ind = 0;
    }
  if (observable.compare("pvmag") == 0 )
    {
      observable_ind = 1;
    }
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

  for (ChargeCode_t charge_code = 0; charge_code < 2; charge_code ++)
    {
      for (unsigned char sample_ind = 0; sample_ind < nsamples; sample_ind ++)
	{
	  const TString  latex_name = TString("chi_full_") + method + "/" + 
	    observable + "/" + 
	    binningmethod + "/chi_table_" + 
	    observables[observable_ind] + "_" + 
	    tag_opt[opt_level] + "_" +
	    tag_charge_types_[charge_code] + "_" + 
	    samples[methodind][cflipind] + "_" + 
	    unfm + "_" + 
	    "full.txt";
	  FILE *& file = pFile[observable_ind][opt_level][charge_code];
	  file = fopen(latex_name, "w");
	  const TString  caption = TString("$\\chi^{2}$ and \\pval s of \\protect\\observabletitle{") + observable + "} including \\protect\\chargetitle{" + chargetags[charge_code] + "}. The theoretical uncertainties are substituted in place of the signal. The results are for the \\protect\\modeltitle{" + method + "} model using \\protect\\unfoldingmethodtitle{" + unfm + "}";

	  const TString  label = TString("chi_table_") + 
	    observables[observable_ind] + "_" + 
	    tag_opt[opt_level] + "_" +
	    tag_charge_types_[charge_code] + "_" +
	    samples[methodind][cflipind] + "_" + 
	    method + 
	    bintag + "_" + 
	    unfm + "_full";
	  prepare_header(file, caption.Data(), label.Data(), observable_ind);
	  map<TString, float *> chi2map;
	  map<TString, int *> ndfmap;
	  map<TString, float * > pvaluemap;
	  for (VectorCode_t jetcode = 0; jetcode < njets; jetcode ++)
	    {
	      const TString binfilename = input_folder + "/" + 
		tag_jet_types_[jetcode] + '_' + 
		tag_charge_types_[charge_code] + '_' + 
		samples[methodind][cflipind] + '_' + 
		observables[observable_ind] + '_' + 
		tag_opt[opt_level] + '_' + 
		method + bintag + "/binfile.txt";
	      FILE * binfile = fopen(binfilename.Data(), "r");
	      int nbins;
	      fscanf(binfile, "%u", &nbins);
	      fclose(binfile);
	      const int ndf(nbins - 1);
	      const TString input_file_name = input_folder + "/" + 
		tag_jet_types_[jetcode] + '_' + 
		tag_charge_types_[charge_code] + '_' + 
		samples[methodind][cflipind] + '_' + 
		observables[observable_ind] + '_' + 
		tag_opt[opt_level] + '_' + 
		method + 
		bintag + "/" + 
		unfm + cflipsuffix[cflipind] + "/chi2.txt";
	      printf("input file name %s\n", input_file_name.Data());
	      system(string("cat ") + input_file_name);
	      FILE  * input_file = fopen(input_file_name, "r");
	      char unc[64];
	      float chi2;
	      float pvalue;
	      while (fscanf(input_file, "%64[^,]%*c%f%f\n", unc, &chi2, &pvalue) != EOF)
		{
		  //		      if (opt_level == OPT and jetcode == 0)
		  //		getchar();
		  TString key(unc);
		  key.ReplaceAll("QCD scale 1", "id1002muR1muF2hdampmt272.7225");
		  key.ReplaceAll("QCD scale 2", "id1003muR1muF0.5hdampmt272.7225");
		  key.ReplaceAll("QCD scale 3", "id1004muR2muF1hdampmt272.7225");
		  key.ReplaceAll("QCD scale 4", "id1005muR2muF2hdampmt272.7225");
		  key.ReplaceAll("QCD scale 5", "id1007muR0.5muF1hdampmt272.7225");
		  key.ReplaceAll("QCD scale 6", "id1009muR0.5muF0.5hdampmt272.7225");
		  key.ReplaceAll("(null)", "t#bar{t}");
		  //		  key.ReplaceAll("QCD scale 7", "");
		  key.ReplaceAll("_", "\\_");
		  key.ReplaceAll("t#bar{t}", "$t\\overline{t}$");

		  printf("key %s\n", key.Data());
		  map<TString, float *>:: iterator it = chi2map.find(key);
		  if (it == chi2map.end())
		    {
		      chi2map[key] = new float[njets];
		      ndfmap[key] = new int[njets];
		      pvaluemap[key] = new float[njets];
		      for (unsigned char ind = 0; ind < njets; ind ++)
			{
			  chi2map[key][ind] = -1.0;
			  ndfmap[key][ind] = 0;
			  pvaluemap[key][ind] = -1.0;
			}
		    }
		  chi2map[key][jetcode] = chi2;
		  ndfmap[key][jetcode] = ndf;
		  pvaluemap[key][jetcode] = pvalue;
		}
	      fclose(input_file);
	    }
	  for(map<TString, float *>::iterator it = chi2map.begin(); it != chi2map.end(); it ++)
	    {
	      for (unsigned char jetcode = 0; jetcode < njets; jetcode ++)
		{
		  if (jetcode == 0)
		    {
		      fprintf(file, "\t\t\t%s", it -> first.Data());
		    }
		  fprintf(file, " & %s & %u & %s", insertnumber(it -> second[jetcode], ".2").Data(), ndfmap[it -> first][jetcode], insertnumber(pvaluemap[it -> first][jetcode], ".3").Data());
		}
	      fprintf(file, "\\\\\n");
	    }
	  for(map<TString, float *>::iterator it = chi2map.begin(); it != chi2map.end(); it ++)
	    {
	      delete [] it -> second;
	      delete [] ndfmap[it -> first];
	      delete [] pvaluemap[it -> first];
	    }
	  //	  fprintf(file, "\\\\\n");
		
	  prepare_footer(file);
	  fclose(file);
	}
    }
}
void prepare_header(FILE * file, const char * caption, const char * label, unsigned char obs_ind)
{
  //  fprintf(file, "\t\\resizebox{\\customwidth}{!}{");
  fprintf(file, "\t\t\\begin{longtable}{@{\\extracolsep{\\fill}}p{%.1fcm}|rcc|rcc|rcc|rcc}\n", env.compare("lx") ? 4.5 : 2.5);
  fprintf(file, "\t\\caption{%s}\n", caption);
  fprintf(file, "\t\\label{tab:%s}\\\\\n", label);
  fprintf(file, "\t\t\t\\noalign{\\global\\arrayrulewidth=0.5mm}\\hline\\noalign{\\global\\arrayrulewidth=0.4pt}\n");
  fprintf(file, 
"\\multicolumn{1}{c|}{\\multirow{2}{*}{\\bf Sample}} & \\multicolumn{3}{c|}{\\bf\\boldmath %s} & \\multicolumn{3}{c|}{\\bf\\boldmath %s} & \\multicolumn{3}{c|}{\\bf\\boldmath %s} & \\multicolumn{3}{c}{\\bf\\boldmath %s}\\\\\n", 
titles[obs_ind][0], 
titles[obs_ind][1], 
titles[obs_ind][2], 
titles[obs_ind][3]);
  fprintf(file, "\t\t\\cline{2-13}");
  if (env.compare("lx") != 0)
    fprintf(file, "\t\t & {\\boldmath\\makecell{$\\chi^{2}$}} & \\makecell{\\bf/Ndf} & {\\boldmath{\\makecell{\\bf \\pval}}} & {\\boldmath\\makecell{$\\chi^{2}$}} & \\makecell{\\bf/Ndf} & {\\boldmath\\makecell{\\bf\\pval}} & {\\boldmath\\makecell{$\\chi^{2}$}} & \\makecell{\\bf/Ndf} & {\\boldmath\\makecell{\\bf\\pval}} & {\\boldmath\\makecell{$\\chi^{2}$}} & \\makecell{\\bf/Ndf} & {\\boldmath\\makecell{\\bf\\pval}}\\\\\n");
  else
    fprintf(file, "\t\t & \\boldmath$\\chi^{2}$ & \\bf/Ndf & \\boldmath\\bf \\pval & \\boldmath$\\chi^{2}$ & \\bf/Ndf & \\boldmath\\bf\\pval & \\boldmath$\\chi^{2}$ & \\bf/Ndf & \\boldmath\\bf\\pval & \\boldmath$\\chi^{2}$ & \\bf/Ndf & \\boldmath\\bf\\pval\\\\\n");
    
  fprintf(file, "\t\t\\hline\n");
}
void prepare_footer(FILE *file)
{
  fprintf(file, "\t\t\\noalign{\\global\\arrayrulewidth=0.5mm}\\hline\n");
  fprintf(file, "\t\t\\end{longtable}\n");

}

TString insertnumber(float n, const char * format)
{
  return Form((string("%") + format + "f").c_str(), n); 
  return TString("\\num{") + Form((string("%") + format + "f}").c_str(), n); 
}
