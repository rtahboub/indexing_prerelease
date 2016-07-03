set terminal postscript eps enhanced color

set output "out/eps/CRTree2DMAXENTRIES.eps"

set title "Varying MAXENTRIES for CRTree2D"

set xlabel "MAXENTRIES."
set ylabel "Time (s)"

plot "out/outfiles/CRTree2DMAXENTRIES.txt" using 1:($3+$5+$7) with linespoints title "CRTree2D" lw 3

