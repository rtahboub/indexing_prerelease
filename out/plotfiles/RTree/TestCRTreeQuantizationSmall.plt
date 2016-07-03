set terminal postscript eps enhanced color 

set output "out/eps/RTree/TestCRTreeQuantizationSmall.eps"

set title "Comparing CR-Tree Quantization Levels"

set xlabel "Num Entries/Node"
set ylabel "Time (s)"

plot "out/outfiles/RTree/TestCRTreeQuantizationSmallAvg.txt" using 1:($3+$5+$7) with linespoints title "uint8_t" lw 3, \
     "" using 1:($9+$11+$13) with linespoints title "uint16_t" lw 3,\
     "" using 1:($15+$17+$19) with linespoints title "uint32_t" lw 3,\
     "" using 1:($21+$23+$25) with linespoints title "uint64_t" lw 3,\
     "" using 1:($27+$29+$31) with linespoints title "RTree" lw 3


#plot "out/outfiles/TestRTreeEntriesAvg.txt" using 1:($3+$5+$7) with linespoints title "query0.5update0.1" lw 3, \
#     "" using 1:($9+$11+$13) with linespoints title "query0.1update0.5" lw 3, \
#     "" using 1:($15+$17+$19) with linespoints title "query0.5update0.5" lw 3,\
#     "" using 1:($21+$23+$25) with linespoints title "query0.5update0.9" lw 3,\
#     "" using 1:($27+$29+$31) with linespoints title "gauss1" lw 3,\
#     "" using 1:($33+$35+$37) with linespoints title "gauss10" lw 3,\
#     "" using 1:($39+$41+$43) with linespoints title "gauss100" lw 3


