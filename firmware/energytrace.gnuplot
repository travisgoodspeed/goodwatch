
set terminal png
set output "energytrace.png"

set title "GoodWatch Current Consumption" font ",20"
set xlabel "Seconds"
set ylabel "Current"
set yrange [0:10e-06]

plot "energytrace.txt" with lines;

