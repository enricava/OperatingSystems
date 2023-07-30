#!/bin/bash

#ENRIQUE CAVANILLAS PUGA

SRC="./src"
TEMP="./temp"
MOUNT_POINT="./mount-point" 

#a --------------------------------------------------------------------

FILE1="myFS.h"
FILE2="fuseLib.c"

rm -r $TEMP
mkdir $TEMP

cp $SRC/$FILE1 $TEMP/$FILE1
echo "[OPERACION]: Copiado Archivo 1 a TEMP"
cp $SRC/$FILE1 $MOUNT_POINT/$FILE1
echo "[OPERACION]: Copiado Archivo 1 a MOUNT-POINT"

cp $SRC/$FILE2 $TEMP/$FILE2	
echo "[OPERACION]: Copiado Archivo 2 a TEMP"
cp $SRC/$FILE2 $MOUNT_POINT/$FILE2
echo "[OPERACION]: Copiado Archivo 2 a MOUNT-POINT"

#b --------------------------------------------------------------------
./my-fsck virtual-disk
diff -s -q $MOUNT_POINT/$FILE1 $SRC/$FILE1
diff -s -q $MOUNT_POINT/$FILE2 $SRC/$FILE2

#bloques de 4Kib -> 4096 bytes
truncate -o -s -1 $TEMP/$FILE1
truncate -o -s -1 $MOUNT_POINT/$FILE1

echo "[OPERACION]: Archivo 1 TRUNCADO -1 bloque en MOUNT_POINT y TEMP"

#c --------------------------------------------------------------------

./my-fsck virtual-disk

diff -s -q $SRC/$FILE1 $MOUNT_POINT/$FILE1
diff -s -q $TEMP/$FILE1 $MOUNT_POINT/$FILE1

#d --------------------------------------------------------------------

FILE3="MyFileSystem.c"
cp $SRC/$FILE3 $MOUNT_POINT/$FILE3
echo "Copiado Archivo 3 a MOUNT_POINT"

#e --------------------------------------------------------------------

./my-fsck virtual-disk

diff -s -q $SRC/$FILE3 $MOUNT_POINT/$FILE3

#f --------------------------------------------------------------------

truncate -o -s +1 $TEMP/$FILE2
truncate -o -s +1 $MOUNT_POINT/$FILE2

echo "[OPERACION]: Archivo 2 TRUNCADO +1 bloque en MOUNT_POINT y TEMP"

#g --------------------------------------------------------------------

./my-fsck virtual-disk

diff -s -q $SRC/$FILE2 $MOUNT_POINT/$FILE2
diff -s -q $TEMP/$FILE2 $MOUNT_POINT/$FILE2

