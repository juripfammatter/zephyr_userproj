#include <stdio.h>
#include <stdbool.h>    // booleans
#include <zephyr/kernel.h>
#include <autoconf.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_if.h>

#define WIFI_SSID CONFIG_WIFI_SSID
#define WIFI_PASSWORD CONFIG_WIFI_PASSWORD

void connect_wifi(void)
{
    struct net_if *iface = net_if_get_default();
    struct wifi_connect_req_params wifi_params = {
        .ssid = WIFI_SSID,
        .ssid_length = strlen(WIFI_SSID),
        .psk = WIFI_PASSWORD,
        .psk_length = strlen(WIFI_PASSWORD),
        .channel = WIFI_CHANNEL_ANY,
        .security = WIFI_SECURITY_TYPE_PSK,
    };

    if (net_mgmt(NET_REQUEST_WIFI_CONNECT, iface, &wifi_params, sizeof(struct wifi_connect_req_params))) {
        printk("WiFi Connection Request Failed\n");
    } else {
        printk("WiFi Connection Request Sent\n");
    }
}

int main(void)
{
	printf("WiFi Connection Test\n");
    printf("Trying to connect to %s\n", WIFI_SSID);
    
    while(true){
        connect_wifi();
        k_msleep(1000);
    }
	return 0;
}
