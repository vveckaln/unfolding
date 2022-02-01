R__ADD_INCLUDE_PATH($COMPOUNDHISTOLIB/interface)
R__ADD_INCLUDE_PATH($ROOUNFOLD/src)
#include "CompoundHistoUnfolding.hh"
#include "Definitions.hh"
R__ADD_LIBRARY_PATH($ROOUNFOLD) // if needed
R__LOAD_LIBRARY(libRooUnfold.so)
R__ADD_LIBRARY_PATH($COMPOUNDHISTOLIB/lib) // if needed
R__LOAD_LIBRARY(libCompoundHisto.so)

int correctbranchnames(const char *filename)
{
  const char *observables[2] = {"pull_angle", "pvmag"};
  const char * jettitlestemp[4] = {"first_leading_jet", "scnd_leading_jet", "leading_b",  "snd_leading_b"};
  const char * channeltitles[2] = {"E", "M"};
  const char * chargetitlestemp[2] = {"all", "charged"};
  TFile *file = TFile::Open(filename, "UPDATE");
  TTree * m = (TTree *) file -> Get("migration");
  // printf("%s %p\n", filename, file);
  // file -> Close();
  // return 0;
  // m -> Print("all");
  for (unsigned char channelind = 0; channelind < 2; channelind ++)
    {
      for (unsigned char observableind = 0; observableind < 2; observableind ++)
	{
	  for (unsigned char recoind = 0; recoind < 2; recoind ++)
	    {
	      for (ChargeCode_t charge_code = 0; charge_code < 2; charge_code ++)
		{
		  
		  for (VectorCode_t jetcode = 0; jetcode < 4; jetcode ++)
		    {
		      TString branchname    = TString(channeltitles[channelind]) + "_" + observables[observableind] + "_" + jettitlestemp[jetcode] + "_" + chargetitlestemp[charge_code] + "_" + tag_recolevel[recoind];
		      TString branchnamenew = TString(channeltitles[channelind]) + "_" + observables[observableind] + "_" + tag_jet_types_[jetcode] + "_" + tag_charge_types_[charge_code] + "_" + tag_recolevel[recoind];
		      
		      TBranch * branch = m -> GetBranch(branchname.Data());
		      if (not branch)
			continue;
		      else 
			{
			  printf("correcting %s\n", filename);
			  // getchar();
			}
		      printf("%s %p\n", branchname.Data(), branch);
		      branch -> SetName(branchnamenew.Data());
		      branch -> SetTitle(TString(branch -> GetTitle()).ReplaceAll(branchname.Data(), branchnamenew.Data()));
		    }
		}
	    }
	}
    }
  m -> Write();
  file -> Close();
  return 0;
}
