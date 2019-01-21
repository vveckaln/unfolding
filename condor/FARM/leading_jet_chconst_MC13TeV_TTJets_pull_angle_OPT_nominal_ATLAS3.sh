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
echo -e "leading_jet_chconst_MC13TeV_TTJets_pull_angle_OPT_nominal_ATLAS3\t${PROCID}\t"`date` >> $UNFOLDINGLIB/condor/registry_$CLUSTERID.txt
source $UNFOLDINGLIB/condor/run_job.sh /eos/user/v/vveckaln/unfolding_nominal/pull_angle/ATLAS3/ leading_jet_chconst_MC13TeV_TTJets_pull_angle_OPT_nominal_ATLAS3 0 1 MC13TeV_TTJets pull_angle 1 nominal True ATLAS3 False 0.15
#EXIT_CODE=$?


if [ $(( $EXIT_CODE_SCRAM || $EXIT_CODE_BIN )) = 0 ];
then
sh $UNFOLDINGLIB/condor/success.sh $EXIT_CODE_SCRAM $EXIT_CODE_BIN $CLUSTERID $PROCID leading_jet_chconst_MC13TeV_TTJets_pull_angle_OPT_nominal_ATLAS3 
else
sh $UNFOLDINGLIB/condor/failure.sh $EXIT_CODE_SCRAM $EXIT_CODE_BIN $CLUSTERID $PROCID leading_jet_chconst_MC13TeV_TTJets_pull_angle_OPT_nominal_ATLAS3
fi
