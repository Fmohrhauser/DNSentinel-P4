#include <errno.h>

#include "freertos/freeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "lwip/sockets.h"
#include "lwip/inet.h"

#include "dns_server.h"

static const char *TAG = "DNS_Server";

static void dns_listener_task(void *pvParameters)
{
    char rx_buffer[512];

    struct sockaddr_in listen_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(53),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(sock < 0) {
        ESP_LOGE(TAG, "Failed to create DNS socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }

    if (bind(sock, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) < 0) {
        ESP_LOGE(TAG, "Failed to bind DNS socket: errno %d", errno);
        close(sock);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "DNS listener started on UDP port 53");

    while(1) {
        struct sockaddr_in source_addr;
        socklen_t source_addr_len = sizeof(source_addr);

        int len = recvfrom(
            sock,
            rx_buffer,
            sizeof(rx_buffer),
            0,
            (struct sockaddr *)&source_addr,
            &source_addr_len
        );

        if (len < 0) {
            ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
            continue;
        }

        char source_ip[INET_ADDRSTRLEN];

        inet_ntoa_r(
            source_addr.sin_addr,
            source_ip,
            sizeof(source_ip)
        );

        ESP_LOGI(
            TAG,
            "DNS packet received from %s:%u (%d bytes)",
            source_ip,
            ntohs(source_addr.sin_port),
            len
        );
    }
    
}

void dns_server_start(void)
{
    xTaskCreate(
        dns_listener_task,
        "dns_listener",
        4096,
        NULL,
        5,
        NULL
    );
}