//./chitable nominal pull_angle optfixed lx
#include "Definitions.hh"
#include <assert.h> 
#include <string> 
using namespace std;
TString insertnumber(float, const char *);
void prepare_header(FILE *, const char * caption, const char * label, unsigned char obs_ind);
void prepare_footer(FILE *);
string env("");
const char *titles[2][4] = {
  {"$\\pullangle(j_{1}^{W}, j_{2}^{W})$", "$\\pullangle(j_{2}^{W}, j_{1}^{W})$", "$\\pullangle(j_{1}^{b}, j_{2}^{b})$", "$\\pullangle(j_{2}^{b}, j_{1}^{b})$"},
  {"$\\left|\\vec{P}(j_{1}^{W})\\right|$", "$\\left|\\vec{P}(j_{2}^{W})\\right|$", "$\\left|\\vec{P}(j_{1}^{b})\\right|$", "$\\left|\\vec{P}(j_{2}^{b})\\right|$"}
  };
const char *sampletitles[3] = {"Powheg + Pythia8", "Powheg + Herwig++", "aMC@NLO + Pythia8"};
int main(int argc, char * argv[])
{
  assert(argc == 5);
  const string method(argv[1]);
  const string observable(argv[2]);
  const string binningmethod(argv[3]);
  env = string(argv[4]);
  system((string("rm -r chi_") + method + "/" + observable + "/" + binningmethod).c_str());
  system((string("mkdir -p chi_") + method + "/" + observable + "/" + binningmethod).c_str());
  const char * observables[] = {"pull_angle", "pvmag"};
  const char * chargetags[2] = {"allconst", "chconst"};
  const char * observabletitles[] = {"pull angle \\pullangle", "magnitude of the pull vector \\pvmag"};
  const char * samples_nominal[] = {"MC13TeV_TTJets", "MC13TeV_TTJets_herwig", "MC13TeV_TTJets2l2nu_amcatnlo"};
  const char * samples_cflip[] = {"MC13TeV_TTJets", "MC13TeV_TTJets_cflip"};
  const char ** samples = nullptr;
  const char * sampletitles_nominal[3] = {"Powheg + Pythia8", "Powheg + Herwig++", "aMC@NLO + Pythia8"};
  const char * sampletitles_cflip[3] = {"Powheg + Pythia8 *", "Powheg + Pythia8 cf"};
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
      const TString  latex_name = TString("chi_") + method + "/" + 
	observable + "/" + 
	binningmethod + "/chi_table_" + 
	observables[observable_ind] + "_" + 
	tag_opt[opt_level] + "_" +
	tag_charge_types_[charge_code] + ".txt";
      FILE *& file = pFile[observable_ind][opt_level][charge_code];
      file = fopen(latex_name, "w");
      const TString  caption = TString("$\\chi^{2}$ and \\pval s of \\protect\\observabletitle{") + observable + "} including \\protect\\chargetitle{" + chargetags[charge_code] + "}. The results are for the \\protect\\modeltitle{" + method + "} model.";
      const TString  label = TString("chi_table_") + 
	observables[observable_ind] + "_" + 
	tag_opt[opt_level] + "_" +
	tag_charge_types_[charge_code] + "_" + 
	method + "_" + binningmethod;
      prepare_header(file, caption.Data(), label.Data(), observable_ind);
      for (unsigned char sample_ind = 0; sample_ind < nsamples; sample_ind ++)
	{
	  fprintf(file, "\t\t%s", sampletitles[sample_ind]); 
	  for (VectorCode_t jetcode = 0; jetcode < 4; jetcode ++)
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
	      // printf("binfilename %s ndf %u\n", binfilename.Data(), ndf);
	      // getchar();
	      const TString input_file_name = input_folder + "/" + 
		tag_jet_types_[jetcode] + '_' + 
		tag_charge_types_[charge_code] + '_' + 
		samples[sample_ind] + '_' + 
		observables[observable_ind] + '_' + 
		tag_opt[opt_level] + '_' + 
		method + bintag + "/chi2.txt";
	      printf("input file name %s\n", input_file_name.Data());
	      //	      system(string("cat ") + input_file_name);
	      FILE  * input_file = fopen(input_file_name, "r");
	      char unc[64];
	      float chi;
	      float pvalue;
	      fscanf(input_file, "%64[^,]%*c%f%f\n", unc, &chi, &pvalue);
	      //		      if (opt_level == OPT and jetcode == 0)
	      //		getchar();
	      printf("%s %f %f\n", unc, chi, pvalue);
	      fclose(input_file);

	      fprintf(file, " & %s & %u & %s", insertnumber(chi, ".2").Data(), ndf, insertnumber(pvalue, ".3").Data());
	    }
	  fprintf(file, "\\\\\n");
	}
      prepare_footer(file);
      fclose(file);
    }
}
void prepare_header(FILE * file, const char * caption, const char * label, unsigned char obs_ind)
{
  fprintf(file, "\t\t\\begin{longtable}{@{\\extracolsep{\\fill}}p{%.1fcm}|rcc|rcc|rcc|rcc}\n", env.compare("lx") ? 4.5 : 2.5);
  fprintf(file, "\t\\caption{%s}\n", caption);
  fprintf(file, "\t\\label{tab:%s}\\\\\n", label);
  fprintf(file, "\t\t\t\\noalign{\\global\\arrayrulewidth=0.5mm}\\hline\\noalign{\\global\\arrayrulewidth=0.4pt}\n");
  fprintf(file, 
"\\multicolumn{1}{c|}{\\multirow{2}{*}{\\bf Sample}} & \\multicolumn{3}{c|}{\\bf\\boldmath %s}&\\multicolumn{3}{c|}{\\bf\\boldmath %s}&\\multicolumn{3}{c|}{\\bf\\boldmath %s}&\\multicolumn{3}{c}{\\bf\\boldmath %s}\\\\\n", 
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
