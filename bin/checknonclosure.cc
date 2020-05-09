#include "CompoundHistoUnfolding.hh"
#include "TStyle.h"
#include "TFile.h"
#include "TMath.h"
#include "TUnfoldDensity.h"
#include "TKey.h"

int main()
{
  gStyle -> SetOptTitle(kFALSE);
  gStyle -> SetOptStat(kFALSE);
  // const TString file_name = "$EOS/unfolding_nominal/pull_angle/ATLAS4/leading_jet_chconst_MC13TeV_TTJets_pull_angle_OPT_nominal_ATLAS4/save.root";
  const TString file_name = "$EOS/unfolding_nominal/pull_angle/optfixed/leading_jet_chconst_MC13TeV_TTJets_pull_angle_OPT_nominal_optfixed/save.root";
  TFile * file = TFile::Open(file_name, "READ");
  CompoundHistoUnfolding * ch = (CompoundHistoUnfolding * )((TKey * ) file -> GetListOfKeys() -> At(0)) -> ReadObj();
  TH2 * hsignal = (TH2 * ) ch -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone("unfolding_signal");
  TH1 * input = ch -> GetLevel(IN) -> GetHU(DATAMBCKG) -> Project(RECO, ch -> CreateName("input"));
  ch -> ApplyScaleFactor(input);
  HistoUnfoldingTH1 * nonclosout = new HistoUnfoldingTH1((SampleDescriptor*) ch -> GetLevel(IN) -> GetHU(SIGNALMO));
  nonclosout -> SetTag(TString(nonclosout -> GetTag()) + "_non_clos");
  nonclosout -> SetCategory("Non closure");
  nonclosout -> SetSysType(THEORSYS);

  HistoUnfoldingTH2 * nonclosin = new HistoUnfoldingTH2((SampleDescriptor*) ch -> GetLevel(IN) -> GetHU(SIGNALMO));
  nonclosin -> SetTag(TString(nonclosin -> GetTag()) + "_non_clos");
  nonclosin -> SetCategory("Non closure");
  nonclosin -> SetSysType(THEORSYS);



  const unsigned char nbins = hsignal -> GetNbinsY();
  double binsrecord[nbins];
  double binsrecordold[nbins];
  for (unsigned char bind = 0; bind < nbins; bind ++)
    {
      binsrecord[bind] = 1.0;
      binsrecordold[bind] = binsrecord[bind];
    }

  float covrecord = 1E6;
  auto varylambdanest = [ &binsrecord, &binsrecordold, & covrecord, nbins, ch, input](unsigned char start)  -> void
    {
      float bins[nbins];

      auto varylambda =[ &bins, &binsrecord, &binsrecordold, & covrecord, nbins, ch, input](unsigned char start, unsigned char k, auto & func) -> void
      {
	const float dev = 0.2;
        const double min = binsrecordold[start] - dev/TMath::Power(2, k);
        const double max = binsrecordold[start] + dev/TMath::Power(2, k);
	// printf("start %u %f %f %u\n", start, min, max, k);
	// getchar();
	const unsigned long M = 1E4;
        double step = (max - min) /  TMath::Power(M * nbins, 1.0/nbins);
	// printf("%f\n", TMath::Power(M, 1.0/nbins));
	// getchar();
	// step *= TMath::Power(2, nbins);
        for (double coeff = min; coeff <= max; coeff += step)
          {
	    
            bins[start] = coeff;
	    if (start == 0)
	      printf("%f\r", bins[start]);
            if (start < nbins - 1)
	      {
		// printf("probe \n");
		func(start + 1, k, func);
	      }
            else
              {
                // for (unsigned char bind = 0; bind < nbins; bind ++)
                //   {
                //      printf("%f ", bins[bind]);                                                                                                                                                          
                //   }
                // printf("\n");                                                                                                                                                                         
		
                TH2 * hin = (TH2 *) ch -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone("hin");
                for (unsigned char bind = 0; bind <= hin -> GetNbinsX() + 1; bind ++)
                  hin -> SetBinContent(bind, 0, 0.0);
                for (unsigned char biny = 1; biny < hin -> GetNbinsY() +1; biny ++)
                  {
                    for(unsigned char binx = 1; binx < hin -> GetNbinsX() + 1; binx ++)
                      {
                        hin -> SetBinContent(binx, biny, hin -> GetBinContent(binx, biny) * bins[biny - 1]);
                      }
                  }
                TH1 * hinx = hin -> ProjectionX();
                hin -> Scale(input -> Integral()/hin->Integral());
                float cov = 0.0;
                for (unsigned char bind = 1; bind < hinx -> GetNbinsX() + 1; bind ++)
                  {
                    cov = TMath::Sqrt(TMath::Power(cov, 2) + TMath::Power(hinx -> GetBinContent(bind) - input -> GetBinContent(bind), 2));
                  }
		//printf("\n");                                                                                                                                                                             
		if (cov < covrecord)
                  {
		    // printf("cov %f ", cov);                                                                                                                                                    
                    covrecord = cov;
                    for (unsigned char bind = 0; bind < nbins; bind ++)
                      {
                        binsrecord[bind] = bins[bind];
			// printf("%.9f ", binsrecord[bind]); 
		      }
		    // printf("\n");
		    // getchar();
                  }
                delete hinx;
                delete hin;
              }
          }
      };
      
      const double tol = 1E-6;
      bool precis = true;
      unsigned char k = 0;
      do
	{
	  printf("new iteration %u\n", k);
	  // getchar();
	  varylambda(0, k, varylambda);
	  k ++;
	  precis = true;
	  for (unsigned char bind = 0; bind < nbins; bind ++)
	    {
	      const float d = fabs(binsrecordold[bind] - binsrecord[bind])/binsrecordold[bind];
	      printf("%u %.9f %.9f %.9f\n", bind, binsrecord[bind], binsrecordold[bind], d);
	      if (d > tol)
		{
		  printf(" not precise\n");
		  precis = false;
		}
	    }
	  if (not precis)
	    {
	      for (unsigned char bind = 0; bind < nbins; bind ++)
		{
		  binsrecordold[bind] = binsrecord[bind];
		}
	    }
	} while (not precis);
    };
  varylambdanest(0);

  TH2 * hinnonclos = (TH2 *) ch -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone("hin");
  for (unsigned char bind = 0; bind <= hinnonclos -> GetNbinsX() + 1; bind ++)
    hinnonclos -> SetBinContent(bind, 0, 0.0);

  for (unsigned char bindy = 1; bindy < hsignal -> GetNbinsY() + 1; bindy ++)
    {
      for (unsigned char bindx = 0; bindx < hsignal -> GetNbinsX() + 1; bindx ++)
        {
          hinnonclos -> SetBinContent(bindx, bindy, hinnonclos -> GetBinContent(bindx, bindy) * binsrecord[bindy - 1]);
        }
    }

  // nonclosin -> GetTH2Ref() = (TH2F *) hinnonclos;

  TH1 * inputnonclos = hinnonclos -> ProjectionX("inpuntnonclos");

  // //inputnonclos -> Print("all");                                                                                                                                                                        
  TUnfoldDensity unfoldnonclos(hsignal, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeBinWidthAndUser);
  // //ApplyScaleFactor(inputnonclos);                                                                                                                                                                      
  for (unsigned char bindx = 0; bindx <= inputnonclos -> GetNbinsX() + 1; bindx ++)
    {
      printf("%f \n", inputnonclos -> GetBinContent(bindx)/ input -> GetBinContent(bindx));
    }

  unfoldnonclos.SetInput(inputnonclos);
  unfoldnonclos.DoUnfold(0.0);
  nonclosout -> GetTH1Ref(RECO) = (TH1F*) unfoldnonclos.GetFoldedOutput(ch -> CreateName(TString(nonclosout -> GetTag()) + "_folded_output"));
  nonclosout -> GetTH1Ref(GEN) = (TH1F*) unfoldnonclos.GetOutput(ch -> CreateName(TString(nonclosout ->GetTag()) + "_output"));

  TH1 * nonclosoutgentest = (TH1 *) nonclosout -> GetTH1(GEN) -> Clone();
  TH1 * nonclosingentest  = hinnonclos -> ProjectionY();
  printf("nonclossys unscaled\n");
  nonclosoutgentest -> Print("all");

  nonclosoutgentest -> Scale(nonclosingentest -> Integral() /nonclosoutgentest -> Integral());
  printf("nonclossys scaled\n");
  nonclosoutgentest -> Print("all");
  nonclosingentest -> Print("all");
  for (unsigned char bind = 1; bind < nonclosoutgentest -> GetNbinsX() + 1; bind ++)
    {
      printf("%u %f\n", bind, nonclosoutgentest -> GetBinContent(bind) - nonclosingentest -> GetBinContent(bind));
    }
  TH1 * houtput = ch -> GetLevel(OUT) -> GetHU(DATAMBCKG) -> GetTH1(GEN);

  nonclosoutgentest -> Scale(houtput -> Integral()/nonclosoutgentest -> Integral());

  for (unsigned char bind = 1; bind < nonclosoutgentest -> GetNbinsX() + 1; bind ++)
    {
      printf("%u %f\n", bind, nonclosoutgentest -> GetBinContent(bind) - houtput -> GetBinContent(bind));
    }

  file -> Close();
  return 0;
}
