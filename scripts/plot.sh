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


# There's surely an easier way to do this...
tracedir=`find out/traces -name "$1"`
outfile=`echo $tracedir | sed 's/out\/traces/out\/outfiles/'`
outfile="${outfile}Avg.txt"

plotfile=`echo $tracedir | sed 's/out\/traces/out\/plotfiles/'`
plotfile="${plotfile}.plt"

epsfile=`echo $tracedir | sed 's/out\/traces/out\/eps/'`
epsfile="${epsfile}.eps"

pdffile=`echo $tracedir | sed 's/out\/traces/out\/pdf/'`
pdffile="${pdffile}.pdf"


echo ""
echo "Creating gnuplot output file..."
echo ""

outname="out/outfiles/$prefix.txt"
docmd "./bin/plot.exec avg --dir=${tracedir}/ --outfile=$outfile"

echo ""
echo "Creating eps plots..."
echo ""

docmd "gnuplot $plotfile"

echo ""
echo "Converting eps to pdf..."
echo ""

docmd "ps2pdf $epsfile $pdffile"

echo ""
echo "Fixing pdf bounding box..."
echo ""

#docmd "perl out/fixBoundingBox.pl out $prefix > /dev/null"
docmd "perl out/fixBoundingBox.pl $epsfile $pdffile > /dev/null"

exit 0