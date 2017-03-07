# linkedon-push

 author Brave Han

 TODO pusher server
 
# EVN

 ubuntu or centos6 up
 cmake gcc g++

 OS use mongodb and redis

# Build

cd linkedon-push

chmod +x auto/*

./auto/depends.sh

mkdir -p build

cd build

cmake ../

make -j4

build done

# Run

workspace $ProjectDir

./lpush



# MAC Build before
mkdir xcode direct
cmake -G "Xcode"  current project root direct
