// #include <SPIFFS.h>
#include "myWebServer.h"
#include "ota_ap.h"

// WiFi AP configuration
IPAddress ap_ip(192, 168, 4, 1);
IPAddress ap_mask(255, 255, 255, 0);
AsyncWebServer server(80);
DNSServer dnsServer;

// HTML for login page
const char *login_html =
    "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
    "<tr>"
    "<td colspan=2>"
    "<center><font size=4><b>ESP32 Login Page</b></font></center>"
    "<br>"
    "</td>"
    "<br>"
    "<br>"
    "</tr>"
    "<td>Username:</td>"
    "<td><input type='text' size=25 name='userid'><br></td>"
    "</tr>"
    "<br>"
    "<br>"
    "<tr>"
    "<td>Password:</td>"
    "<td><input type='Password' size=25 name='pwd'><br></td>"
    "<br>"
    "<br>"
    "</tr>"
    "<tr>"
    "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
    "</tr>"
    "</table>"
    "</form>"
    "<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='admin')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    "alert('Error Password or Username')"
    "}"
    "}"
    "</script>";

// HTML for server update page
const char *server_html =
    "<form method='POST' action='/update' enctype='multipart/form-data' id='upload_form'>"
    "<input type='file' name='update'>"
    "<input type='submit' value='Update'>"
    "<div id='prg'>progress: 0%</div>"
    "<script src='/javascript'></script>"
    "<script>"
    "$(document).ready(function() {"
    "$('form').on('submit', function(event) {"
    "event.preventDefault();"
    "let formData = new FormData(this);"
    "$.ajax({"
    "url: '/update',"
    "type: 'POST',"
    "data: formData,"
    "processData: false,"
    "contentType: false,"
    "xhr: function() {"
    "let xhr = new XMLHttpRequest();"
    "xhr.upload.onprogress = function(event) {"
    "if (event.lengthComputable) {"
    "let percent = Math.round((event.loaded / event.total) * 100);"
    "$('#prg').text('progress: ' + percent + '%');"
    "}"
    "};"
    "return xhr;"
    "},"
    "success: function(response) {"
    "if (response === 'OK') {"
    "alert('Прошивка успішно завантажена!');"
    "}"
    "else {"
    "alert('Помилка завантаження прошивки!');"
    "}"
    "},"
    "error: function() {"
    "alert('Помилка сервера!');"
    "}"
    "});"
    "});"
    "});"
    "</script>"
    "</form>";

// Serve JavaScript content
void onJavaScript(AsyncWebServerRequest *request)
{
    if (jquery_min_js_v3_2_1_gz && jquery_min_js_v3_2_1_gz_len > 0)
    {
        AsyncWebServerResponse *response = request->beginResponse(200, "text/javascript",
                                                                  reinterpret_cast<const uint8_t *>(jquery_min_js_v3_2_1_gz),
                                                                  jquery_min_js_v3_2_1_gz_len);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    }
    else
    {
        request->send(500, "text/plain", "JavaScript resource not available");
    }
}

void handleNotFound(AsyncWebServerRequest *request)
{
    request->send(200, "text/html; charset=utf-8", login_html);
}

void handleGoogle(AsyncWebServerRequest *request)
{
    request->send(204, "text/html", "");
}

void handleIn(AsyncWebServerRequest *request)
{
    String temp = "<html> <head> <title> Restnow </title> \r\n"
                  "<meta http-equiv=\"Refresh\" content=\"1\" /> \r\n"
                  "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\r\n"
                  "<body> \r\n" +
                  String(millis()) + "<br><br>" + " \"needPress\": " + String(needPress) + "<br>, \r\n" + " \"millis()\": " + String(millis()) + "<br>, \r\n" + " \"IsMotorRun()\": " + String(IsMotorRun()) + "<br>, \r\n" + " \"run_permit\": " + String(run_permit) + "<br>, \r\n" + " \"lastCycle\": " + String(lastCycle) + "<br>, \r\n" + " \"maxCycle\": " + String(maxCycle) + "<br>, \r\n" + " \"bootCount\": " + String(bootCount) + "<br>, \r\n" + " \"temp\": " + String(temperatureRead()) + "<br>°C, \r\n" + " \"MHz\": " + String(getCpuFrequencyMhz()) + "<br>, \r\n"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         "</body> \r\n</html> \r\n";
    request->send(200, "text/html; charset=utf-8", temp);
}

void handleOn(AsyncWebServerRequest *request)
{
    String result = " \"digitalRead\": " + String(digitalRead(POWER_BUTTON_PIN)) + ", \r\n";
    digitalWrite(POWER_BUTTON_PIN, !digitalRead(POWER_BUTTON_PIN));
    request->send(200, "text/html; charset=utf-8", result);
}

void handle1(AsyncWebServerRequest *request)
{
    String result = "1 \r\n";
    digitalWrite(BUTTON_PIN_7, HIGH);
    delay(300);
    digitalWrite(BUTTON_PIN_7, LOW);
    request->send(200, "text/html; charset=utf-8", result);
}

void handle2(AsyncWebServerRequest *request)
{
    String result = "2 \r\n";
    digitalWrite(BUTTON_PIN_6, HIGH);
    delay(300);
    digitalWrite(BUTTON_PIN_6, LOW);
    request->send(200, "text/html; charset=utf-8", result);
}

void handle3(AsyncWebServerRequest *request)
{
    String result = "3 \r\n";
    digitalWrite(BUTTON_PIN_5, HIGH);
    delay(300);
    digitalWrite(BUTTON_PIN_5, LOW);
    request->send(200, "text/html; charset=utf-8", result);
}

void handle4(AsyncWebServerRequest *request)
{
    String result = "4 \r\n";
    digitalWrite(BUTTON_START_PIN_4, HIGH);
    delay(300);
    digitalWrite(BUTTON_START_PIN_4, LOW);
    request->send(200, "text/html; charset=utf-8", result);
}

void handle5(AsyncWebServerRequest *request)
{
    String result = "5 \r\n";
    digitalWrite(BUTTON_PIN_3, HIGH);
    delay(300);
    digitalWrite(BUTTON_PIN_3, LOW);
    request->send(200, "text/html; charset=utf-8", result);
}

void handle6(AsyncWebServerRequest *request)
{
    String result = "6 \r\n";
    digitalWrite(BUTTON_PIN_2, HIGH);
    delay(300);
    digitalWrite(BUTTON_PIN_2, LOW);
    request->send(200, "text/html; charset=utf-8", result);
}

void handle7(AsyncWebServerRequest *request)
{
    String result = "7 \r\n";
    digitalWrite(BUTTON_PIN_1, HIGH);
    delay(300);
    digitalWrite(BUTTON_PIN_1, LOW);
    request->send(200, "text/html; charset=utf-8", result);
}

void handle8(AsyncWebServerRequest *request)
{
    String result = "8 \r\n";
    digitalWrite(POWER_BUTTON_PIN, HIGH);
    delay(300);
    digitalWrite(POWER_BUTTON_PIN, LOW);
    request->send(200, "text/html; charset=utf-8", result);
}

void handleRestart(AsyncWebServerRequest *request)
{
    String result = "Restart \r\n";
    needRestart = true;
    request->send(200, "text/html; charset=utf-8", result);
}

void WebServerSetup()
{
    WiFi.disconnect(true);
    WiFi.softAPdisconnect(true);
    delay(100);
    WiFi.setHostname("RestNowww");
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin("Pytin-Xyilo", "BestCode123");
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000)
    {
        delay(100);
    }
    if (WiFi.status() == WL_CONNECTED)
        Serial.println("Connected! IP: " + WiFi.localIP().toString());
    else
        Serial.println("WiFi failed, using AP mode");
    WiFi.softAP("RestNoWWW", "12344321");
    WiFi.softAPConfig(ap_ip, ap_ip, ap_mask);
    Serial.println("AP mode, IP: " + WiFi.softAPIP().toString());
    // if (!SPIFFS.begin(true))
    // {
    //     Serial.println("SPIFFS failed");
    //     return;
    // }
    // Serial.println("SPIFFS OK");
    // if (!SPIFFS.exists("/index.html"))
    //     Serial.println("index.html not found");
    server.onNotFound(handleNotFound);
    server.on("/generate_204", HTTP_GET, handleGoogle);
    server.on("/javascript", HTTP_GET, onJavaScript);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", login_html); });
    server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", login_html); });
    server.on("/serverIndex", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", server_html); });
    server.on("/in", HTTP_GET, handleIn);
    server.on("/on", HTTP_GET, handleOn);
    server.on("/1", HTTP_GET, handle1);
    server.on("/2", HTTP_GET, handle2);
    server.on("/3", HTTP_GET, handle3);
    server.on("/4", HTTP_GET, handle4);
    server.on("/5", HTTP_GET, handle5);
    server.on("/6", HTTP_GET, handle6);
    server.on("/7", HTTP_GET, handle7);
    server.on("/8", HTTP_GET, handle8);
    server.on("/restart", HTTP_GET, handleRestart);
    server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        request->send(200, "text/plain", Update.hasError() ? "FAIL" : "OK");
        if (!Update.hasError()) ESP.restart(); }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
              {
        if (!index) { Serial.printf("Update Start: %s\n", filename.c_str()); Update.begin(UPDATE_SIZE_UNKNOWN); }
        if (Update.write(data, len) != len) Update.printError(Serial);
        if (final) { if (Update.end(true)) Serial.printf("Update Success: %u\n", index + len); else Update.printError(Serial); } });
    server.begin();
    dnsServer.setTTL(300);
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
    dnsServer.start(53, "*", WiFi.softAPIP());
}

// void WebServerSetup()
// {
//     WiFi.disconnect(true);
//     WiFi.mode(WIFI_OFF);
//     Serial.println("Wi-Fi відключено.");
// }

void WebServerRun() {}
void WebServerLoop()
{
    if ((WiFi.status() != WL_CONNECTED) && (millis() > 30000) && (WiFi.softAPgetStationNum() == 0))
    {
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
    }
    if ((WiFi.status() == WL_CONNECTED) || (WiFi.softAPgetStationNum() > 0))
    {
        dnsServer.processNextRequest();
    }
}

// void WebServerLoop() {}