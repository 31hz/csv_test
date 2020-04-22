# csv_test
Example of processing CSV files

# Installation
Target: Digital Ocean Centos 8.1 Droplet

yum install autoconf automake gcc-c++ git make boost-devel

git clone https://github.com/31hz/csv_test.git

cd csv_test

git checkout master

autoreconf -i

./configure

make

# Run
./csv_test --concurrency 3 --column 1 --aggregation_function SUM --csvglob='/tmp/*.csv'

