#! /bin/sh
EXIT_CODE_SCRAM=$1
EXIT_CODE_PYTHON=$2
CLUSTERID=$3
PROCID=$4
JOB_TAG=$5
UNFOLDINGLIB=/afs/cern.ch/work/v/vveckaln/private/UnfoldingLIB
echo -e "$1\t$2\t$4\t$5\t\t"`hostname`"\t"`pwd`"\t"`date` >> $UNFOLDINGLIB/condor/successful_jobs$3.txt
