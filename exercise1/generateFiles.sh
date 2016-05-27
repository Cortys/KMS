#!/bin/bash

mkdir test

for i in {1..10}
do
	sizes="1 10 100 1000"

	if [[ ! $1 ]]; then
		arr=($sizes)
		k=$((i%4))

		sizes=${arr[k]}

		echo $k;
	fi

	for size in $sizes
	do
		filename="test/test-$size-$i";

		echo "Generating ${filename}...";
		rm ${filename};
		dd if=/dev/urandom of=${filename} bs=1k count=${size}k;
	done
done
