md = "0 1 2"

outfile = sprintf("./plot/plot.png")
data(id) = sprintf("./log/log%s.txt", id)
ttl(id) = sprintf("packetSink%s", id)
set xlab "time(s)"
set ylab "received bytes"
set grid
set terminal png size 800, 600
set output outfile
plot for [m in md] data(m) title ttl(m) with linespoints

md = "toRight maxToRight toLeft maxToLeft"

outfile = sprintf("./plot/plot_integral.png")
data(id) = sprintf("./log/log_integral_%s.txt", id)
ttl(id) = sprintf("%s", id)
set xlab "time(s)"
set ylab "received bytes: summed in 1s unit"
set grid
set terminal png size 800, 600
set output outfile
plot for [m in md] data(m) title ttl(m) with lines