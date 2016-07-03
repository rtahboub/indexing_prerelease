set terminal postscript eps enhanced color 24
#set terminal png truecolor nocrop enhanced large

#set title "Varying Bucket Size Simple Grid"

set yrange [0:]

set xlabel "Entries per Bucket"
set ylabel "Time (s)"

# set output "out/eps/SimpleGrid/SimpleGridBucketSizeSmall.eps"
# plot "out/outfiles/SimpleGrid/SimpleGridBucketSizeSmallAvg.txt" using 1:(($3+$5+$7)/100.0) with linespoints title "Small Trace" lw 5

set output "out/eps/SimpleGrid/SimpleGridBucketSizeLarge.eps"
plot "out/outfiles/SimpleGrid/SimpleGridBucketSizeLargeAvg.txt" using 1:(($3+$5+$7)/100.0) with linespoints notitle lw 7

# set output "out/eps/SimpleGrid/SimpleGridBucketSizeHuge.eps"
# plot "out/outfiles/SimpleGrid/SimpleGridBucketSizeHugeAvg.txt" using 1:(($3+$5+$7)/100.0) with linespoints title "Huge Trace" lw 5

# set output "out/eps/SimpleGrid/SimpleGridBucketSizeGauss.eps"
# plot "out/outfiles/SimpleGrid/SimpleGridBucketSizeGaussAvg.txt" using 1:(($3+$5+$7)/100.0) with linespoints title "Gauss Trace" lw 5