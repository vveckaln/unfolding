#include "Definitions.hh"
#include <assert.h> 
#include <string> 
#include <map>
using namespace std;
void prepare_header(FILE *, const char * caption, const char * label, FILE * binfile);
void prepare_footer(FILE *);
  const char ** sampletitles = nullptr;



int main(int argc, char * argv[])
{
  assert(argc == 2);
  const string method(argv[1]);
  //  const string rel(argv[2]);
  system((string("rm -r unc_") + method).c_str());
  system((string("mkdir unc_") + method).c_str());
  const char * observables[] = {"pull_angle", "pvmag"};
  const char * observabletitles[] = {"pull angle $\\theta_{p}$", "magnitude of the pull vector $\\left|\\vec{P}\\right|$"};
  const char * samples_nominal[] = {"MC13TeV_TTJets", "MC13TeV_TTJets_herwig", "MC13TeV_TTJets2l2nu_amcatnlo"};
  const char * samples_cflip[] = {"MC13TeV_TTJets", "MC13TeV_TTJets_cflip"};
  const char ** samples = nullptr;
  const char * sampletitles_nominal[3] = {"Powheg+Pythia8", "Powheg+Herwig", "aMC@NLO+Pythia8"};
  const char * sampletitles_cflip[3] = {"Powheg+Pythia8 *", "Powheg+Pythia8 cf"};
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
  
  const TString input_folder = "/eos/user/v/vveckaln/unfolding_" + method + "/";
  for (unsigned char observable_ind = 0; observable_ind < 2; observable_ind ++) 
    {
      for (unsigned char opt_level = 0; opt_level < 2; opt_level ++)
	{
	  for (ChargeCode_t charge_code = 0; charge_code < 2; charge_code ++)
	    {
	      for (VectorCode_t jetcode = 0; jetcode < 4; jetcode ++)
		{
		  for (RecoLevelCode_t recocode = 0; recocode < 2; recocode ++)
		    {
		      for (ResultLevelCode_t resultlevel = 0; resultlevel < 2; resultlevel ++)
			{
			  const TString  unc_name = TString("unc_") + method + "/unc_table_" + 
				tag_jet_types_[jetcode] + '_' +
				tag_charge_types_[charge_code] + "_" + 
				observables[observable_ind] + "_" + 
				tag_opt[opt_level] + "_" +
				tag_recolevel[recocode] + "_" + 
				tag_resultlevel[resultlevel] + ".txt";
			  printf("%s\n", unc_name.Data());
			  FILE * file = fopen(unc_name, "w");
			  const TString  caption = TString("uncertainty ") + 
			    observabletitles[observable_ind] + " " + 
			    tag_opt[opt_level] + " " +
			    tag_charge_types_[charge_code]+ " " + 
			    tag_recolevel[recocode] + " " + 
			    tag_resultlevel[resultlevel];
			  const TString  label = TString("unc_table_") + 
			    observables[observable_ind] + "_" + 
			    tag_opt[opt_level] + "_" +
			    tag_charge_types_[charge_code]+ "_" + 
			    tag_recolevel[recocode] + "_" + 
			    tag_resultlevel[resultlevel];
			  const TString bin_file_name = input_folder + tag_jet_types_[jetcode] + '_' + tag_charge_types_[charge_code] + '_' + samples[0] + '_' + observables[observable_ind] + '_' + tag_opt[opt_level] + '_' + method + "/binfile.txt";
			  FILE * binfile = fopen(bin_file_name, "r");
			  prepare_header(file, caption.Data(), label.Data(), binfile);
			  fclose(binfile);
			  map<TString, float * > diffmap;
			  unsigned char line = 0;
			  for (unsigned char sample_ind = 0; sample_ind < nsamples; sample_ind ++)
			    {
			      /*result = fscanf(input_file[sample_ind], "%s\t", unc);
				result = fscanf(input_file[sample_ind], "%f\t", &sys);
				result = fscanf(input_file[sample_ind], "%f\n", &sig);
			      */
			      const TString input_file_name = input_folder + tag_jet_types_[jetcode] + '_' + tag_charge_types_[charge_code] + '_' + samples[sample_ind] + '_' + observables[observable_ind] + '_' + tag_opt[opt_level] + '_' + method + "/unc_" + tag_resultlevel[resultlevel] + "_" + 	tag_recolevel[recocode] + "_full.txt";
			      FILE * input_file = fopen(input_file_name, "r");
			      char unc[64];
			      float sys(0.0);
			      float sig(0.0);
			      int result = 0;
			      if (TString(unc_name) == "unc_nominal/unc_table_leading_jet_allconst_pull_angle_OPT_gen_in.txt")
				{
				  //			      printf("cat sample %u\n", sample_ind);
				  //system(("cat " + input_file_name).Data());
				}
			      char rubbish[512];
			      while (result = fscanf(input_file, "%64[^,]%*c %f %f%512[^,\n]%*c", unc, &sys, &sig, rubbish) != EOF)
				{
				  if (TString(unc_name) == "unc_nominal/unc_table_leading_jet_allconst_pull_angle_ORIG_gen_out.txt")
				    {
				      //			      printf("cat sample %u\n", sample_ind);
				      //system(("cat " + input_file_name).Data());

				      //				      printf("unc %s %f %f\n", unc, sys, sig);
				      ///getchar();
				    }
				  const float diffnom = 100 * fabs(sys - sig)/sig;
				  TString key(unc);
				  if (method.compare("nominal") == 0)
				    {
				      if (key != "MC13TeV_TTJets_herwig" and key.Contains("herwig"))
					{
					  key.ReplaceAll("_herwig", "");
					}
				    }
				  if (method.compare("cflip") == 0)
				    {
				      if (key != "MC13TeV_TTJets_cflip" and key.Contains("cflip"))
					{
					  key.ReplaceAll("_cflip", "");
					}
				    }
				  key.ReplaceAll("_", "\\_");
				  key.ReplaceAll("t#bar{t}", "$t\\overline{t}$");

				  map<TString, float * >::iterator mit = diffmap.find(key);
				  if (mit == diffmap.end())
				    {
				      diffmap[key] = new float[nsamples];
				      for (unsigned char ind = 0; ind < nsamples; ind ++)
					{
					  diffmap[key][ind] = - 10.0;
					}
				    }
				  diffmap[key][sample_ind] = diffnom;
				}
			    }
			  for (map<TString, float * >::iterator mit = diffmap.begin(); mit != diffmap.end(); mit ++)
			    {
			      fprintf(file, "\t\t\t%s", mit -> first.Data());
			      for (unsigned char sample_ind = 0; sample_ind < nsamples; sample_ind ++)
				{
				  if (mit -> second[sample_ind] != -10.0)
				    fprintf(file, "\t&\t%.3f", mit -> second[sample_ind]);
				  else
				    fprintf(file, "\t&\tX\t");

				}
			      fprintf(file, "\\\\\n");
			    }
			  for (map<TString, float * >::iterator mit = diffmap.begin(); mit != diffmap.end(); mit ++)
			    {
			      delete [] mit -> second;
			    }
			  prepare_footer(file);
			  fclose(file);
			}
		    }
		    
		}
	    }
	}
    }
}    



void prepare_header(FILE * file, const char * caption, const char * label, FILE * binfile)
{
  fprintf(file, "\\begin{table}[htp]\n");
  fprintf(file, "\t\\begin{center}\n");
  fprintf(file, "\t\\caption{%s}\n", caption);
  fprintf(file, "\t\\label{tab:%s}\n", label);
  fprintf(file, "\t\t\\begin{tabular}{l|cc}\n");
  fprintf(file, "\t\t\t\\hline\n");
  fprintf(file, 
	  "\t\t\t\\makecell[c]{\\multirow{2}{*}{nuisance}} & \\multicolumn{2}{c}{uncertainty in bin 1 (\\%)}\\\\\n");
  fprintf(file, "\t\t\t & %s & %s \\\\\n", 
	  sampletitles[0], 
	  sampletitles[1]);
  float lowedge1, lowedge2;
  fscanf(binfile, "%*s %f %f\n", &lowedge1, &lowedge2);
  fprintf(file, "\t\t\t & \\multicolumn{2}{c}{%.2f - %.2f}\\\\\n", lowedge1, lowedge2);
  fprintf(file, "\t\t\t\\hline\n");
}

void prepare_footer(FILE *file)
{
  fprintf(file, "\t\t\\hline\n");
  fprintf(file, "\t\t\\end{tabular}\n");
  fprintf(file, "\t\\end{center}\n");
  fprintf(file, "\\end{table}\n");

}
