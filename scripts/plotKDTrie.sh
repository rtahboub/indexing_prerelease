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


trace_prefix="out/traces/LinearizedKDTrie"
out_prefix="out/outfiles/LinearizedKDTrie"


echo ""
echo "Creating gnuplot output files..."
echo ""


docmd "./bin/plot.exec avg --dir=$trace_prefix/TestKDTrieThreshold/ --outfile=$out_prefix/TestKDTrieThresholdAvg.txt"

echo ""
echo "Creating eps plots..."
echo ""

docmd "gnuplot out/plotfiles/LinearizedKDTrie/TestKDTrieThreshold.plt"



echo ""
echo "Converting eps to png..."
echo ""

docmd "convert -density 100 out/eps/LinearizedKDTrie/TestKDTrieThresholdLarge.eps -flatten out/png/LinearizedKDTrie/TestKDTrieThresholdLarge.png"


# echo ""
# echo "Converting eps to pdf..."
# echo ""


#docmd "ps2pdf out/eps/LinearizedKDTrie/TestKDTrieThresholdSmall.eps out/pdf/LinearizedKDTrie/TestKDTrieThresholdSmall.pdf"
#docmd "ps2pdf out/eps/LinearizedKDTrie/TestKDTrieThresholdLarge.eps out/pdf/LinearizedKDTrie/TestKDTrieThresholdLarge.pdf"
# docmd "ps2pdf out/eps/LinearizedKDTrie/TestKDTrieThresholdHuge.eps out/pdf/LinearizedKDTrie/TestKDTrieThresholdHuge.pdf"
# docmd "ps2pdf out/eps/LinearizedKDTrie/TestKDTrieThresholdGauss.eps out/pdf/LinearizedKDTrie/TestKDTrieThresholdGauss.pdf"



echo ""
echo "Fixing pdf bounding box..."
echo ""

# docmd "perl out/fixBoundingBox.pl out/eps/LinearizedKDTrie/TestKDTrieThresholdSmall.eps out/pdf/LinearizedKDTrie/TestKDTrieThresholdSmall.pdf > /dev/null"

#docmd "perl out/fixBoundingBox.pl out/eps/LinearizedKDTrie/TestKDTrieThresholdLarge.eps out/pdf/LinearizedKDTrie/TestKDTrieThresholdLarge.pdf > /dev/null"

# docmd "perl out/fixBoundingBox.pl out/eps/LinearizedKDTrie/TestKDTrieThresholdHuge.eps out/pdf/LinearizedKDTrie/TestKDTrieThresholdHuge.pdf > /dev/null"

# docmd "perl out/fixBoundingBox.pl out/eps/LinearizedKDTrie/TestKDTrieThresholdGauss.eps out/pdf/LinearizedKDTrie/TestKDTrieThresholdGauss.pdf > /dev/null"

exit 0