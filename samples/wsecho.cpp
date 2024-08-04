#include "cppwebs.hpp"

int main()
{
    ignacionr::cppwebs webs{"ws://localhost:8080"};
    webs.add_controller(
        "*", "*",
        ignacionr::cppwebs::ControllerWS{[](struct mg_connection *cn, struct mg_http_message *, auto upgrade)
        {
            upgrade([cn](int ev, std::string_view data)
            {
                if (ev == MG_EV_WS_OPEN)
                {
                    mg_ws_send(cn, "Hello world!", 12, WEBSOCKET_OP_TEXT);
                }
                else if (ev == MG_EV_WS_MSG)
                {
                    mg_ws_send(cn, data.data(), data.size(), WEBSOCKET_OP_TEXT);
                }
            });
        }});
    webs.start();
    return 0;
}
