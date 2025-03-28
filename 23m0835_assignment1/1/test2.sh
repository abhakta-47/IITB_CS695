#!/bin/bash

# Function to spawn child processes
spawn_children() {
    local num_children=$1
    for i in $(seq 1 $num_children); do
        (sleep 1000) &
    done
}

# Number of child processes to spawn
NUM_CHILDREN=5

echo "Spawning $NUM_CHILDREN child processes..."
spawn_children $NUM_CHILDREN

# Print the parent PID and child PIDs
PARENT_PID=$$
echo "Parent PID: $PARENT_PID"
echo "Child PIDs:"
pgrep -P $PARENT_PID

# Keep the script running to keep the child processes alive
echo "Press [CTRL+C] to stop..."
wait