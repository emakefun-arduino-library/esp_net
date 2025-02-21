#include "SoftwareSerial.h"
#include "esp_net.h"
#include "udp_session.h"

#define WIFI_SSID F("emakefun")
#define WIFI_PASSWD F("501416wf")

namespace {
SoftwareSerial g_serial(5, 6);  // RX, TX
em::EspNet g_net_manager(g_serial);
em::UdpSession g_udp_session(g_serial,
                             "192.168.1.30",  // remote ip
                             5000,            // remote port
                             6000             // local port
);

}  // namespace

void setup() {
  Serial.begin(115200);
  g_serial.begin(9600);

  em::ErrorCode ret = em::kOK;

  Serial.println("net module init");
  ret = g_net_manager.Init();

  if (ret != em::kOK) {
    Serial.print("net module init failed: ");
    Serial.print(ToString(ret));
    while (true);
  }

  em::net::MacAddress mac_address;
  ret = g_net_manager.MacAddress(&mac_address);
  if (ret != em::kOK) {
    Serial.print("get mac address failed: ");
    Serial.print(ToString(ret));
    while (true);
  }

  Serial.print("mac address: ");
  Serial.println(mac_address.ToString());

  Serial.println("wifi connecting...");
  ret = g_net_manager.ConnectWifi(WIFI_SSID, WIFI_PASSWD);
  if (ret != em::kOK) {
    Serial.print("wifi connect failed: ");
    Serial.println(ToString(ret));
    while (true);
  }

  em::net::IpAddress local_ip;
  if (em::kOK == g_net_manager.LocalIpAddress(&local_ip)) {
    Serial.print("local ip: ");
    Serial.println(local_ip.ToString());
  }

  Serial.println("udp opening...");
  if (!g_udp_session.Open()) {
    Serial.println("udp open failed");
    while (true);
  }

  Serial.println("udp connected");
}

void loop() {
  if (!g_udp_session.Connected()) {
    delay(2000);
    return;
  }

  if (g_udp_session.Available() > 0) {
    auto received_data = g_udp_session.Read();
    Serial.print("received by ");
    Serial.print(received_data.remote_ip);
    Serial.print(':');
    Serial.print(received_data.remote_port);
    Serial.print(" content: ");
    Serial.println(received_data.data);
  }

  const char send_data[] = "data 123456789!!!";
  g_udp_session.Write(send_data, sizeof(send_data));
  delay(10);
}