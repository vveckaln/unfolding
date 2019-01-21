#include "Definitions.hh"
#include "SampleDescriptor.hh"
#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "THStack.h"
#include "TStyle.h"
#include <assert.h> 
#include <string> 
#include "HistoUnfoldingTH2.hh"
#include "HistoUnfoldingTH1.hh"
#include "TF2.h"
int main()
{
  gStyle -> SetErrorX(0);
  TCanvas c("check", "check");
  TFile * f = TFile::Open("test.root", "RECREATE");
  SampleDescriptor sd;
  sd.SetName("name");
  sd . SetTitle("sd");
  sd . SetTag("tag");
  //sd . SetColor("color");
  sd._colornum = 0;
  sd. SetXsec(5);
  sd. ls();
  sd.Write();
  printf("probe A\n");
  HistoUnfoldingTH2 * hu = new HistoUnfoldingTH2("huname", "hutitle", 20, 0.0, 30.0, 10, 0.0, 5.0, &sd);
  hu->ls();  
  TF2 * xyg = new TF2("xyg","xygaus", 0.0, 10.0, 0.0, 10.0); 
  xyg -> SetParameters(1.0, 5.0, 2.0, 5.0, 2.0); //amplitude, meanx,sigmax,meany,sigmay 
  hu->GetTH2() -> FillRandom("xyg");
  hu->GetTH2() -> SetDirectory(nullptr);
  HistoUnfoldingTH1 hu1(hu);
  TH1D * hreco = new TH1D("reco", "reco", 20, 0.0, 10.0);
  hreco -> SetDirectory(nullptr);
  hreco -> FillRandom("gaus");
  TH1D * hgen = new TH1D("gen", "gen", 20, 0.0, 10.0);
  hgen -> SetDirectory(nullptr);
  hgen -> FillRandom("gaus", 2000);
  hreco -> Write();
  hgen -> Write();
  hu1.GetTH1Ref(RECO) = hreco;
  hu1.GetTH1Ref(GEN) = hgen;
  HistoUnfolding * huparent = & hu1;
  huparent -> SetName("hu1");
  huparent -> Write();
  f -> Close();
}
