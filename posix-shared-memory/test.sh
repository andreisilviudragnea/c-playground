#!/usr/bin/env bash

./test_writer < /etc/services &
sleep 1
./test_reader > out.txt
diff out.txt /etc/services
if [ $? -eq 0 ]; then
    echo "Success"
fi
rm out.txt
