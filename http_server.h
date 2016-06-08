#ifndef __HTTP_SERVER_H__07032016_RAJEN__
#define __HTTP_SERVER_H__07032016_RAJEN__

#include "buffered_client_iostream.h"
#include "tcp_connection.h"
#include "socket_factory.h"
#include "tcp_socket.h"
#include "thread_pool.h"
#include "acceptor_base.h"
#include "http_request.h"
#include "http_response.h"
#include "http_servlet.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <memory>
#include <mutex>
class http_server{
public:
	class http_client : public buffered_client_iostream, 
	public virtual http_request, public virtual http_response{
		public:
			http_client(std::map<std::string,std::shared_ptr<http_servlet> >& servlets_map) : 
				buffered_client_iostream(std::make_unique<tcp_socket>()), _servlets_map(servlets_map){
				set_condition(string_found("\r\n\r\n"));	
				set_condition(string_found("\n\n"));	
			}
		public:
			virtual void notify() override;
			virtual bool write(const void* data, size_t size) override;
		private:
			bool set_request_header(const std::string& http_header_data);
			bool decode_request_method(std::string& http_header_data);
			bool decode_http_headers(std::string& http_header_data);
			http_servlet* find_servlet(const std::string& request_uri);
		private:
			std::map<std::string, std::shared_ptr<http_servlet> >& _servlets_map;
	};

	class http_accpetor : public acceptor_base{
		public:
			http_accpetor(std::map<std::string, std::shared_ptr<http_servlet> >& servlets_map) : _servlets_map(servlets_map){
			}
		public:
			virtual std::shared_ptr<client_iostream> get_new_client() override{
				return std::make_shared<http_client>(_servlets_map);
			}
			virtual void notify_accept(std::shared_ptr<client_iostream> client, acceptor_status_t status) override{
				std::lock_guard<std::mutex> lk(_client_map_mutex);
				_client_map[client->get_fd()] = client;
				client->register_close_handler(std::bind(&http_accpetor::close,this,std::placeholders::_1));
			}
		public:
			void close(int fd){
				std::lock_guard<std::mutex> lk(_client_map_mutex);
				auto it = _client_map.find(fd);	
				if(it != _client_map.end()){
					_client_map.erase(it);
				}	
			}
		private:
			std::map<int,std::shared_ptr<client_iostream>> _client_map;	
			std::mutex _client_map_mutex;
			std::map<std::string, std::shared_ptr<http_servlet> >& _servlets_map;
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
	http_server(socket_factory& sf, unsigned int max_thread) : _tcp_connection(sf, max_thread){
	}
	http_server(socket_factory& sf, std::shared_ptr<thread_pool> threads) :	_tcp_connection(sf, threads){
	}
public:
	void register_servlet(const std::string& pattern, std::shared_ptr<http_servlet> servlet){
		_servlets_map[pattern] = servlet;
	}
	void start(const endpoint &e,int max_connection,bool block=true){
		_tcp_connection.start_listening(std::make_shared<http_accpetor>(_servlets_map),tcp_connection::endpoint(e.port,e.ip,e.backlog),
				max_connection,block);
		_tcp_connection.start_reactor();
	}	
private:
	std::map<std::string, std::shared_ptr<http_servlet> > _servlets_map;
	tcp_connection _tcp_connection;
};
#endif
