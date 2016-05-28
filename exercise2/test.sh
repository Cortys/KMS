#! /bin/bash

TMPFILE=`mktemp` || exit 1

trap 'rm -f $TMPFILE' INT TERM EXIT

./producer 2> $TMPFILE &

PRODUCERID=$!

trap 'rm -f $TMPFILE; kill $PRODUCERID' INT TERM EXIT

echo "Waiting for producer to create shared buffer..."

SHMID=""
i=0

while [[ -z $SHMID && $i -lt 100 ]]; do
	SHMID=`cat $TMPFILE`
	i=`expr $i + 1`
	sleep 0.1s
done

if [[ -z $SHMID ]]; then
	exit 1
fi

./consumer -p $PRODUCERID -s $SHMID &

CONSUMERID=$!

trap 'rm -f $TMPFILE; kill $PRODUCERID; kill $CONSUMERID' INT TERM EXIT

read input
