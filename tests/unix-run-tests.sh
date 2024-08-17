#!/bin/bash

mkdir build
cd build
cmake ..
make

for file in ./*; do

    if [[ -x "$file" && ! -d "$file" ]]; then
        echo "Running $file..."
        ./"$file"
    fi

done

