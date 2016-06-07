#include "http_server.h"
#include "utils.h"
#include <vector>
#include <sstream>
#include <iostream>

void http_server::http_client::notify(){
	while(is_data_available())
	{
		std::vector<char> data = get_data();
		if(data.empty())
			return;
		std::string http_packet_data(data.begin(),data.end());
		if(get_content_length() == 0){

			std::string http_header_data=http_packet_data;
			if(set_request_header(http_header_data)){
				if(size_t l = get_content_length()){
					clear_conditions();
					set_condition(datasize(l));
					continue;
				}
				else if(_request._method == "GET"){
					http_servlet* servlet = find_servlet(_request._uri);
					if(servlet == nullptr)
						servlet = find_servlet("/");
					if(servlet != nullptr)
						servlet->do_get(*this,*this);
					else
						std::cout<<"No Servlet found"<<std::endl;

				}
			}
		}
		else{
			_request._body.write(static_cast<const char*>(&data[0]),data.size());
			clear_conditions();
			set_condition(string_found("\r\n\r\n"));
			set_condition(string_found("\n\n"));
			http_servlet* servlet = find_servlet(_request._uri);
			if(servlet == nullptr)
				servlet = find_servlet("/");
			if(servlet != nullptr)
				servlet->do_post(*this,*this);
			else
				std::cout<<"No Servlet found"<<std::endl;
		}
	}
}

http_servlet* http_server::http_client::find_servlet(const std::string& request_uri)
{
	auto it = _servlets_map.find(request_uri);
	if( it == _servlets_map.end() ){
		it = _servlets_map.find(request_uri + "*");
		if( it == _servlets_map.end() ){
			it = _servlets_map.find(request_uri + "/*");
		}
	}
	if( it != _servlets_map.end() ){
		return it->second->get_new_instance().get();
	}
	return nullptr;
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
	if( (pos=http_header_data.find("HTTP/")) == std::string::npos) 
	{
		std::cout<<__LINE__<<"Returning false"<<std::endl;
		return false;
	}
	if( (pos = http_header_data.find("\n",pos)) == std::string::npos)
	{
		std::cout<<__LINE__<<"Returning false"<<std::endl;
		return false;
	}
	_request.clear();
	_response.clear();

	std::string request_header = http_header_data.substr(0, pos+1);
	http_header_data.erase(0,pos+1);
	pos = request_header.find("GET");
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
		std::cout<<__LINE__<<"Returning false"<<std::endl;
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
		else{
			_request._uri = uri;
		}
	}
	else{
		std::cout<<__LINE__<<"Returning false"<<std::endl;
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
	return true;
}




//------------------------- http_server_encode---------------------------------
bool http_server::http_client::write(const void* data, size_t size){
	std::stringstream ss;
	if(_responseCode.empty() && !_response._method.empty()){
		ss << _response._method << " ";
		ss << _response._uri << " ";
		ss << "HTTP/1.1";
	}
	else{
		if(_responseCode.empty())
			_responseCode="200 OK";
		ss << "HTTP/1.1 ";
		ss << _responseCode;
	}
	ss << "\r\n";
	if(_response._headers.find("Connection") == _response._headers.end())
	{
		set_header("Connection","Keep-Alive");
	}
	for( auto &it : _response._headers){
		ss<< it.first << ": ";	
		ss<< it.second<<"\r\n";
	}		

	if(size > 0)
	{
		ss<< "Content-Length: "<< size << "\r\n";
	}
	ss<<"\r\n";
	ss<<static_cast<const char*>(data);		
	_request.clear();
	_response.clear();
	return buffered_client_iostream::write(ss.str().c_str(),ss.str().size());
}
