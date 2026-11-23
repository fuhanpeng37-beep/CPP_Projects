.PHONY:all
all:main

main:main.cc
	g++ -o $@ $^ -std=c++11

# ws:wsserver.cpp
# 	g++ -o $@ $^ -std=c++11 -pthread -lboost_system

# mysql:mysql.cpp
# 	g++ -o $@ $^ -std=c++11 -L/usr/lib64/mysql -lmysqlclient

.PHONY:clean
clean:
	rm -r main

