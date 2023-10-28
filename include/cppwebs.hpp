#pragma once
#include "mongoose.h"
#include <string>
#include <string_view>
#include <functional>
#include <map>
#include <chrono>

namespace ignacionr
{
    class cppwebs
    {
    public:
        using Controller = std::function<void(struct mg_connection *, struct mg_http_message *)>;

        cppwebs(const char *url);
        ~cppwebs();

        void Start(std::chrono::duration<int> how_long = std::chrono::minutes(5));
        void AddController(std::string const &host, const std::string &path, Controller controller);

    private:
        static void handle_page(struct mg_connection *nc, struct mg_http_message * hm, auto it);
        static void ev_handler(struct mg_connection *nc, int ev, void *ev_data, void *fn_data);

        struct mg_mgr mgr;
        std::string url_;
        std::map<std::string, std::map<std::string, Controller>> controllers_;
    };
}
