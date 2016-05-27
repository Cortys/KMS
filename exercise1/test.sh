for mode in s p t
do
	echo "Mode $mode:";
	for i in {1..20}
	do
		echo "Test $i: \c";
		time ./aufg4 ${mode} $1 | grep -Eoh "real\s+[0-9]+m[0-9\.]+s";
	done
	echo "";
done
