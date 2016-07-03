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

trace_prefix="out/traces/MovingIndex"
out_prefix="out/outfiles/MovingIndex"

docmd "./bin/plot.exec avg --dir=$trace_prefix/MovingCrossover/ --outfile=$out_prefix/MovingCrossoverAvg.txt"
