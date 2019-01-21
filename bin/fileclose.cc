#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TApplication.h"
int main()
{
  TApplication app("myapp", 0, 0);
  TFile *f = TFile::Open("testfileclose.root", "RECREATE");
  TH1F h("histo", "histo", 5, -2, 2);
  h.FillRandom("gaus");
  h.Write();
  h.SetDirectory(nullptr);
  f -> Close();
  TCanvas c;
  h.Draw();
  app.Run(kTRUE);
  app.Terminate();
}
