cd ~/CM/hw6
rm log/*
cd ~/CM/ns-allinone-3.32/ns-3.32/scratch/ && waf --run hw6 > ~/CM/hw6/log/cmdlog.txt 2>&1
cd ~/CM/hw6
mv ~/CM/ns-allinone-3.32/ns-3.32/hw* ./log

python3 analyze.py
gnuplot pltscript.gnuplot