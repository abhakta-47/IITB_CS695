#!/bin/bash

# Complete this script to deploy external-service and counter-service in two separate containers
# You will be using the conductor tool that you completed in task 3.

source setup.sh

# Creating link to the tool within this directory
ln -s ../task3/conductor.sh conductor.sh
ln -s ../task3/setup.sh setup.sh

# Function to clean up background processes on exit
cleanup() {
    echo -e "\e[31mStopping background processes...\e[0m"
    ./conductor.sh stop es-con 2> /dev/null # 2>&1
    ./conductor.sh stop cs-con 2> /dev/null # 2>&1
    exit 0
}
trap cleanup SIGINT

# 1. Build images for the containers
echo -e "\e[34mBuilding cs-image\e[0m"
./conductor.sh build cs-image csfile
echo -e "\e[32mBuilt cs-image\e[0m"

echo -e "\e[34mBuilding es-image\e[0m"
./conductor.sh build es-image esfile
echo -e "\e[32mBuilt es-image\e[0m"

# 2. Run two containers say es-cont and cs-cont which should run in background. 
echo -e "\e[34mStarting cs-con\e[0m"
./conductor.sh run cs-image cs-con 'tail -f /dev/null' > /dev/null 2>&1 &

echo -e "\e[34mStarting es-con\e[0m"
./conductor.sh run es-image es-con 'tail -f /dev/null' > /dev/null 2>&1 &

sleep 0.5
for i in {5..1}; do
    echo -ne "\e[33mWaiting for containers to be up .... $i\e[0m\r"
    sleep 1
done
echo -ne "\e[0m\r"

# 3. Configure network

echo -e "\e[34mConfiguring network for es-con\e[0m"
./conductor.sh addnetwork -i -e 8080-3000 es-con
echo -e "\e[32mNetwork configured for es-con\e[0m"

echo -e "\e[34mConfiguring network for cs-con\e[0m"
./conductor.sh addnetwork -i cs-con
echo -e "\e[32mNetwork configured for cs-con\e[0m"

echo -e "\e[34mSetting up peer network between cs-con and es-con\e[0m"
./conductor.sh peer cs-con es-con
echo -e "\e[32mPeer network setup complete\e[0m"

# 5. Get IP address of cs-con
echo -e "\e[34mFetching IP address of cs-con\e[0m"
CS_CON_IP=$(./conductor.sh exec cs-con -- ip -4 addr show | grep "inet ${IP4_SUBNET}" | awk '{print $2}' | cut -d'/' -f 1 | head -n 1)
echo -e "\e[32mCS Container IP: $CS_CON_IP\e[0m"

# 6. Start counter service
echo -e "\e[34mStarting counter service in cs-con\e[0m"
./conductor.sh exec cs-con -- app/counter-service 8080 1 &

# 7. Start external service
echo -e "\e[34mStarting external service in es-con\e[0m"
./conductor.sh exec es-con -- python3 app/app.py http://"$CS_CON_IP":8080 &

sleep 0.5
for i in {5..1}; do
    echo -ne "\e[33mWaiting for processes to be up .... $i\e[0m\r"
    sleep 1
done
echo -ne "\e[0m\r"

HOST_IP=$(ip -4 addr show ${DEFAULT_IFC} | grep "inet " | awk '{print $2}' | cut -d'/' -f 1 | head -n 1)
# 8. Verify service on localhost
echo -e "\e[34mVerifying service on $HOST_IP:3000\e[0m"
for i in {5..1}; do
    curl http://$HOST_IP:3000
    echo 
    sleep 1
done
echo -e "\e[32mVerification complete\e[0m"

# 9. Provide instruction for external verification
echo -e "\e[34mTo verify from another system, use: curl http://$HOST_IP:3000\e[0m"

for i in {15..1}; do
    echo -ne "\e[31mClosing in $i seconds...\e[0m\r"
    sleep 1
done
echo -ne "\e[0m\r"

cleanup