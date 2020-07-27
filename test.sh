rm -f ./a.out
rm -f ./testing.dht
rm -f /mnt/pmem/zwh_test/testing.dht
gcc -g test.c ./src/diskhash.c -lpmem -o a.out
./a.out
