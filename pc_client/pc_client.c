#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h> 
#include <sys/time.h>

struct client_request{
  char type[3];
  char src_fname[20];
  long total_len;
  char dst_fname[20];
  long msg_len;
};

void error(char *msg)
{
    perror(msg);
    exit(0);
}
/* Time in seconds from some point in the past */
double dwalltime();

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    if (argc < 4 || ( strcmp(argv[3],"GET")!=0 && strcmp(argv[3],"PUT")!=0 && strcmp(argv[3],"RM")!=0 && strcmp(argv[3],"LS")!=0)) {
       fprintf(stderr,"usage %s hostname port [GET|PUT] src_filename <bytes> <dst_filename> <msg_bytes>\n", argv[0]);
       fprintf(stderr,"                       RM src_filename\n");
       fprintf(stderr,"                       LS\n");
       exit(0);
    }
    else if  ( (strcmp(argv[3],"GET")==0||strcmp(argv[3],"PUT")==0) && argc<6) {
	   fprintf(stderr,"usage %s hostname port [GET|PUT] src_filename bytes <dst_filename> <msg_bytes>\n", argv[0]);
	   exit(0);
	}
	else if  (strcmp(argv[3],"RM")==0 && argc!=5) {
       fprintf(stderr,"                       RM src_filename\n");
	   exit(0);
	}
	

    char buffer[256];
	//TOMA EL NUMERO DE PUERTO DE LOS ARGUMENTOS
    portno = atoi(argv[2]);
	
	//CREA EL FILE DESCRIPTOR DEL SOCKET PARA LA CONEXION
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//AF_INET - FAMILIA DEL PROTOCOLO - IPV4 PROTOCOLS INTERNET
	//SOCK_STREAM - TIPO DE SOCKET 
	
    if (sockfd < 0) 
        error("ERROR opening socket");
	
	//TOMA LA DIRECCION DEL SERVER DE LOS ARGUMENTOS
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
	
	//COPIA LA DIRECCION IP Y EL PUERTO DEL SERVIDOR A LA ESTRUCTURA DEL SOCKET
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
     serv_addr.sin_port = htons(portno);
	
	//DESCRIPTOR - DIRECCION - TAMAÑO DIRECCION
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    //LEE DEL ARCHIVO PARAMETRO 3 LA LONGITUD PASADA POR PARAMETRO 4 
    //Y LA GUARDA EN buffer_f
    // 1:hostname 2:port 3:[GET|PUT] 4:src_filename 5:<bytes> 6:<dst_filename> 7:<msg_bytes>\n", argv[0]);
	if (strcmp(argv[3],"LS")==0) {
 		//ENVIA UN MENSAJE AL SOCKET CON LA LONGITUD A ENVIAR
		char req_msg[255];
		strcpy(req_msg, "LS ");
		int n = write(sockfd,req_msg,strlen(req_msg));
		//n = write(sockfd,argv[4],strlen(argv[4]));
		if (n < 0) 
		     error("ERROR writing to socket");
	}
	else if (strcmp(argv[3],"RM")==0) {
 		//ENVIA UN MENSAJE AL SOCKET CON LA LONGITUD A ENVIAR
		char req_msg[255];
		strcpy(req_msg, "RM ");
		strcat(req_msg, argv[4]);
		int n = write(sockfd,req_msg,strlen(req_msg));
		//n = write(sockfd,argv[4],strlen(argv[4]));
		if (n < 0) 
		     error("ERROR writing to socket");
	}
	
    // 1:hostname 2:port 3:[GET|PUT] 4:src_filename 5:<bytes> 6:<dst_filename> 7:<msg_bytes>\n", argv[0]);
	else if (strcmp(argv[3],"GET")==0){
 		//ENVIA UN MENSAJE AL SOCKET CON LA LONGITUD A ENVIAR
		char req_msg[255];
		strcpy(req_msg, "GET ");
		strcat(req_msg, argv[4]);
		strcat(req_msg, " ");
		strcat(req_msg, argv[5]);
		strcat(req_msg, " ");
		strcat(req_msg, argv[(argc>7?7:5)]);
		int n = write(sockfd,req_msg,strlen(req_msg));
		//n = write(sockfd,argv[4],strlen(argv[4]));
		if (n < 0) 
		     error("ERROR writing to socket");
		printf("Here is the length to receive: %s\n",argv[5]);
		char buffer_f[atoi(argv[5])];
		//LEE EL CUERPO DEL MENSAJE DEL CLIENTE EN buffer_f 

		int msg_len=atoi(argv[5]);
		int msg_received_size=0;
		n=0;
		while (n>=0 && msg_received_size < msg_len){ 
		     n=read(sockfd,buffer_f+msg_received_size,msg_len-msg_received_size);
		     if (n < 0) error("ERROR reading from socket");
		     printf("->%ld\n",(long unsigned)n);
		     msg_received_size+=n;
		}
		
    	//n = read(sockfd,buffer_f,sizeof(buffer_f));
		if (n < 0) error("ERROR reading from socket");
		printf("Readed bytes: %lu\n",(unsigned long)msg_received_size);
		printf("Saving received_msg...");

		//GUARDA EL CONTENIDO DE buffer_f EN EL ARCHIVO 'received_msg'
		//AUNQUE SE HAYAN RECIBIDO MENOS DE LO ESPERADO  
		FILE* f;
		f=fopen(argv[(argc>6?6:4)],"wb");
		fwrite(buffer_f,1,msg_received_size,f);
		fclose(f);
	}

    // 1:hostname 2:port 3:[GET|PUT] 4:src_filename 5:<bytes> 6:<dst_filename> 7:<msg_bytes>\n", argv[0]);
	else if  (strcmp(argv[3],"PUT")==0){
 		//ARMA EL MENSAJE A ENVIAR AL SERVER CON LA OPERACION A REALIZAR
		char req_msg[255];
		strcpy(req_msg, "PUT ");
		strcat(req_msg, argv[(argc>6?6:4)]);
		strcat(req_msg, " ");
		strcat(req_msg, argv[5]);
		strcat(req_msg, " ");
		strcat(req_msg, argv[(argc>7?7:5)]);
		printf("\nMensaje:'%s'\n",req_msg);
 		//ENVIA UN MENSAJE ARMADO
		int n = write(sockfd,req_msg,sizeof(req_msg));
		//n = write(sockfd,argv[4],strlen(argv[4]));
		if (n < 0) 
		     error("ERROR writing to socket");
        printf("mensaje enviado, inicia apertura de archivo fuente\n");

        //ABRE EL ARCHIVO PARAMETRO #4:src_filename
	    FILE *f, *f_sent, *f_rtt;
		if ((f = fopen(argv[4], "rb")) == NULL) 
			error("Error opening file");


        //DEFINE VARIABLES PARA CONTAR LOS BYTES RECIBIDOS, EL TAMAÑO MAXIMO POR ENVIO, Y TIEMPOS 
    	long to_send_size=atol(argv[5]);
        uint32_t received_size=0;
        long msg_max_size=atol(argv[(argc>7?7:5)]);
        long msg_size;
        double timetick, rtt;
        printf("inicia apertura de archivos\n");
        //ABRE ARCHIVOS PARA ESCRIBIR LO QUE ENVIA Y LOS rtt, Y PREPARA EL BUFFER 
        f_sent=fopen("sent_msg","wb");
        f_rtt=fopen("rtt.txt","w");
        char buffer_f[msg_max_size];

        printf("inicia envio\n");
        //MIENTRAS EL SERVIDOR NO RECIBIO TODO
        while (received_size<to_send_size){
            msg_size=(to_send_size-received_size>msg_max_size? msg_max_size: to_send_size-received_size);
            fseek(f,received_size,SEEK_SET);
            fread(buffer_f,sizeof(char), msg_size,f);
            long prev=received_size;
            //TOMA EL TIEMPO DE INICIO
            timetick = dwalltime();
            printf("enviando %lu\n",msg_size);
            n = write(sockfd,buffer_f,msg_size);//ASUMIENDO QUE EN LA ESCRITURA NO ESTA LIMITADO
            printf("->%d",n);
            if (n < msg_size) error("ERROR writing to socket all message");// SI FALLA LA ASUNCION
            n = read(sockfd,&received_size,sizeof(received_size));
            //TOMA EL TIEMPO DESDE INICIO EN MILISEGUNDOS 
            rtt=(dwalltime()-timetick)*1000;
            printf("sizeof(received_size)=%d\n",(int) sizeof(received_size));
            if (n < sizeof(received_size)) error("ERROR reading from socket size of received_size");
            //GUARDA LOS BYTES QUE RECIBIO EL SERVIDOR Y EL TIEMPO
            fprintf(f_rtt, "%f - %ld \n",rtt, received_size-prev);
            fwrite(buffer_f,sizeof(char), received_size-prev,f_sent);
        }
        fclose(f_sent);
        fclose(f_rtt);
        fclose(f);
        bzero(buffer,256);

 		close(sockfd);
		return 0;
	}
}
double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}
