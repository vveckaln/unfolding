#include "Definitions.hh"
#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "THStack.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include <assert.h> 
#include <string> 
int main()
{
  gStyle -> SetErrorX(0);
  TCanvas c("check", "check");
  TH1F *h = new TH1F("test", "test", 2, 0, 2);
  TH1F *h2 = new TH1F("test2", "test2", 2, 0, 2);
  h -> SetBinContent(1, 1.0);
  h -> SetBinContent(2, 3.0);
  h -> SetBinError(1, 0.5);
  h -> SetBinError(2, 0.8);
  h2 -> SetBinContent(1, 1.0);
  h2 -> SetBinContent(2, 2.0);
  TGraphErrors * g =new TGraphErrors(h);
  g-> SetMarkerStyle(kFullCircle);
  g -> SetMarkerColor(kBlack);
  g -> SetLineColor(kRed);
  h2 -> SetMinimum(0.0);
  h2-> SetMaximum(4.0);
  h2 -> Draw();
  g -> Draw("EPSAME");
  c.SaveAs("check.png");
}
