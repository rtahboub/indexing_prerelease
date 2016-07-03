set terminal postscript eps enhanced color 

set output "out/eps/RTree/TestCRTreeQuantizationHuge.eps"

set title "Comparing CR-Tree Quantization Levels"

set xlabel "Num Entries/Node"
set ylabel "Time (s)"

plot "out/outfiles/RTree/TestCRTreeQuantizationHugeAvg.txt" using 1:($3+$5+$7) with linespoints title "uint8_t" lw 3, \
     "" using 1:($9+$11+$13) with linespoints title "uint16_t" lw 3,\
     "" using 1:($15+$17+$19) with linespoints title "uint32_t" lw 3,\
     "" using 1:($21+$23+$25) with linespoints title "uint64_t" lw 3,\
     "" using 1:($27+$29+$31) with linespoints title "RTree" lw 3
