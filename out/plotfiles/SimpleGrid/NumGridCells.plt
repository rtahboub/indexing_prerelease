set terminal postscript eps enhanced color 24

#set title "Varying Num. Grid Cells in Simple Grid"

set xlabel "Grid cells per side"
set ylabel "Avg. Time per Tick (s)"

set output "out/eps/SimpleGrid/NumGridCellsLarge.eps"
#set output "out/eps/SimpleGrid/NumGridCellsLarge.png"

# plot "out/outfiles/SimpleGrid/NumGridCellsLargeAvg.txt" using 1:(($3+$5+$7)/100.0) notitle with linespoints lw 7

plot "out/outfiles/SimpleGrid/NumGridCellsLargeAvg.txt" using 1:(($3+$5+$7)/100.0):(($9+$11+$13)/100.0):(($15+$17+$19)/100.0) with errorbars ls 1 lw 7 notitle,\
     "" using 1:(($3+$5+$7)/100.0) notitle with lines ls 1 lw 7
     


# set output "out/eps/SimpleGrid/NumGridCellsSmall.eps"
# plot "out/outfiles/SimpleGrid/NumGridCellsSmallAvg.txt" using 1:($3+$5+$7) with linespoints title "Small Trace" lw 5

# set output "out/eps/SimpleGrid/NumGridCellsLarge.eps"
# plot "out/outfiles/SimpleGrid/NumGridCellsLargeAvg.txt" using 1:($3+$5+$7) with linespoints title "Large Trace" lw 5

# set output "out/eps/SimpleGrid/NumGridCellsHuge.eps"
# plot "out/outfiles/SimpleGrid/NumGridCellsHugeAvg.txt" using 1:($3+$5+$7) with linespoints title "Huge Trace" lw 5


# set output "out/eps/SimpleGrid/NumGridCellsGauss.eps"
# plot "out/outfiles/SimpleGrid/NumGridCellsGaussAvg.txt" using 1:($3+$5+$7) with linespoints title "Gauss Trace" lw 5












