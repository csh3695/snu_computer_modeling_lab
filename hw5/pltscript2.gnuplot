md = "polling sporadic"

outfile = sprintf("./plot/scale_plot.png")
data(b) = sprintf("./log2/%s.txt", b)
set xlab "Execution time (avg inter-arrival time = 100x)" 
set ylab "Response time"
set grid
set terminal png size 800, 600
set output outfile
plot for [m in md] data(m) title m with lines
