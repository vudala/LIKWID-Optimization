#!/usr/bin/gnuplot -c
set grid
set style data point
set style function line
set style line 1 lc 3 pt 7 ps 0.3
set boxwidth 1
set xtics
set xrange ["0":]
set xlabel  "N (bytes)"

#
# ALTERNATIVA 1: Tabelas em arquivos separados (2 colunas)
#
set ylabel  "<metrica 1>"
set title   "<campo[marcador 1]>"
set terminal qt 0 title "<campo[marcador 1]>"
plot 'plot_exemplo-01.dat' title "<sem otimização>" with linespoints, \
     'plot_exemplo-02.dat' title "<com otimização>" with linespoints

#
# ALTERNATIVA 2: Tabela com 3 colunas 
#
set ylabel  "<metrica 1>"
set title   "<campo[marcador 1]>"
set terminal qt 1 title "<campo[marcador 1]>"
plot 'plot_exemplo-03.dat' using 1:2 title "<sem otimização>" with linespoints, \
     '' using 1:3 title "<com otimização>" with linespoints

pause mouse

# Gerando figura PNG
set terminal png
set output "funcao__NxMetrica.png"
plot 'plot_exemplo-03.dat' using 1:2 title "<sem otimização>" with linespoints, \
     '' using 1:3 title "<com otimização>" with linespoints
replot
unset output

