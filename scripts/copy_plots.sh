#! /bin/sh
echo "Copyting " $1
rm -r ~/www/colour_flow//ReReco2016_b312177/$1
cp -r $1 ~/www/colour_flow//ReReco2016_b312177/
cp ~/www_scripts/*  ~/www/colour_flow//ReReco2016_b312177/$1
if [ -f ~/www/colour_flow//ReReco2016_b312177/$1/orig ];
then
    cp ~/www_scripts/*  ~/www/colour_flow//ReReco2016_b312177/$1/orig
fi
if [ -f ~/www/colour_flow//ReReco2016_b312177/$1/opt ]; 
then
    cp ~/www_scripts/*  ~/www/colour_flow//ReReco2016_b312177/$1/opt
fi