set terminal postscript eps enhanced color

set output "out/eps/RTree2DMAXENTRIES.eps"

set title "Varying MAXENTRIES for RTree2D"

set xlabel "MAXENTRIES."
set ylabel "Time (s)"

plot "out/outfiles/RTree2DMAXENTRIES.txt" using 1:($3+$5+$7) with linespoints title "RTree2D" lw 3

