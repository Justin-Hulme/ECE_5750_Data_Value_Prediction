#!/bin/bash

resdir=$1
trdir=traces

mkdir -p ${resdir}

for i in `ls ${trdir}`
do
    ./cvp -v ${trdir}/$i > ${resdir}/$i.txt
done

