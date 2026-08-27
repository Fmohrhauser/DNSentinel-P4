idf_component_register(SRCS
                        "ethernet.c"
                        "dns_server.c"
                        "main.c"
                       PRIV_REQUIRES esp_netif esp_eth esp_event lwip
                       INCLUDE_DIRS "include")
