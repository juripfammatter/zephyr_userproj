#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/net/wifi_mgmt.h>
// #include <zephyr/net/net_if.h>
#include <zephyr/net/socket.h>
// #include <zephyr/net/http/client.h>
#include <autoconf.h> // final kconfig defines

char WIFI_SSID[] = CONFIG_WIFI_SSID;
char WIFI_PASSWORD[] = CONFIG_WIFI_PASSWORD;

#define HTTP_HOST "192.168.1.120"
#define HTTP_PORT "1880"
#define HTTP_PATH "/esp_data"

#define REQUEST_TEMPLATE                                                                           \
	"GET " HTTP_PATH "?temp=%s HTTP/1.1\r\n"                                                   \
	"Host: " HTTP_HOST ":" HTTP_PORT "\r\n"                                                    \
	"Connection: close\r\n"                                                                    \
	"\r\n"

#define MAX_RETRY_TIME_MS 30000 // Maximum time to wait for IP (30 seconds)
#define RETRY_INTERVAL_MS 5000

// static struct http_client_request req;
// static struct http_client_response rsp;

int connect_wifi(void)
{
	if (strcmp(WIFI_SSID, "") == 0) {
		printf("No SSID given, aborting\n");
		return -1;
	}
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
	if (net_mgmt(NET_REQUEST_WIFI_CONNECT, iface, &wifi_params,
		     sizeof(struct wifi_connect_req_params))) {
		printf("WiFi Connection Request Failed\n");
	} else {
		printf("WiFi Connection Request Sent\n");
	}

	// Wait for IP address
	int retry_count = 0;
	while (retry_count * RETRY_INTERVAL_MS < MAX_RETRY_TIME_MS) {
		struct net_if_ipv4 *ipv4 = iface->config.ip.ipv4;
		if (ipv4 && ipv4->unicast[0].ipv4.is_used &&
		    ipv4->unicast[0].ipv4.address.in_addr.s4_addr32[0] != 0) {
			char ip_str[NET_IPV4_ADDR_LEN];
			net_addr_ntop(AF_INET, &ipv4->unicast[0].ipv4.address.in_addr, ip_str,
				      sizeof(ip_str));
			printf("IP address obtained: %s\n", ip_str);
			return 0;
		} else {
			printf("Waiting for IP allocation\n");
		}

		k_msleep(RETRY_INTERVAL_MS);
		retry_count++;
	}

	return -1;
}

int send_http_get(float temperature)
{
	int sock, ret;
	struct sockaddr_in addr;
	char buf[4096];
	char request[256];
	char temp_str[10];

	printf("Preparing HTTP GET request for http://" HTTP_HOST ":" HTTP_PORT HTTP_PATH "\n");

	sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		printf("Failed to create socket: %d\n", sock);
		return -1;
	}

	printf("sock = %d\n", sock);

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(strtol(HTTP_PORT, NULL, 10));

	ret = zsock_inet_pton(AF_INET, HTTP_HOST, &addr.sin_addr);
	if (ret != 1) {
		printf("Invalid address: %d\n", ret);
		zsock_close(sock);
		return -1;
	}

	ret = zsock_connect(sock, (struct sockaddr *)&addr, (socklen_t)sizeof(addr));
	if (ret < 0) {
		printf("Failed to connect: %d\n", ret);
		zsock_close(sock);
		return -1;
	} else {
		printf("Connection successful\n");
	}

	// Convert float to string
	snprintf(temp_str, sizeof(temp_str), "%.2f", (double)temperature);

	// Format the full request string
	snprintf(request, sizeof(request), REQUEST_TEMPLATE, temp_str);

	// printf("Sending request: %s", request);
	ret = zsock_send(sock, request, sizeof(request), 0);
	if (ret < 0) {
		printf("Failed to send request: %d\n", ret);
		zsock_close(sock);
		return -1;
	}

	printf("Request sent, waiting for response...\n");

	do {
		ret = zsock_recv(sock, buf, sizeof(buf) - 1, 0);
		if (ret < 0) {
			if (errno == EINTR) {
				// Interrupted system call, try again
				continue;
			}
			printf("Error in recv: %d (%s)\n", errno, strerror(errno));
			break;
		} else if (ret == 0) {
			printf("Connection closed by server\n");
			break;
		} else {
			buf[ret] = '\0'; // Null-terminate the received data
			printf("Received %zd bytes: %s\n", ret, buf);
		}
	} while (ret > 0);

	printf("closing socket %i\n", sock);
	zsock_close(sock);
	return 0;
}

int main(void)
{
	printf("WiFi Connection Test\n");
	printf("Trying to connect to %s\n", WIFI_SSID);

	int ret = connect_wifi();
	if (ret != 0) {
		return -1;
	}
	// k_msleep(10000);

	while (true) {
		send_http_get((float)42.0);
		k_msleep(5000);
	}
	return 0;
}
