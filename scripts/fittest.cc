int fittest()
{
  TFile *f = TFile::Open("migrationfile.root");
  TH2 *h = (TH2*) f -> Get("m");
  h -> SetDirectory(nullptr);
  f -> Close();
  TCanvas * c = new TCanvas;
  TH1 * h1 = h -> ProjectionY("proj", 1, 1);
  h1 -> Draw();
  TAxis * Xaxis = h -> GetXaxis();
  TF1 f1 = TF1("f1", "gaus", 0, 0.1);//Xaxis -> GetXmin(), Xaxis -> GetXmax());
  //  f1.SetRange(1, h -> GetNbinsX());
  f1.SetParameters(100, (Xaxis -> GetXmax() - Xaxis -> GetXmin())/2.0, 0.01);
  TFitResultPtr r = h1 -> Fit(& f1, "QRLMS");
  printf("mean %f stdev %f chi2 %f\n", r -> GetParams()[1], r -> GetParams()[2], r -> Chi2());
  TObjArray slices;
  h -> FitSlicesX(& f1, 1, h -> GetNbinsX(), 0, "QNRLM", &slices);
  const char * _folder = "slices";
  

  TCanvas c2("c2", "c2");
  ((TH1D*)slices[0]) -> Draw();
  c2.Print((string(_folder) + "/slices_N.eps").c_str());
  ((TH1D*)slices[1]) -> GetYaxis() -> SetRangeUser(Xaxis -> GetXmin(), Xaxis -> GetXmax());
  ((TH1D*)slices[1]) -> Draw();
  c2.Print((string(_folder) + "/slices_mean.eps").c_str());
  ((TH1D*)slices[2]) -> GetYaxis() -> SetRangeUser(0.0, Xaxis -> GetXmax());
  ((TH1D*)slices[2]) -> Draw();
  c2.Print((string(_folder) + "/slices_sigma.eps").c_str());

  return 0;
}
