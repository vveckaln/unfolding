#include "JsonParser.hh"
#include "CompoundHistoUnfolding.hh"
#include "Utilities.hh"
#include "TFile.h"
#include <stdio.h>
#include "TApplication.h"
#include "TCanvas.h"
#include "TColor.h"
#include "TH1F.h"
// READ READ READ READ READ READ READ READ
int main()
{
  //TApplication app("myapp", 0, 0);
  TFile * f = TFile::Open("testCompoundHistoUnfolding.root");
  CompoundHistoUnfolding  * chisto = (CompoundHistoUnfolding *) f -> Get("migration");
  chisto -> Do();
  /*chisto -> CreateCombinedPlot();
  app.Run(kTRUE);
  
  app.Terminate();*/
  printf("probe A\n");
  f -> Close();
}
