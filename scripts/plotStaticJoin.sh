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


docmd "./scripts/plot.sh StaticJoinScalePoints"
docmd "./scripts/plot.sh StaticJoinScaleQueries"
docmd "./scripts/plot.sh StaticJoinScaleUpdates"
docmd "./scripts/plot.sh StaticJoinGauss"


# echo ""
# echo "Creating eps plots..."
# echo ""

# docmd "gnuplot out/plotfiles/StaticJoin/StaticJoinScalePoints.plt"
# docmd "gnuplot out/plotfiles/StaticJoin/StaticJoinScaleQueries.plt"
# docmd "gnuplot out/plotfiles/StaticJoin/StaticJoinScaleUpdates.plt"
# docmd "gnuplot out/plotfiles/StaticJoin/StaticJoinGauss.plt"

# echo ""
# echo "Converting eps to pdf..."
# echo ""

# docmd "ps2pdf out/eps/StaticJoin/StaticJoinScalePoints.eps out/pdf/StaticJoin/StaticJoinScalePoints.pdf"
# docmd "ps2pdf out/eps/StaticJoin/StaticJoinScaleQueries.eps out/pdf/StaticJoin/StaticJoinScaleQueries.pdf"
# docmd "ps2pdf out/eps/StaticJoin/StaticJoinScaleUpdates.eps out/pdf/StaticJoin/StaticJoinScaleUpdates.pdf"
# docmd "ps2pdf out/eps/StaticJoin/StaticJoinGauss.eps out/pdf/StaticJoin/StaticJoinGauss.pdf"


# echo ""
# echo "Fixing pdf bounding box..."
# echo ""

# docmd "perl out/fixBoundingBox.pl out/eps/StaticJoin/StaticJoinScalePoints.eps out/pdf/StaticJoin/StaticJoinScalePoints.pdf > /dev/null"

# docmd "perl out/fixBoundingBox.pl out/eps/StaticJoin/StaticJoinScaleQueries.eps out/pdf/StaticJoin/StaticJoinScaleQueries.pdf > /dev/null"

# docmd "perl out/fixBoundingBox.pl out/eps/StaticJoin/StaticJoinScaleUpdates.eps out/pdf/StaticJoin/StaticJoinScaleUpdates.pdf > /dev/null"

# docmd "perl out/fixBoundingBox.pl out/eps/StaticJoin/StaticJoinGauss.eps out/pdf/StaticJoin/StaticJoinGauss.pdf > /dev/null"


