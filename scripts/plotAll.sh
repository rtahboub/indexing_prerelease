#!/bin/bash

function docmd() {
    echo "$1"
    if( eval "(" $1 ")" )
    then 
        true
    else
	echo "Command failed"
	exit 1
    fi
}


echo ""
echo "Creating gnuplot output files..."
echo ""

for dir in out/traces/*
do
    if [ -d "$dir" ] 
    then
        outname="out/outfiles/`basename $dir`.txt"
        docmd "./bin/plot.exec --dir=$dir/ --outfile=$outname"
    fi
done


echo ""
echo "Creating eps plots..."
echo ""

for script in out/plotfiles/*.plt
do
    docmd "gnuplot $script"
done



echo ""
echo "Converting eps to pdf..."
echo ""

for epsfile in out/eps/*.eps
do
    if [ -e $epsfile ] 
    then
	epsbase=`basename $epsfile`
        docmd "ps2pdf $epsfile out/pdf/${epsbase/.eps/.pdf}"
    fi
done



echo ""
echo "Fixing pdf bounding box..."
echo ""


for pdffile in out/pdf/*.pdf
do
    if [ -e $pdffile ]
    then
        pdfbase=`basename $pdffile`
        docmd "perl out/fixBoundingBox.pl out ${pdfbase/.pdf/} > /dev/null"
    fi
done



exit 0