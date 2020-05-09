//To run: 
//   root -l -q 'scripts/ematrixtotal.cc("nominal", "pull_angle", "ATLAS3")'
// https://root-forum.cern.ch/t/start-root-with-a-root-file-and-script/28635/3
R__ADD_INCLUDE_PATH($COMPOUNDHISTOLIB/interface)
#include "CompoundHistoUnfolding.hh"
R__ADD_LIBRARY_PATH($COMPOUNDHISTOLIB/lib) // if needed
R__LOAD_LIBRARY(libCompoundHisto.so)

int ematrixtotal(const char * methodchar, const char * observable, const char * binmethod)
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
  // gStyle -> SetOptStat(0);
  // gStyle -> SetOptTitle(0);
  // gROOT -> SetBatch(kTRUE);
  /*  for (unsigned char observable_ind = 0; observable_ind < 2; observable_ind ++) 
      {*/
  // system(TString("mkdir -p ") + dir);
  // system(TString("rm ") + dir + "/*");

  // for (unsigned char opt_level = 0; opt_level < 2; opt_level ++)
  //   {
  for (ChargeCode_t charge_code = 0; charge_code < 2; charge_code ++)
    {
      if (charge_code != 0)
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
	  TH2 * ematrixtotal = ch -> ematrixtotal;
	  TCanvas * c = new TCanvas;
	  c -> SetRightMargin(0.15);
	  ematrixtotal -> Draw("COLZTEXT");
	  ematrixtotal -> SetMarkerSize(2.0);
	  c -> Update();
	  c -> Modified();
	  c -> SaveAs("ematrixtotal.png");
	  getchar();
	}
    }
  return 0;
}
