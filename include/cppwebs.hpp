#pragma once
#include <string>
#include <string_view>
#include <filesystem>
#include <functional>
#include <map>
#include <chrono>
#include <variant>
#include "mongoose.h"

namespace ignacionr
{
    class cppwebs
    {
    public:
        using Controller = std::function<void(struct mg_connection *, struct mg_http_message *)>;
        using WebSocketHandler = std::function<void(int, std::string_view)>;
        using UpgradeFn = std::function<void(WebSocketHandler)>;
        using ControllerWS = std::function<void(struct mg_connection *, struct mg_http_message *, UpgradeFn)>;
        using controller_t = std::variant<Controller, ControllerWS>;
        using ControllersMap = std::map<std::string, std::map<std::string, controller_t>>;
        using WebScoketHandlerMap = std::map<mg_connection *, WebSocketHandler>;

        cppwebs(const char *url);
        ~cppwebs();

        void start(std::chrono::duration<int> how_long = std::chrono::minutes(5));
        void stop() { running_ = false; }
        void add_controller(std::string const &host, const std::string &path, controller_t controller);
        void add_directory(std::string const &host, std::string const &path, std::filesystem::path const &directory);

    private:
        bool handle_page(struct mg_connection *nc, struct mg_http_message * hm, auto it);
        static void ev_handler(struct mg_connection *nc, int ev, void *ev_data, void *fn_data);

        struct mg_mgr mgr;
        std::string url_;
        ControllersMap controllers_;
        WebScoketHandlerMap ws_handlers_;
        bool running_{true};
    };
}
