set terminal postscript eps enhanced color 

set output "out/eps/ScaleNumPointsBreakdown.eps"

set title "Scaling the number of Points"

set style fill solid border -1

set style histogram rowstacked
set style data histograms
set boxwidth 0.75

set ylabel "Time (s)"   

set xtics mirror rotate by -45

set key left

# TODO bas: I don't know how to cluster stacked histograms, so you can only
# plot the breakdown for single strategy. 

# Column numbers:
# PBSM 3 5 7
# BinarySearch 9 11 13
# CRTree 15 17 19
# Linearized KDTrie 21 23 25
# OrthogonalRangeTree 27 29 31
# RTree 33 35 37


plot "out/outfiles/ScaleNumPointsBreakdown.txt" using 3:xticlabels(1) title "Build", \
     ""  using 5 title "Query", \
     ""  using 7 title "Update"




