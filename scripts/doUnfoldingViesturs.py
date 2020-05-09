import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.gErrorIgnoreLevel = ROOT.kError
import optparse
import os,sys,io
import json
import re
from collections import OrderedDict
from math import sqrt
from array import *
import random
import numpy
import copy

debug = True

"""
steer the script
"""
def main():
    fIn = ROOT.TFile.Open('migration.root')
    matrix = fIn.Get('matrix')
    
    rebin = 4 # ad-hoc factor for scaling the 20x40 binning, not optimized
    matrix.RebinX(rebin)
    matrix.RebinY(rebin)
    print(matrix.GetNbinsX(), matrix.GetNbinsY())
    
    # What you stored as overflow needs to be in underflow for TUnfold!
    for i in range(1, matrix.GetNbinsX()+1):
        matrix.SetBinContent(i, 0, matrix.GetBinContent(i, matrix.GetNbinsY()+1))
        matrix.SetBinContent(i, matrix.GetNbinsY()+1, 0.)
    for i in range(1, matrix.GetNbinsY()+1):
        matrix.SetBinContent(0, i, matrix.GetBinContent(matrix.GetNbinsX()+1, i))
        matrix.SetBinContent(matrix.GetNbinsX()+1, i, 0.)
    
    fakedata = matrix.ProjectionX()
    
    dataUnfolded, dataFoldedBack, dataBkgSub, dataStatCov = unfold(matrix, fakedata)
    
    # PLOT
    c = ROOT.TCanvas('c','c',500,500)
    c.cd()
    c.SetRightMargin(0.05)
    c.SetLeftMargin(0.12)
    c.SetTopMargin(0.1)
    c.SetTopMargin(0.05)
    
    normalizeAndDivideByBinWidth(dataUnfolded)
    dataUnfolded.GetYaxis().SetRangeUser(0., dataUnfolded.GetMaximum()*1.5)
    dataUnfolded.Draw()
    
    c.Print('unfolded.pdf')
    

def unfold(Morig, data, tau = 0):
    # Background subtraction
    dataBkgSub = data.Clone('dataBkgSub')
    
    M = Morig.Clone('M')
    
    sigRecoNonGen = M.ProjectionX("sigRecoNonGen", 0, 0);
    sigReco       = M.ProjectionX("sigReco");
    
    # Remove fake background by hand
    for i in range(dataBkgSub.GetNbinsY()+2):
        sf = 1.
        if sigReco.GetBinContent(i) > 0:
          sf = sigRecoNonGen.GetBinContent(i) / sigReco.GetBinContent(i)
        dataBkgSub.SetBinContent(i, (1.-sf)*dataBkgSub.GetBinContent(i))
        dataBkgSub.SetBinError  (i, (1.-sf)*dataBkgSub.GetBinError(i))
        M.SetBinContent(i, 0, 0.)
    
    # Do unfolding
    unfold = ROOT.TUnfoldDensity(M, ROOT.TUnfold.kHistMapOutputVert, ROOT.TUnfold.kRegModeCurvature, ROOT.TUnfold.kEConstraintArea, ROOT.TUnfoldDensity.kDensityModeUser)
    
    if (unfold.SetInput(dataBkgSub) >= 10000):
        print('SetInput bad return value >= 10000')
        return
    
    # Unfold without regularization -> no bias
    unfold.DoUnfold(0)
    
    # Retrieve results
    dataUnfoldedOrig = unfold.GetOutput("UnfoldedOrig")
    # get error matrix (input distribution [stat] errors only)
    dataStatCov = unfold.GetEmatrixTotal("StatCov");
    
    dataUnfolded = normalizeAndDivideByBinWidth(unfold.GetOutput("Unfolded"))
    dataFoldedBack = normalizeAndDivideByBinWidth(unfold.GetFoldedOutput("FoldedBack"))
        
    return dataUnfolded, dataFoldedBack, dataBkgSub, dataStatCov



def normalizeAndDivideByBinWidth(hist):
    hist.Scale(1./hist.Integral())
    for i in range(1, hist.GetNbinsX()+1):
        hist.SetBinContent(i, hist.GetBinContent(i)/hist.GetBinWidth(i))
        hist.SetBinError  (i, hist.GetBinError(i)  /hist.GetBinWidth(i))
    return hist

"""
Adapt plots to limited range
(Useful for ratio plots! Otherwise, they are not drawn when the central point is outside the range.)
"""
def limitToRange(h, ratiorange):
    for i in xrange(1,h.GetNbinsX()+1):
        up = h.GetBinContent(i) + h.GetBinError(i)
        if (up > ratiorange[1]):
            up = ratiorange[1]
        dn = h.GetBinContent(i) - h.GetBinError(i)
        if (dn < ratiorange[0]):
            dn = ratiorange[0]
        h.SetBinContent(i, (up + dn)/2.)
        h.SetBinError(i, (up - dn)/2.)

"""
for execution from another script
"""
if __name__ == "__main__":
    main()
    #sys.exit(main())

