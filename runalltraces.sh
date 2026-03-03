#!/bin/bash


# Results Directory: change this line to your need
resdir=allzero



# This is where traces are

trdir=traces


mkdir ${resdir}
for i in `ls ${trdir}`
do
	./cvp -v ${trdir}/$i > ${resdir}/$i.txt
done

