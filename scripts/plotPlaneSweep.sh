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

# docmd "./scripts/plot.sh PlaneSweepScalePoints"
# docmd "./scripts/plot.sh PlaneSweepScaleQueries"
# docmd "./scripts/plot.sh PlaneSweepScaleUpdates"
# docmd "./scripts/plot.sh PlaneSweepGauss"
# docmd "./scripts/plot.sh PBSMTileDim"


trace_prefix="out/traces/PlaneSweep"
out_prefix="out/outfiles/PlaneSweep"


echo ""
echo "Creating gnuplot output files..."
echo ""


docmd "./bin/plot.exec avg --dir=$trace_prefix/PBSMPartitionNum/ --outfile=$out_prefix/PBSMPartitionNumAvg.txt"


echo ""
echo "Creating eps plots..."
echo ""

docmd "gnuplot out/plotfiles/PlaneSweep/PBSMPartitionNum.plt"


echo ""
echo "Converting eps to pdf..."
echo ""

#docmd "ps2pdf out/eps/PlaneSweep/PBSMPartitionNumSmall.eps out/pdf/PlaneSweep/PBSMPartitionNumSmall.pdf"
docmd "ps2pdf out/eps/PlaneSweep/PBSMPartitionNumLarge.eps out/pdf/PlaneSweep/PBSMPartitionNumLarge.pdf"
# docmd "ps2pdf out/eps/PlaneSweep/PBSMPartitionNumHuge.eps out/pdf/PlaneSweep/PBSMPartitionNumHuge.pdf"
# docmd "ps2pdf out/eps/PlaneSweep/PBSMPartitionNumGauss.eps out/pdf/PlaneSweep/PBSMPartitionNumGauss.pdf"

echo ""
echo "Fixing pdf bounding box..."
echo ""

# docmd "perl out/fixBoundingBox.pl out/eps/PlaneSweep/PBSMPartitionNumSmall.eps out/pdf/PlaneSweep/PBSMPartitionNumSmall.pdf > /dev/null"

docmd "perl out/fixBoundingBox.pl out/eps/PlaneSweep/PBSMPartitionNumLarge.eps out/pdf/PlaneSweep/PBSMPartitionNumLarge.pdf > /dev/null"

# docmd "perl out/fixBoundingBox.pl out/eps/PlaneSweep/PBSMPartitionNumHuge.eps out/pdf/PlaneSweep/PBSMPartitionNumHuge.pdf > /dev/null"

# docmd "perl out/fixBoundingBox.pl out/eps/PlaneSweep/PBSMPartitionNumGauss.eps out/pdf/PlaneSweep/PBSMPartitionNumGauss.pdf > /dev/null"

exit 0