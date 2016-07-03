set terminal postscript eps enhanced color 24

#set title "Cost Breakdown for 50,000 points"

set style fill solid border -1

set style histogram rowstacked
set style data histograms
set boxwidth 0.75

set ylabel "Time (s)"   

set xtics mirror rotate by -45


set xtics   ("RTree" 0.000000, "CRTree" 1.00000, "Range Tree" 2.0000, "Lin. KDTrie" 3.00000, "Grid" 4.00000)
set key left

set output "out/eps/StaticIndex/StaticIndexPointsBreakdown.eps"

plot "out/outfiles/StaticIndex/StaticIndexPointsBreakdown.txt" using 3 title "Build", \
     ""  using 5 title "Query"

# plot "out/outfiles/StaticIndex/StaticIndexPointsBreakdown.txt" using 15 title "Build", \
#      ""  using 17 title "Query"#, \
# #     ""  using 19 title "Update"

set xtics   ("RTree" 0.000000, "CRTree" 1.00000)
set output "out/eps/StaticIndex/StaticIndexHugeBreakdown.eps"

plot "out/outfiles/StaticIndex/StaticIndexHugeBreakdown.txt" using 3 title "Build", \
      ""  using 5:key(1) title "Query"

#      #, \
# #     ""  using 19 title "Update"
