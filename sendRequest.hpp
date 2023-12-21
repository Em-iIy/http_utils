#pragma once

#include <string>
#include <netdb.h>

struct request_hdr_t
{
	std::string	method;
	std::string	host;
	std::string	path;
	std::string	content_type;
	request_hdr_t(void){};
	request_hdr_t(const std::string &_method, const std::string &_host, const std::string &_path, const std::string &_content_type): method(_method), host(_host), path(_path), content_type(_content_type){};
};


class sendRequest {
private:
	int			_sockFd = -1;
	sockaddr_in	_sockAddr;
	socklen_t	_sockAddrLen = sizeof(this->_sockAddr);

	std::string	_method;
	std::string	_host;
	int			_port = 80;
	std::string	_path;
	std::string	_content_type;
	std::string	_body;
	std::string	_response;
	std::string	_request;

	void	_initSocket(void);

public:
	sendRequest(void);
	~sendRequest();

	void				createRequest(void);
	void				Send(void);
	const std::string	&Recv(void);
	const std::string	&sendRecv(void);
	const std::string	&sendRecv(const std::string &request);

	void	setHdr(const request_hdr_t &hdr);
	void	setMethod(const std::string &method);
	void	setHost(const std::string &method);
	void	setPort(int port);
	void	setPath(const std::string &path);
	void	setContentType(const std::string &type);
	void	setBody(const std::string &body);
	void	appendBody(const std::string &str);

	const std::string	&getResponse(void) const;
	const std::string	&getRequest(void) const;
};
