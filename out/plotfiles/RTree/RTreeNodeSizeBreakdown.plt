set terminal postscript eps enhanced color 24

set title "RTree/CRTree Node Size"

set style fill solid border -1

set style histogram rowstacked
set style data histograms
set boxwidth 0.75

set ylabel "Time (s)"   

set xtics mirror rotate by -45

set key left


set output "out/eps/RTree/RTreeNodeSizeAvgBreakdown.eps"

plot "out/outfiles/RTree/RTreeNodeSizeAvgBreakdown.txt" using 3:xticlabels(1) title "Build", \
     ""  using 5 title "Query", \
     ""  using 7 title "Update"
