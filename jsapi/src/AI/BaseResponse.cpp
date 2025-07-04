#include "BaseResponse.hpp"

BaseResponse::BaseResponse(bool success, int statusCode, std::string errorMessage)
    : success(success), statusCode(statusCode), errorMessage(errorMessage) {}
nlohmann::json BaseResponse::toJson() const
{
    nlohmann::json json;
    json["success"] = success;
    json["statusCode"] = statusCode;
    if (!errorMessage.empty())
        json["errorMessage"] = errorMessage;
    return json;
}
