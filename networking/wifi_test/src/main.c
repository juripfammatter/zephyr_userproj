#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/http/client.h>
#include <autoconf.h>

#define WIFI_SSID CONFIG_WIFI_SSID
#define WIFI_PASSWORD CONFIG_WIFI_PASSWORD

#define HTTP_HOST "192.168.1.120"
#define HTTP_PORT "1880"
#define HTTP_PATH "/esp_data"

#define REQUEST_TEMPLATE "GET " HTTP_PATH "?temp=%s HTTP/1.1\r\n" \
                         "Host: " HTTP_HOST ":" HTTP_PORT "\r\n"  \
                         "Connection: close\r\n"                  \
                         "\r\n"

// static struct http_client_request req;
// static struct http_client_response rsp;

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
    if (net_mgmt(NET_REQUEST_WIFI_CONNECT, iface, &wifi_params, sizeof(struct wifi_connect_req_params)))
    {
        printf("WiFi Connection Request Failed\n");
    }
    else
    {
        printf("WiFi Connection Request Sent\n");
    }
}

int send_http_get(float temperature)
{
    int sock, ret;
    struct sockaddr_in addr;
    char buf[4096];
    char request[256];
    char temp_str[10];

    printf("Preparing HTTP GET request for http://" HTTP_HOST ":" HTTP_PORT HTTP_PATH "\n");

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
    {
        printf("Failed to create socket: %d\n", sock);
        return -1;
    }

    printf("sock = %d\n", sock);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(HTTP_PORT));

    ret = inet_pton(AF_INET, HTTP_HOST, &addr.sin_addr);
    if (ret != 1)
    {
        printf("Invalid address: %d\n", ret);
        close(sock);
        return -1;
    }

    ret = connect(sock, (struct sockaddr *)&addr, (socklen_t)sizeof(addr));
    if (ret < 0)
    {
        printf("Failed to connect: %d\n", ret);
        close(sock);
        return -1;
    }
    else
    {
        printf("Connection successful\n");
    }

    // Convert float to string
    snprintf(temp_str, sizeof(temp_str), "%.2f", temperature);

    // Format the full request string
    snprintf(request, sizeof(request), REQUEST_TEMPLATE, temp_str);

    printf("Sending request: %s", request);
    ret = send(sock, request, sizeof(request), 0);
    if (ret < 0)
    {
        printf("Failed to send request: %d\n", ret);
        close(sock);
        return -1;
    }

    printf("Request sent, waiting for response...\n");

    do
    {
        ret = recv(sock, buf, sizeof(buf) - 1, 0);
        if (ret < 0)
        {
            if (errno == EINTR)
            {
                // Interrupted system call, try again
                continue;
            }
            printf("Error in recv: %d (%s)\n", errno, strerror(errno));
            break;
        }
        else if (ret == 0)
        {
            printf("Connection closed by server\n");
            break;
        }
        else
        {
            buf[ret] = '\0'; // Null-terminate the received data
            printf("Received %zd bytes: %s\n", ret, buf);
        }
    } while (ret > 0);

    close(sock);
    return 0;
}

int main(void)
{
    printf("WiFi Connection Test\n");
    printf("Trying to connect to %s\n", WIFI_SSID);

    connect_wifi();
    k_msleep(10000);

    while (true)
    {
        send_http_get(42.0);
        k_msleep(10000);
    }
    return 0;
}
