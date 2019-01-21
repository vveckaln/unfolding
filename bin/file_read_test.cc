#include <stdio.h>
#include <stdlib.h>
int main ()
{
  float chi;
  unsigned int ndf;
  FILE * pFile;

  pFile = fopen ("/eos/user/v/vveckaln/unfolding_cflip/leading_jet_allconst_MC13TeV_TTJets_pull_angle_ORIG/chi.txt", "r");
  system("cat /eos/user/v/vveckaln/unfolding_cflip/leading_jet_allconst_MC13TeV_TTJets_pull_angle_ORIG/chi.txt");
  fscanf (pFile, "%f\n", &chi);
  fscanf (pFile, "%u", &ndf);
  fclose (pFile);
  printf ("chi %f ndf %u\n", chi, ndf);
  return 0;
}
