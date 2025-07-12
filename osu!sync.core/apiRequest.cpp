//
// Created by chenjintang on 25-7-9.
//

#include "apiRequest.hpp"
#include <map>
#include "errcodes.hpp"
#include <3rdpartyInclude/nlohmann/json.hpp>
#include <3rdpartyInclude/httplib.h>
using namespace nlohmann;
using namespace std;

errorCode apiRequest::getToken() {
    httplib::Client cli("http://120.26.51.57", 8080);
    auto res=cli.Get("/start-auth");
    if (res->status==200) {

    }
}

