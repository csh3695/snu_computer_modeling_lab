set xlab "traffic intensity"
set ylab "avg no. of customers"
set grid
set terminal png size 800,600
set output './plot/Lplot.png'
plot "./log/plot_log/l.txt" title "Simulation result" with linespoints, "./log/plot_log/l_ref.txt" title "Theoretical" with linespoints

set xlab "traffic intensity"
set ylab "avg waiting time"
set grid
set terminal png size 800,600
set output './plot/Wplot.png'
plot "./log/plot_log/w.txt" title "Simulation result" with linespoints, "./log/plot_log/w_ref.txt" title "Theoretical" with linespoints

set xlab "traffic intensity"
set ylab "avg size of queue"
set grid
set terminal png size 800,600
set output './plot/Lqplot.png'
plot "./log/plot_log/lq.txt" title "Simulation result" with linespoints, "./log/plot_log/lq_ref.txt" title "Theoretical" with linespoints

set xlab "time"
set xrange [0:10001]
set ylab "traffic intensity"
set yrange [0:1.5]
set zlab "size of queue"
set grid
set terminal png size 1600,1200
set logscale z
set output './plot/time_load_qsize.png'
splot "./log/plot_log/time_load_qsize.txt" title "Time - Traffic intensity - Size of queue"
