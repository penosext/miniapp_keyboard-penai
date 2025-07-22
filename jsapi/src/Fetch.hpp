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
#include <atomic>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <Exceptions/CurlError.hpp>

#define ASSERT_CURL_OK(expr)                                     \
    do                                                           \
    {                                                            \
        CURLcode res = (expr);                                   \
        if (res != CURLE_OK && res != CURLE_ABORTED_BY_CALLBACK) \
            THROW_CURL_ERROR(res);                               \
    } while (false)

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
    std::string method;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    bool stream;
    StreamCallback streamCallback;
    size_t timeout;
    bool followRedirects = true;
    std::shared_ptr<std::atomic<bool>> cancelled;

    FetchOptions(std::string method = "GET",
                 std::unordered_map<std::string, std::string> headers = {},
                 std::string body = "",
                 bool stream = false,
                 StreamCallback streamCallback = nullptr,
                 size_t timeout = 10,
                 std::shared_ptr<std::atomic<bool>> cancelled = nullptr)
        : method(method), headers(headers), body(body),
          stream(stream), streamCallback(streamCallback), timeout(timeout),
          cancelled(cancelled) {}
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
