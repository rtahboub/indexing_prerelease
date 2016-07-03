set terminal postscript eps enhanced color 24

set xlabel "Tiles/Side"
set ylabel "Time (s)"
set yrange [0:]

set key horiz bottom

set output "out/eps/PlaneSweep/PBSMTileDim.eps"

plot "out/outfiles/PlaneSweep/PBSMTileDimAvg.txt" using 1:(($3+$5+$7)/100.0) with linespoints title "1 hotspot" lw 7, \
     "" using 1:(($9+$11+$13)/100.0) with linespoints title "10 hotspots" lw 7, \
     "" using 1:(($15+$17+$19)/100.0) with linespoints title "100 hotspots" lw 7, \
     "" using 1:(($21+$23+$25)/100.0) with linespoints title "1000 hotspots" lw 7