#include <stdio.h>
//#include <stdlib.h>
#include "TString.h"
int main()
{
  const TString filename("root://eosuser.cern.ch//eos/user/v/vveckaln/binning_pull_angle/pull_angle_chconst_leading_jet/bins_chconst_leading_jet.txt");
  FILE *file = fopen("$EOS/binning_pull_angle/pull_angle_chconst_leading_jet/bins_chconst_leading_jet.txt", "r");
  //system((TString("cat ") + "/eos/user/v/vveckaln/binning_pull_angle/pull_angle_chconst_leading_jet/bins_chconst_leading_jet.txt").Data());
  //FILE * file = fopen("bins_chconst_leading_jet.txt", "r");
  printf("file %p\n", file);
  fclose(file);
}

