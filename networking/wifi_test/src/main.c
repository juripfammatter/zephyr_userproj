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
    struct wifi_connect_req_params wifi_params = {0};
    
    wifi_params.ssid = WIFI_SSID;
    wifi_params.ssid_length = strlen(WIFI_SSID);
    wifi_params.psk = WIFI_PASSWORD;
    wifi_params.psk_length = strlen(WIFI_PASSWORD);
    wifi_params.channel = WIFI_CHANNEL_ANY;
    wifi_params.security = WIFI_SECURITY_TYPE_PSK;
    wifi_params.band = WIFI_FREQ_BAND_2_4_GHZ; 
    wifi_params.mfp = WIFI_MFP_OPTIONAL;

    k_msleep(1000);
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
    
    connect_wifi();

    while(true){
        
        k_msleep(1000);
    }
	return 0;
}
