#! /bin/sh
bmethod=$1

if [[ $bmethod == "" ]];
then
    echo "provide a binning method"
    exit 1
fi

eos root://eosuser.cern.ch rm -r $EOS/unfolding_nominal/pull_angle/$bmethod
eos root://eosuser.cern.ch rm -r $EOS/unfolding_nominal/pvmag/$bmethod

eos root://eosuser.cern.ch rm -r $EOS/unfolding_cflip/pull_angle/$bmethod
eos root://eosuser.cern.ch rm -r $EOS/unfolding_cflip/pvmag/$bmethod

eos root://eosuser.cern.ch rm -r $EOS/unfolding_dire2002/pull_angle/$bmethod
eos root://eosuser.cern.ch rm -r $EOS/unfolding_dire2002/pvmag/$bmethod

eos root://eosuser.cern.ch rm -r $EOS/unfolding_sherpa/pull_angle/$bmethod
eos root://eosuser.cern.ch rm -r $EOS/unfolding_sherpa/pvmag/$bmethod

eos root://eosuser.cern.ch rm -r $EOS/unfolding_herwig7/pull_angle/$bmethod
eos root://eosuser.cern.ch rm -r $EOS/unfolding_herwig7/pvmag/$bmethod
