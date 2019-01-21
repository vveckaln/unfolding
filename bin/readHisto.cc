#include "CompoundHisto.hh"
#include "TFile.h"
int main()
{
  TFile *f = TFile::Open("testHisto.root");
  Histo *h = (Histo*) f -> Get("histo");
  h -> ls();
  printf("integral %f\n", h -> _th1 -> Integral());
}
