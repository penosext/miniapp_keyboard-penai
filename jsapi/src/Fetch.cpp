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

#include <Fetch.hpp>
#include <iostream>
#include <sstream>

Response::Response(int status, std::string body)
    : status(status), body(body), ok(status >= 200 && status < 300) {}
nlohmann::json Response::json()
{
    try
    {
        return nlohmann::json::parse(body);
    }
    catch (const std::exception &e)
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
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            if (line.substr(0, 6) == "data: ")
                options->streamCallback(line.substr(6));
        }
    }
    return totalSize;
}
size_t Fetch::HeaderCallback(char *buffer, size_t size, size_t nitems, std::map<std::string, std::string> *headers)
{
    size_t totalSize = size * nitems;
    std::string header(buffer, totalSize);
    if (!header.empty() && header.back() == '\n')
        header.pop_back();
    if (!header.empty() && header.back() == '\r')
        header.pop_back();
    size_t colonPos = header.find(':');
    if (colonPos != std::string::npos)
    {
        std::string key = header.substr(0, colonPos);
        std::string value = header.substr(colonPos + 1);
        while (!key.empty() && key.back() == ' ')
            key.pop_back();
        while (!value.empty() && value.front() == ' ')
            value.erase(0, 1);
        while (!value.empty() && value.back() == ' ')
            value.pop_back();
        (*headers)[key] = value;
    }
    return totalSize;
}

Response Fetch::fetch(const std::string &url, const FetchOptions &options)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        throw std::runtime_error("Failed to initialize curl");

    std::string responseBody;
    std::map<std::string, std::string> responseHeaders;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &responseHeaders);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, options.timeout);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, options.followRedirects ? 1L : 0L);

    if (options.stream && options.streamCallback)
    {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, StreamWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &options);
    }
    else
    {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
    }

    if (options.method == "GET")
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    else if (options.method == "POST")
    {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        if (!options.body.empty())
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, options.body.c_str());
    }
    else
    {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, options.method.c_str());
        if (!options.body.empty())
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, options.body.c_str());
    }

    struct curl_slist *headers = nullptr;
    for (const auto &header : options.headers)
    {
        std::string headerStr = header.first + ": " + header.second;
        headers = curl_slist_append(headers, headerStr.c_str());
    }
    if (headers)
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);

    long responseCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

    if (headers)
        curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK)
        throw std::runtime_error("curl_easy_perform() failed: " + std::string(curl_easy_strerror(res)));

    Response response(responseCode, responseBody);
    response.headers = responseHeaders;
    return response;
}
