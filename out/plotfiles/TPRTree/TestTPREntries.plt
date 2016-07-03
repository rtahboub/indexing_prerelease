set terminal postscript eps enhanced color 24

set title "Testing TPR-Tree Entries Layout"

set xlabel "Num Points"
set ylabel "Time (s)"

set xtics rotate by -45

set output "out/eps/TPRTree/TestTPREntries.eps"
plot "out/outfiles/TPRTree/TestTPREntriesAvg.txt" using 1:(($3+$5+$7)/100.0) with linespoints title "TPRTree2D" lw 5,\
     "" using 1:(($9+$11+$13)/100.0) with linespoints title "TPRTree2DEntry" lw 5



