#include "cppwebs.hpp"
#include <cstdio>
#include <stdexcept>

namespace ignacionr
{
    cppwebs::cppwebs(const char *url) : url_{url}
    {
        mg_mgr_init(&mgr);
    }

    cppwebs::~cppwebs()
    {
        mg_mgr_free(&mgr);
    }

    void cppwebs::Start(std::chrono::duration<int> how_long)
    {
        if (mg_http_listen(&mgr, url_.c_str(), ev_handler, this) == nullptr)
        {
            throw std::runtime_error("Failed to start the web server");
        }
        printf("Starting web server on %s\n", url_.c_str());

        auto const start = std::chrono::system_clock::now();
        while (std::chrono::system_clock::now() - start < how_long)
        {
            mg_mgr_poll(&mgr, 1000);
        }

        puts("Stopping server...\n");
    }
    void cppwebs::AddController(std::string const &host, const std::string &path, Controller controller)
    {
        controllers_[path][host] = controller;
    }

    void cppwebs::handle_page(struct mg_connection *nc, struct mg_http_message *hm, auto it)
    {
        auto host_hdr = mg_http_get_header(hm, "host");
        if (host_hdr && host_hdr->ptr)
        {
            try
            {
                auto host = std::string{host_hdr->ptr, host_hdr->len};
                auto it_host = it->second.find(host);
                if (it_host == it->second.end())
                {
                    it_host = it->second.find("*");
                }
                if (it_host != it->second.end())
                {
                    it_host->second(nc, hm);
                }
                else
                {
                    mg_http_reply(nc, 404, "Content-Type: text/plain\r\n", "Not Found (try a different host)");
                }
            }
            catch (std::exception &ex)
            {
                mg_http_reply(nc, 500, "Content-Type: text/plain\r\n", "%s", ex.what());
            }
        }
    }

    void cppwebs::ev_handler(struct mg_connection *nc, int ev, void *ev_data, void *fn_data)
    {
        if (ev == MG_EV_HTTP_MSG)
        {
            auto pThis = static_cast<cppwebs *>(fn_data);
            struct mg_http_message *hm = (struct mg_http_message *)ev_data;
            std::string_view uri(hm->uri.ptr, hm->uri.len);

            auto it = pThis->controllers_.find(std::string{uri});
            if (it != pThis->controllers_.end())
            {
                handle_page(nc, hm, it);
            }
            else if (it = pThis->controllers_.find("*"); it != pThis->controllers_.end())
            {
                handle_page(nc, hm, it);
            }
            else
            {
                mg_http_reply(nc, 404, "Content-Type: text/plain\r\n", "Not Found");
            }
        }
    }
}
