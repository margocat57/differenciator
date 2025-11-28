set xlabel "x"
set ylabel "f(x)"
set title "График функции sin(x) + 1/10*sin(10x)"
set grid
plot sin(x) + 0.1*sin(10*x) with lines title "sin(x) + 0.1*sin(10x)"