
set terminal dumb

set title "GoodWatch Current Consumption" font ",20"
set xlabel "Seconds"
set ylabel "Current"
#set yrange [0:10e-06]
set xrange [10:60]

plot "energytrace.txt";

