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

#docmd "./scripts/plot.sh StaticIndexScaleQueries"
#docmd "./scripts/plot.sh StaticIndexScaleUpdates"
#docmd "./scripts/plot.sh StaticIndexGauss"

trace_prefix="out/traces/StaticIndex"
out_prefix="out/outfiles/StaticIndex"

echo ""
echo "Creating gnuplot output files..."
echo ""

docmd "./bin/plot.exec avg --dir=$trace_prefix/StaticIndexScalePoints/ --outfile=$out_prefix/StaticIndexScalePointsAvg.txt"

# docmd "./bin/plot.exec breakdown --dir=$trace_prefix/StaticIndexScalePoints/ --outfile=$out_prefix/StaticIndexScalePointsBrk.txt"

# docmd "./bin/plot.exec breakdown --dir=$trace_prefix/StaticIndexHuge/ --outfile=$out_prefix/StaticIndexHugeBreakdown.txt"

# Bit of a hack to remove BinarySearch from the plot since it doesn't have build cost. 
# docmd "tail -n5 $out_prefix/StaticIndexScalePointsBrk.txt > $out_prefix/StaticIndexPointsBreakdown.txt"
# docmd "rm $out_prefix/StaticIndexScalePointsBrk.txt"


echo ""
echo "Creating eps plots..."
echo ""

docmd "gnuplot out/plotfiles/StaticIndex/StaticIndexScalePoints.plt"
# docmd "gnuplot out/plotfiles/StaticIndex/StaticIndexPointsBreakdown.plt"

echo ""
echo "Converting eps to png..."
echo ""

docmd "convert -density 100 out/eps/StaticIndex/StaticIndexScalePoints.eps -flatten out/png/StaticIndex/StaticIndexScalePoints.png"


echo ""
echo "Converting eps to pdf..."
echo ""

#docmd "ps2pdf out/eps/StaticIndex/StaticIndexScalePoints.eps out/pdf/StaticIndex/StaticIndexScalePoints.pdf"

# docmd "ps2pdf out/eps/StaticIndex/StaticIndexPointsBreakdown.eps out/pdf/StaticIndex/StaticIndexPointsBreakdown.pdf"
# docmd "ps2pdf out/eps/StaticIndex/StaticIndexHugeBreakdown.eps out/pdf/StaticIndex/StaticIndexHugeBreakdown.pdf"

echo ""
echo "Fixing pdf bounding box..."
echo ""

#docmd "perl out/fixBoundingBox.pl out/eps/StaticIndex/StaticIndexScalePoints.eps out/pdf/StaticIndex/StaticIndexScalePoints.pdf > /dev/null"

# docmd "perl out/fixBoundingBox.pl out/eps/StaticIndex/StaticIndexPointsBreakdown.eps out/pdf/StaticIndex/StaticIndexPointsBreakdown.pdf > /dev/null"

# docmd "perl out/fixBoundingBox.pl out/eps/StaticIndex/StaticIndexHugeBreakdown.eps out/pdf/StaticIndex/StaticIndexHugeBreakdown.pdf > /dev/null"
