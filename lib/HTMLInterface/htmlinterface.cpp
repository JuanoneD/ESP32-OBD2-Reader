    #include "htmlinterface.h"
    
    String HTMLInterface::getHTML() {
    String html = "<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<style>";
    html += "body { font-family: -apple-system, sans-serif; background: #1c1c1e; color: white; text-align: center; padding: 20px; }";
    html += ".card { background: #2c2c2e; padding: 20px; border-radius: 20px; margin-bottom: 20px; box-shadow: 0 4px 15px rgba(0,0,0,0.3); }";
    html += ".litros { font-size: 48px; font-weight: bold; color: #30d158; }";
    html += "button { width: 100%; padding: 15px; margin: 10px 0; border: none; border-radius: 12px; font-size: 18px; font-weight: bold; cursor: pointer; }";
    html += ".btn-reset { background: #ff3b30; color: white; }"; // Vermelho
    html += ".btn-add { background: #007aff; color: white; }";   // Azul
    html += ".btn-save { background: #34c759; color: white; }";  // Verde
    html += "input { width: 100%; padding: 12px; border-radius: 8px; border: 1px solid #3a3a3c; background: #1c1c1e; color: white; margin-top: 10px; font-size: 16px; }";
    html += "</style></head><body>";

    html += "<h1>Fiesta Street</h1>";
    
    html += "<div class='card'>";
    html += "<div>Combustível Estimado</div>";
    html += "<div class='litros'>" + String(PreferencesHandle::getInstance().getFuel(), 1) + "L</div>";
    html += "<div>" + String((PreferencesHandle::getInstance().getFuel() / PreferencesHandle::getInstance().getTankCapacity()) * 100, 0) + "% do tanque</div>";
    html += "</div>";

    html += "<form action='/reset' method='POST'><button class='btn-reset'>RESET (TANQUE CHEIO)</button></form>";
    html += "<form action='/add10' method='POST'><button class='btn-add'>ADICIONAR 10 LITROS</button></form>";

    html += "<div class='card'>";
    html += "<h3>Calibração (Fator)</h3>";
    html += "<form action='/setfator' method='POST'>";
    html += "<input type='text' name='fator' value='" + String(PreferencesHandle::getInstance().getConsumptionFactor(), 12) + "'>";
    html += "<button class='btn-save'>SALVAR FATOR</button></form>";
    html += "</div>";

    html += "</body></html>";
    return html;
    }

    void HTMLInterface::handleRoot() {
    server.send(200, "text/html", getHTML());
    }

    void HTMLInterface::handleReset() {
    PreferencesHandle::getInstance().setFuel(PreferencesHandle::getInstance().getTankCapacity());
    server.sendHeader("Location", "/");
    server.send(303);
    }

    void HTMLInterface::handleAdd10() {
    PreferencesHandle::getInstance().setFuel(PreferencesHandle::getInstance().getFuel() + 10);
    if (PreferencesHandle::getInstance().getFuel() > PreferencesHandle::getInstance().getTankCapacity()) {
        PreferencesHandle::getInstance().setFuel(PreferencesHandle::getInstance().getTankCapacity());
    }
    server.sendHeader("Location", "/");
    server.send(303);
    }

    void HTMLInterface::handleSetFator() {
    if (server.hasArg("fator")) {
        PreferencesHandle::getInstance().setConsumptionFactor(server.arg("fator").toFloat());
    }
    server.sendHeader("Location", "/");
    server.send(303);
    }

    void HTMLInterface::begin() {
    WiFi.softAP("ESP32_PAINEL");
    server.on("/", std::bind(&HTMLInterface::handleRoot, this));
    server.on("/reset", HTTP_POST, std::bind(&HTMLInterface::handleReset, this));
    server.on("/add10", HTTP_POST, std::bind(&HTMLInterface::handleAdd10, this));
    server.on("/setfator", HTTP_POST, std::bind(&HTMLInterface::handleSetFator, this));
    server.begin();
    }

    void HTMLInterface::handleClient() {
    server.handleClient();
    }

    HTMLInterface::HTMLInterface() {
    }

