#include "Client.hpp"

static void eventHandler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data) {
    
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (connection_retry_num < 5U) {
            esp_wifi_connect();
            connection_retry_num++;
            printf( "retry to connect to the AP\n");
        } else {
            xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
        }
        printf("connect to the AP fail\n");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        // ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        connection_retry_num = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

bool Wifi::init() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &eventHandler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &eventHandler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            /* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (pasword len => 8).
             * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
             * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
             * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
             */
            .threshold = {
                .authmode = WIFI_AUTH_WPA2_PSK,
            },
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
            .sae_h2e_identifier = "",
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    printf( "wifi_init_sta finished\n");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        printf( "connected to ap SSID:%s\n",
                 WIFI_SSID);
    } else if (bits & WIFI_FAIL_BIT) {
        printf( "Failed to connect to SSID:%s\n",
                 WIFI_SSID);
        return false;
    } else {
        printf("UNEXPECTED EVENT\n");
        return false;
    }

    return true;
}

/*----------------------------------------------------------------------------------------------------*/

Client::Client(std::string host_ip) {
    _host_ip = host_ip;
    _addr_family = 0U;
    _ip_protocol = 0U;
    _socket = -1;
    _connected = false;
}

Client::~Client() {
    disconnectFromHost();
}

bool Client::init() {
    inet_pton(AF_INET, _host_ip.c_str(), &_dest_addr.sin_addr);
    _dest_addr.sin_family = AF_INET;
    _dest_addr.sin_port = htons(PORT);
    _addr_family = AF_INET;
    _ip_protocol = IPPROTO_IP;

    return true;
}

bool Client::connectToHost() {
    int32_t err;

    if (false == _connected) {
        _socket =  socket(_addr_family, SOCK_STREAM, _ip_protocol);
        if (_socket < 0) {
            printf("Unable to create socket\n");
            return false;
        }
        printf("Socket created, connecting to %s:%d\n", _host_ip.c_str(), PORT);

        err = connect(_socket, (struct sockaddr *)&_dest_addr, sizeof(_dest_addr));
        if (err != 0) {
            printf("Socket unable to connect\n");
            return false;
        }
        printf("Successfully connected\n");
        _connected = true;
    }
    
    return true;
}

void Client::disconnectFromHost() {
    if (true == _connected) {
        if (-1 != _socket) {
            printf("Successfully disconnected\n");
            shutdown(_socket, 0);
            close(_socket);
            _socket = -1;
            _connected = false;
            return;
        }
    }
    printf("Warning: Client not connected to host\n");
}

bool Client::sendAudioSamples(int32_t* samples) {
    int32_t err;

    for (std::size_t i = 0u; i < AUDIO_BUFFER_SIZE; ++i) {
        samples[i] = samples[i] >> 8;
    }
    if (true == _connected) {
        err = send(_socket, samples, AUDIO_BUFFER_SIZE * sizeof(int32_t), 0);
        if (err < 0) {
            printf("Error occurred during sending\n");
            return false;
        }
        return true;
    } else {
        printf("Warning: Client not connected to host\n");
        return false;
    }
}