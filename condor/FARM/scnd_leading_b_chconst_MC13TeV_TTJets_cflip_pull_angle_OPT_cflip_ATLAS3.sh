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
echo -e "scnd_leading_b_chconst_MC13TeV_TTJets_cflip_pull_angle_OPT_cflip_ATLAS3\t${PROCID}\t"`date` >> $UNFOLDINGLIB/condor/registry_$CLUSTERID.txt
source $UNFOLDINGLIB/condor/run_job.sh /eos/user/v/vveckaln/unfolding_cflip/pull_angle/ATLAS3/ scnd_leading_b_chconst_MC13TeV_TTJets_cflip_pull_angle_OPT_cflip_ATLAS3 3 1 MC13TeV_TTJets_cflip pull_angle 1 cflip False ATLAS3 False 0.15
#EXIT_CODE=$?


if [ $(( $EXIT_CODE_SCRAM || $EXIT_CODE_BIN )) = 0 ];
then
sh $UNFOLDINGLIB/condor/success.sh $EXIT_CODE_SCRAM $EXIT_CODE_BIN $CLUSTERID $PROCID scnd_leading_b_chconst_MC13TeV_TTJets_cflip_pull_angle_OPT_cflip_ATLAS3 
else
sh $UNFOLDINGLIB/condor/failure.sh $EXIT_CODE_SCRAM $EXIT_CODE_BIN $CLUSTERID $PROCID scnd_leading_b_chconst_MC13TeV_TTJets_cflip_pull_angle_OPT_cflip_ATLAS3
fi