#include "http_server.h"
#include "tcp_socket.h"
#include "http_servlet.h"
#include "tcp_socket_factory.h"
#include <iostream>
#include <memory>
#include <ctime>
#include <sys/time.h>

std::string get_timestamp(){
        struct timeval time_val;
        if(gettimeofday(&time_val,NULL)==0){
                char time_stamp[50];
                struct tm tm_time;
                localtime_r(&time_val.tv_sec, &tm_time);
                strftime(time_stamp,sizeof(time_stamp),"%T",&tm_time);
                std::string s_time_stamp(time_stamp);
                s_time_stamp.append(":"+std::to_string(time_val.tv_usec/1000));
                return s_time_stamp;
        }
        return "";
}
class servlet : public http_servlet{
public:
	virtual void do_get(http_request& request, http_response& response) override{
		std::cout<<get_timestamp()<< " | Request: [URI:"<<request.get_path_info()<<"] "<<request.get_body().str()<<std::endl;
		std::string resp_data("<Response><ResponseCode>Success</ResponseCode></Response>");
		std::cout<<get_timestamp()<<" | Response: "<<resp_data<<std::endl;
		response.write(resp_data.c_str(),resp_data.length());
	}
	virtual void do_post(http_request& request, http_response& response) override{
		do_get(request,response);
	}
	virtual std::shared_ptr<http_servlet> get_new_instance() override{
		return std::make_shared<servlet>();
	}
};

int main(int argc, char* argv[]){
	int port=1234;
	if(argc > 1)
		port = std::stoi(argv[1]);
	tcp_socket_factory sf;
	http_server s(sf, 10);
	s.register_servlet("/",std::make_shared<servlet>());
	s.start(http_server::endpoint(port));
}
