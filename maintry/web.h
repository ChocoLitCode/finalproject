#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <FS.h>
#include <LittleFS.h>

String processor(const String& var);
String getTemperature();
String getHumidity();