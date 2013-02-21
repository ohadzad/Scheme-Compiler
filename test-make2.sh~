echo "called with"  $1  $2  $3 >> 1.log
rm -f main.scm
cp $1 ./main.scm
make
rm -f main.log
./main >> main.log
rm -f $3
diff main.log $2 >> $3
