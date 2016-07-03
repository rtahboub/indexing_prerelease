set terminal postscript eps enhanced color 24

set output "out/eps/PlaneSweep/PlaneSweepScalePoints.eps"

#set title "Scaling the number of Points"

set key left top

set xlabel "Num. of Points"
set ylabel "Avg. Time per Tick (s)"

set yrange [0:]

set xtics   ("10K" 10000, "30K" 30000, "50K" 50000, "70K" 70000, "90K" 90000)

plot "out/outfiles/PlaneSweep/PlaneSweepScalePointsAvg.txt" using 1:(($3+$5+$7)/100.0):(($9+$11+$13)/100.0):(($15+$17+$19)/100.0) with errorbars ls 1 lw 7 notitle,\
     "" using 1:(($3+$5+$7)/100.0) with lines title "Forward-Sweep" ls 1 lw 7,\
     "" using 1:(($21+$23+$25)/100.0):(($27+$29+$31)/100.0):(($33+$35+$37)/100.0) with errorbars ls 2 lw 7 notitle,\
     "" using 1:(($21+$23+$25)/100.0) with lines title "List-Sweep" ls 2 lw 7,\
     "" using 1:(($39+$41+$43)/100.0):(($45+$47+$49)/100.0):(($51+$53+$55)/100.0) with errorbars ls 3 lw 7 notitle,\
     "" using 1:(($39+$41+$43)/100.0) with lines title "Striped-Sweep 8" ls 3 lw 7,\
     "" using 1:(($57+$59+$61)/100.0):(($63+$65+$67)/100.0):(($69+$71+$73)/100.0) with errorbars ls 4 lw 7 notitle,\
     "" using 1:(($57+$59+$61)/100.0) with lines title "Striped-Sweep 16" ls 4 lw 7,\
     "" using 1:(($75+$77+$79)/100.0):(($81+$83+$85)/100.0):(($87+$89+$91)/100.0) with errorbars ls 5 lw 7 notitle,\
     "" using 1:(($75+$77+$79)/100.0) with lines title "Striped-Sweep 32" ls 5 lw 7,\
     "" using 1:(($93+$95+$97)/100.0):(($99+$101+$103)/100.0):(($105+$107+$109)/100.0) with errorbars ls 6 lw 7 notitle,\
     "" using 1:(($93+$95+$97)/100.0) with lines title "Striped-Sweep 64" ls 6 lw 7,\
     "" using 1:(($111+$113+$115)/100.0):(($117+$119+$121)/100.0):(($123+$125+$127)/100.0) with errorbars ls 7 lw 7 notitle,\
     "" using 1:(($111+$113+$115)/100.0) with lines title "Striped-Sweep 128" ls 7 lw 7



# plot "out/outfiles/PlaneSweep/PlaneSweepScalePointsAvg.txt" using 1:($3+$5+$7) with linespoints title "ForwardPlaneSweep" lw 5,\
#      "" using 1:($9+$11+$13) with linespoints title "ListSweep" lw 5,\
#      "" using 1:($15+$17+$19) with linespoints title "StripedSweep8" lw 5,\
#      "" using 1:($21+$23+$25) with linespoints title "StripedSweep16" lw 5,\
#      "" using 1:($27+$29+$31) with linespoints title "StripedSweep32" lw 5,\
#      "" using 1:($33+$35+$37) with linespoints title "StripedSweep64" lw 5,\
#      "" using 1:($39+$41+$43) with linespoints title "StripedSweep128" lw 5,\
#      "" using 1:($45+$47+$49) with linespoints title "ForwardPBSM" lw 5,\
#      "" using 1:($51+$53+$55) with linespoints title "ListPBSM" lw 5
     



     
