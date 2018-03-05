#include "TTree.h"
#include "TH2D.h"
#include "TH1.h"
#include <vector>
using namespace std;
enum level {ORIG, OPT};
enum type  {RECO, GEN};
typedef unsigned char TypeCode_t;
extern const char  * level_tag[2];  
extern TString       tag_syst;
extern unsigned char level_ind;
extern float         orig_bin_width;
extern TString       pic_folder;
extern int           test_int;
void Do();
void FillFromTree(TTree &, TH2D &h);
void FillFromTree(TTree &, TH1D &h, TypeCode_t);

vector<float> splitForMinSigma(TH2D & h, float factor = 0.5);
vector<float> splitForMinSigma_v2 (TH2D & h, const char * = "X", float factor = 0.5);

void Print(TObject *, const char * = "");
TString CreateName(const char *);
TString CreateTitle(const char *);
void NormaliseToBinWidth(TH1*);
