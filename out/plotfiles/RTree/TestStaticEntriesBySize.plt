set terminal postscript eps enhanced color 

set output "out/eps/RTree/TestStaticEntriesBySize.eps"

set title "Varying Size of an RTree Node"

set xlabel "Node Size in Bytes"
set ylabel "Time (s)"

plot "out/outfiles/RTree/TestStaticRTreeEntriesBySizeAvg.txt" using 1:($3+$5+$7) with linespoints title "RTree" lw 3, \
     "out/outfiles/RTree/TestStaticCRTreeEntriesBySizeAvg.txt" using 1:($3+$5+$5) with linespoints title "CRTree" lw 3
