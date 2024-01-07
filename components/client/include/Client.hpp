#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"

#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "lwip/err.h"
#include "lwip/sys.h"

#include "Macros.hpp"
#include "AudioBuffer.hpp"

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static void eventHandler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);

static EventGroupHandle_t wifi_event_group;
static uint32_t connection_retry_num = 0U;

class Wifi {

public:
    bool init();
};

class Client {
private:
    bool _connected;
public:
    std::string _host_ip;
    int32_t _addr_family;
    int32_t _ip_protocol;
    struct sockaddr_in _dest_addr;
    int32_t _socket;

    Client(std::string host_ip);
    ~Client();
    bool init();
    bool connectToHost();
    void disconnectFromHost();
    bool sendAudioSamples(int32_t* samples);
};