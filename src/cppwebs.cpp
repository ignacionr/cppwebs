#include "cppwebs.hpp"
#include <cstdio>
#include <stdexcept>
#include <iostream>

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

    void cppwebs::start(std::chrono::duration<int> how_long)
    {
        if (mg_http_listen(&mgr, url_.c_str(), ev_handler, this) == nullptr)
        {
            throw std::runtime_error("Failed to start the web server");
        }
        printf("Starting web server on %s\n", url_.c_str());

        auto const start = std::chrono::system_clock::now();
        while (std::chrono::system_clock::now() - start < how_long && running_)
        {
            mg_mgr_poll(&mgr, 3000);
        }

        puts("Stopping server...\n");
    }

    void cppwebs::add_controller(std::string const &host, const std::string &path, controller_t controller)
    {
        std::cerr << "Adding controller for path: " << path << " and host: " << host << "\n";
        controllers_[path][host] = controller;
    }

    struct directory_server
    {
        directory_server(std::filesystem::path const &base_dir) : base_dir_{base_dir}
        {
            opts_.root_dir = base_dir_.data();
        }
        directory_server(const directory_server &src)
        {
            base_dir_ = src.base_dir_;
            opts_.root_dir = base_dir_.data();
        }
        directory_server(directory_server &&src)
        {
            base_dir_ = std::move(src.base_dir_);
            opts_.root_dir = base_dir_.data();
        }
        void operator()(mg_connection *nc, mg_http_message *hm)
        {
            mg_http_serve_dir(nc, hm, &opts_);
        }
        mg_http_serve_opts opts_{};
        std::string base_dir_;
    };

    void cppwebs::add_directory(std::string const &host, std::string const &path, std::filesystem::path const &directory)
    {
        add_controller(host, path, directory_server(directory));
    }

    bool cppwebs::handle_page(struct mg_connection *nc, struct mg_http_message *hm, auto it)
    {
        bool handled{false};
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
                    handled = true;
                    std::visit([&nc, &hm, this](auto &c){
                        if constexpr (std::is_assignable_v<Controller,decltype(c)>)
                        {
                            std::cerr << "Handling the request with a Controller\n";
                            c(nc, hm);
                        }
                        else if constexpr (std::is_assignable_v<ControllerWS, decltype(c)>)
                        {
                            std::cerr << "Handling the request with a ControllerWS\n";
                            c(nc, hm, [&nc, &hm, this] (WebSocketHandler ws_handler){
                                mg_ws_upgrade(nc, hm, nullptr);
                                ws_handlers_[nc] = ws_handler;
                            });
                        }
                        else
                        {
                            static_assert(false, "Invalid controller type");
                        }
                    }, it_host->second);
                }
            }
            catch (std::exception &ex)
            {
                mg_http_reply(nc, 500, "Content-Type: text/plain\r\n", "%s", ex.what());
            }
        }
        return handled;
    }

    void cppwebs::ev_handler(struct mg_connection *nc, int ev, void *ev_data, void *fn_data)
    {
        if (ev == MG_EV_HTTP_MSG)
        {
            auto pThis = static_cast<cppwebs *>(fn_data);
            struct mg_http_message *hm = (struct mg_http_message *)ev_data;
            std::string_view uri(hm->uri.ptr, hm->uri.len);

            auto it = pThis->controllers_.find(std::string{uri});
            bool handled{false};
            if (it != pThis->controllers_.end())
            {
                handled = pThis->handle_page(nc, hm, it);
            }
            
            if (!handled)
            {
                if (it = pThis->controllers_.find("*"); it != pThis->controllers_.end())
                {
                    handled = pThis->handle_page(nc, hm, it);
                }
            }

            if (!handled)
            {
                mg_http_reply(nc, 404, "Content-Type: text/plain\r\n", "Not Found");
            }
        }
        else if (ev == MG_EV_WS_MSG)
        {
            auto pThis = static_cast<cppwebs *>(fn_data);
            auto ws_handler = pThis->ws_handlers_.find(nc);
            if (ws_handler != pThis->ws_handlers_.end())
            {
                auto wm = static_cast<struct mg_ws_message *>(ev_data);
                ws_handler->second(MG_EV_WS_MSG, {wm->data.ptr, wm->data.len});
            }
        }
        else if (ev == MG_EV_WS_OPEN)
        {
            auto pThis = static_cast<cppwebs *>(fn_data);
            auto ws_handler = pThis->ws_handlers_.find(nc);
            if (ws_handler != pThis->ws_handlers_.end())
            {
                ws_handler->second(MG_EV_WS_OPEN, {});
            }
        }
        else if (ev == MG_EV_CLOSE)
        {
            auto pThis = static_cast<cppwebs *>(fn_data);
            pThis->ws_handlers_.erase(nc);
        }
    }
}
