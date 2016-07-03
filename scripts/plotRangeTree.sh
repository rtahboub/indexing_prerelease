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



trace_prefix="out/traces/OrthogonalRangeTree"
out_prefix="out/outfiles/OrthogonalRangeTree"


echo ""
echo "Creating gnuplot output files..."
echo ""


docmd "./bin/plot.exec avg --dir=$trace_prefix/RangeTreeNodeSize/ --outfile=$out_prefix/RangeTreeNodeSizeAvg.txt"


echo ""
echo "Creating eps plots..."
echo ""

docmd "gnuplot out/plotfiles/OrthogonalRangeTree/RangeTreeNodeSize.plt"


# echo ""
# echo "Converting eps to pdf..."
# echo ""


docmd "ps2pdf out/eps/OrthogonalRangeTree/RangeTreeNodeSizeSmall.eps out/pdf/OrthogonalRangeTree/RangeTreeNodeSizeSmall.pdf"
docmd "ps2pdf out/eps/OrthogonalRangeTree/RangeTreeNodeSizeLarge.eps out/pdf/OrthogonalRangeTree/RangeTreeNodeSizeLarge.pdf"
# docmd "ps2pdf out/eps/OrthogonalRangeTree/RangeTreeNodeSizeHuge.eps out/pdf/OrthogonalRangeTree/RangeTreeNodeSizeHuge.pdf"
docmd "ps2pdf out/eps/OrthogonalRangeTree/RangeTreeNodeSizeGauss.eps out/pdf/OrthogonalRangeTree/RangeTreeNodeSizeGauss.pdf"

echo ""
echo "Fixing pdf bounding box..."
echo ""

docmd "perl out/fixBoundingBox.pl out/eps/OrthogonalRangeTree/RangeTreeNodeSizeSmall.eps out/pdf/OrthogonalRangeTree/RangeTreeNodeSizeSmall.pdf > /dev/null"

docmd "perl out/fixBoundingBox.pl out/eps/OrthogonalRangeTree/RangeTreeNodeSizeLarge.eps out/pdf/OrthogonalRangeTree/RangeTreeNodeSizeLarge.pdf > /dev/null"

# docmd "perl out/fixBoundingBox.pl out/eps/OrthogonalRangeTree/RangeTreeNodeSizeHuge.eps out/pdf/OrthogonalRangeTree/RangeTreeNodeSizeHuge.pdf > /dev/null"

docmd "perl out/fixBoundingBox.pl out/eps/OrthogonalRangeTree/RangeTreeNodeSizeGauss.eps out/pdf/OrthogonalRangeTree/RangeTreeNodeSizeGauss.pdf > /dev/null"

exit 0