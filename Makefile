CXX=g++
CXXFLAGS:=-std=c++14 -g -static-libstdc++ -static-libgcc
TCP_SERVER=../tcp_server
THREAD_POOL=../thread_pool
OBJS=http_server.o Test.o

Test.exe: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(TCP_SERVER)/libtcpserver.a $(THREAD_POOL)/libthreadpool.a  -lpthread -o Test.exe
http_server.o:
	$(CXX) $(CXXFLAGS) -I$(TCP_SERVER) -I$(THREAD_POOL) -c http_server.cpp
Test.o:
	$(CXX) $(CXXFLAGS) -I$(TCP_SERVER) -I$(THREAD_POOL) -c Test.cpp
clean:
	rm *.o
