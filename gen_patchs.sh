#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <file>"
    exit 1
fi

if [ ! -f "$1" ]; then
    echo "File not found"
    exit 1
fi

echo Extracting patches from $1

sectors=(170 175 196)
for i in "${sectors[@]}"; do
    echo dd if="$1" of="patch_$i.bin" bs=512 skip="$i" count=1
    dd if="$1" of="patch_$i.bin" bs=512 skip="$i" count=1
done
