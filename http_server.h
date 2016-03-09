#ifndef __HTTP_SERVER_H__07032016_RAJEN__
#define __HTTP_SERVER_H__07032016_RAJEN__

#include "buffered_client_iostream.h"
#include "tcp_server.h"
#include "tcp_socket.h"
#include "thread_pool.h"
#include "acceptor_base.h"
#include "http_request.h"
#include "http_response.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <memory>
class http_server{
public:
	class http_client : public buffered_client_iostream{
		public:
			http_client() : buffered_client_iostream(std::make_shared<tcp_socket>()){
				set_condition(string_found("\n\n"));	
			}
		public:
			virtual void read(void* data, size_t size) override;
		private:
			bool set_request_header(const std::string& http_header_data);
			bool decode_request_method(std::string& http_header_data);
			bool decode_http_headers(std::string& http_header_data);
		private:
			http_request _request;
			http_response _response;
	};

	class http_accpetor : public acceptor_base{
		public:
			virtual std::shared_ptr<client_iostream> get_new_client() override{
				return std::make_shared<http_client>();
			}
			virtual void notify_accept(std::shared_ptr<client_iostream> client, acceptor_status_t status) override{
				_client_map[client->get_socket().get_fd()] = client;
				client->register_close_handler(std::bind(&http_accpetor::close,this,std::placeholders::_1));
			}
		public:
			void close(int fd){
				auto it = _client_map.find(fd);	
				if(it != _client_map.end()){
					_client_map.erase(it);
				}	
			}
		private:
			std::map<int,std::shared_ptr<client_iostream>> _client_map;	
	};


	class endpoint{
	public:
		endpoint(int port, const std::string& ip="", int backlog=5){
			this->port = port;
			this->ip = ip;
			this->backlog = backlog;
		}
	private:
		int port;
		std::string ip;
		int backlog;
	friend class http_server;
	};
public:
	http_server(std::shared_ptr<socket_base> socket, unsigned int max_thread) : 
		_tcp_server(socket, max_thread,_acceptor){
	}
	http_server(std::shared_ptr<socket_base> socket, std::shared_ptr<thread_pool> threads) : 
		_tcp_server(socket, threads, _acceptor){
	}
public:
	void start(const endpoint &e, bool block=true){
		_tcp_server.start(tcp_server::endpoint(e.port,e.ip,e.backlog), block);
	}	
private:
	tcp_server _tcp_server;
	http_accpetor _acceptor;
};
#endif
