#ifndef POSTER_H
#define POSTER_H
#include <json.h>
#include <WinSock2.h>
#include <memory>
#include <iostream>
#include <sstream>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

enum RequestType
{
    GET,
    POST,
    NULL_REQUEST_TYPE
};

enum ContentType
{
    application_json,
    application_x_www_form_urlencoded,
    multipart_form_data,
    text_plain,
    application_xml,
    NULL_CONTENT_TYPE
};

class Response
{
private:
    JSONMAP headers = JSONMAP();
    std::string body = "";
    short status = 0;
    std::string statusText = "";

public:
    Response(){};
    Response(short status, JSONMAP headers)
    {
        this->status = status;
        this->headers = headers;
    }
    Response(std::string str)
    {

        size_t pos_start = 0;
        size_t pos_end = str.find('\n', pos_start);
        std::string line = str.substr(pos_start, pos_end - pos_start);
        size_t pos_status_code = line.find(' ', 0) + 1;
        size_t pos_status_message = line.find(' ', pos_status_code) + 1;
        this->status = std::stoi(line.substr(pos_status_code, pos_status_message - pos_status_code));
        this->statusText = line.substr(pos_status_message, line.length() - pos_status_message);
        pos_start = pos_end + 1;
        pos_end = str.find('\n', pos_start);
        line = str.substr(pos_start, pos_end - pos_start);
        do
        {
            size_t pos_key_end = line.find(":");
            std::string key = line.substr(0, pos_key_end);
            size_t pos_val_start = line.find_first_not_of(": \t", pos_key_end);
            std::string val = line.substr(pos_val_start, pos_end - pos_val_start - 1);
            headers.set(key, val);
            pos_start = pos_end + 1;
            pos_end = str.find('\n', pos_start);
            line = str.substr(pos_start, pos_end - pos_start);
        } while (line != "\r");
        this->body = str.substr(pos_end + 1, str.length() - pos_end - 1);
    }
    std::string toString()
    {
        std::ostringstream stream;
        stream << "Status: " << status << " " << statusText << std::endl;
        stream << "Headers: " << headers << std::endl;
        stream << "Body: " << body << std::endl;
        return stream.str();
    }
};

class Request
{
private:
    JSONMAP headers = JSONMAP();
    ContentType content_type = NULL_CONTENT_TYPE;
    RequestType request_type = NULL_REQUEST_TYPE;
    std::string body = "";
    std::string host = "";
    std::string port = "";
    std::string path = "";
    std::string params = "";

public:
    Request(){};
    Request(std::string request)
    {
        std::string req = request;
        size_t pos = 0;
        std::string cur_str = "";
        pos = req.find(' ', pos);
        this->setRequestType(req.substr(0, pos));
        req.erase(0, pos + 1);
        pos = req.find(':', 0);
        if (req[pos + 1] == '/')
            pos = req.find(':', pos + 1);
        if (pos != std::string::npos)
        {
            this->setHost(req.substr(0, pos));
            req.erase(0, pos + 1);
            pos = req.find(' ', 0);
            this->setPort(req.substr(0, pos));
            req.erase(0, pos + 1);
        }
        else
        {
            pos = req.find(' ', 0);
            this->setHost(req.substr(0, pos));
            req.erase(0, pos + 1);
        }
        if (req.length() > 0)
        {
            this->setPath(req);
        }
        if (this->host.find("https://") != std::string::npos)
        {
            this->setPort("443");
            this->setHost(this->host.substr(8, this->host.length() - 8));
        }
        else if (this->host.find("http://") != std::string::npos)
        {
            this->setPort("80");
            this->setHost(this->host.substr(7, this->host.length() - 7));
        }
    }
    void setHost(std::string host_string)
    {
        this->host = host_string;
    }
    void setPort(std::string port_string)
    {
        this->port = port_string;
    }
    void setPath(std::string path_string)
    {
        this->path = path_string;
    }
    void setParameters(std::string params_string)
    {
        this->params = params_string;
    }
    void setHeaders(JSONMAP headers_json)
    {
        this->headers = headers_json;
    }
    void setHeaders(std::string headers_string)
    {
        this->headers = JSONParser::loads(headers_string);
    }
    void setContentType(ContentType type)
    {
        this->content_type = type;
    }
    void setContentType(std::string type)
    {
        if (type == "application/json")
            this->content_type = ContentType::application_json;
        else if (type == "application/x-www-form-urlencoded")
            this->content_type = ContentType::application_x_www_form_urlencoded;
        else if (type == "application/xml")
            this->content_type = ContentType::application_xml;
        else if (type == "multipart/formdata")
            this->content_type = ContentType::multipart_form_data;
        else if (type == "text/plain")
            this->content_type = ContentType::text_plain;
    }
    void setRequestType(RequestType type)
    {
        this->request_type = type;
    }
    void setRequestType(std::string type)
    {
        if (type == "GET")
            this->request_type = RequestType::GET;
        else if (type == "POST")
            this->request_type = RequestType::POST;
    }
    void setBody(JSONMAP body_json)
    {
        this->body = body_json.toString();
    }
    void setBody(std::string body_string)
    {
        this->body = body_string;
    }
    std::string toString()
    {
        std::stringstream ss;
        ss << "GET "
           << this->path
           << (params.length() < 1 ? "" : "?")
           << this->params
           << " HTTP/1.1\r\n"
           << "Host: " << this->host
           << (port.length() < 1 ? "" : ":")
           << this->port
           << "\r\n"
           << "Connection: close\r\n"
           << "\r\n";
        return ss.str();
    }
    Response fetch()
    {
        if (this->request_type == GET)
        {
            // process and return get request
            // request string:
            //"GET PATH?PARAMETERS HTTP/1.1\r\n"
            //"Host: ADDRESS\r\n"
            //"Connection: close\r\n"
            //"\r\n"
            std::stringstream ss;
            ss << "GET "
               << this->path
               << (params.length() < 1 ? "" : "?")
               << this->params
               << " HTTP/1.1\r\n"
               << "Host: " << this->host
               << (port.length() < 1 ? "" : ":")
               << this->port
               << "\r\n"
               << "Connection: close\r\n"
               << "\r\n";
            std::string ss_s = ss.str();
            const char *getRequest = ss_s.c_str();
            // const char *getRequest = "GET / HTTP/1.1\r\n"
            //                          "Host: 127.0.0.1\r\n"
            //                          "Connection: close\r\n"
            //                          "\r\n";

            // Initialize WinSock
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            {
                std::cerr << "WSAStartup failed" << std::endl;
                return Response(400, JSONParser::loads("{\"statusText\": \"WSAStartup Failed\"}"));
            }

            // Create Socket
            SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (clientSocket == INVALID_SOCKET)
            {
                std::cerr << "Socket creation failed" << std::endl;
                return Response(400, JSONParser::loads("{\"statusText\": \"Socket creation failed\"}"));
            }

            // Resolve the server address
            sockaddr_in serverAddress;
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_addr.s_addr = inet_addr((this->host).c_str());
            if (this->port != "")
                serverAddress.sin_port = htons(std::stoi(this->port));

            // Connect to the server
            if (connect(clientSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
            {
                std::cerr << "Connection failed" << std::endl;
                closesocket(clientSocket);
                WSACleanup();
                return Response(400, JSONParser::loads("{\"statusText\": \"Connection failed\"}"));
            }

            // Send the Get Request
            if (send(clientSocket, getRequest, strlen(getRequest), 0) == SOCKET_ERROR)
            {
                std::cerr << "Send failed" << std::endl;
                closesocket(clientSocket);
                WSACleanup();
                return Response(400, JSONParser::loads("{\"statusText\": \"Send failed\"}"));
            }

            // Process Response
            const int bufferSize = 1024;
            char buffer[bufferSize];
            int bytesReceived;
            std::stringstream receivedStream;

            do
            {
                bytesReceived = recv(clientSocket, buffer, bufferSize, 0);
                if (bytesReceived > 0)
                {
                    receivedStream.write(buffer, bytesReceived);
                }
                else if (bytesReceived < 0)
                {
                    std::cerr << "Recv failed" << std::endl;
                    closesocket(clientSocket);
                    WSACleanup();
                    return Response(400, JSONParser::loads("{\"statusText\": \"Recv failed\"}"));
                }
            } while (bytesReceived > 0);

            // Close Socket and cleanup
            closesocket(clientSocket);
            WSACleanup();
            return Response(receivedStream.str());
        }
        return Response(400, JSONMAP());
    }
};

#endif