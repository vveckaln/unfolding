#include "Utilities.hh"
#include "TCanvas.h"
#include "TF1.h"

void Do()
{
  printf("test_int %u\n", test_int);
}

TString CreateName(const char * name)
{
  return TString(name) + "_" + tag_opt[opt_level_ind] + "_" + tag_sys[sys_level_ind] + "_" + tag_sf[sf_level_ind];
}
TString CreateTitle(const char * title)
{
  return TString(title) + " " + tag_opt[opt_level_ind] + " " + tag_sys[sys_level_ind] + " " + tag_sf[sf_level_ind];;

}
void Print(TObject * obj, const char * option)
{
  if (not obj)
    return;
  const TString name = obj -> GetName();
  TCanvas c(TString("c_") + name, TString("c_") + name);
  obj -> Draw(option);
  c.SaveAs(pic_folder + "/" + name + ".png");
}

void NormaliseToBinWidth(TH1 *h)
{
  for (unsigned char ind = 1; ind < h -> GetNbinsX() + 1; ind ++)
    {
      h -> SetBinContent(ind, h -> GetBinContent(ind) * orig_bin_width / h -> GetBinWidth(ind));
      h -> SetBinError(ind, h -> GetBinError(ind) * orig_bin_width / h -> GetBinWidth(ind));

    }
}

void ProcessHistogram(TH1 * h)
{
  if (not h)
    return;
  NormaliseToBinWidth(h);
  h -> SetMinimum(0.0);
  Print(h);
}

 vector<float> splitForMinSigma(TH2D & h, float factor)
{
  TAxis * Xaxis = h.GetXaxis();
  //TAxis * Yaxix = h.GetYaxis();

  TF1 f1 = TF1("f1", "gaus", Xaxis -> GetXmin(), Xaxis -> GetXmax());
  f1.SetParameters(100, (Xaxis -> GetXmax() - Xaxis -> GetXmin())/2., 0.01);
  TObjArray slices;
  h.FitSlicesX(& f1, 1, h.GetNbinsX(), 0, "QNRLM", &slices);
    
  TCanvas c2("c2", "c2");
  ((TH1D*)slices[0]) -> Draw();
  c2.Print(pic_folder + "/slices_N.eps");
  ((TH1D*)slices[1]) -> GetYaxis() -> SetRangeUser(Xaxis -> GetXmin(), Xaxis -> GetXmax());
  ((TH1D*)slices[1]) -> Draw();
  c2.Print(pic_folder + "/slices_mean.eps");
  ((TH1D*)slices[2]) -> GetYaxis() -> SetRangeUser(0.0, Xaxis -> GetXmax());
  ((TH1D*)slices[2]) -> Draw();
  c2.Print(pic_folder + "/slices_sigma.eps");
  
  vector<float> bins;
 
  bins.push_back(Xaxis -> GetXmin());
  vector<float> exact;
  exact.push_back(Xaxis -> GetXmin());
  
  for (unsigned short i = 1; i < h.GetNbinsX()+1; i++)
    {
      const float mean  = ((TH1D *)slices[1]) -> GetBinContent(i);
      const float sigma = ((TH1D *)slices[2]) -> GetBinContent(i) * factor;
      printf("i %u mean %f sigma %f mean - sigma %f exact.back() %f \n", i, mean, sigma, mean - sigma, exact.back());
      if (mean - sigma > exact.back())
      {
	if (mean + sigma < Xaxis -> GetXmax())
	  {
	    printf("mean + sigma %f\n", mean + sigma);
	    exact.push_back(mean + sigma);
	    const float newbinedge = Xaxis -> GetBinUpEdge((Xaxis -> FindBin(mean + sigma)));
	    if (newbinedge > bins.back()) 
	      {
		printf("newbinedge %f, bins.back() %f\n", newbinedge, bins.back());
		bins.push_back(newbinedge);
	      }			    
	
	  }
	else
	  {
	    printf("         !!!! mean + sigma > Xaxis -> GetXmax()\n");
	  }
      }
     
    }
  if (bins.size() > 1)
    {
      bins.back() = Xaxis -> GetXmax();
    }
  return bins;
}

 vector<float> splitForMinSigmaM(TH2D & h, float factor)
{
  TAxis * Xaxis = h.GetXaxis();
  //TAxis * Yaxix = h.GetYaxis();

  TF1 f1 = TF1("f1", "gaus", Xaxis -> GetXmin(), Xaxis -> GetXmax());
  f1.SetParameters(100, (Xaxis -> GetXmax() - Xaxis -> GetXmin())/2., 0.01);
  TObjArray slices;
  h.FitSlicesX(& f1, 1, h.GetNbinsX(), 0, "QNRLM", &slices);
    
  TCanvas c2("c2", "c2");
  ((TH1D*)slices[0]) -> Draw();
  c2.Print(pic_folder + "/slices_N.eps");
  ((TH1D*)slices[1]) -> GetYaxis() -> SetRangeUser(Xaxis -> GetXmin(), Xaxis -> GetXmax());
  ((TH1D*)slices[1]) -> Draw();
  c2.Print(pic_folder + "/slices_mean.eps");
  ((TH1D*)slices[2]) -> GetYaxis() -> SetRangeUser(0.0, Xaxis -> GetXmax());
  ((TH1D*)slices[2]) -> Draw();
  c2.Print(pic_folder + "/slices_sigma.eps");
  
  vector<float> bins;
 
  bins.push_back(Xaxis -> GetXmin());
  vector<float> exact;
  exact.push_back(Xaxis -> GetXmin());
  const float integral = h.Integral();
  float binfraction = 0.0;
  for (unsigned char ind = 0; ind < h.GetNbinsX() + 1; ind ++)
    {
      binfraction += h.ProjectionX("px", ind, ind) -> Integral()/integral;
      const float mean      = ((TH1D *)slices[1]) -> GetBinContent(ind);
      const float meanError = ((TH1D *)slices[1]) -> GetBinError(ind);
      if (mean == 0 or meanError/mean > 0.25)
	continue;
      const float sigma = ((TH1D *) slices[2]) -> GetBinContent(ind) * factor;
      if (mean - sigma > exact.back())
	 {
	   if (mean + sigma < h.GetXaxis() -> GetXmax())
	     {
	       exact.push_back(mean + sigma);
	       const float newbinedge = h.GetXaxis() -> GetBinUpEdge(h.GetXaxis() -> FindBin(mean + sigma));
	       if (newbinedge > bins.back() and binfraction > 0.01)
		 {
		   bins.push_back(newbinedge);
		   binfraction = 0.0;
		 }
	     }
	 }
	 }
  if (bins.size() > 1)
    {
      bins.back() = Xaxis -> GetXmax();
    }
  return bins;
}



vector<float> splitForMinSigma_v2(TH2D & h, const char * option, float factor)
{
  vector<float> bins;
  TAxis * axis;
  TObjArray slices;
  void (TH2D::*f_pointer)(TF1*, Int_t, Int_t, Int_t , Option_t*, TObjArray* );
  if (TString(option) == "X")
    {
      axis = h.GetXaxis();
      f_pointer = &TH2D::FitSlicesX;
    }
  else 
    {
      axis = h.GetYaxis();
      f_pointer = &TH2D::FitSlicesY;
    }
  const float up = axis -> GetXmax();
  const float down = axis -> GetXmin();
  TF1 f1 = TF1("f1", "gaus", down, up);
  f1.SetParameters(100, (up - down)/2., 1.0);
  (h .* f_pointer)(& f1, 1, h.GetNbinsX(), 0, "QNRLM", &slices);


  TCanvas c2("c2", "c2");
  ((TH1D*)slices[0]) -> Draw();
  c2.Print(pic_folder + "/slices_N_" + TString(option) + ".png");
  ((TH1D*)slices[1]) -> GetYaxis() -> SetRangeUser(axis -> GetXmin(), axis -> GetXmax());
  ((TH1D*)slices[1]) -> Draw();
  c2.Print(pic_folder + "/slices_mean_" + TString(option) + ".png");
  ((TH1D*)slices[2]) -> GetYaxis() -> SetRangeUser(0.0, axis -> GetXmax());
  ((TH1D*)slices[2]) -> Draw();
  c2.Print(pic_folder + "/slices_sigma_" + TString(option) + ".png");
  const unsigned short ind_start = axis -> GetNbins() % 2 == 0 ? axis -> GetNbins()/2 : (axis -> GetNbins() - 1)/2 + 1;
  
  vector<float> bins_up;
  {
    unsigned short ind_prev = ind_start;
    float mean_prev = ((TH1D *)slices[1]) -> GetBinContent(ind_prev);
    float sigma_prev = ((TH1D *)slices[2]) -> GetBinContent(ind_prev) * factor;
    for (unsigned short ind = ind_start; ind < axis -> GetNbins() + 1; ind ++)
      {
	const float mean  = ((TH1D *)slices[1]) -> GetBinContent(ind);
	const float sigma = ((TH1D *)slices[2]) -> GetBinContent(ind) * factor;
	//	printf("ind_down %u mean %f sigma %f mean - sigma %f mean_prev + sigma_prev %f mean + sigma %f\n", ind, mean, sigma, mean - sigma, mean_prev + sigma_prev, mean + sigma);
	if (mean - sigma > mean_prev + sigma_prev and mean + sigma < axis -> GetXmax())
      {
	const float distance = mean - mean_prev;
	const float split = mean - sigma - (mean_prev + sigma_prev);
	const float ratio = split/distance;
	const float segment1 = sigma_prev + split * sigma_prev/(sigma + sigma_prev);
	const float segment2 = sigma + split * sigma/(sigma + sigma_prev);
	bins_up.push_back(mean_prev + segment1);
	//	printf("    pushed back %f\n", bins_up.back());
	mean_prev = mean;
	sigma_prev = sigma;
      }
  }
  if (bins_up.size() > 1)
    {
      bins_up.back() = axis -> GetXmax();
    }
 
}
// *************************** DOWN *************************
  vector<float> bins_down;
  {
    unsigned short ind_prev = ind_start;
    float mean_prev = ((TH1D *)slices[1]) -> GetBinContent(ind_prev);
    float sigma_prev = ((TH1D *)slices[2]) -> GetBinContent(ind_prev) * factor;
    for (  unsigned short ind = ind_start; ind > 0; ind --)
      {
	const float mean  = ((TH1D *)slices[1]) -> GetBinContent(ind);
	const float sigma = ((TH1D *)slices[2]) -> GetBinContent(ind) * factor;
	//printf("ind_down %u mean %f sigma %f\n", ind, mean, sigma);
	if (mean + sigma < mean_prev - sigma_prev and mean - sigma > axis -> GetXmin())
	  {
	    const float distance = mean_prev - mean;
	    const float split = mean_prev - sigma_prev - (mean  + sigma);
	    const float ratio = split/distance;
	    const float segment1 = sigma_prev + split * sigma_prev/(sigma + sigma_prev);
	    const float segment2 = sigma + split * sigma/(sigma + sigma_prev);
	    bins_down.push_back(mean_prev - segment1);
	    mean_prev = mean;
	    sigma_prev = sigma;
	    //	    printf("pushed back %f\n", bins_down.back());
	  }
      }
    if (bins_down.size() > 1)
      {
	bins_down.back() = axis -> GetXmin();
      }

  }
  for (short ind = bins_down.size() - 1; ind > -1; ind --)
    {
      //printf("%u\n", ind);

      bins.push_back(bins_down.at(ind));
    }
  for (unsigned short ind = 0; ind < bins_up.size(); ind ++)
    {
      //printf("%u\n", ind);

      bins.push_back(bins_up.at(ind));
    }
  return bins;
}


void FillFromTree(TTree & tree, TH2D &h)
{
  Double_t pull_angle_reco;
  Double_t weight_reco;
  Double_t pull_angle_gen;
  Double_t weight_gen;
  tree.SetBranchAddress("pull_angle_reco", &pull_angle_reco);
  tree.SetBranchAddress("weight_reco",     &weight_reco);
  tree.SetBranchAddress("pull_angle_gen",  &pull_angle_gen);
  tree.SetBranchAddress("weight_gen",      &weight_gen);
  printf("Filling from  tree; %lu entries\n", tree. GetEntries());
  for (unsigned long event_ind = 0; event_ind < tree.GetEntries(); event_ind ++)
    {
      if (event_ind % 10000 == 0)
	printf("%u\r", event_ind);
      tree.GetEntry(event_ind);
      /*      printf("event ind %lu weight_gen, %.12f weight_reco %.12f, pull_angle_gen %f pull_angle_reco %f\n", event_ind, weight_gen, weight_reco, pull_angle_gen, pull_angle_reco);

      if (weight_reco == weight_gen)
	{
	  getchar();
	  }*/
      const double weight = weight_reco == weight_gen ? 0.5*(weight_reco + weight_gen) : 1.0*(weight_reco + weight_gen);
      h.Fill(pull_angle_reco == -10 ? pull_angle_reco : TMath::Abs(pull_angle_reco), pull_angle_gen == -10.0 ? pull_angle_gen : TMath::Abs(pull_angle_gen), weight);
      //      h.Fill(pull_angle_reco, pull_angle_gen, weight);

    }
  printf("Filling from tree done \n");
}

void FillFromTree(TTree & tree, TH1D &h, RecoLevelCode_t type)
{
  Double_t pull_angle_reco;
  Double_t weight_reco;
  Double_t pull_angle_gen;
  Double_t weight_gen;
  tree.SetBranchAddress("pull_angle_reco", &pull_angle_reco);
  tree.SetBranchAddress("weight_reco",     &weight_reco);
  tree.SetBranchAddress("pull_angle_gen",  &pull_angle_gen);
  tree.SetBranchAddress("weight_gen",      &weight_gen);
  printf("Filling from  tree; %lu entries\n", tree. GetEntries());
  for (unsigned long event_ind = 0; event_ind < tree.GetEntries(); event_ind ++)
    {
      if (event_ind % 10000 == 0)
	printf("%u\r", event_ind);
      tree.GetEntry(event_ind);
      const double weight     = type == RECO ? weight_reco : weight_gen;
      const double pull_angle = type == RECO ? pull_angle_reco : pull_angle_gen;

      h.Fill(pull_angle, weight);
    }
  printf("Filling from tree done \n");
}

void FillFromTreeCor(TTree & tree, TH2D &h, bool abs)
{
  Float_t pull_angle_reco;
  Float_t weight;
  Float_t pull_angle_gen;
  tree.SetBranchAddress("pull_angle_reco", &pull_angle_reco);
  tree.SetBranchAddress("weight",          &weight);
  tree.SetBranchAddress("pull_angle_gen",  &pull_angle_gen);
  printf("Filling from  tree; %lu entries\n", tree. GetEntries());
  for (unsigned long event_ind = 0; event_ind < tree.GetEntries(); event_ind ++)
    {
      if (event_ind % 10000 == 0)
	printf("%u\r", event_ind);
      tree.GetEntry(event_ind);
      //           printf("event ind %lu weight %.12f pull_angle_gen %f pull_angle_reco %f\n", event_ind, weight, pull_angle_gen, pull_angle_reco);

      if (abs)
	h.Fill(pull_angle_reco == -10 ? pull_angle_reco : TMath::Abs(pull_angle_reco), pull_angle_gen == -10.0 ? pull_angle_gen : TMath::Abs(pull_angle_gen), weight);
      else
	h.Fill(pull_angle_reco == -10 ? pull_angle_reco : pull_angle_reco, pull_angle_gen == -10.0 ? pull_angle_gen : pull_angle_gen, weight);
      //	h.Fill(pull_angle_reco, pull_angle_gen, weight);

    }
  printf("Filling from tree done \n");
}

void FillFromTreeCor(TTree & tree, TH1D &h, RecoLevelCode_t type)
{
  float pull_angle_reco;
  float weight;
  float pull_angle_gen;
  tree.SetBranchAddress("pull_angle_reco", &pull_angle_reco);
  tree.SetBranchAddress("pull_angle_gen",  &pull_angle_gen);
  tree.SetBranchAddress("weight",         &weight);
  printf("Filling from  tree; %lu entries\n", tree. GetEntries());
  for (unsigned long event_ind = 0; event_ind < tree.GetEntries(); event_ind ++)
    {
      if (event_ind % 10000 == 0)
	printf("%u\r", event_ind);
      tree.GetEntry(event_ind);
      const double pull_angle = type == RECO ? pull_angle_reco : pull_angle_gen;

      h.Fill(pull_angle, weight);
    }
  printf("Filling from tree done \n");
}



TH1 * MakeAbs(TH1 *h)
{
  vector<float> bins;
  const unsigned char hNbins = h -> GetNbinsX();
  const unsigned int middle = hNbins/2 + 1;
  bins.push_back(0.0);
  for (unsigned char ind = middle; ind <= h -> GetNbinsX() ; ind ++)
    {
      bins.push_back(h -> GetXaxis() -> GetBinUpEdge(ind));
    }
  float * binsfl = convertVectorToArray<float>(bins);
  TH1F *habs = new TH1F(TString(h -> GetName()) + "_abs", h -> GetTitle(), bins.size() -1, binsfl);
  habs -> GetXaxis() -> SetTitle( h-> GetXaxis() -> GetTitle());
  habs -> GetYaxis() -> SetTitle( h-> GetYaxis() -> GetTitle());
  for (unsigned char ind = 0; ind < habs -> GetNbinsX() ; ind ++)
    {
      const unsigned int length = hNbins - (middle + ind);
      const float frac = habs-> GetBinWidth(ind + 1)/h -> GetBinWidth(middle + ind);
      habs -> SetBinContent(ind + 1, frac *(h -> GetBinContent(1 + length) + h -> GetBinContent(middle + ind)));
    }
  habs -> SetBinContent(0, h -> GetBinContent(0));
  habs -> SetBinContent(habs -> GetNbinsX() + 1, h -> GetBinContent(h -> GetNbinsX() + 1));
  for (unsigned char ind = 1; ind <= habs -> GetNbinsX() ; ind ++)
    {
      printf("ind %u content %f\n", habs -> GetBinContent(ind));
    }
  for (unsigned char ind = 1; ind <= h -> GetNbinsX() ; ind ++)
    {
      printf("ind %u content %f\n", h -> GetBinContent(ind));
    }

  return habs;
}

float * GetBinArray(TH1 *h)

{
  printf("h %p\n", h);
  const unsigned char size = h -> GetNbinsX() + 1; 
  float * bins = new float[size];
 
  for (unsigned char ind = 0; ind <= h-> GetNbinsX(); ind ++)
    {
      bins[ind] = h -> GetXaxis() -> GetBinUpEdge(ind);
    }
  return bins;
}

TH1 * adjust(TH1 *h)
{
  const unsigned char newsize = h -> GetNbinsX() - 2 + 1;
  float * newbins = new float[newsize];
  unsigned char newbinsind = 0;
  newbins[0] = h -> GetBinLowEdge(1);
  newbinsind ++;
  for (unsigned char ind = 1; ind <= h -> GetNbinsX(); ind ++)
    {
      if (ind == 1)
	continue;
      if (ind == h -> GetNbinsX() - 1)
	continue;
      newbins[newbinsind] = h -> GetXaxis() -> GetBinUpEdge(ind); 
      newbinsind ++;
    }
  for (unsigned char ind = 0; ind < newsize; ind ++)
    {
      printf("ind %u edge %f\n", ind, newbins[ind]);
    }
  TH1 * hnew = new TH1F( TString(h-> GetName()) + "_adj", h -> GetTitle(), newsize - 1, newbins);
  unsigned char hnewbinind = 1;
  unsigned char holdbinind = 1;
  float store = 0;
  do
    {
      if (holdbinind == 1 or holdbinind == h -> GetNbinsX() - 1) 
	store += h -> GetBinContent(holdbinind);
      else
	{
	  hnew -> SetBinContent(hnewbinind, h -> GetBinContent(holdbinind) + store);
	  hnewbinind ++;
	  store = 0;
	}
      
      holdbinind ++;
    }while(holdbinind <= h -> GetNbinsX());
  printf("adjusted bin content\n");
  for (unsigned char ind = 1; ind <= hnew -> GetNbinsX(); ind ++)
    {
      printf("ind %u %f\n", ind, hnew -> GetBinContent(ind));
    }
  return hnew;
}
