#ifndef __HTTP_SERVLET_H__12032016_RAJEN_H__
#define __HTTP_SERVLET_H__12032016_RAJEN_H__
#include "http_request.h"
#include "http_response.h"
class http_servlet{
public:
	virtual void do_get(http_request& request, http_response& response) = 0;
	virtual void do_post(http_request& request, http_response& response) = 0;
	virtual std::shared_ptr<http_servlet> get_new_instance() = 0;
};
#endif
