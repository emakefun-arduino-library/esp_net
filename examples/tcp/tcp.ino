#include "SoftwareSerial.h"
#include "esp_net.h"
#include "tcp_session.h"

#define WIFI_SSID F("emakefun")
#define WIFI_PASSWD F("501416wf")

namespace {
SoftwareSerial g_serial(3, 2);  // RX, TX
em::EspNet g_net_manager(g_serial);
em::TcpSession g_tcp_session_0(g_serial,
                               "192.168.1.30",  // remote ip
                               5000             // remote port
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

  Serial.println("tcp opening...");
  if (!g_tcp_session_0.Open()) {
    Serial.println("tcp open failed");
    while (true);
  }

  Serial.println("tcp connected");
}

void loop() {
  if (!g_tcp_session_0.Connected()) {
    Serial.println("tcp disconnected");
    delay(2000);
    return;
  }

  if (g_tcp_session_0.Available() > 0) {
    auto received_data = g_tcp_session_0.Read();
    Serial.print("received by ");
    Serial.print(received_data.remote_ip);
    Serial.print(':');
    Serial.print(received_data.remote_port);
    Serial.print(" content: ");
    Serial.println(received_data.data);
  }

  const char send_data[] = "data 123456789!!!";
  g_tcp_session_0.Write(send_data, sizeof(send_data));
  delay(100);
}