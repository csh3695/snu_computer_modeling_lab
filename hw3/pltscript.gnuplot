set xlab "lambda"
set ylab "Utilization"
set grid
set terminal png size 800,600
set output "./plot/U.png"
theo_res(n) = sprintf("./log/plot_log/theo_res/U%d.txt", n)
theo_title(n) = sprintf("U%d(theoretical)", n+1)
sim_res(n) = sprintf("./log/plot_log/sim_res/U%d.txt", n)
sim_title(n) = sprintf("U%d(simulation)", n+1)
plot for[i = 0:3] theo_res(i) title theo_title(i) with linespoints, for[i = 0:3] sim_res(i) title sim_title(i) with linespoints


set xlab "lambda"
set ylab "L"
set grid
set logscale y
set terminal png size 800,600
set output './plot/Lplot.png'
plot "./log/plot_log/theo_res/L.txt" title "Theoretical" with linespoints, "./log/plot_log/sim_res/L.txt" title "Simulation" with linespoints

set xlab "lambda"
set ylab "W"
set grid
set terminal png size 800,600
set output './plot/Wplot.png'
plot "./log/plot_log/theo_res/W.txt" title "Theoretical" with linespoints, "./log/plot_log/sim_res/W.txt" title "Simulation" with linespoints


do for [t=0:3] {
    outfile = sprintf('./plot/W%dplot.png',t)
    theo_res = sprintf('./log/plot_log/theo_res/W%d.txt', t)
    sim_res = sprintf('./log/plot_log/sim_res/W%d.txt', t)
    set xlab "lambda"
    set ylab sprintf("W%d", t+1)
    set grid
    set terminal png size 800,600
    set output outfile
    plot theo_res title "Theoretical" with linespoints, sim_res title "Simulation" with linespoints
}


do for [t=0:3] {
    outfile = sprintf('./plot/L%dplot.png',t)
    theo_res = sprintf('./log/plot_log/theo_res/L%d.txt', t)
    sim_res = sprintf('./log/plot_log/sim_res/L%d.txt', t)
    set xlab "lambda"
    set ylab sprintf("L%d", t+1)
    set grid
    set terminal png size 800,600
    set output outfile
    plot theo_res title "Theoretical" with linespoints, sim_res title "Simulation" with linespoints
}
