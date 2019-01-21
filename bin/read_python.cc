#include <stdlib.h>
#include <stdio.h>
#include <string>
using namespace std;
int main()
{
  FILE *fp;
  char var[128];

  fp = popen("python read_python.py -j $PROJECT/data/era2016/expsyst_samples.json -m sample_element --row 3 --column 0", "r");
  while (fgets(var, sizeof(var), fp) != NULL) {}
  printf("float result %f\n", stof(var));
  printf("var %s varend\n", var);
  return 0;
  char tagnnl[128];
  unsigned char ind = 0;
  for (char * p = var; *p != '\n'; p ++)
    {
      tagnnl[ind] = *p;
      ind ++;
    }
  tagnnl[ind] = '\0';
  printf("tag %s tagend\n", tagnnl);
  pclose(fp);

}
