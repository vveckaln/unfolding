#include "Definitions.hh"
#include <assert.h> 
#include <string> 
#include <map>
using namespace std;
TString insertnumber(float, const char *);
void prepare_header(FILE *, const char * caption, const char * label, FILE * binfile, RecoLevelCode_t code);
void prepare_footer(FILE *);
const char ** sampletitles = nullptr;
string env("");


int main(int argc, char * argv[])
{
  assert(argc == 5);
  const string method(argv[1]);
  const string observable(argv[2]);
  const string binningmethod(argv[3]);
  env = string(argv[4]);
  system((string("rm -r unc_") + method + "_full/" + observable + "/" + binningmethod).c_str());
  system((string("mkdir -p unc_") + method + "_full/" + observable + "/" + binningmethod).c_str());
  const char * observables[] = {"pull_angle", "pvmag"};
  const char * observabletitles[] = {"pull angle $\\theta_{p}$", "magnitude of the pull vector $\\left|\\vec{P}\\right|$"};
  const char * samples_nominal[] = {"MC13TeV_TTJets", "MC13TeV_TTJets_herwig", "MC13TeV_TTJets2l2nu_amcatnlo"};
  const char * samples_cflip[] = {"MC13TeV_TTJets", "MC13TeV_TTJets_cflip"};
  const char * chargetags[2] = {"allconst", "chconst"};
  const char * recotags[2] = {"generator", "reconstruction"};
  const char * jettags[4] = {"leading_jet", "scnd_leading_jet", "leading_b", "scnd_leading_b"};
  const char * opjettags[4] = {"scnd_leading_jet", "leading_jet", "scnd_leading_b", "leading_b"};
  const char * tagresult[2][2] = {{"input at the generator level", "input at the reconstruction level"}, {"unfolded ouput", "folded back output"}};
  const char ** samples = nullptr;
  const char * sampletitles_nominal[3] = {"\\POWHEG+\\PYTHIA 8", "\\POWHEG+\\HERWIG", "aMC@NLO+\\PYTHIA 8"};
  const char * sampletitles_cflip[3] = {"\\POWHEG+\PYTHIA 8 *", "\\POWHEG+\\PYTHIA 8 cf"};
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
      for (VectorCode_t jetcode = 0; jetcode < 4; jetcode ++)
	{
	  for (unsigned char sample_ind = 0; sample_ind < nsamples; sample_ind ++)
	    {
	      const TString bin_file_name = input_folder + "/" + 
		tag_jet_types_[jetcode] + '_' + 
		tag_charge_types_[charge_code] + '_' + 
		samples[sample_ind] + '_' + 
		observables[observable_ind] + '_' + 
		tag_opt[opt_level] + '_' + 
		method + bintag + "/binfile.txt";
	      FILE * binfile = fopen(bin_file_name, "r");
	      for (ResultLevelCode_t resultlevel = 0; resultlevel < 2; resultlevel ++)
		{
			  
		  for (RecoLevelCode_t recocode = 0; recocode < 2; recocode ++)
		    {
		      /*result = fscanf(input_file[sample_ind], "%s\t", unc);
			result = fscanf(input_file[sample_ind], "%f\t", &sys);
			result = fscanf(input_file[sample_ind], "%f\n", &sig);
		      */
		      const TString  unc_name = TString("unc_") + method + "_full/" + 
			observable  + "/" + 
			binningmethod + 
			"/unc_table_full_" + 
			tag_jet_types_[jetcode] + '_' +
			tag_charge_types_[charge_code] + "_" + 
			observables[observable_ind] + "_" + 
			tag_opt[opt_level] + "_" +
			tag_recolevel[recocode] + "_" + 
			tag_resultlevel[resultlevel] + "_" + samples[sample_ind] + ".txt";
		      printf("%s\n", unc_name.Data());
		      FILE * file = fopen(unc_name, "w");
		      TString  caption;
		      if (observable_ind == 0)
			{
			  caption = TString("Bin by bin weight of uncertainty (shapes) for \\protect\\observabletitle{") + 
			    observable + "} including \\protect\\chargetitle{" + chargetags[charge_code] + "} from the \\protect\\jettitle{" +
			    jettags[jetcode] + "} to the \\protect\\jettitle{" + opjettags[jetcode] + "} at the \\protect\\recoleveltitle{" + tag_recolevel[recocode] + "} level. The results are for " + tagresult[resultlevel][recocode] + " for the " + sampletitles[sample_ind] + " sample.";
			}
		      else
			{
			  caption = TString("Bin by bin weight of uncertainty (shapes) for \\protect\\observabletitle{") + 
			    observable + "} including \\protect\\chargetitle{" + chargetags[charge_code] + "} of the \\protect\\jettitle{" +
			    jettags[jetcode] + "} at the \\protect\\recoleveltitle{" + tag_recolevel[recocode] + "} level. The results are for " + tagresult[resultlevel][recocode] + " for the " + sampletitles[sample_ind] + " sample.";
			}
		      caption += " The binning method of \\protect\\binningtitle{" + binningmethod + "} is used.";
		      const TString  label = TString("unc_table_full") + 
			observables[observable_ind] + "_" + 
			tag_opt[opt_level] + "_" +
			tag_charge_types_[charge_code]+ "_" + 
			tag_recolevel[recocode] + "_" + 
			tag_resultlevel[resultlevel] + "_" + 
			samples[sample_ind] + "_" +
			method + "_" + binningmethod;
		      int nbins;
		      fseek(binfile, 0, SEEK_SET);
		      fscanf(binfile, "%u", &nbins);
		      if (recocode == RECO)
			nbins *= 2;
		      printf("nbins %u\n", nbins);
		      prepare_header(file, caption.Data(), label.Data(), binfile, recocode);
		      map<TString, float* > diffmap;
		      unsigned char line = 0;
		      const TString input_file_name = input_folder + "/" + 
			tag_jet_types_[jetcode] + '_' + 
			tag_charge_types_[charge_code] + '_' + 
			samples[sample_ind] + '_' + 
			observables[observable_ind] + '_' + 
			tag_opt[opt_level] + '_' + 
			method + bintag + 
			"/unc_" + tag_resultlevel[resultlevel] + "_" + 	
			tag_recolevel[recocode] + "_full.txt";
		      FILE * input_file = fopen(input_file_name, "r");
		      //			      printf("cat sample %u\n", sample_ind);
		      //system(("cat " + input_file_name).Data());
		      char unc[64];
		      float sys;
		      float sig;
		      int result = 0;
		      while (result = fscanf(input_file, "%64[^,]%*c", unc) != EOF)
			{
			  TString key(unc);
			  if (key != "MC13TeV_TTJets_herwig" and key.Contains("herwig"))
			    {
			      key.ReplaceAll("_herwig", "");
			    }
			  key.ReplaceAll("_", "\\_");
			  key.ReplaceAll("t#bar{t}", "$t\\overline{t}$");
			  printf("%s\n", key.Data());

			  map<TString, float * >::iterator mit = diffmap.find(key);
			  if (mit == diffmap.end())
			    {
			      diffmap[key] = new float[nbins];
			      for (unsigned char ind = 0; ind < nbins; ind ++)
				{
				  diffmap[key][ind] = - 10.0;
				}
			    }
			  char temp;
			  unsigned char ind = 0;
			  do {
			    fscanf(input_file, "%f %f%c", &sys, &sig, &temp);
			    printf("bin %u %s %s\n",  ind, insertnumber(sys, ".9").Data(), insertnumber(sig, ".9").Data());
			    const float diffnom = 100 * fabs(sys - sig)/sig;
			    diffmap[key][ind] = diffnom;
			    ind ++;
			  } while (temp != '\n');
			}
		      for (map<TString, float * >::iterator mit = diffmap.begin(); mit != diffmap.end(); mit ++)
			{
			  fprintf(file, "\t\t\t%s", mit -> first.Data());
			  for (unsigned char bind = 0; bind < nbins; bind ++)
			    {
			      fprintf(file, "\t&\t%s", insertnumber(mit -> second[bind], ".3").Data());
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
	      fclose(binfile);
	    }
		    
	}
    }
}
    



void prepare_header(FILE * file, const char * caption, const char * label, FILE * binfile, RecoLevelCode_t code)
{
  //fprintf(file, "\\begin{longtable}[htp]\n");
  // fprintf(file, "\t\\begin{center}\n");
  int nbins;
  fseek(binfile, 0, SEEK_SET);
  fscanf(binfile, "%u", & nbins);
  string line1(""), line2(""), line3(""); 
  float ledge1, ledge2;
  for (unsigned char bind = 0; bind < nbins; bind ++ )
    {
      if (bind == 0)
	{
	  fscanf(binfile, "%f %f", &ledge1, &ledge2);
	}
      else
	{
	  ledge1 = ledge2;
	  fscanf(binfile, "%f", &ledge2);
	}
      char line2add[128];
      char line3add[128];
      line1 += "c";
      if (code == GEN)
	{
	  if (env.compare("lx") != 0)
	    {
	      sprintf(line2add, " & \\makecell{\\textbf{bin %u}} ", bind + 1);
	      sprintf(line3add, " & \\makecell{\\textbf{%s\\textendash%s}}", insertnumber(ledge1, ".2").Data(), insertnumber(ledge2, ".2").Data());
	    }
	  else
	    {
	      sprintf(line2add, " & \\textbf{bin %u} ", bind + 1);
	      sprintf(line3add, " & \\textbf{%s\\textendash%s}", insertnumber(ledge1, ".2").Data(), insertnumber(ledge2, ".2").Data());
	    }
	  line2 += line2add;
	  line3 += line3add;
	}
      if (code == RECO)
	{
	  line1 += "c";
	  if (env.compare("lx") != 0)
	    {
	      sprintf(line2add, " & \\makecell{\\textbf{bin %u}} & \\makecell{\\textbf{bin %u}} ", 2*bind, 2*bind + 1);
	      sprintf(line3add, " & \\makecell\\{textbf{%s\\textendash%s}} & \\makecell{\\textbf{%s\\textendash%s}}", insertnumber(ledge1, ".2").Data(), insertnumber(0.5*(ledge2 + ledge1), ".2").Data(), insertnumber(0.5*(ledge2 + ledge1), ".2").Data(), insertnumber(ledge2, ".2").Data());
	    }
	  else
	    {
	      sprintf(line2add, " & \\textbf{bin %u} & \\textbf{bin %u} ", 2*bind, 2*bind + 1);
	      sprintf(line3add, " & \\textbf{%s\\textendash%s} & \\textbf{%s\\textendash%s}", insertnumber(ledge1, ".2").Data(), insertnumber(0.5*(ledge2 + ledge1), ".2").Data(), insertnumber(0.5*(ledge2 + ledge1), ".2").Data(), insertnumber(ledge2, ".2").Data());
	    }
	  line2 += line2add;
	  line3 += line3add;

	}
    }
	 
  fprintf(file, "\t\t\\begin{longtable}{p{0.6\\linewidth}|%s}\n", line1.c_str());
  fprintf(file, "\t\\caption{%s}\n", caption);
  fprintf(file, "\t\\label{tab:%s}\\\\\n", label);
  fprintf(file, "\t\t\t\\noalign{\\global\\arrayrulewidth=0.5mm}\\hline\\noalign{\\global\\arrayrulewidth=0.4pt}\n");
  if (env.compare("lx") != 0)
    fprintf(file, "\t\t\t\\makecell[c]{\\multirow{3}{*}{\\textbf{Nuisance}}} &\\multicolumn{%u}{c}{\\textbf{Uncertainty in bins [\\%]}}\\\\\n", code == RECO ? 2*nbins : nbins);
  else
    fprintf(file, "\t\t\t\\multirow{3}{*}{\\textbf{Nuisance}} &\\multicolumn{%u}{c}{\\textbf{Uncertainty in bins [\\%]}}\\\\\n", code == RECO ? 2*nbins : nbins);
    
  fprintf(file, "\t\t\t%s \\\\\n", line2.c_str());
  
  fprintf(file, "\t\t\t%s \\\\\n", line3.c_str());
  fprintf(file, "\t\t\t\\hline\n");
}

void prepare_footer(FILE *file)
{
  fprintf(file, "\t\t\\noalign{\\global\\arrayrulewidth=0.5mm}\\hline\n");
  fprintf(file, "\t\t\\end{longtable}\n");
  // fprintf(file, "\t\\end{center}\n");
  // fprintf(file, "\\end{table}\n");

}

TString insertnumber(float n, const char * format)
{
  return Form((string("%") + format + "f").c_str(), n);
  return TString("\\num{") + Form((string("%") + format + "f}").c_str(), n); 
}
