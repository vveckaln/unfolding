#include "TH1F.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TApplication.h"
void func();
int main()
{
  TApplication app("myapp", 0, 0);
  func();
  app.Run(kTRUE);
  app.Terminate();
  
}

void func()
{
  TH1F * h = new TH1F("histo", "histo", 5, -3, 3);
  h -> FillRandom("gaus");
  TH1F * h2 = new TH1F("histo2", "histo2", 5, -10, 10);
  h2 -> FillRandom("gaus");
  h2 -> SetFillColor(kRed);
  TCanvas *c = new TCanvas("compound", "compound", 500, 500);
  TPad *pad = new TPad("pad1", "pad1", 0.0, 0.2, 1.0, 1.0);
  pad -> Draw();
  pad -> cd();
  h -> Draw(); 
  
  //pad -> Draw();
  c-> cd();
  TPad *pad2 = new TPad("pad2", "pad2", 0.0, 0.0, 1.0, 0.2);
  pad2 -> Draw();
  pad2 -> cd();
  h2 -> Draw();
  //  pad -> SetRightMargin(0.05);
  //pad -> SetLeftMargin(0.12);
  //pad -> SetTopMargin(0.06);
  //pad -> SetBottomMargin(0.01);
  //p1 -> Draw();

  /*pad2 -> SetBottomMargin(0.05);
  pad2 -> SetRightMargin(0.05);
  pad2 -> SetLeftMargin(0.12);
  pad2 -> SetTopMargin(0.01);*/
  //  pad2 -> SetGridx(kFALSE);
  //pad2 -> SetGridy(kTRUE);
  //FormatHistograms();
  c -> SetBottomMargin(0.0);
  c -> SetLeftMargin(0.0);
  c -> SetTopMargin(0);
  c -> SetRightMargin(0.00);
  
  //pad2 -> Draw();

}
