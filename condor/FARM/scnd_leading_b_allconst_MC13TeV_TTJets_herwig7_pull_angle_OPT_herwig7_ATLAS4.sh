#!/bin/bash
echo "hostname " `hostname`
echo "pwd" `pwd`
echo "klist" >&2
klist >&2
echo "end klist" >&2
echo "ls EOS" >&2
eos root://eosuser.cern.ch ls /eos/user/v/vveckaln >&2
echo "end ls EOS" >&2
CLUSTERID=$1
PROCID=$2
UNFOLDINGLIB=/afs/cern.ch/work/v/vveckaln/private/UnfoldingLIB
echo -e "scnd_leading_b_allconst_MC13TeV_TTJets_herwig7_pull_angle_OPT_herwig7_ATLAS4\t${PROCID}\t"`date` >> $UNFOLDINGLIB/condor/registry_$CLUSTERID.txt
source $UNFOLDINGLIB/condor/run_job.sh /eos/user/v/vveckaln/unfolding_herwig7/pull_angle/ATLAS4/ scnd_leading_b_allconst_MC13TeV_TTJets_herwig7_pull_angle_OPT_herwig7_ATLAS4 3 0 MC13TeV_TTJets_herwig7 pull_angle 1 herwig7 False ATLAS4 False 0.0 False
#EXIT_CODE=$?


if [ $(( $EXIT_CODE_SCRAM || $EXIT_CODE_BIN )) = 0 ];
then
sh $UNFOLDINGLIB/condor/success.sh $EXIT_CODE_SCRAM $EXIT_CODE_BIN $CLUSTERID $PROCID scnd_leading_b_allconst_MC13TeV_TTJets_herwig7_pull_angle_OPT_herwig7_ATLAS4 
else
sh $UNFOLDINGLIB/condor/failure.sh $EXIT_CODE_SCRAM $EXIT_CODE_BIN $CLUSTERID $PROCID scnd_leading_b_allconst_MC13TeV_TTJets_herwig7_pull_angle_OPT_herwig7_ATLAS4
fi
