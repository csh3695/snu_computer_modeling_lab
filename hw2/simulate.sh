rm ./log/*
make
./a.out >> ./log/log.txt

python3 analyze.py