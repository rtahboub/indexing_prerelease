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


trace_prefix="out/traces/SimpleGrid"
out_prefix="out/outfiles/SimpleGrid"

echo ""
echo "Creating gnuplot output files..."
echo ""

# docmd "./bin/plot.exec avg --dir=$trace_prefix/NumGridCellsSmall/ --outfile=$out_prefix/NumGridCellsSmallAvg.txt"
docmd "./bin/plot.exec avg --dir=$trace_prefix/NumGridCellsLarge/ --outfile=$out_prefix/NumGridCellsLargeAvg.txt"
# docmd "./bin/plot.exec avg --dir=$trace_prefix/NumGridCellsHuge/ --outfile=$out_prefix/NumGridCellsHugeAvg.txt"
# docmd "./bin/plot.exec avg --dir=$trace_prefix/NumGridCellsGauss/ --outfile=$out_prefix/NumGridCellsGaussAvg.txt"

#docmd "./bin/plot.exec avg --dir=$trace_prefix/SimpleGridBucketSizeSmall/ --outfile=$out_prefix/SimpleGridBucketSizeSmallAvg.txt"
docmd "./bin/plot.exec avg --dir=$trace_prefix/SimpleGridBucketSizeLarge/ --outfile=$out_prefix/SimpleGridBucketSizeLargeAvg.txt"
#docmd "./bin/plot.exec avg --dir=$trace_prefix/SimpleGridBucketSizeHuge/ --outfile=$out_prefix/SimpleGridBucketSizeHugeAvg.txt"
#docmd "./bin/plot.exec avg --dir=$trace_prefix/SimpleGridBucketSizeGauss/ --outfile=$out_prefix/SimpleGridBucketSizeGaussAvg.txt"


echo ""
echo "Creating eps plots..."
echo ""

docmd "gnuplot out/plotfiles/SimpleGrid/NumGridCells.plt"
docmd "gnuplot out/plotfiles/SimpleGrid/SimpleGridBucketSize.plt"


echo ""
echo "Converting eps to png..."
echo ""

docmd "convert -density 100 out/eps/SimpleGrid/NumGridCellsLarge.eps -flatten out/png/SimpleGrid/NumGridCellsLarge.png"
docmd "convert -density 100 out/eps/SimpleGrid/SimpleGridBucketSizeLarge.eps -flatten out/png/SimpleGrid/SimpleGridBucketSizeLarge.png"


# echo ""
# echo "Converting eps to pdf..."
# echo ""

# docmd "ps2pdf out/eps/SimpleGrid/NumGridCellsSmall.eps out/pdf/SimpleGrid/NumGridCellsSmall.pdf"
#docmd "ps2pdf out/eps/SimpleGrid/NumGridCellsLarge.eps out/pdf/SimpleGrid/NumGridCellsLarge.pdf"
# docmd "ps2pdf out/eps/SimpleGrid/NumGridCellsHuge.eps out/pdf/SimpleGrid/NumGridCellsHuge.pdf"
# docmd "ps2pdf out/eps/SimpleGrid/NumGridCellsGauss.eps out/pdf/SimpleGrid/NumGridCellsGauss.pdf"

# docmd "ps2pdf out/eps/SimpleGrid/SimpleGridBucketSizeSmall.eps out/pdf/SimpleGrid/SimpleGridBucketSizeSmall.pdf"
# docmd "ps2pdf out/eps/SimpleGrid/SimpleGridBucketSizeLarge.eps out/pdf/SimpleGrid/SimpleGridBucketSizeLarge.pdf"
# docmd "ps2pdf out/eps/SimpleGrid/SimpleGridBucketSizeHuge.eps out/pdf/SimpleGrid/SimpleGridBucketSizeHuge.pdf"
#docmd "ps2pdf out/eps/SimpleGrid/SimpleGridBucketSizeGauss.eps out/pdf/SimpleGrid/SimpleGridBucketSizeGauss.pdf"

echo ""
echo "Fixing pdf bounding box..."
echo ""

# docmd "perl out/fixBoundingBox.pl out/eps/SimpleGrid/NumGridCellsSmall.eps out/pdf/SimpleGrid/NumGridCellsSmall.pdf > /dev/null"

#docmd "perl out/fixBoundingBox.pl out/eps/SimpleGrid/NumGridCellsLarge.eps out/pdf/SimpleGrid/NumGridCellsLarge.pdf > /dev/null"

# docmd "perl out/fixBoundingBox.pl out/eps/SimpleGrid/NumGridCellsHuge.eps out/pdf/SimpleGrid/NumGridCellsHuge.pdf > /dev/null"

# docmd "perl out/fixBoundingBox.pl out/eps/SimpleGrid/NumGridCellsGauss.eps out/pdf/SimpleGrid/NumGridCellsGauss.pdf > /dev/null"


# docmd "perl out/fixBoundingBox.pl out/eps/SimpleGrid/SimpleGridBucketSizeSmall.eps out/pdf/SimpleGrid/SimpleGridBucketSizeSmall.pdf > /dev/null"

# docmd "perl out/fixBoundingBox.pl out/eps/SimpleGrid/SimpleGridBucketSizeLarge.eps out/pdf/SimpleGrid/SimpleGridBucketSizeLarge.pdf > /dev/null"


# docmd "perl out/fixBoundingBox.pl out/eps/SimpleGrid/SimpleGridBucketSizeHuge.eps out/pdf/SimpleGrid/SimpleGridBucketSizeHuge.pdf > /dev/null"

# docmd "perl out/fixBoundingBox.pl out/eps/SimpleGrid/SimpleGridBucketSizeGauss.eps out/pdf/SimpleGrid/SimpleGridBucketSizeGauss.pdf > /dev/null"

exit 0