#include "cppwebs.hpp"

int main()
{
    ignacionr::cppwebs webs{"localhost:80"};
    webs.add_controller(
        "*", "/string",
        [](struct mg_connection *cn, struct mg_http_message *)
        {
            mg_http_reply(cn, 200, "Content-Type: text/plain\n", "Hello world!");
        });
    webs.add_directory("*", "*", "./html");
    webs.start();
    return 0;
}
