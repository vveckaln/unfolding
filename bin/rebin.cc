#include "TH1F.h"
#include "TCanvas.h"
int main()
{
  TH1F test("histo", "histo", 3, 0.0, 3.0);
  for (unsigned char ind = 0; ind < test.GetNbinsX() + 1; ind ++)
    {
      printf("ind %u bin %f\n", ind, test.GetBinLowEdge(ind + 1));
    }
  TCanvas c1("c1", "c1");
  test.Draw();
  c1.SaveAs("before_rebin.png");
  test.Rebin(2);
  for (unsigned char ind = 0; ind < test.GetNbinsX() + 1; ind ++)
    {
      printf("ind %u bin %f\n", ind, test.GetBinLowEdge(ind + 1));
    }
  TCanvas c2("c2", "c2");
  test.Draw();
  c2.SaveAs("after_rebin.png");
}
