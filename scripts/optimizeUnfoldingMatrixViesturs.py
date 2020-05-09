#! /usr/bin/env python
import ROOT
import optparse
import json
import sys
import os
import time
from array import *
from math import sqrt

def splitForMinSigma(h, factor = 0.5):
    f1 = ROOT.TF1("f1", "gaus", h.GetXaxis().GetXmin(), h.GetXaxis().GetXmax());
    f1.SetParameters(100, (h.GetXaxis().GetXmax()-h.GetXaxis().GetXmin())/2., 0.01);
    slices = ROOT.TObjArray()
    h.FitSlicesX(f1, 1, h.GetNbinsX(), 0, "QNRLM", slices)
    
    bins  = [h.GetXaxis().GetXmin()]
    exact = [h.GetXaxis().GetXmin()]
    
    integral = h.Integral()
    binfraction = 0.
    for i in range(1, h.GetNbinsX()+1):
      binfraction += h.ProjectionX('px', i, i).Integral()/integral
      mean  = slices[1].GetBinContent(i)
      meanError = slices[1].GetBinError(i)
      if (mean == 0 or meanError/mean > 0.25): continue
      sigma = slices[2].GetBinContent(i) * factor
      if (mean - sigma) > exact[-1]:
        if (mean + sigma < h.GetXaxis().GetXmax()):
          exact.append(mean+sigma)
          newbinedge = h.GetXaxis().GetBinUpEdge((h.GetXaxis().FindBin(mean+sigma)))
          if (newbinedge > bins[-1] and binfraction > 0.01):
              bins.append(newbinedge)
              binfraction = 0.
    
    bins[-1] = h.GetXaxis().GetXmax()
    
    return bins
        
    
"""
steer
"""
def main():
    fIn = ROOT.TFile.Open('migration.root')
    print fIn
    matrix = fIn.Get('migration_matrix_orig_nosys_sf')
    print matrix
    bins = splitForMinSigma(matrix, factor = 0.3)
    print('generator bins', bins)
    
    # reco bin splitting
    divisor = 2
    recobins = []
    for i in range(len(bins)-1):
        for j in range(divisor):
            step = abs(bins[i]-bins[i+1])/divisor*j
            recobins.append(bins[i] + step)
    recobins.append(bins[-1])
    print('reco bins', recobins)
        

if __name__ == "__main__":
	sys.exit(main())
