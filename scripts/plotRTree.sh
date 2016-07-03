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


trace_prefix="out/traces/RTree"
out_prefix="out/outfiles/RTree"

echo ""
echo "Creating gnuplot output files..."
echo ""

docmd "./bin/plot.exec avg --dir=$trace_prefix/RTreeNodeSize/ --outfile=$out_prefix/RTreeNodeSizeAvg.txt"
docmd "./bin/plot.exec avg --dir=$trace_prefix/CRTree8NodeSize/ --outfile=$out_prefix/CRTree8NodeSizeAvg.txt"
docmd "./bin/plot.exec avg --dir=$trace_prefix/CRTree16NodeSize/ --outfile=$out_prefix/CRTree16NodeSizeAvg.txt"
docmd "./bin/plot.exec avg --dir=$trace_prefix/CRTree32NodeSize/ --outfile=$out_prefix/CRTree32NodeSizeAvg.txt"


#docmd "./bin/plot.exec breakdown --dir=$trace_prefix/RTreeNodeSize/ --outfile=$out_prefix/RTreeNodeSizeAvgBreakdown.txt"


echo ""
echo "Creating eps plots..."
echo ""

docmd "gnuplot out/plotfiles/RTree/RTreeNodeSize.plt"
# docmd "gnuplot out/plotfiles/RTree/RTreeNodeSizeBreakdown.plt"


echo ""
echo "Converting eps to png..."
echo ""

docmd "convert -density 100 out/eps/RTree/RTreeNodeSize.eps -flatten out/png/RTree/RTreeNodeSize.png"


echo ""
echo "Converting eps to pdf..."
echo ""

#docmd "ps2pdf out/eps/RTree/RTreeNodeSizeSmall.eps out/pdf/RTree/RTreeNodeSizeSmall.pdf"
docmd "ps2pdf out/eps/RTree/RTreeNodeSizeLarge.eps out/pdf/RTree/RTreeNodeSize.pdf"
#docmd "ps2pdf out/eps/RTree/RTreeNodeSizeHuge.eps out/pdf/RTree/RTreeNodeSizeHuge.pdf"
#docmd "ps2pdf out/eps/RTree/RTreeNodeSizeGauss.eps out/pdf/RTree/RTreeNodeSizeGauss.pdf"

echo ""
echo "Fixing pdf bounding box..."
echo ""


#docmd "perl out/fixBoundingBox.pl out/eps/RTree/RTreeNodeSizeSmall.eps out/pdf/RTree/RTreeNodeSizeSmall.pdf > /dev/null"
docmd "perl out/fixBoundingBox.pl out/eps/RTree/RTreeNodeSize.eps out/pdf/RTree/RTreeNodeSize.pdf > /dev/null"
#docmd "perl out/fixBoundingBox.pl out/eps/RTree/RTreeNodeSizeHuge.eps out/pdf/RTree/RTreeNodeSizeHuge.pdf > /dev/null"
#docmd "perl out/fixBoundingBox.pl out/eps/RTree/RTreeNodeSizeGauss.eps out/pdf/RTree/RTreeNodeSizeGauss.pdf > /dev/null"

exit 0