#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

OUTPUT_DIR="output"
JGRAPH_BIN="./lib/jgraph/jgraph/jgraph"

make

mkdir -p $OUTPUT_DIR

echo "Generating audio graphs..."
FILES=$(find audio -type f -name '*.wav' | shuf | head -n 5)

counter=1

for file in $FILES; do
    echo "Processing $file for time domain graph..."

    ./bin/tonetrace -t "$file"
    
    $JGRAPH_BIN -P src/time_domain.jgr | ps2pdf - | convert -density 300 - -quality 100 "$OUTPUT_DIR/time_domain_$counter.jpg"

    ((counter++))
done

counter=1

for file in $FILES; do
    echo "Processing $file for frequency domain graph..."

    # Generate a random time value
    time_value=$(awk -v min=1 -v max=10 'BEGIN{srand(); print min+rand()*(max-min)}')
    
    ./bin/tonetrace -f "$file" $time_value
    
    $JGRAPH_BIN -P src/frequency_domain.jgr | ps2pdf - | convert -density 300 - -quality 100 "$OUTPUT_DIR/frequency_domain_$counter.jpg"
    
    ((counter++))
done