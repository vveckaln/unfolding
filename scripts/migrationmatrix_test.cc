int migrationmatrix_test()
{
  const unsigned char Number = 6;

  Double_t Red[Number]   =  {0.75, 0.75, 0.0,     0.0, 0.0,       0.0,};
  Double_t Green[Number] =  {1.00, 1.00, 0.3,     0.3, 0.7,       0.7 };
  Double_t Blue[Number]  =  {0.75, 0.75, 0.0,     0.1, 0.0,       0.0 };
  //Double_t  Stops[Number] = {0.00, 0.50, 0.50001, 0.6, 0.6000001, 1.0};
  Double_t  Stops[Number] = {0.00, 50.0, 50.001,  60.0,  60.00001, 100.0};
  TColor::CreateGradientColorTable(Number, Stops, Red, Green, Blue, 10);

  gStyle -> SetPaintTextFormat("2.2f");
  gStyle -> SetOptStat(0);
  TH2 * h = new TH2F("h", "h", 3, -0.5, 2.5, 3, -0.5, 2.5);
  h -> Fill(0.0, 0.0, 62.0);
  h -> Fill(0.0, 1.0, 51.0);
  h -> Fill(0.0, 2.0, 40.0);

  h -> Fill(1.0, 0.0, 20.0);
  h -> Fill(1.0, 1.0, 70.0);
  h -> Fill(1.0, 2.0, 26.0);

  h -> Fill(2.0, 0.0, 10.0);
  h -> Fill(2.0, 1.0, 27.0);
  h -> Fill(2.0, 2.0, 66.0);
  // h -> SetMinimum(0.0);
  // h -> SetMaximum(100.0);

  TCanvas * c = new TCanvas;
  h -> Draw("COLZTEXT");
  h -> SetMarkerSize(3.0);

  return 0;


  // const unsigned char Number = 8;

  // Double_t Red[Number]   =  {0.75, 0.75, 0.6,      0.6,  0.4,     0.4,   0.2,       0.2};
  // Double_t Green[Number] =  {1.0,  1.0,  1.0,      1.0,  1.0,     1.0,   1.0,       1.0};  
  // Double_t Blue[Number]  =  {0.75, 0.75, 0.6,      0.6,  0.4,     0.4,   0.2,       0.2};
  // Double_t Stops[Number] =  {0.0 , 20.0, 20.0001,  40.0, 40.001,  60.0,  60.00001,  100.0};

  

  // TColor::CreateGradientColorTable(Number, Stops, Red, Green, Blue, 2*Number-2);
  // gStyle -> SetPaintTextFormat("2.2f");
  // gStyle -> SetOptStat(0);
  // TH2 * test = new TH2F("test", "test", 2, -0.5, 1.5, 2, -0.5, 1.5);
  // test -> Fill(0.0, 0.0, 75.0);
  // test -> Fill(0.0, 1.0, 15.0);
  // test -> Fill(1.0, 0.0, 55.0);
  // test -> Fill(1.0, 1.0, 25.0);
  // TCanvas * ctest = new TCanvas;
  // test -> Draw("COLZ");
  // printf("test drawn\n");
  // return 0;
}
