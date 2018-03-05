#include "Utilities.hh"
#include "TCanvas.h"
#include "TF1.h"
const char * level_tag[2]   = {"orig", "opt"};  
TString tag_syst            = "";
unsigned char level_ind     = 0;
float orig_bin_width        = 0;
TString pic_folder          = "";
int  test_int               = 0;
void Do()
{
  printf("test_int %u\n", test_int);
}

TString CreateName(const char * name)
{
  return TString(name) + "_" + level_tag[level_ind] + "_" + tag_syst;
}
TString CreateTitle(const char * title)
{
  return TString(title) + " " + level_tag[level_ind] + " " + tag_syst;

}
void Print(TObject * obj, const char * option)
{
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
    }
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
  c2.Print("migr_analysis/slices_N.eps");
  ((TH1D*)slices[1]) -> GetYaxis() -> SetRangeUser(Xaxis -> GetXmin(), Xaxis -> GetXmax());
  ((TH1D*)slices[1]) -> Draw();
  c2.Print("migr_analysis/slices_mean.eps");
  ((TH1D*)slices[2]) -> GetYaxis() -> SetRangeUser(0.0, Xaxis -> GetXmax());
  ((TH1D*)slices[2]) -> Draw();
  c2.Print("migr_analysis/slices_sigma.eps");
  
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
            h.Fill(TMath::Abs(pull_angle_reco), TMath::Abs(pull_angle_gen), weight);
      //      h.Fill(pull_angle_reco, pull_angle_gen, weight);

    }
  printf("Filling from tree done \n");
}

void FillFromTree(TTree & tree, TH1D &h, TypeCode_t type)
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
