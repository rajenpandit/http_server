CXX=g++
CXXFLAGS:=-std=c++14 -static-libstdc++ -static-libgcc -Wall -Wextra -Wpedantic -O3
TCP_SERVER=../tcp_server
THREAD_POOL=../thread_pool
OBJS=http_server.o http_packet.o Test.o

Test.exe: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(TCP_SERVER)/libtcpserver.a $(THREAD_POOL)/libthreadpool.a  -lpthread -o Test.exe
http_packet.o:
	$(CXX) $(CXXFLAGS) -I$(TCP_SERVER) -I$(THREAD_POOL) -c http_packet.cpp
http_server.o:
	$(CXX) $(CXXFLAGS) -I$(TCP_SERVER) -I$(THREAD_POOL) -c http_server.cpp
Test.o:
	$(CXX) $(CXXFLAGS) -I$(TCP_SERVER) -I$(THREAD_POOL) -c Test.cpp
clean:
	rm *.o
