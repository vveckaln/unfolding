#! /bin/sh
rm -r $1_nosyst
./Unfolding_abs_syst nosyst $1_nosyst
sh copy_plots.sh $1_nosyst