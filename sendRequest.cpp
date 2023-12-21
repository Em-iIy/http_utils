#include "sendRequest.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <exception>
#include <cstring>
#include <poll.h>

static pollfd	initPFD(int fd)
{
	pollfd ret;
	ret.fd = fd;
	ret.events = POLLIN;
	return (ret);
}

void	sendRequest::_initSocket(void)
{
	if (this->_sockFd != -1)
		throw std::runtime_error("Socket already initialized");
	
	if (this->_host == "")
		throw std::runtime_error("No host to connect to");
	hostent	*host = gethostbyname(this->_host.c_str());
	if (!host)
		throw std::runtime_error("Couldn't resolve " + this->_host);
	this->_sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_sockFd < 0)
	{
		perror("socket");
		throw std::runtime_error("");
	}
	this->_sockAddr.sin_family = AF_INET;
	this->_sockAddr.sin_port = htons(this->_port);
	memcpy(&this->_sockAddr.sin_addr.s_addr, host->h_addr, host->h_length);

	if (connect(this->_sockFd, (sockaddr *)&this->_sockAddr, this->_sockAddrLen) < 0)
	{
		perror("connect");
		throw std::runtime_error("Couldn't connect to " + this->_host + ":" + std::to_string(this->_port));
	}
}

void	sendRequest::createRequest(void)
{
	// Put request msg together
	this->_request = this->_method + " " + this->_path + " HTTP/1.1\r\n";
	if (this->_host != "")
		this->_request += "Host: " + this->_host + "\r\n";
	if (this->_content_type != "")
		this->_request += "Content-Type: " + this->_content_type + "\r\n";
	this->_request += "Content-Lenght: " + std::to_string(this->_body.length()) + "\r\n";
	this->_request += "\r\n" + this->_body;
}

sendRequest::sendRequest(void)
{
}

sendRequest::~sendRequest()
{
	if (this->_sockFd > 0)
		close(this->_sockFd);
}

void	sendRequest::Send(void)
{
	// Initialize socket if it isn't yet
	if (this->_sockFd == -1)
		this->_initSocket();
	
	// Send request msg to host
	if (send(this->_sockFd, this->_request.c_str(), this->_request.length(), MSG_NOSIGNAL) < 0)
	{
		perror("send");
		throw std::runtime_error("");
	}
}

const std::string	&sendRequest::Recv(void)
{
	// Initialize socket if it isn't yet
	if (this->_sockFd == -1)
		this->_initSocket();
	
	pollfd	pfd[1];
	pfd[0] = initPFD(this->_sockFd);
	this->_response = "";
	char	buffer[4096];
	ssize_t	bRead = 1;
	while (true)
	{
		int rc = poll(pfd, 1, 100);
		if (rc == 0)
			break ;
		else if (rc > 0 && pfd->revents & POLLIN)
		{
			bRead = recv(this->_sockFd, buffer, sizeof(buffer) - 1, 0);
			if (bRead == 0)
				throw std::runtime_error("Connection closed by host");
			else if (bRead < 0)
			{
				perror("recv");
				throw std::runtime_error("");
			}
			buffer[bRead] = '\0';
			this->_response += buffer;
		}
		else
			perror("poll");
	}
	return (this->_response);
}

const std::string	&sendRequest::sendRecv(void)
{
	this->createRequest();
	this->Send();
	this->Recv();
	return (this->_response);
}

const std::string	&sendRequest::sendRecv(const std::string &request)
{
	this->_request = request;
	this->Send();
	this->Recv();
	return (this->_response);
}

void	sendRequest::setHdr(const request_hdr_t &hdr)
{
	this->_method = hdr.method;
	this->_host = hdr.host;
	this->_path = hdr.path;
	this->_content_type = hdr.content_type;
}

void	sendRequest::setMethod(const std::string &method)
{
	this->_method = method;
}

void	sendRequest::setHost(const std::string &host)
{
	this->_host = host;
}

void	sendRequest::setPort(int port)
{
	this->_port = port;
}

void	sendRequest::setPath(const std::string &path)
{
	this->_path = path;
}

void	sendRequest::setContentType(const std::string &type)
{
	this->_content_type = type;
}

void	sendRequest::setBody(const std::string &body)
{
	this->_body = body;
}

void	sendRequest::appendBody(const std::string &str)
{
	this->_body += str;
}

const std::string	&sendRequest::getResponse(void) const
{
	return (this->_response);
}

const std::string	&sendRequest::getRequest(void) const
{
	return (this->_request);
}
