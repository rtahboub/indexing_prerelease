set terminal postscript eps enhanced color 

set output "out/eps/RTree/TestStaticCRTree.eps"

set title "Comparing CR Tree Quantization Levels"

set xlabel "Num Entries/Node"
set ylabel "Time (s)"

plot "out/outfiles/RTree/TestStaticCRTreeAvg.txt" using 1:($3+$5+$7) with linespoints title "Standard" lw 3, \
     "" using 1:($9+$11+$13) with linespoints title "Static" lw 3

