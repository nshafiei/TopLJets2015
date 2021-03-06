#!/usr/bin/env python

import ROOT
import sys
import os
import pickle

def main():

    """loops over the nuisance groups found in a datacard and fixes all nuisances in the group repeating the fit"""

    url=sys.argv[1]

    #read datacard
    with open(url,'r') as f:
        dc=[l.split() for l in f]

    #identify the groups
    nuisGroups=[]
    for l_tkns in dc:
        if len(l_tkns)<2: continue
        if l_tkns[1]!='group' : continue
        nuisGroups.append( (l_tkns[0],l_tkns[3:]) )

    #create the datacards and run 
    nllVals=[]
    for group,nuisList in nuisGroups:
        with open('datacard_fixed.dat','w') as f:
            for l_tkns in dc:
                nTkns=len(l_tkns)
                toWrite=True 
                if nTkns>0:
                    if l_tkns[0] in nuisList          : toWrite=False
                    if nTkns>1 and l_tkns[1]=='group' : toWrite=False
                    if l_tkns[0]=='kmax'              : l_tkns[1]='*'
                if not toWrite : continue
                f.write(' '.join(l_tkns)+'\n')
        os.system('text2hdf5.py datacard_fixed.dat')
        os.system('combinetf.py datacard_fixed.dat.hdf5 -o fitresults_fixed.root')
        inF=ROOT.TFile.Open('fitresults_fixed.root')
        tree=inF.Get('fitresults')
        tree.GetEntry(0)
        nll=tree.nllvalfull
        inF.Close()
        nllVals.append( (group,nll) )

    #remove temporary files
    os.system('rm datacard_fixed.dat')
    os.system('rm fitresults_fixed.rot')

    #dump results to pickle file
    with open('fitresults_fixedgroups.pck','w') as cache:
        pickle.dump(nllVals,cache,pickle.HIGHEST_PROTOCOL)

if __name__ == "__main__":
    sys.exit(main())
