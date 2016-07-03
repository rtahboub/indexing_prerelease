set terminal postscript eps enhanced color

set output "out/eps/MovingCrossover.eps"

set title "Varying Query and Update Fractions"

set xabel "Query Frac."
set ylabel "Update Frac."
set zlabel "Time (s)"

splot "out/outfiles/MovingCrossover.txt" using 1:2:($4+$6+$8) title "RTree2D",\
      "" using 1:2:($10+$12+$14) title "TPRTree2D"