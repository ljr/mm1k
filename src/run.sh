#! /bin/bash

# MAX NUMBER OF REPLICAS IS 200! SEE SIZE OF seeds.txt
NUMBER_OF_REPLICAS=30

MEAN_ARRIVAL=40
MEAN_SERVICE=26
STEP_PERCENTAGE=0.5
WINDOW_SIZE=5000
N_REQUESTS=100000


function run {
	i=0
	for s in $(cat seeds.txt)
	do
		echo " >>>>>>>>>>>>>>> Running replica $i >>>>>>>>>>>>>>> "
		./mm1 $s $MEAN_ARRIVAL $MEAN_SERVICE $STEP_PERCENTAGE $WINDOW_SIZE $N_REQUESTS 2>out_$MEAN_ARRIVAL-$MEAN_SERVICE-r$i.csv
		i=$((i+1))
		if (($i == $NUMBER_OF_REPLICAS))
		then
			break
		fi
	done
}

function merge {
	of=out_$MEAN_ARRIVAL-$MEAN_SERVICE-consolidated.csv
	cut -d ';' -f 1,3 ./out_$MEAN_ARRIVAL-$MEAN_SERVICE-r0.csv > tmp-$of 
	for i in $(seq 1 $((NUMBER_OF_REPLICAS-1)))
	do
		echo -n $i...
		cp tmp-$of $of
		cut -d ';' -f 1,3 ./out_$MEAN_ARRIVAL-$MEAN_SERVICE-r$i.csv | join -t ';' $of - > tmp-$of 
	done
	cp tmp-$of $of
	rm tmp-$of
}



run
merge

