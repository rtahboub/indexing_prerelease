set terminal postscript eps enhanced color 24

#set title "Varying Lifetime for STRIPES"

set xlabel "Lifetime (ticks)"
set ylabel "Time (s)"

set key bottom right

set yrange [0:]

# set output "out/eps/STRIPES/STRIPESHorizonSmall.eps"
# plot "out/outfiles/STRIPES/STRIPESHorizonSmallAvg.txt" using 1:($3+$5+$7) with linespoints title "Small Trace" lw 5

set output "out/eps/STRIPES/STRIPESHorizonLarge.eps"
#plot "out/outfiles/STRIPES/STRIPESHorizonLargeAvg.txt" using 1:($3+$5+$7) with linespoints title "Large Trace" lw 5


plot "out/outfiles/STRIPES/STRIPESHorizonLargeAvg.txt" using 1:(($3+$5+$7)/100.0) with linespoints title "STRIPES" lw 7,\
     "" using 1:(($9+$11+$13)/100.0) with linespoints title "Bulkload TPRTree" lw 7,\
     "" using 1:(($15+$17+$19)/100.0) with linespoints title "Std. TPRTree" lw 7

# set output "out/eps/STRIPES/STRIPESHorizonHuge.eps"
# plot "out/outfiles/STRIPES/STRIPESHorizonHugeAvg.txt" using 1:($3+$5+$7) with linespoints title "Huge Trace" lw 5


# set output "out/eps/STRIPES/STRIPESHorizonGauss.eps"
# plot "out/outfiles/STRIPES/STRIPESHorizonGaussAvg.txt" using 1:($3+$5+$7) with linespoints title "GAuss Trace" lw 5
