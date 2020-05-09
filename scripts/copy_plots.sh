#! /bin/sh
echo "Copying " $1
if [ -d ~/www/colour_flow//ReReco2016_b312177/$1 ];
then
    rm -r ~/www/colour_flow//ReReco2016_b312177/$1
fi
cp -r $1 ~/www/colour_flow//ReReco2016_b312177/
cp ~/www_scripts/*  ~/www/colour_flow//ReReco2016_b312177/$1
if [ -d ~/www/colour_flow//ReReco2016_b312177/$1/orig ];
then
    cp ~/www_scripts/*  ~/www/colour_flow//ReReco2016_b312177/$1/orig
fi
if [ -d ~/www/colour_flow//ReReco2016_b312177/$1/opt ]; 
then
    cp ~/www_scripts/*  ~/www/colour_flow//ReReco2016_b312177/$1/opt
fi