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


trace_prefix="out/traces/STRIPES"
out_prefix="out/outfiles/STRIPES"

echo ""
echo "Creating gnuplot output files..."
echo ""

#docmd "./bin/plot.exec avg --dir=$trace_prefix/STRIPESHorizonSmall/ --outfile=$out_prefix/STRIPESHorizonSmallAvg.txt"
docmd "./bin/plot.exec avg --dir=$trace_prefix/STRIPESHorizonLarge/ --outfile=$out_prefix/STRIPESHorizonLargeAvg.txt"
#docmd "./bin/plot.exec avg --dir=$trace_prefix/STRIPESHorizonHuge/ --outfile=$out_prefix/STRIPESHorizonHugeAvg.txt"
#docmd "./bin/plot.exec avg --dir=$trace_prefix/STRIPESHorizonGauss/ --outfile=$out_prefix/STRIPESHorizonGaussAvg.txt"

echo ""
echo "Creating eps plots..."
echo ""

docmd "gnuplot out/plotfiles/STRIPES/STRIPESHorizon.plt"

# echo ""
# echo "Converting eps to pdf..."
# echo ""


#docmd "ps2pdf out/eps/STRIPES/STRIPESHorizonSmall.eps out/pdf/STRIPES/STRIPESHorizonSmall.pdf"
docmd "ps2pdf out/eps/STRIPES/STRIPESHorizonLarge.eps out/pdf/STRIPES/STRIPESHorizonLarge.pdf"
#docmd "ps2pdf out/eps/STRIPES/STRIPESHorizonHuge.eps out/pdf/STRIPES/STRIPESHorizonHuge.pdf"
#docmd "ps2pdf out/eps/STRIPES/STRIPESHorizonGauss.eps out/pdf/STRIPES/STRIPESHorizonGauss.pdf"

echo ""
echo "Fixing pdf bounding box..."
echo ""

#docmd "perl out/fixBoundingBox.pl out/eps/STRIPES/STRIPESHorizonSmall.eps out/pdf/STRIPES/STRIPESHorizonSmall.pdf > /dev/null"

docmd "perl out/fixBoundingBox.pl out/eps/STRIPES/STRIPESHorizonLarge.eps out/pdf/STRIPES/STRIPESHorizonLarge.pdf > /dev/null"

#docmd "perl out/fixBoundingBox.pl out/eps/STRIPES/STRIPESHorizonHuge.eps out/pdf/STRIPES/STRIPESHorizonHuge.pdf > /dev/null"

#docmd "perl out/fixBoundingBox.pl out/eps/STRIPES/STRIPESHorizonGauss.eps out/pdf/STRIPES/STRIPESHorizonGauss.pdf > /dev/null"

exit 0