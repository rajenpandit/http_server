#include "http_server.h"
#include "utils.h"
void http_server::http_client::read(void* data, size_t size){
	if(_request.get_content_length() == 0){
		std::string http_header_data(static_cast<char*>(data));
		if(set_request_header(http_header_data)){
			if(size_t l = _request.get_content_length())
				set_condition(datasize(l));
		}
	}
	else{
		_request._body.write(static_cast<char*>(data),size);
		set_condition(string_found("\n\n"));
	}
}

bool http_server::http_client::set_request_header(const std::string& http_header_data){
	std::string header_data = http_header_data;
	if(decode_request_method(header_data)){
		return decode_http_headers(header_data);
	}
	return false;
}
bool http_server::http_client::decode_request_method(std::string& http_header_data){
	std::string::size_type pos;
	if( (pos = http_header_data.find("\n")) == std::string::npos || (http_header_data.find("HTTP/") == std::string::npos) )
		return false;

	std::string request_header = http_header_data.substr(0, pos+1);
	http_header_data.erase(0,pos+1);
	pos = request_header.find("GET");
	_request.clear();
	if( pos != std::string::npos )
		_request._method.assign("GET");
	else if((pos = request_header.find("POST")) != std::string::npos){
		_request._method.assign("POST");
	}
	else if((pos = request_header.find("PUT")) != std::string::npos){
		_request._method.assign("PUT");
	}
	else if((pos = request_header.find("DELETE")) != std::string::npos){
		_request._method.assign("DELETE");
	}
	if( _request._method.empty()){
		return false;
	}
	request_header.erase(0,pos + _request._method.length());
	/* --------------Getting URI ----------------*/
	pos = request_header.find_first_not_of(' '); //points to URI
	if(pos != std::string::npos)
		request_header.erase(0,pos);	
	pos = request_header.find(" ");
	if(pos != std::string::npos){
		std::string uri	= request_header.substr(0,pos);
		request_header.erase(0,pos);
		// URI may contain domain
		if((pos = uri.find("://")) != std::string::npos){
			if((pos = uri.find("/",pos+sizeof("://")-1)) != std::string::npos){
				_request._headers["Host"] = uri.substr(0,pos);
				_request._uri = uri.substr(pos);
			}
			else if((pos = uri.find(" ",pos+1)) != std::string::npos){
				_request._headers["Host"] = uri.substr(0,pos);
				_request._uri.assign("/");
			}
		}
	}
	else{
		return false;
	}
	/*--------------Getting HTTP Version---------*/

	if((pos = request_header.find("HTTP/")) != std::string::npos){ //points to HTTP Version
		_request._version = request_header.substr(pos,sizeof("HTTP/1.1")-1);
	}
	
	return true;
}
bool http_server::http_client::decode_http_headers(std::string& header_data){
	std::string::size_type pos;
	while(true){
		std::string header;
		if((pos = header_data.find("\r\n")) != std::string::npos){
			header = header_data.substr(0,pos);
			header_data.erase(0,pos+2);
		}
		else if((pos = header_data.find("\n")) != std::string::npos){
			header = header_data.substr(0,pos);
			header_data.erase(0,pos+1);
		}
		else{
			break;
		}
		auto pair = utils::get_name_value_pair(header,":");	
		if(!pair.first.empty()){
			_request._headers[utils::trim(pair.first)] = utils::trim(pair.second);	
		}
	}
	for( auto it = _request._headers.begin() ; it != _request._headers.end() ; ++it){
		std::cout << it->first << " : " << it->second << '\n';
	}
	return true;
}
