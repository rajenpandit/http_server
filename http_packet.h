#ifndef __HTTP_PACKET_H__09032016_RAJEN__
#define __HTTP_PACKET_H__09032016_RAJEN__

#include <string>
#include <sstream>
#include <iostream>
#include <map>

class http_packet{
public:
	enum http_method{GET,POST,PUT,DELETE};
protected:
	http_packet() : _body(std::ios_base::in | std::ios_base::out | std::ios_base::binary){
	}
	void clear(){
		_headers.clear();
		_parameters.clear();
		_method.clear();
		_uri.clear();
		_version.clear();
		_query_string.clear();
		_body.clear();
	}
protected:
	std::map<std::string,std::string> _headers;	
	std::map<std::string,std::string> _parameters;	
	std::string _method;
	std::string _uri;
	std::string _query_string;
	std::string _version;
	std::stringstream _body;
public:
	friend class http_client;
	friend class http_server;
};

#endif
