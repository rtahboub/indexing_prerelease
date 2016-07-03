set terminal postscript eps enhanced color 24

set title "Varying Horizon for TPRTree"

set xlabel "Horizon (ticks)"
set ylabel "Time (s)"

set output "out/eps/TPRTree/TestTPRHorizonSmall.eps"
plot "out/outfiles/TPRTree/TestTPRHorizonAvg.txt" using 1:($3+$5+$7) with linespoints title "Small Trace" lw 5

# set output "out/eps/TPRTree/TestTPRHorizonLarge.eps"
# plot "out/outfiles/TPRTree/TestTPRHorizonAvg.txt" using 1:($9+$11+$13) with linespoints title "Large Trace" lw 5

# set output "out/eps/TPRTree/TestTPRHorizonHuge.eps"
# plot "out/outfiles/TPRTree/TestTPRHorizonAvg.txt" using 1:($15+$17+$19) with linespoints title "Huge Trace" lw 5

# set output "out/eps/TPRTree/TestTPRHorizonGauss.eps"
# plot "out/outfiles/TPRTree/TestTPRHorizonAvg.txt" using 1:($21+$23+$25) with linespoints title "Gauss Trace" lw 5


