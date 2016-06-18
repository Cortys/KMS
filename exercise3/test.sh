#! /bin/bash

BRed='\e[1;31m'
Gre='\e[0;32m'
RCol='\e[0m'

results='results.txt'
vmstatResults='vmstatResults.txt'

sizes=("-r 10000 -c 10000" "-r 100 -c 1000000")
inits=("-i r" "-i c")
sums=("-s n" "-s r" "-s c")

time=`which time`

i=1

echo "vmstat output:" > $vmstatResults
vmstat -n 1 >> $vmstatResults &
vmstatId=$!
trap 'kill $vmstatId; exit' INT TERM EXIT

echo "Test results:" > $results

for size in "${sizes[@]}"; do
	for init in "${inits[@]}"; do
		for sum in "${sums[@]}"; do
			msg="$size $init $sum:"

			echo -e "${BRed}Run $i: $Gre$msg$RCol"
			echo "Run $i: $msg" >> $results

			echo "Run $i: $msg" >> $vmstatResults

			$time -v ./aufg5 $size $init $sum 2>&1 1>/dev/null | grep -E "User|System|Elapsed|Major|Minor" | tee -a $results
			echo "End of run $i." >> $vmstatResults
			i=`expr $i + 1`
			sleep 2s
		done
	done
done
