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



trace_prefix="out/traces/SynchTraversal"
out_prefix="out/outfiles/SynchTraversal"


echo ""
echo "Creating gnuplot output files..."
echo ""


docmd "./bin/plot.exec avg --dir=$trace_prefix/SynchTraversalNodeSize/ --outfile=$out_prefix/SynchTraversalNodeSizeAvg.txt"


echo ""
echo "Creating eps plots..."
echo ""

docmd "gnuplot out/plotfiles/SynchTraversal/SynchTraversalNodeSize.plt"


# echo ""
# echo "Converting eps to pdf..."
# echo ""


docmd "ps2pdf out/eps/SynchTraversal/SynchTraversalNodeSize.eps out/pdf/SynchTraversal/SynchTraversalNodeSize.pdf"


# docmd "ps2pdf out/eps/SynchTraversal/SynchTraversalNodeSizeSmall.eps out/pdf/SynchTraversal/SynchTraversalNodeSizeSmall.pdf"
# docmd "ps2pdf out/eps/SynchTraversal/SynchTraversalNodeSizeLarge.eps out/pdf/SynchTraversal/SynchTraversalNodeSizeLarge.pdf"
# docmd "ps2pdf out/eps/SynchTraversal/SynchTraversalNodeSizeHuge.eps out/pdf/SynchTraversal/SynchTraversalNodeSizeHuge.pdf"
# docmd "ps2pdf out/eps/SynchTraversal/SynchTraversalNodeSizeGauss.eps out/pdf/SynchTraversal/SynchTraversalNodeSizeGauss.pdf"

echo ""
echo "Fixing pdf bounding box..."
echo ""

docmd "perl out/fixBoundingBox.pl out/eps/SynchTraversal/SynchTraversalNodeSize.eps out/pdf/SynchTraversal/SynchTraversalNodeSize.pdf > /dev/null"



# docmd "perl out/fixBoundingBox.pl out/eps/SynchTraversal/SynchTraversalNodeSizeSmall.eps out/pdf/SynchTraversal/SynchTraversalNodeSizeSmall.pdf > /dev/null"
# docmd "perl out/fixBoundingBox.pl out/eps/SynchTraversal/SynchTraversalNodeSizeLarge.eps out/pdf/SynchTraversal/SynchTraversalNodeSizeLarge.pdf > /dev/null"
# docmd "perl out/fixBoundingBox.pl out/eps/SynchTraversal/SynchTraversalNodeSizeHuge.eps out/pdf/SynchTraversal/SynchTraversalNodeSizeHuge.pdf > /dev/null"
# docmd "perl out/fixBoundingBox.pl out/eps/SynchTraversal/SynchTraversalNodeSizeGauss.eps out/pdf/SynchTraversal/SynchTraversalNodeSizeGauss.pdf > /dev/null"

exit 0