#include "FS.h"
#include "SPIFFS.h"
#include "esp_spiffs.h"
#define FORMAT_SPIFFS_IF_FAILED true
#include <lwip/sockets.h>
#include <WiFi.h>
#include "helpers.h"
#include "put.h"
#include "get.h"
#include "ls.h"
#define SSID "yourAP"
#define PASSWORD "yourPassword"
#define LED_BUILTIN 2 // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

/*  ejemplo de uso
 *
 *  192.168.4.1 3000 LS
 *  192.168.4.1 3000 GET copyPR1 225730 copyPR1 10000
 *  192.168.4.1 3000 PUT copyPR1 225730 copyPR1 10000
 *
 */

void setWifi()
{
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.printf("\n WiFi connected\n IP address: %s\n", WiFi.localIP());
}

void cline_from_serial(cline_req *req)
{
    char buffer[256];
    uint8_t i = 0;
    while (!Serial.available())
    {
        delay(100);
    }
    while (Serial.available())
    {
        buffer[i++] = Serial.read();
    }
    buffer[--i] = 0;
    Serial.printf("message: <%s> \n", buffer);
    cline_req_from_char_array(req, buffer);
}

void setWifiConn()
{
    const char *ssid = "yourAP";
    const char *password = "yourPassword";
    // We start by connecting to a WiFi network

    Serial.print("\n\nConnecting to: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\n WiFi connected");
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED on

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Gateway IP: ");
    Serial.println(WiFi.gatewayIP());
}

int socketConn(IPAddress ip, int port)
{
    struct sockaddr_in serv_addr;
    int n;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    //  char server[] = "192.168.4.1";
    // inet_addr(server)
    serv_addr.sin_addr.s_addr = ip; // WiFi.gatewayIP();
    serv_addr.sin_port = htons(port);

    Serial.println("punto 3 ");

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    return sockfd;
}

int process_req(int sockfd, cline_req *cl_req)
{
    Serial.printf("cl_req->op= \"%s\") \n", cl_req->op);
    Serial.printf("strcmp(cl_req->op, \"GET\")=%d \n", strcmp(cl_req->op, "GET"));
    if ((strcmp(cl_req->op, "GET") && strcmp(cl_req->op, "PUT") && strcmp(cl_req->op, "RM") && strcmp(cl_req->op, "LS")))
    {
        Serial.printf("usage hostname port [GET|PUT] src_filename <bytes> <dst_filename> <msg_bytes>\n");
        Serial.printf("                     RM src_filename\n");
        Serial.printf("                     LS\n");
        return 0;
    }
    else if ((!strcmp(cl_req->op, "GET") || !strcmp(cl_req->op, "PUT")) && cl_req->total_len == 0)
    {
        Serial.printf("usage hostname port [GET|PUT] src_filename bytes <dst_filename> <msg_bytes>\n");
        return 0;
    }
    else if (!strcmp(cl_req->op, "RM") && !strlen(cl_req->src_fname))
    {
        fprintf(stderr, "                       RM src_filename\n");
        return 0;
    }
    serv_req s_req;
    strcpy(s_req.op, cl_req->op);
    if (!strcmp(s_req.op, "LS"))
    {
        fixed_write(sockfd, &s_req, sizeof(serv_req), 0);
    }
    else if (!strcmp(s_req.op, "RM"))
    {
        strcpy(s_req.fname, cl_req->src_fname);
        fixed_write(sockfd, &s_req, sizeof(serv_req), 0);
    }
    else if (!strcmp(s_req.op, "GET"))
    {
        get(sockfd, cl_req);
    }
    else if (!strcmp(s_req.op, "PUT"))
    {
        put(sockfd, cl_req);
    }

    close(sockfd);
    return 0;
}
void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW); // Turn the LED off
    Serial.begin(115200);
    delay(10);
    setWifiConn();
    if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
    {
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    // monta SPIFFS en /
    esp_vfs_spiffs_conf_t conf = {
        .base_path = SPIFFS_BASE_PATH,
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true};
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
}

void loop()
{
    ls(SPIFFS, "/", 0);
    cline_req cl_req;
    cline_from_serial(&cl_req);
    serv_req srv_req;

    int sockfd = socketConn(inet_addr(cl_req.host), cl_req.port);
    if (sockfd > 0)
    {
        process_req(sockfd, &cl_req);
        close(sockfd);
        Serial.print(">");
        delay(2000);
    }
}