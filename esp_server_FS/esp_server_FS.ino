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

void setWifi()
{
    Serial.println();
    Serial.println("Configuring access point...");
    WiFi.softAP(SSID, PASSWORD);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
}

int openSocket(int port)
{
    int sockfd, n;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // CREA EL FILE DESCRIPTOR DEL SOCKET PARA LA CONEXION
    if (sockfd < 0)
        perror("ERROR opening socket");
    Serial.write("SUCCESS opening socket");


    bzero((char *)&serv_addr, sizeof(serv_addr)); // ASIGNA EL PUERTO Y LA IP EN DONDE ESCUCHA (SU PROPIA IP)
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) // VINCULA EL FILE DESCRIPTOR CON LA DIRECCION Y EL PUERTO
        perror("ERROR on binding");
    Serial.write("SUCCESS on binding");

    listen(sockfd, 5); // SETEA LA CANTIDAD QUE PUEDEN ESPERAR MIENTRAS SE MANEJA UNA CONEXION
    return sockfd;
}

int waitCon(int sockfd)
{
    int newsockfd;
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen); // SE BLOQUEA A ESPERAR UNA CONEXION
    if (newsockfd < 0)
        error("ERROR on accept");
    return newsockfd;
}

void recReq(int newsockfd, serv_req *req)
{
    Serial.println("> waiting request ...");

    int n, count = 0;
    while (count < sizeof(serv_req))
    {
        n = read(newsockfd, req + count, sizeof(serv_req) - count); // LEE EL MENSAJE DEL CLIENTE CON LA LONGITUD
        if (n < 0)
            perror("ERROR reading from socket");
        count += n;
    }
    Serial.printf("> request received for op %s... \n", req->op);
}

int sockfd;
void setup()
{

    Serial.begin(115200);
    delay(10);
    pinMode(LED_BUILTIN, OUTPUT);
    setWifi();
    sockfd = openSocket(3000);

    if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
    {
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    ls(SPIFFS, "/", 0);

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
    int newsockfd;
    Serial.println(">");
    Serial.println("> waiting connection ...");
    newsockfd = waitCon(sockfd);
    serv_req req;
    recReq(newsockfd, &req);
    digitalWrite(LED_BUILTIN, HIGH);
    if (!strcmp(req.op, "GET"))
    {
        get(newsockfd, &req);
    }
    if (!strcmp(req.op, "PUT"))
    {
        put(newsockfd, &req);
    }
    if (!strcmp(req.op, "RM"))
    {
        Serial.printf("borrar... : %s \n", req.fname);
        unlink(full_path(req.fname));
    }
    delay(2000);
    digitalWrite(LED_BUILTIN, LOW);
    close(newsockfd);
    Serial.println("");
    ls(SPIFFS, "/", 0);
}
