#include "SoftwareSerial.h"
#include "esp_net.h"

#define WIFI_SSID F("emakefun")
#define WIFI_PASSWD F("501416wf")

namespace {
SoftwareSerial g_serial(3, 2);  // RX, TX
em::EspNet g_net_manager(g_serial);
}  // namespace

void setup() {
  Serial.begin(115200);
  g_serial.begin(9600);

  em::ErrorCode err = em::kOK;

  Serial.println("module init");
  err = g_net_manager.Init();
  if (err != em::kOK) {
    Serial.print("module init failed: ");
    Serial.println(ToString(err));
    while (true);
  }

  Serial.println("wifi connecting...");
  err = g_net_manager.ConnectWifi(WIFI_SSID, WIFI_PASSWD);
  if (err != em::kOK) {
    Serial.print("wifi connect failed: ");
    Serial.println(ToString(err));
    while (true);
  }

  Serial.println("wifi connected");
  delay(2000);
}

void loop() {
  em::net::IpAddress local_ip;
  if (em::kOK == g_net_manager.LocalIpAddress(&local_ip)) {
    Serial.print("local ip: ");
    Serial.println(local_ip.ToString());
  }

  em::net::MacAddress mac_address;
  g_net_manager.MacAddress(&mac_address);
  if (em::kOK == g_net_manager.MacAddress(&mac_address)) {
    Serial.print("mac: ");
    Serial.println(mac_address.ToString());
  }

  String ssid;
  if (em::kOK == g_net_manager.Ssid(&ssid)) {
    Serial.print("ssid: ");
    Serial.println(ssid);
  }

  em::net::MacAddress bssid;
  if (em::kOK == g_net_manager.Bssid(&bssid)) {
    Serial.print("bssid: ");
    Serial.println(bssid.ToString());
  }

  delay(2000);
}