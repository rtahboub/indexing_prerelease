set terminal postscript eps enhanced color 24
#set terminal png truecolor nocrop enhanced large


#set title "Comparing Threshold Values for Linearized KD Trie"

set xlabel "Threshold"
set ylabel "Time (s)"

set yrange [0:]

set output "out/eps/LinearizedKDTrie/TestKDTrieThresholdLarge.eps"
plot "out/outfiles/LinearizedKDTrie/TestKDTrieThresholdAvg.txt" using 1:(($3+$5+$7)/100.0) notitle with linespoints lw 7


# set output "out/eps/LinearizedKDTrie/TestKDTrieThresholdSmall.eps"
# plot "out/outfiles/LinearizedKDTrie/TestKDTrieThresholdAvg.txt" using 1:($3+$5+$7) with linespoints title "Small Trace" lw 5

# set output "out/eps/LinearizedKDTrie/TestKDTrieThresholdLarge.eps"
# plot "out/outfiles/LinearizedKDTrie/TestKDTrieThresholdAvg.txt" using 1:($9+$11+$13) with linespoints title "Large Trace" lw 5

# set output "out/eps/LinearizedKDTrie/TestKDTrieThresholdHuge.eps"
# plot "out/outfiles/LinearizedKDTrie/TestKDTrieThresholdAvg.txt" using 1:($15+$17+$19) with linespoints title "Huge Trace" lw 5

# set output "out/eps/LinearizedKDTrie/TestKDTrieThresholdGauss.eps"
# plot "out/outfiles/LinearizedKDTrie/TestKDTrieThresholdAvg.txt" using 1:($21+$23+$25) with linespoints title "Gauss Trace" lw 5

