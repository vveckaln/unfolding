#include "WeightRecord.hh"
#include "TChain.h"
int main()
{
  const std::map<char,int> mymap = 
    {
      {'b', 100},
      {'a', 200},
      {'c', 300}};

  for (map<char, int>::const_iterator it = mymap.cbegin(); it !=mymap.cend(); ++it )
    {
      printf("%c %u\n", it -> first, it -> second);
    }

  
  TChain chain("migration");
  chain.Add("root://eosuser.cern.ch//eos/user/v/vveckaln/test_output/migration/MC13TeV_TTJets/migration_MC13TeV_TTJets_0.root/migration");
  WeightMap *wmap = nullptr;
  printf("windmap.size %lu\n", windmap.size());
  for (map<unsigned char, pair<Weights, Vari>>::const_iterator it = windmap.cbegin(); it !=windmap.cend(); ++it )
    {
      printf("%u\n", it -> first);
    }
  std::pair<Weights, Vari> p = windmap.at(1);
  Weights w = p.first;
  TString title = weightstitles.at(w);
  printf("weight title %s \n", title.Data());
  chain.SetBranchAddress("weight", &wmap);
  for (unsigned long event_ind = 0; event_ind < chain.GetEntries(); event_ind ++)
    {
      chain.GetEntry(event_ind);
      wmap -> ls();
      printf("size %lu\n", (*wmap) -> size());
      //      wmap -> Reset();
      //getchar();
    }
  delete wmap;
  return 0;
}
