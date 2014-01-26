#!/bin/bash
for i in `seq 1 2000`
do
curl -I http://localhost:9001/&
done
#sleep 10
