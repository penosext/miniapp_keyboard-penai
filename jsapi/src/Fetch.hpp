// Copyright (C) 2025 Langning Chen
//
// This file is part of miniapp.
//
// miniapp is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// miniapp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with miniapp.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

#define ASSERT_CURL_OK(expr)                                         \
    do                                                               \
    {                                                                \
        CURLcode res = (expr);                                       \
        if (res != CURLE_OK)                                         \
            throw std::runtime_error(                                \
                "Curl error " + std::to_string(res) +                \
                " while executing " + #expr +                        \
                " on " + __FILE__ + ":" + std::to_string(__LINE__)); \
    } while (0)

using StreamCallback = std::function<void(const std::string &chunk)>;

class Response
{
public:
    int status;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    bool ok;

    Response(int status, std::string body);
    nlohmann::json json();
    std::string text();
    bool isOk();
};

class FetchOptions
{
public:
    std::string method = "GET";
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    int timeout = 30; // 超时时间（秒）
    bool followRedirects = true;
    bool stream = false;
    StreamCallback streamCallback = nullptr;

    FetchOptions(std::string method = "GET",
                 std::unordered_map<std::string, std::string> headers = {},
                 std::string body = "",
                 bool stream = false,
                 StreamCallback streamCallback = nullptr)
        : method(method), headers(headers), body(body), stream(stream), streamCallback(streamCallback) {}
};

class Fetch
{
private:
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *data);
    static size_t StreamWriteCallback(void *contents, size_t size, size_t nmemb, void *userdata);
    static size_t HeaderCallback(char *buffer, size_t size, size_t nitems, std::unordered_map<std::string, std::string> *headers);

public:
    static Response fetch(const std::string &url, const FetchOptions &options = FetchOptions{});
};
