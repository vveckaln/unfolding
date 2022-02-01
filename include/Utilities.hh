#include "TTree.h"
#include "TH2D.h"
#include "TH1.h"
#include <vector>
#include "Definitions.hh"
using namespace std;
void Do();
void FillFromTree(TTree &, TH2D &h);
void FillFromTree(TTree &, TH1D &h, RecoLevelCode_t);

void FillFromTreeCor(TTree &, TH2D &h, bool = true);
void FillFromTreeCor(TTree &, TH1D &h, RecoLevelCode_t);


vector<float> splitForMinSigma(TH2D & h, float factor = 0.5);
vector<float> splitForMinSigmaM(TH2D & h, float factor = 0.5);

vector<float> splitForMinSigma_v2 (TH2D & h, const char * = "X", float factor = 0.5);

void Print(TObject *, const char * = "");
TString CreateName(const char *);
TString CreateTitle(const char *);
void NormaliseToBinWidth(TH1 *);
void ProcessHistogram(TH1 *);
TH1 * MakeAbs(TH1 *);
template<class T>
T * convertVectorToArray(vector<T> v)
{
  T *array = new T[v.size()];
  for (unsigned long ind = 0; ind < v.size(); ind ++)
    {
      array[ind] = v[ind];
    }
  return array;
}

float * GetBinArray(TH1 *h);

TH1* adjust(TH1 *h);
