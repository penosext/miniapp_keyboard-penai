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

#include "Fetch.hpp"
#include "strUtils.hpp"
#include <iostream>
#include <sstream>

Response::Response(int status, std::string body) : status(status), body(body), ok(status >= 200 && status < 300) {}
nlohmann::json Response::json()
{
    try
    {
        return nlohmann::json::parse(body);
    }
    catch (const nlohmann::json::parse_error &e)
    {
        throw std::runtime_error("Failed to parse JSON: " + std::string(e.what()));
    }
}
std::string Response::text() { return body; }
bool Response::isOk() { return ok; }

size_t Fetch::WriteCallback(void *contents, size_t size, size_t nmemb, std::string *data)
{
    size_t totalSize = size * nmemb;
    data->append((char *)contents, totalSize);
    return totalSize;
}
size_t Fetch::StreamWriteCallback(void *contents, size_t size, size_t nmemb, void *userdata)
{
    size_t totalSize = size * nmemb;
    const FetchOptions *options = static_cast<const FetchOptions *>(userdata);
    if (options && options->streamCallback)
    {
        std::string chunk((char *)contents, totalSize);
        std::istringstream stream(chunk);
        std::string line;
        while (std::getline(stream, line))
        {
            line = strUtils::trimEnd(line);
            if (line.substr(0, 6) == "data: ")
                options->streamCallback(line.substr(6));
        }
    }
    return totalSize;
}
size_t Fetch::HeaderCallback(char *buffer, size_t size, size_t nitems, std::unordered_map<std::string, std::string> *headers)
{
    size_t totalSize = size * nitems;
    std::string header(buffer, totalSize);
    header = strUtils::trimEnd(header);
    size_t colonPos = header.find(':');
    if (colonPos != std::string::npos)
    {
        std::string key = header.substr(0, colonPos);
        std::string value = header.substr(colonPos + 1);
        (*headers)[strUtils::trimEnd(key)] = strUtils::trim(value);
    }
    return totalSize;
}

Response Fetch::fetch(const std::string &url, const FetchOptions &options)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        throw std::runtime_error("Failed to initialize curl: " + std::string(curl_easy_strerror(CURLE_FAILED_INIT)));

    long responseCode = 0;
    std::string responseBody;
    std::unordered_map<std::string, std::string> responseHeaders;

    ASSERT_CURL_OK(curl_easy_setopt(curl, CURLOPT_URL, url.c_str()));
    ASSERT_CURL_OK(curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback));
    ASSERT_CURL_OK(curl_easy_setopt(curl, CURLOPT_HEADERDATA, &responseHeaders));
    ASSERT_CURL_OK(curl_easy_setopt(curl, CURLOPT_TIMEOUT, options.timeout));
    ASSERT_CURL_OK(curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, options.followRedirects ? 1L : 0L));

    if (options.stream && options.streamCallback)
    {
        ASSERT_CURL_OK(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, StreamWriteCallback));
        ASSERT_CURL_OK(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &options));
    }
    else
    {
        ASSERT_CURL_OK(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback));
        ASSERT_CURL_OK(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody));
    }

    if (options.method == "GET")
        ASSERT_CURL_OK(curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L));
    else if (options.method == "POST")
    {
        ASSERT_CURL_OK(curl_easy_setopt(curl, CURLOPT_POST, 1L));
        if (!options.body.empty())
            ASSERT_CURL_OK(curl_easy_setopt(curl, CURLOPT_POSTFIELDS, options.body.c_str()));
    }
    else
    {
        ASSERT_CURL_OK(curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, options.method.c_str()));
        if (!options.body.empty())
            ASSERT_CURL_OK(curl_easy_setopt(curl, CURLOPT_POSTFIELDS, options.body.c_str()));
    }

    struct curl_slist *headers = nullptr;
    for (const auto &header : options.headers)
        headers = curl_slist_append(headers, std::string(header.first + ": " + header.second).c_str());
    if (headers)
        ASSERT_CURL_OK(curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers));

    ASSERT_CURL_OK(curl_easy_perform(curl));

    ASSERT_CURL_OK(curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode));

    if (headers)
        curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    Response response(responseCode, responseBody);
    response.headers = responseHeaders;
    return response;
}
