#include "FS.h"
#include "SPIFFS.h"
#include "esp_spiffs.h"
/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true
#include <lwip/sockets.h>
//#include <netinet/in.h> 

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "helpers.h"
#include "putfile.h"
#include "getfile.h"
//#include "writefile.h"
#include "listdir.h"
#define SSID "yourAP"
#define PASSWORD "yourPassword"
#define LED_BUILTIN 2   // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

// Set these to your desired credentials.


void setWifi(){
    // const char *ssid = "yourAP";
    // const char *password = "yourPassword";
    Serial.println();
    Serial.println("Configuring access point...");
  
    // You can remove the password parameter if you want the AP to be open.
    WiFi.softAP(SSID, PASSWORD);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    
}

int openSocket(int port){
    int sockfd, n;
    struct sockaddr_in serv_addr;
    
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);                     //CREA EL FILE DESCRIPTOR DEL SOCKET PARA LA CONEXION
    if (sockfd < 0)
        perror("ERROR opening socket");
    Serial.write("SUCCESS opening socket");
        
    int bsize=0;
    socklen_t optlen;
    optlen = sizeof(bsize);
  
    int retv_gso = getsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&bsize,&optlen);
    Serial.printf("getsockopt retv = %d. err msg=%s\n",retv_gso,strerror(errno));
    Serial.printf("rcv Buf Size = %d\n", bsize);
int a = 4095;//1023;//65535;
if ( (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &a ,sizeof(int)) ) < 0 )
{
    Serial.printf("Error setting sock opts..\n");
}
    retv_gso = getsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&bsize,&optlen);
    Serial.printf("getsockopt retv = %d. err msg=%s\n",retv_gso,strerror(errno));
    Serial.printf("rcv Buf Size = %d\n", bsize);


  
    bzero((char *) &serv_addr, sizeof(serv_addr));               //ASIGNA EL PUERTO Y LA IP EN DONDE ESCUCHA (SU PROPIA IP)
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
     
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)  //VINCULA EL FILE DESCRIPTOR CON LA DIRECCION Y EL PUERTO
         perror("ERROR on binding");
    Serial.write("SUCCESS on binding");
     
    listen(sockfd,5);                                            //SETEA LA CANTIDAD QUE PUEDEN ESPERAR MIENTRAS SE MANEJA UNA CONEXION     
    return sockfd;
}

int waitCon(int sockfd){
    int newsockfd;
    struct sockaddr_in cli_addr;
    socklen_t clilen=sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);   // SE BLOQUEA A ESPERAR UNA CONEXION    
    digitalWrite(LED_BUILTIN, HIGH);
    return newsockfd;
}
           
void recReq(int newsockfd, request *req){
    int n = read(newsockfd,req,sizeof(request));                                        //LEE EL MENSAJE DEL CLIENTE CON LA LONGITUD
    if (n < 0)
        perror("ERROR reading from socket");
}






int sockfd;
void setup() {

  Serial.begin(115200);
  delay(10);

  setWifi();
  sockfd=openSocket(3000);

  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
      Serial.println("SPIFFS Mount Failed");
      return;
  }    
  listDir(SPIFFS, "/", 0);

  // monta SPIFFS en /
  esp_vfs_spiffs_conf_t conf = {
    .base_path = "/spiffs",
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = true
  };

  // Use settings defined above to initialize and mount SPIFFS filesystem.
  // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
  esp_err_t ret = esp_vfs_spiffs_register(&conf);
   
}
 
void loop() {
  int newsockfd;
  Serial.println(">");
  Serial.println("> waiting message ...");
  if (newsockfd < 0)
    error("ERROR on accept");
  newsockfd=waitCon(sockfd);
  request req;
  recReq(newsockfd, &req);
  if (!strcmp(req.type,"GET")){
    get_file(newsockfd,&req);
  }
  if (!strcmp(req.type,"PUT")){
    put_file(newsockfd,&req);    
  }
  if (!strcmp(req.type,"RM")){
    Serial.printf("borrar... : %s \n",req.fname);
    unlink(req.fname);    
  }
  delay(2000);
  close(newsockfd);
  Serial.println("");
  listDir(SPIFFS, "/", 0); 

}
