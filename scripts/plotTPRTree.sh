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


docmd "./scripts/plot.sh TPRTreeNodeSize"

trace_prefix="out/traces/TPRTree"
out_prefix="out/outfiles/TPRTree"


echo ""
echo "Creating gnuplot output files..."
echo ""


# docmd "./bin/plot.exec avg --dir=$trace_prefix/TestTPRHorizon/ --outfile=$out_prefix/TestTPRHorizonAvg.txt"
# docmd "./bin/plot.exec avg --dir=$trace_prefix/TestTPREntries/ --outfile=$out_prefix/TestTPREntriesAvg.txt"


echo ""
echo "Creating eps plots..."
echo ""

# docmd "gnuplot out/plotfiles/TPRTree/TestTPRHorizon.plt"
# docmd "gnuplot out/plotfiles/TPRTree/TestTPREntries.plt"

# echo ""
# echo "Converting eps to pdf..."
# echo ""


# docmd "ps2pdf out/eps/TPRTree/TestTPRHorizonSmall.eps out/pdf/TPRTree/TestTPRHorizonSmall.pdf"
# docmd "ps2pdf out/eps/TPRTree/TestTPRHorizonLarge.eps out/pdf/TPRTree/TestTPRHorizonLarge.pdf"
# docmd "ps2pdf out/eps/TPRTree/TestTPRHorizonHuge.eps out/pdf/TPRTree/TestTPRHorizonHuge.pdf"
# docmd "ps2pdf out/eps/TPRTree/TestTPRHorizonGauss.eps out/pdf/TPRTree/TestTPRHorizonGauss.pdf"

# docmd "ps2pdf out/eps/TPRTree/TestTPREntries.eps out/pdf/TPRTree/TestTPREntries.pdf"

echo ""
echo "Fixing pdf bounding box..."
echo ""

# docmd "perl out/fixBoundingBox.pl out/eps/TPRTree/TestTPREntries.eps out/pdf/TPRTree/TestTPREntries.pdf > /dev/null"

# docmd "perl out/fixBoundingBox.pl out/eps/TPRTree/TestTPRHorizonSmall.eps out/pdf/TPRTree/TestTPRHorizonSmall.pdf > /dev/null"

# docmd "perl out/fixBoundingBox.pl out/eps/TPRTree/TestTPRHorizonLarge.eps out/pdf/TPRTree/TestTPRHorizonLarge.pdf > /dev/null"

# docmd "perl out/fixBoundingBox.pl out/eps/TPRTree/TestTPRHorizonHuge.eps out/pdf/TPRTree/TestTPRHorizonHuge.pdf > /dev/null"

# docmd "perl out/fixBoundingBox.pl out/eps/TPRTree/TestTPRHorizonGauss.eps out/pdf/TPRTree/TestTPRHorizonGauss.pdf > /dev/null"

exit 0