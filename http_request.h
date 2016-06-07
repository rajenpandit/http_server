#ifndef __HTTP_REQUEST_H__09032016_RAJEN_H__
#define __HTTP_REQUEST_H__09032016_RAJEN_H__

#include "http_packet.h"
#include <string>
#include <map>

class http_request{
public:
	std::string get_header(const std::string& name){
		auto it = _request._headers.find(name);
		if(it != _request._headers.end())
			return it->second;
		return "";
	}
	std::string get_parameter(const std::string& name){
		auto it = _request._parameters.find(name);
		if(it != _request._parameters.end())
			return it->second;
		return "";

	}
	std::string get_content_type(){
		return get_header("Content-Type");
	}
	int get_content_length(){
		const std::string& content_length = get_header("Content-Length");	
		if(!content_length.empty()){
			return std::stoi(content_length);
		}
		return 0;
	}
	std::string get_context_path(){
		return "";
	}
	std::string get_method(){
		return _request._method; 
	}
	/* Returns path after domain name (ex: http://example.com/path/src?name=xyz&data=1234) 	*
	 *							  ^^^^^^^^  			*/
	std::string get_path_info(){
		return _request._uri;
	}
	/* Returns query string that is contained in the request URL after path */
	std::string get_query_string(){
		return "";
	}
	/* Returns IP Address */
	std::string get_remote_addr(){
		return "";
	}
	std::string get_remote_host(){
		return "";
	}
	int get_server_port(){
	}
	std::map<std::string,std::string> get_headers(){
		return _request._headers;
	}
	std::map<std::string,std::string> get_parameters(){
		return _request._parameters;
	}
	std::stringstream& get_body(){
		return _request._body;
	}
protected:
	http_packet _request;
};

class http_servlet_request{
public:
	std::string get_header(const std::string& name){
		return _request->get_header(name);
	}
	std::string get_parameter(const std::string& name){
		return _request->get_parameter(name);
	}
	std::string get_content_type(){
		return _request->get_content_type();
	}
	int get_content_length(){
		_request->get_content_length();
	}
	std::string get_context_path(){
		return _request->get_context_path();
	}
	std::string get_method(){
		return _request->get_method(); 
	}
	std::string get_path_info(){
		return _request->get_path_info();
	}
	std::string get_query_string(){
		return _request->get_query_string();
	}
	std::string get_remote_addr(){
		return _request->get_remote_addr();
	}
	std::string get_remote_host(){
		return _request->get_remote_host();
	}
	int get_server_port(){
		return _request->get_server_port();
	}
	std::map<std::string,std::string> get_headers(){
		return _request->get_headers();
	}
	std::map<std::string,std::string> get_parameters(){
		return _request->get_parameters();
	}
	std::stringstream& get_body(){
		return _request->get_body();
	}
private:
	std::shared_ptr<http_request> _request;

};

#endif
