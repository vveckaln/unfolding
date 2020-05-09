#! /bin/sh
#sh condor/launchjobs.sh ATLAS4 False False False
bmethod=$1
dopresample=$2
dodebug=$3
reg=$4

presample=""
if [[ $dopresample == "True" ]];
then
    presample="-n"
fi

debug=""

if [[ $dodebug == "True" ]];
then
    debug="-r"
fi


if [[ $bmethod == "" ]];
then
    echo "provide a binning method"
    exit 1
fi

python condor/submitjobs.py --binning_method=$bmethod $presample                    --observable=pull_angle $debug --reg=$reg
#python condor/submitjobs.py --binning_method=$bmethod $presample                    --observable=pvmag      $debug --reg=$reg

python condor/submitjobs.py --binning_method=$bmethod $presample --method=cflip     --observable=pull_angle $debug --reg=$reg
#python condor/submitjobs.py --binning_method=$bmethod $presample --method=cflip     --observable=pvmag      $debug --reg=$reg

if [[ $presample == "" ]];
then

python condor/submitjobs.py --binning_method=$bmethod $presample --method=dire2002  --observable=pull_angle  $debug 
#python condor/submitjobs.py --binning_method=$bmethod $presample --method=dire2002  --observable=pvmag       $debug 

python condor/submitjobs.py --binning_method=$bmethod $presample --method=sherpa    --observable=pull_angle  $debug 
#python condor/submitjobs.py --binning_method=$bmethod $presample --method=sherpa    --observable=pvmag       $debug

python condor/submitjobs.py --binning_method=$bmethod $presample --method=herwig7   --observable=pull_angle  $debug
#python condor/submitjobs.py --binning_method=$bmethod $presample --method=herwig7   --observable=pvmag       $debug

fi

