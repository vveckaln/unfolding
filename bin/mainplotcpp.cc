#include "TH1.h"
#include "TLegendEntry.h"
#include "TList.h"
#include "TLegend.h"
int main()
{
		      TH1 * data = nullptr;
		      TH1F * h1= new TH1F("h1", "h1", 5, 0, 5); //nullptr
		      TH1F * h2 = new TH1F("h2", "h2", 5, 0, 5); //nullptr;
		      // if (resultcode == IN)
		      // 	{
		      // 	  // hherwig -> GetTH2() -> Add(ch -> GetBackgroundTotal() -> GetTH2());
		      // 	  //		      	  hherwigreco = hherwig -> Project(recocode, "hherwigreco");
		      // 	  // if (cflip)
		      // 	//     cflipreco = cflip -> Project(recocode, "cflipreco");
		      // 	}
		      // else
		      // 	{
		      // 	  hherwigreco = hherwig -> GetTH1(recocode);
		      // 	  if (cflip)
		      // 	    cflipreco = cflip -> GetTH1(recocode);
		      // 	  //hherwigreco -> Add(bckgproj);
		      // 	}
		      // hherwigreco -> SetMarkerStyle(kOpenSquare);
		      // hherwigreco -> SetMarkerColor(kGreen);
		      // hherwigreco -> SetLineColor(kGreen);
		      // hherwigreco -> SetLineWidth(0);
		      // hherwigreco -> SetLineStyle(kDashed);
		      // if (cflipreco)
		      // 	{
		      // 	  cflipreco -> SetMarkerStyle(kOpenCircle);
		      // 	  cflipreco -> SetMarkerColor(kRed);
		      // 	  cflipreco -> SetLineColor(kRed);
		      // 	  cflipreco -> SetLineWidth(0);
		      // 	  cflipreco -> SetLineStyle(kDashed);
		      // 	}
		      // TH1 * hherwigrecocl = nullptr;
		      // TH1 * cfliprecocl = nullptr;
		      // printf("probe A data %p\n", data);
		      // if (data)
		      // 	{
		      // 	  hherwigrecocl = (TH1 *) hherwigreco -> Clone(TString("hherwigrecocl_") + tag_recolevel[recocode]);
		      // 	  hherwigrecocl -> Divide(data);
		      // 	  p2 -> cd();
		      // 	  hherwigrecocl -> Draw("PSAME");
		      // 	  if (cflipreco)
		      // 	    {
		      // 	      cfliprecocl = (TH1 *) cflipreco -> Clone(TString("cfliprecocl_") + tag_recolevel[recocode]);
		      // 	      cfliprecocl -> Divide(data);
		      // 	      p2 -> cd();
		      // 	      cfliprecocl -> Draw("PSAME");
		      // 	    }
		      // 	}
		      // stack -> Add(hherwigreco, "P");
		      // stack -> Add(cflipreco, "P");
		      // printf("probe B data %p\n", data);
		      /*	  TString title;
				  switch (resultcode)
				  {
				  case IN:
				  title = "t#bar{t} Herwig++";
				  break;
				  case OUT:
				  switch(recocode)
				  {
				  case RECO:
				  title = "data (t#bar{t} Herwig++) folded back";
				  break;
				  case GEN:
				  title = "data (t#bar{t} Herwig++) unfolded";
				  break;
				  }
				  }*/
		      // printf("hherwigreco\n");
		      // hherwigreco ->Print("ALL");
		      // chups -> NormaliseToBinWidth(hherwigreco);
		      // hherwigreco -> Scale(1.0/calculatearea(hherwigreco));
		      // //		      hherwigreco -> Print("ALL");
		      // //getchar();
		      // if (ch_cflip)
		      // 	{
		      // 	  ch_cflip -> NormaliseToBinWidth(cflipreco);
		      // 	  cflipreco -> Scale(1.0/calculatearea(cflipreco));
		      // 	}
		      // p1 -> cd();
		      // stack -> Draw("nostackSAME");
		      // const unsigned char nfsr = 2;
		      // HistoUnfolding * h_fsr[nfsr] = {
		      // 	chups -> GetSys(IN/*resultcode*/, "MC13TeV_TTJets_fsrup"), 
		      // 	chups -> GetSys(IN/*resultcode*/, "MC13TeV_TTJets_fsrdn")
		      // };
		      // Style_t fsrMarkerStyle[nfsr] = {kOpenTriangleUp, kOpenTriangleDown};
		      // TH1 *hh_fsr[nfsr];
		      // TH1 *hh_fsr_cl[nfsr] = {nullptr, nullptr};
		      TList * lentries = new TList();//legend -> GetListOfPrimitives();
		      // for (unsigned char fsr_ind = 0; fsr_ind < nfsr; fsr_ind ++)
		      // 	{
		      // 	  if (resultcode == IN)
		      // 	    {
		      // 	      h_fsr[fsr_ind] -> GetTH2() -> Add(ch -> GetBackgroundTotal() -> GetTH2());
		      // 	      //hh_fsr[fsr_ind] = h_fsr[fsr_ind] -> Project(recocode, TString(h_fsr[fsr_ind] -> GetTag()));
		      // 	    }
		      // 	  else
		      // 	    {
		      // 	      //				  hh_fsr[fsr_ind] = h_fsr[fsr_ind] -> GetTH1(recocode);
		      // 	      //hh_fsr[fsr_ind] -> Add(bckgproj);
		      // 	    }
		      // 	  /*!*/
		      // 	  hh_fsr[fsr_ind] = h_fsr[fsr_ind] -> Project(recocode, TString(h_fsr[fsr_ind] -> GetTag()));
		      // 	  /*!*/
		      // 	  hh_fsr[fsr_ind] -> SetLineWidth(0.0);
		      // 	  hh_fsr[fsr_ind] -> SetMarkerColor(kBlack);
		      // 	  hh_fsr[fsr_ind] -> SetMarkerStyle(fsrMarkerStyle[fsr_ind]);
		      // 	  stack -> Add(hh_fsr[fsr_ind], "P");
		      // 	  //				legend -> AddEntry(hh_fsr[fsr_ind], h_fsr[fsr_ind] -> GetTitle(), "p");
		      // 	  lentries -> AddAt(new TLegendEntry(hh_fsr[fsr_ind], h_fsr[fsr_ind] -> GetTitle(), "p"), 1);

		      // 	  if (data)
		      // 	    {
		      // 	      hh_fsr_cl[fsr_ind] = (TH1*) hh_fsr[fsr_ind] -> Clone(TString(hh_fsr[fsr_ind] -> GetName()) + "_cl");
		      // 	      hh_fsr_cl[fsr_ind] -> Divide(data);
		      // 	      p2 -> cd();
		      // 	      hh_fsr_cl[fsr_ind] -> Draw("PSAME");
		      // 	    }
		      // 	  chups -> NormaliseToBinWidth(hh_fsr[fsr_ind]);
		      // 	  hh_fsr[fsr_ind] -> Scale(1.0/calculatearea(hh_fsr[fsr_ind]));
		      // 	}
		      // printf("probe C %p\n", data);
		      //			      legend -> AddEntry(hherwigreco, hherwigreco -> GetTitle(), "lp");
		      //  legend -> AddEntry(cflipreco, cflipreco -> GetTitle(), "lp");
		      //printf("hherwigreco %s\n", hherwigreco -> GetTitle());
		      lentries -> AddAt(new TLegendEntry(h1, h1 -> GetTitle(), "lp"), 1);
		      printf("probe D %p\n", data);
		      //if (cflipreco)
			lentries -> AddAt(new TLegendEntry(h2, h2 -> GetTitle(), "lp"), 1);
			printf("data %p\n", data);
			if (data)
			  {
			    data -> Print("ALL");
			  }
		      printf("probe C0\n");
		      //chups -> NormaliseToBinWidth(data);
		      printf("probe D\n");
		      //data -> Print("ALL");
		      //		      getchar();
		      //		      data -> Scale(1.0/calculatearea(data));
		      
		      // canvas -> SaveAs(dir + "/" + canvas -> GetName() + ".png");
		      // canvas -> SaveAs(dir + "/" + canvas -> GetName() + ".eps");
		      printf("probe E\n");
		      // if (resultcode == OUT and recocode == GEN)
		      //   {
		      //     TH1 * hsignalGEN = chups -> GetLevel(IN) -> Project(GEN, "signal");
		      //     hsignalGEN -> SetLineColor(kRed);
		      //     p2 -> cd();
		      //     hsignalGEN -> Draw("SAMEHIST");
		      //   }

		      //		    }
		      //		}
		      //	    }
		      //	}
      //    }
  //    }

  return 0;
}
