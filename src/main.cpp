#include <algorithm>
#include <iostream>
#include <functional>
#include <httplib.h>
#include <memory>
#include <string>

class HealthHandler
{
public:
    void handle(const httplib::Request &, httplib::Response & _resp)
    {
        std::cout << "health check called" << std::endl;

        std::string body = "{\"status\":\"UP\"}\n";
        _resp.status = 200;
        _resp.set_content(body, "text/plain");
    }
};

using Handler =
    std::function<void(const httplib::Request &, httplib::Response &)>;

class Server
{
public:
    Server()
    {
        m_service = std::unique_ptr<httplib::Server>(new httplib::Server());
    }

    ~Server()
    {
        stop();
    }

    void route(
        const std::string & _path,
        const std::string & _method,
        Handler _handler)
    {
        std::string method(_method.size(), 0);
        std::transform(_method.begin(), _method.end(), method.begin(), ::toupper);

        if(method == "GET")
        {
            m_service->Get(_path, _handler);
        }
        else if(method == "POST")
        {
            m_service->Post(_path, _handler);
        }
        else if(method == "PUT")
        {
            m_service->Put(_path, _handler);
        }
        else if(method == "DELETE")
        {
            m_service->Delete(_path, _handler);
        }
        else if(method == "PATCH")
        {
            m_service->Patch(_path, _handler);
        }
        else
        {
            std::cerr << "Invalid HTTP method" << std::endl;

            return;
        }
    }

    void start(
        const std::string & _ip,
        unsigned int _port,
        unsigned int _threads)
    {
        stop();

        if(_threads > 0)
        {
            m_service->new_task_queue = [_threads] { return new httplib::ThreadPool(_threads); };
        }

        m_service->listen(_ip, _port);
    }

    void stop()
    {
        if(m_service->is_running())
        {
            m_service->stop();
        }
    }

private:
    std::unique_ptr<httplib::Server> m_service;
};

int main(const int /*_argc*/, const char ** /*_argv*/)
{
    HealthHandler hh;

    Server server;
    server.route(
        "/health", "GET",
        std::bind(&HealthHandler::handle, hh, std::placeholders::_1,
                  std::placeholders::_2));
    server.start("127.0.0.1", 8080, 2);

    return EXIT_SUCCESS;
}
