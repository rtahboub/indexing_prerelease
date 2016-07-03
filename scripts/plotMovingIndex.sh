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

#docmd "./scripts/plot.sh MovingIndexScaleQueries"
# docmd "./scripts/plot.sh MovingIndexScaleUpdates"
# docmd "./scripts/plot.sh MovingIndexGauss"
# docmd "./scripts/plot.sh MovingIndexScalePoints"
# docmd "./scripts/plot.sh MovingIndexHorizon"

 trace_prefix="out/traces/MovingIndex"
 out_prefix="out/outfiles/MovingIndex"

# echo ""
# echo "Creating gnuplot output files..."
# echo ""

#docmd "./bin/plot.exec breakdown --dir=$trace_prefix/MovingIndexScalePoints/ --outfile=$out_prefix/MovingIndexPointsBreakdown.txt"

# echo ""
# echo "Creating eps plots..."
# echo ""

#docmd "gnuplot out/plotfiles/MovingIndex/MovingIndexPointsBreakdown.plt"
docmd "gnuplot out/plotfiles/MovingIndex/MovingIndexScaleQueries.plt"

# echo ""
# echo "Converting eps to pdf..."
# echo ""

#docmd "ps2pdf out/eps/MovingIndex/MovingIndexPointsBreakdown.eps out/pdf/MovingIndex/MovingIndexPointsBreakdown.pdf"
docmd "ps2pdf out/eps/MovingIndex/MovingIndexScaleQueries.eps out/pdf/MovingIndex/MovingIndexScaleQueries.pdf"

# echo ""
# echo "Fixing pdf bounding box..."
# echo ""

docmd "perl out/fixBoundingBox.pl out/eps/MovingIndex/MovingIndexScaleQueries.eps out/pdf/MovingIndex/MovingIndexScaleQueries.pdf > /dev/null"

