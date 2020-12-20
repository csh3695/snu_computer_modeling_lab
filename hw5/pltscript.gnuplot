sz = "big small"
md = "background deferrable polling sporadic"

do for [s in sz] {
    outfile = sprintf("./plot/%splot.png", s)
    data(a, b) = sprintf("./log/%s_%s.txt", a, b)
    if (s eq "big") { 
        set xlab "Inter arrival time(avg exec time = 10s)" 
    } else { 
        set xlab "Inter arrival time(avg exec time = 0.1s)" 
    }
    set ylab "Response time"
    set grid
    set terminal png size 800, 600
    set output outfile
    plot for [m in md] data(s, m) title m with linespoints
}