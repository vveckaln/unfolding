{
  printf("$UNFOLDINGLIB/rootlogon.C\n");
  getchar();
  if (gSystem -> Getenv("$COMPOUNDHISTOLIB")) 
    {
      gInterpreter -> AddIncludePath(gSystem -> ExpandPathName("$COMPOUNDHISTOLIB/interface"));
    }
  if (gSystem -> Getenv("$ROOUNFOLD")) 
    {
      gInterpreter -> AddIncludePath(gSystem -> ExpandPathName("$ROOUNFOLD/src"));
    }

}
