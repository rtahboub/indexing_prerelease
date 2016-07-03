set terminal postscript eps enhanced color 24

#set title "Cost Breakdown for 50,000 points"

set style fill solid border -1

set style histogram rowstacked
set style data histograms
set boxwidth 0.75

set ylabel "Time (s)"   

set xtics mirror rotate by -45

set xtics   ("TPRTree" 0.000000, "STRIPES" 1.00000)
set key right

set output "out/eps/MovingIndex/MovingIndexPointsBreakdown.eps"


plot "out/outfiles/MovingIndex/MovingIndexPointsBreakdown.txt" using 15 title "Build", \
      ""  using 17 title "Query", \
      ""  using 19 title "Update"

