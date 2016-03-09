#ifndef __HTTP_RESPONSE_H__09032016_RAJEN__
#define __HTTP_RESPONSE_H__09032016_RAJEN__

#include <string>
#include <map>
#include "http_packet.h"

class http_response : public http_packet{
public:
	void set_header(const std::string& name, const std::string& value){
		_headers[name] = value;
	}
	void set_parameter(const std::string& name, const std::string& value){
		_parameters[name] = value;
	}
	void set_content_type(const std::string& content_type){
		_headers[std::string("Content-Type")] = content_type;
	}
	void set_method(http_method method){
		switch(method){
			case GET:
				_method.assign("GET");break;
			case POST:
				_method.assign("POST");break;
			case PUT:
				_method.assign("PUT");break;
			case DELETE:
				_method.assign("DELETE");break;
		};
	}
	
	void set_headers(std::map<std::string,std::string> headers){
		_headers = headers;
	}
	void set_parameters(std::map<std::string,std::string> parameters){
		_parameters = parameters;
	}
};


#endif
