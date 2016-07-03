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

docmd "./bin/datagen.exec network --edgefile=traces/network_files/ol.ascii-edge.dat --nodefile=traces/network_files/ol.ascii-node.dat --outfile=traces/olnetwork.txt --numTicks=1 --numPoints=100000 --space=100000 --probQuery=0.5 --qMin=100 --qMax=100 --binary=false --maxSpeed=100"

docmd "./bin/datagen.exec network --edgefile=traces/network_files/sg-ascii-edge.dat --nodefile=traces/network_files/sg-ascii-node.dat --outfile=traces/sgnetwork.dat --numTicks=100 --numPoints=100000 --space=100000 --probQuery=0.5 --qMin=100 --qMax=100 --binary=true --maxSpeed=100"

docmd "./bin/datagen.exec network --edgefile=traces/network_files/sa-ascii-edge.dat --nodefile=traces/network_files/sa-ascii-node.dat --outfile=traces/sanetwork.dat --numTicks=100 --numPoints=100000 --space=100000 --probQuery=0.5 --qMin=100 --qMax=100 --binary=true --maxSpeed=100"
