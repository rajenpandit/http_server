#ifndef __HTTP_REQUEST_H__09032016_RAJEN_H__
#define __HTTP_REQUEST_H__09032016_RAJEN_H__

#include "http_packet.h"
#include <string>
#include <map>

class http_request : public http_packet{
public:
	std::string get_header(const std::string& name){
		auto it = _headers.find(name);
		if(it != _headers.end())
			return it->second;
		return "";
	}
	std::string get_parameter(const std::string& name){
		auto it = _parameters.find(name);
		if(it != _parameters.end())
			return it->second;
		return "";

	}
	std::string get_content_type(){
		return get_header("Content-Type");
	}
	int get_content_length(){
		const std::string& content_length = get_header("Content-Lenght");	
		if(!content_length.empty()){
			return std::stoi(content_length);
		}
		return 0;
	}
	std::string get_context_path(){
		return "";
	}
	std::string get_method(){
		return _method; 
	}
	/* Returns path after domain name (ex: http://example.com/path/src?name=xyz&data=1234) 	*
	 *							  ^^^^^^^^  			*/
	std::string get_path_info(){
		return _uri;
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
		return _headers;
	}
	std::map<std::string,std::string> get_parameters(){
		return _parameters;
	}
};



#endif
