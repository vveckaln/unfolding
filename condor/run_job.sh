#! /bin/sh
WORKDIR=`pwd`
UNFOLDINGLIB=/afs/cern.ch/work/v/vveckaln/private/UnfoldingLIB
PROJECT=/afs/cern.ch/work/v/vveckaln/private/CMSSW_8_0_26_patch1/src/TopLJets2015/TopAnalysis
OUTPUTDIR=$1
JOBTAG=$2
JETIND=$3
CHARGEIND=$4
SAMPLETAG=$5
OBSERVABLE=$6
LEVELIND=$7
METHOD=$8
PRESAMPLETTJETS=${9}
BINNINGMETHOD=${10}
BINS=${11}
SFACTOR=${12}
mkdir output
cd /afs/cern.ch/work/v/vveckaln/private/CMSSW_8_0_26_patch1
echo $PATH
MY_SCRAM=$(which scram)
echo "SCRAM is " $MY_SCRAM
scram_result=`scram r -sh`
EXIT_CODE_SCRAM=$?
echo "EXIT_CODE_SCRAM" $EXIT_CODE_SCRAM >&2
eval $scram_result
cd ${WORKDIR}
echo $ROOTSYS
echo $LD_LIBRARY_PATH
export EOS=/eos/user/v/vveckaln
echo "$JETIND $CHARGEIND $SAMPLETAG $OBSERVABLE $LEVELIND $METHOD $PRESAMPLETTJETS $BINNINGMETHOD $BINS $SFACTOR"
$UNFOLDINGLIB/job $JETIND $CHARGEIND $SAMPLETAG $OBSERVABLE $LEVELIND $METHOD $PRESAMPLETTJETS $BINNINGMETHOD $BINS $SFACTOR
EXIT_CODE_BIN=$?
echo "EXIT_CODE_BIN " $EXIT_CODE_BIN >&2
mkdir -p $OUTPUTDIR/$JOBTAG

for file in output/*;
do
    bnfile=$(basename $file)
    ${PROJECT}/scripts/EOS_file_copy.sh ${file} $OUTPUTDIR/$JOBTAG/${bnfile}
#exit $EXIT_CODE_PYTHON || $EXIT_CODE_SCRAM
done
