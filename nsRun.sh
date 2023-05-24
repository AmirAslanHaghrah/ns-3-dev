#!/bin/bash
./waf
for seed in 6557 6688 8481 4198 2697 8559 1065 4754 2587 5501
do
    for speed in 80 70 60 50 40 30 20 10
    do
        # for type in 4 #3 2 1 0
        # do
        #     echo "seed: $seed, speed: $speed, type: $type"
        #     ./waf --run "fuzzyH $seed $type $speed"                      
        # done

        echo "seed: $seed, speed: $speed"
        ./waf --run "fuzzyH $seed 0 $speed" &
        ./waf --run "fuzzyH $seed 1 $speed" &         
        ./waf --run "fuzzyH $seed 2 $speed" &           
        ./waf --run "fuzzyH $seed 3 $speed" &           
        ./waf --run "fuzzyH $seed 4 $speed" & 
        
        wait
    done
done


