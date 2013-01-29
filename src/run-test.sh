#! /bin/bash

# MAX NUMBER OF REPLICAS IS 200! SEE SIZE OF seeds.txt
NUMBER_OF_REPLICAS=250


PROG=./test
RESULTS=results

MEAN_ARRIVAL=$4
MEAN_SERVICE=$2

mkdir $RESULTS -p


function run {
	mapfile -t s1 < seeds-test-stream1.txt
	mapfile -t s2 < seeds-test-stream2.txt
	for i in $(seq 0 $((NUMBER_OF_REPLICAS-1)))
	do
		echo -e "\n\n\t >>>>>>>>>>>>>>> Running replica $i >>>>>>>>>>>>>>> \n\n\n"
		#echo ${s1[$i]}
		$PROG ${s1[$i]} ${s2[$i]} $1 $2 $3 $4 $5 $6 $7 2>./$RESULTS/out_$MEAN_ARRIVAL-$MEAN_SERVICE-r$i.csv
	done
}

function merge {
	of=$RESULTS/out_consolidated.csv
	cut -d ',' -f 1,3 $RESULTS/out_$MEAN_ARRIVAL-$MEAN_SERVICE-r0.csv > $of-tmp 
	for i in $(seq 1 $((NUMBER_OF_REPLICAS-2)))
	do
		echo -n $i...
		cp $of-tmp $of
		cut -d ',' -f 1,3 $RESULTS/out_$MEAN_ARRIVAL-$MEAN_SERVICE-r$i.csv | join -t ',' $of - > $of-tmp
	done
	cp $of-tmp $of
	rm $of-tmp
	
	octave -q run_mean.m
	cut -d ',' -f 1,2 $RESULTS/out_$MEAN_ARRIVAL-$MEAN_SERVICE-r0.csv | join -t ',' $RESULTS/out-mean.csv - > t.oct 

}



run $@
merge

