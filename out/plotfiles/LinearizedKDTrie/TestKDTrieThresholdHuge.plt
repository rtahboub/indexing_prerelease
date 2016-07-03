set terminal postscript eps enhanced color 

set output "out/eps/LinearizedKDTrie/TestKDTrieThresholdHuge.eps"

set title "Comparing Threshold Values for Linearized KD Trie"

set xlabel "Threshold"
set ylabel "Time (s)"

plot "out/outfiles/LinearizedKDTrie/TestKDTrieThresholdHugeAvg.txt" using 1:($3+$5+$7) with linespoints title "Lin. KD Trie" lw 3

