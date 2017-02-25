#include "http_connection.h"
#include "utils.h"
#include <vector>
#include <sstream>
#include <iostream>
using namespace rpt;
void http_connection::decode_header(const std::vector<char>& data){
	//std::cout<<"Data Received:"<<(char*)&data[0]<<std::endl;
	if(data.empty())
		return;
	if(_resrecvd == false){
		_response.clear();
	}
	if((_response.get_content_length() == 0)){
		_response.assign(data);
		if(_response.decode()){
			if(size_t l = _response.get_content_length()){
				clear_conditions();
				set_condition(datasize(l));
				notify_to(std::bind(&http_connection::decode_body,this,std::placeholders::_1));
			}
			else{
				if(_callback != nullptr){
					_callback(shared_from_this(),shared_from_this());
				}
				else if(_client_callback != nullptr){
					_client_callback(shared_from_this());
				}
				else{
					set_resrecvd();
				}
			}
		}
		else{
			std::cout<<"Error:"<<"Decode Failed"<<std::endl;
		}
	}
}

void http_connection::decode_body(const std::vector<char>& data){
	if(data.empty())
		return;
	_response.append(data);
	clear_conditions();
	set_condition(string_found("\r\n\r\n"));
	set_condition(string_found("\n\n"));
	notify_to(std::bind(&http_connection::decode_header,this,std::placeholders::_1));

	if(_callback != nullptr){
		_callback(shared_from_this(),shared_from_this());
	}
	else if(_client_callback != nullptr){
		_client_callback(shared_from_this());
	}
	else{
		set_resrecvd();
	}
}

//------------------------- http_server_encode---------------------------------
bool http_connection::write_to_stream(const void* data, size_t size){
	return buffered_client_iostream::write(data,size);
}
