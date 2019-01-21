#!/bin/bash
echo "hostname " `hostname`
echo "pwd" `pwd`
echo "klist" >&2
klist >&2
echo "end klist" >&2
echo "ls EOS" >&2
ls /eos/user/v/vveckaln >&2
echo "end ls EOS" >&2
CLUSTERID=$1
PROCID=$2
UNFOLDINGLIB=/afs/cern.ch/work/v/vveckaln/private/UnfoldingLIB
echo -e "leading_jet_allconst_MC13TeV_TTJets_herwig_pvmag_ORIG_nominal\t${PROCID}\t"`date` >> $UNFOLDINGLIB/condor/registry_$CLUSTERID.txt
source $UNFOLDINGLIB/condor/run_job.sh /eos/user/v/vveckaln/unfolding_nominal/pvmag/ORIG/ leading_jet_allconst_MC13TeV_TTJets_herwig_pvmag_ORIG_nominal 0 0 MC13TeV_TTJets_herwig pvmag 0 nominal False ATLAS3 False 0.3
#EXIT_CODE=$?


if [ $(( $EXIT_CODE_SCRAM || $EXIT_CODE_BIN )) = 0 ];
then
sh $UNFOLDINGLIB/condor/success.sh $EXIT_CODE_SCRAM $EXIT_CODE_BIN $CLUSTERID $PROCID leading_jet_allconst_MC13TeV_TTJets_herwig_pvmag_ORIG_nominal 
else
sh $UNFOLDINGLIB/condor/failure.sh $EXIT_CODE_SCRAM $EXIT_CODE_BIN $CLUSTERID $PROCID leading_jet_allconst_MC13TeV_TTJets_herwig_pvmag_ORIG_nominal
fi
