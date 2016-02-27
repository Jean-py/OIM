#!/bin/bash

BASE=./
TEST=./test_files

function test_localmean {
	echo "testing " $1 " filter sizes for local mean filtering ..."
	if [ ! -d $TEST/localmean ];then
		mkdir $TEST/localmean
	fi
	rm -rf $TEST/localmean/*
	touch $TEST/localmean/timings.txt
	cat $TEST/file_list.txt | while read thefile
	do
		for ((filtersize=1; filtersize<=$1; filtersize++)); do
			fbname=$(basename "$thefile" .pgm);
			if [ ! -e $TEST/localmean/$fbname-lmf$(($filtersize * 2 + 1)).pgm ]; then
				./imagefilter 0 $TEST/noisy/$fbname.pgm $TEST/localmean/$fbname-lmf$(($filtersize * 2 + 1)).pgm  $(($filtersize * 2 + 1)) >> $TEST/localmean/timings.txt		
			fi
		done;
	done;
}

function test_nonlocalmean {
	echo "testing " $1 " filter sizes for non local mean filtering ..."
	if [ ! -d $TEST/nonlocalmean ];then
		mkdir $TEST/nonlocalmean
	fi
	#rm -rf $TEST/nonlocalmean/*
	touch $TEST/nonlocalmean/timings.txt
	cat $TEST/file_list.txt | while read thefile
	do
		for ((filtersize=1; filtersize<=$1; filtersize++)); do
			fbname=$(basename "$thefile" .pgm);
			if [ ! -e $TEST/nonlocalmean/$fbname-nlmf$(($filtersize * 2 + 1)).pgm ]; then
				./imagefilter 1 $TEST/noisy/$fbname.pgm $TEST/nonlocalmean/$fbname-nlmf$(($filtersize * 2 + 1)).pgm  $(($filtersize * 2 + 1)) >> $TEST/nonlocalmean/timings.txt		
			fi
		done;
	done;

}


function test_distance {
	echo "computing distances according to " $1 " texture sizes ..."
	rm -f $TEST/comparison.txt
	touch $TEST/comparison.txt
	cat $TEST/file_list.txt | while read thereference
	# distances for mean filter
	do
		echo LOCAL MEAN FILTER ---
		for ((filtersize=1; filtersize<=$1; filtersize++));
		do
			fbname=$(basename "$thereference" .pgm);
			if [ -e $TEST/localmean/$fbname-lmf$(($filtersize * 2 + 1)).pgm ]; then
				./imagefilter 2 $TEST/references/$thereference $TEST/localmean/$fbname-lmf$(($filtersize * 2 + 1)).pgm $(($filtersize * 2 + 1)) >> $TEST/comparison.txt
			fi
		done
		echo NON LOCAL MEAN FILTER ---
		for ((filtersize=1; filtersize<=$1; filtersize++));
		do
			fbname=$(basename "$thereference" .pgm);
			if [ -e $TEST/nonlocalmean/$fbname-nlmf$(($filtersize * 2 + 1)).pgm ]; then
				./imagefilter 2 $TEST/references/$thereference $TEST/nonlocalmean/$fbname-nlmf$(($filtersize * 2 + 1)).pgm $(($filtersize * 2 + 1)) >> $TEST/comparison.txt
			fi
		done
	done;

}

function test_all {

test_localmean $1
test_nonlocalmean $1
test_distance $1
}

function test {
	test_$1 $2
}

test $1 15

exit 0
