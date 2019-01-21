#include "Definitions.hh"
#include <assert.h> 
#include <string> 
#include <map>
using namespace std;
void prepare_header(FILE *, const char * caption, const char * label, unsigned char obs_ind);
void prepare_footer(FILE *);
const unsigned int njets = 4;
const char *titles[2][4] = {
  {"$\\theta_{p}(j_{1}^{W}, j_{2}^{W})$", "$\\theta_{p}(j_{2}^{W}, j_{1}^{W})$", "$\\theta_{p}(j_{1}^{b}, j_{2}^{b})$", "$\\theta_{p}(j_{2}^{b}, j_{2}^{b})$"},
  {"$\\left|\\vec{P}(j_{1}^{W})\\right|$", "$\\left|\\vec{P}(j_{2}^{W})\\right|$", "$\\left|\\vec{P}(j_{1}^{b})\\right|$", "$\\left|\\vec{P}(j_{2}^{b})\\right|$"}
  };
const char *sampletitles[3] = {"Powheg+Pythia8", "Powheg+Herwig", "aMC@NLO+Pythia8"};
int main(int argc, char * argv[])
{
  assert(argc == 4);
  const string method(argv[1]);
  const string observable(argv[2]);
  const string binningmethod(argv[3]);
  system((string("rm -r chi_full_") + method + "/" + observable + "/" + binningmethod).c_str());
  system((string("mkdir -p chi_full_") + method + "/" + observable + "/" + binningmethod).c_str());
  const char * observables[] = {"pull_angle", "pvmag"};
  const char * chargetags[2] = {"allconst", "chconst"};
  const char * observabletitles[] = {"pull angle $\\theta_{p}$", "magnitude of the pull vector $\\left|\\vec{P}\\right|$"};
  const char * samples_nominal[] = {"MC13TeV_TTJets", "MC13TeV_TTJets_herwig", "MC13TeV_TTJets2l2nu_amcatnlo"};
  const char * samples_cflip[] = {"MC13TeV_TTJets", "MC13TeV_TTJets_cflip"};
  const char ** samples = nullptr;
  const char * sampletitles_nominal[3] = {"Powheg+Pythia8", "Powheg+Herwig", "aMC@NLO+Pythia8"};
  const char * sampletitles_cflip[3] = {"Powheg+Pythia8 *", "Powheg+Pythia8 cf"};
  const char ** sampletitles = nullptr;
  
  unsigned char nsamples = 0;
  if (method.compare("nominal") == 0)
    {
      samples = samples_nominal;
      sampletitles = sampletitles_nominal;
      nsamples = 2;
    }
  if (method.compare("cflip") == 0)
    {
      samples = samples_cflip;
      sampletitles = sampletitles_cflip;
      nsamples = 2;
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
	    samples[sample_ind] + "_full.txt";
	  FILE *& file = pFile[observable_ind][opt_level][charge_code];
	  file = fopen(latex_name, "w");
	  const TString  caption = TString("$\\chi^{2}$ and p-values of the \\protect\\observabletitle{") + observable + "} including \\protect\\chargetitle{" + chargetags[charge_code] + "}. The nuisances are substituted in place of the signal. The results are for the \\protect\\modeltitle{" + method + "} model.";

	  const TString  label = TString("chi_table_") + 
	    observables[observable_ind] + "_" + 
	    tag_opt[opt_level] + "_" +
	    tag_charge_types_[charge_code] + "_" +
	    samples[sample_ind] + "_" + 
	    method + bintag + "_full";
	  prepare_header(file, caption.Data(), label.Data(), observable_ind);
	  map<TString, float *> chi2map;
	  map<TString, int *> ndfmap;
	  map<TString, float * > pvaluemap;
	  for (VectorCode_t jetcode = 0; jetcode < njets; jetcode ++)
	    {
	      const TString binfilename = input_folder + "/" + 
		tag_jet_types_[jetcode] + '_' + 
		tag_charge_types_[charge_code] + '_' + 
		samples[sample_ind] + '_' + 
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
		samples[sample_ind] + '_' + 
		observables[observable_ind] + '_' + 
		tag_opt[opt_level] + '_' + 
		method + bintag + "/chi2.txt";
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
		  fprintf(file, " & %.2f & %u & %.3f", it -> second[jetcode], ndfmap[it -> first][jetcode], pvaluemap[it -> first][jetcode]);
		}
	      fprintf(file, "\\\\\n");
	    }
	  for(map<TString, float *>::iterator it = chi2map.begin(); it != chi2map.end(); it ++)
	    {
	      delete [] it -> second;
	      delete [] ndfmap[it -> first];
	      delete [] pvaluemap[it -> first];
	    }
	  fprintf(file, "\\\\\n");
		
	  prepare_footer(file);
	  fclose(file);
	}
    }
}
void prepare_header(FILE * file, const char * caption, const char * label, unsigned char obs_ind)
{
  fprintf(file, "\\begin{table}[htp]\n");
  fprintf(file, "\t\\begin{center}\n");
  fprintf(file, "\t\\caption{%s}\n", caption);
  fprintf(file, "\t\\label{tab:%s}\n", label);
  fprintf(file, "\t\\resizebox{\\customwidth}{!}{");
  fprintf(file, "\t\t\\begin{tabular}{|l|rrr|rrr|rrr|rrr|}\n");
  fprintf(file, "\t\t\\hline\n");
  fprintf(file, 
"\\makecell[c]{\\multirow{2}{*}{Nuisances}}&\\multicolumn{3}{|c|}{%s}&\\multicolumn{3}{|c|}{%s}&\\multicolumn{3}{|c|}{%s}&\\multicolumn{3}{|c|}{%s}\\\\\n", 
titles[obs_ind][0], 
titles[obs_ind][1], 
titles[obs_ind][2], 
titles[obs_ind][3]);
  fprintf(file, "\t\t\\cline{2-13}");
  fprintf(file, "\t\t&$\\chi^{2}$&\\textbackslash Ndf&p-value&$\\chi^{2}$&\\textbackslash Ndf&p-value&$\\chi^{2}$&\\textbackslash Ndf&p-value&$\\chi^{2}$&\\textbackslash Ndf&p-value\\\\\n");
  fprintf(file, "\t\t\\hline\n");
}
void prepare_footer(FILE *file)
{
  fprintf(file, "\t\t\\hline\n");
  fprintf(file, "\t\t\\end{tabular}\n");
  fprintf(file, "\t}\n");
  fprintf(file, "\t\\end{center}\n");
  fprintf(file, "\\end{table}\n");

}
