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
echo -e "leading_jet_chconst_MC13TeV_TTJets_pull_angle_ORIG_nominal_binSIGMA_0p2\t${PROCID}\t"`date` >> $UNFOLDINGLIB/condor/registry_$CLUSTERID.txt
source $UNFOLDINGLIB/condor/run_job.sh /eos/user/v/vveckaln/bins/pull_angle/SIGMA_0p2 leading_jet_chconst_MC13TeV_TTJets_pull_angle_ORIG_nominal_binSIGMA_0p2 0 1 MC13TeV_TTJets pull_angle 0 nominal True SIGMA True 0.2
#EXIT_CODE=$?


if [ $(( $EXIT_CODE_SCRAM || $EXIT_CODE_BIN )) = 0 ];
then
sh $UNFOLDINGLIB/condor/success.sh $EXIT_CODE_SCRAM $EXIT_CODE_BIN $CLUSTERID $PROCID leading_jet_chconst_MC13TeV_TTJets_pull_angle_ORIG_nominal_binSIGMA_0p2 
else
sh $UNFOLDINGLIB/condor/failure.sh $EXIT_CODE_SCRAM $EXIT_CODE_BIN $CLUSTERID $PROCID leading_jet_chconst_MC13TeV_TTJets_pull_angle_ORIG_nominal_binSIGMA_0p2
fi