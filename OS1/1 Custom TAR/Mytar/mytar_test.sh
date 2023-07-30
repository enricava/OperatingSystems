#!/bin/bash

if ! test -f mytar -a -x mytar ; then
	echo "No se ha compilado la practica"
	exit 1
fi


# Tmp
if test -e tmp ; then
	rm -r tmp
fi

mkdir ./tmp
cd tmp


echo "Hello world!" > file1.txt
head /etc/passwd > file2.txt
head -c 1024 /dev/urandom > file3.dat

../mytar -c -f filetar.mytar file1.txt file2.txt file3.dat 

mkdir out
cp filetar.mytar out/filetar.mytar
cd out

../../mytar -x -f ../filetar.mytar

if diff file1.txt ../file1.txt && diff file2.txt ../file2.txt && diff file3.dat ../file3.dat ; then
	cd ../..
	echo "Correcto"
	exit 0
else
	cd ../..
	echo "Some extracted file does not match its original version"
	exit 1
fi
