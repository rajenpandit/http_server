#ifndef __HTTP_RESPONSE_H__09032016_RAJEN__
#define __HTTP_RESPONSE_H__09032016_RAJEN__

#include <string>
#include <map>
#include "http_packet.h"

class http_response {
public:
	void set_header(const std::string& name, const std::string& value){
		_response._headers[name] = value;
	}
	void set_parameter(const std::string& name, const std::string& value){
		_response._parameters[name] = value;
	}
	void set_content_type(const std::string& content_type){
		_response._headers[std::string("Content-Type")] = content_type;
	}
#if 0
	void set_method(http_packet::http_method method){
		switch(method){
			case http_packet::GET:
				_response._method.assign("GET");break;
			case http_packet::POST:
				_response._method.assign("POST");break;
			case http_packet::PUT:
				_response._method.assign("PUT");break;
			case http_packet::DELETE:
				_response._method.assign("DELETE");break;
			default:
				_response._method.assign("POST");
		};
	}
#endif
	void set_headers(std::map<std::string,std::string> headers){
		_response._headers = headers;
	}
	void set_parameters(std::map<std::string,std::string> parameters){
		_response._parameters = parameters;
	}
	void set_responseCode(const std::string& responseCode){
		_responseCode = responseCode;
	}
	virtual bool write(const void* data, size_t size) = 0;
protected:
	http_packet _response;
	std::string _responseCode;
};


#endif
