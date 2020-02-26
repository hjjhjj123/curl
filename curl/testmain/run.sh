rm -rf main
#g++ -o main main.cpp -L./lib -lcurl -L./ -lcurlInterface -I./include
g++ -o main *.cpp -L./lib -lcurl -I./include
