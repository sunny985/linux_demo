#!/bin/sh
if [ -e "fifo_test" ]
then
	rm fifo_test
fi
gcc fifo.c -o fifo
chmod 777 fifo
./fifo
# echo will be blocked until read comes
echo "write is blocked..."
echo "hello world" > fifo_test &
sleep 5
cat fifo_test
echo "read comes, write retruns"

echo ""
sleep 2

# cat will be blocked until write comes
echo "read is blocked..."
cat fifo_test &
sleep 5
echo "hello world" > fifo_test
echo "write comes, read retruns"