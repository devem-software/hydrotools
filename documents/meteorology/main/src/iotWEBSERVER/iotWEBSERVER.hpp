#include "iotWEBSERVER.h"
#include "index.h"

void handle_OnConnect();
void handle_ApiData();
void handle_NotFound();
String dataJson(int d, float t, float h);

void InitializeWebServer()
{
  server.on("/", []()
            { server.send(200, "text/html", index_html); });
  server.on("/data", []()
            { dataJson(dataTime, t, h);
              server.send(200, "application/json", dataWebServer); });
  server.onNotFound([]()
                    { server.send(404, "text/plain", "Opps! regrese a la pagina anterior"); });
  server.begin();
}

void updateWebServer()
{
  server.handleClient();
}

void handle_OnConnect()
{
  server.send(200, "text/html", index_html);
}

void handle_ApiData()
{
  // String data = dataJson(dataTime, t, h);
  server.send(200, "application/json", dataWebServer);
}

void handle_NotFound()
{
  server.send(404, "text/plain", "Opps! regrese a la pagina anterior");
}

String dataJson(int d, float t, float h)
{
  String dataJ = "{ \"d\": ";
  dataJ += String(d);
  dataJ += " , \"t\" : ";
  dataJ += String(t);
  dataJ += " , \"h\" : ";
  dataJ += String(h);
  dataJ += "}";

  dataWebServer = dataJ;
}
