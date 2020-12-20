set xlab "traffic intensity"
set ylab "avg no. of customers"
set grid
set terminal png size 800,600
set output './plot/Lplot.png'
plot "./log/plot_log/l_ref.txt" title "Theoretical" with linespoints, "./log/plot_log/l_hw1.txt" title "Simulation-from-scratch" with linespoints, "./log/plot_log/l.txt" title "SMPL simulation result" with linespoints


set xlab "traffic intensity"
set ylab "avg waiting time"
set grid
set terminal png size 800,600
set output './plot/Wplot.png'
plot "./log/plot_log/w_ref.txt" title "Theoretical" with linespoints, "./log/plot_log/w_hw1.txt" title "Simulation-from-scratch" with linespoints, "./log/plot_log/w.txt" title "SMPL simulation result" with linespoints

set xlab "traffic intensity"
set ylab "avg size of queue"
set grid
set terminal png size 800,600
set output './plot/Lqplot.png'
plot "./log/plot_log/lq_ref.txt" title "Theoretical" with linespoints, "./log/plot_log/lq_hw1.txt" title "Simulation-from-scratch" with linespoints, "./log/plot_log/lq.txt" title "SMPL simulation result" with linespoints
