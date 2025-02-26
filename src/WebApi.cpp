// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022-2024 Thomas Basler and others
 */
#include "WebApi.h"
#include "Configuration.h"
#include "defaults.h"
#include <AsyncJson.h>

WebApiClass::WebApiClass()
    : _server(HTTP_PORT)
{
}

void WebApiClass::init(Scheduler& scheduler)
{
    _webApiConfig.init(_server, scheduler);
    _webApiDevice.init(_server, scheduler);
    _webApiDevInfo.init(_server, scheduler);
    _webApiDtu.init(_server, scheduler);
    _webApiEventlog.init(_server, scheduler);
    _webApiFirmware.init(_server, scheduler);
    _webApiGridprofile.init(_server, scheduler);
    _webApiInverter.init(_server, scheduler);
    _webApiLimit.init(_server, scheduler);
    _webApiMaintenance.init(_server, scheduler);
    _webApiMqtt.init(_server, scheduler);
    _webApiNetwork.init(_server, scheduler);
    _webApiNtp.init(_server, scheduler);
    _webApiPower.init(_server, scheduler);
    _webApiPrometheus.init(_server, scheduler);
    _webApiSecurity.init(_server, scheduler);
    _webApiSysstatus.init(_server, scheduler);
    _webApiWebapp.init(_server, scheduler);
    _webApiWsConsole.init(_server, scheduler);
    _webApiWsLive.init(_server, scheduler);
    _webApiBattery.init(_server, scheduler);
    _webApiPowerMeter.init(_server, scheduler);
    _webApiPowerLimiter.init(_server, scheduler);
    _webApiWsVedirectLive.init(_server, scheduler);
    _webApiVedirect.init(_server, scheduler);
    _webApiWsHuaweiLive.init(_server, scheduler);
    _webApiHuaweiClass.init(_server, scheduler);
    _webApiWsBatteryLive.init(_server, scheduler);

    _server.begin();
}

bool WebApiClass::checkCredentials(AsyncWebServerRequest* request)
{
    CONFIG_T& config = Configuration.get();
    if (request->authenticate(AUTH_USERNAME, config.Security.Password)) {
        return true;
    }

    AsyncWebServerResponse* r = request->beginResponse(401);

    // WebAPI should set the X-Requested-With to prevent browser internal auth dialogs
    if (!request->hasHeader("X-Requested-With")) {
        r->addHeader("WWW-Authenticate", "Basic realm=\"Login Required\"");
    }
    request->send(r);

    return false;
}

bool WebApiClass::checkCredentialsReadonly(AsyncWebServerRequest* request)
{
    CONFIG_T& config = Configuration.get();
    if (config.Security.AllowReadonly) {
        return true;
    } else {
        return checkCredentials(request);
    }
}

void WebApiClass::sendTooManyRequests(AsyncWebServerRequest* request)
{
    auto response = request->beginResponse(429, "text/plain", "Too Many Requests");
    response->addHeader("Retry-After", "60");
    request->send(response);
}

void WebApiClass::writeConfig(JsonVariant& retMsg, const WebApiError code, const String& message)
{
    if (!Configuration.write()) {
        retMsg["message"] = "Write failed!";
        retMsg["code"] = WebApiError::GenericWriteFailed;
    } else {
        retMsg["type"] = "success";
        retMsg["message"] = message;
        retMsg["code"] = code;
    }
}

WebApiClass WebApi;
