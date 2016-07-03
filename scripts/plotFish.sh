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


#./scripts/plot.sh ScaleFish

docmd "gnuplot out/plotfiles/Fish/ScaleFish.plt"
docmd "ps2pdf out/eps/Fish/ScaleFish.eps out/pdf/Fish/ScaleFish.pdf"
docmd "perl out/fixBoundingBox.pl out/eps/Fish/ScaleFish.eps out/pdf/Fish/ScaleFish.pdf > /dev/null"