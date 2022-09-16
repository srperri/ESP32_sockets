#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include "helpers.h"
/*  ejemplo de compilación y ejecución
 *
 *  gcc -o pc_client pc_client.c helpers.c -Wall -lm
 *
 */

int main(int argc, char *argv[])
{
	int sockfd, portno, n, count;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	if (argc < 4 || (strcmp(argv[3], "GET") != 0 && strcmp(argv[3], "PUT") != 0 && strcmp(argv[3], "RM") != 0 && strcmp(argv[3], "LS") != 0))
	{
		fprintf(stderr, "usage %s hostname port [GET|PUT] src_filename <bytes> <dst_filename> <msg_bytes>\n", argv[0]);
		fprintf(stderr, "                       RM src_filename\n");
		fprintf(stderr, "                       LS\n");
		exit(0);
	}
	else if ((strcmp(argv[3], "GET") == 0 || strcmp(argv[3], "PUT") == 0) && argc < 6)
	{
		fprintf(stderr, "usage %s hostname port [GET|PUT] src_filename bytes <dst_filename> <msg_bytes>\n", argv[0]);
		exit(0);
	}
	else if (strcmp(argv[3], "RM") == 0 && argc != 5)
	{
		fprintf(stderr, "                       RM src_filename\n");
		exit(0);
	}

	// TOMA EL NUMERO DE PUERTO DE LOS ARGUMENTOS
	portno = atoi(argv[2]);

	// CREA EL FILE DESCRIPTOR DEL SOCKET PARA LA CONEXION
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	// AF_INET - FAMILIA DEL PROTOCOLO - IPV4 PROTOCOLS INTERNET
	// SOCK_STREAM - TIPO DE SOCKET

	if (sockfd < 0)
		error("ERROR opening socket");

	// TOMA LA DIRECCION DEL SERVER DE LOS ARGUMENTOS
	server = gethostbyname(argv[1]);
	if (server == NULL)
	{
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;

	// COPIA LA DIRECCION IP Y EL PUERTO DEL SERVIDOR A LA ESTRUCTURA DEL SOCKET
	bcopy((char *)server->h_addr,
		  (char *)&serv_addr.sin_addr.s_addr,
		  server->h_length);
	serv_addr.sin_port = htons(portno);

	// DESCRIPTOR - DIRECCION - TAMAÑO DIRECCION
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR connecting");

	// LEE DEL ARCHIVO PARAMETRO 3 LA LONGITUD PASADA POR PARAMETRO 4
	// Y LA GUARDA EN buffer
	//  1:hostname 2:port 3:[GET|PUT] 4:src_filename 5:<bytes> 6:<dst_filename> 7:<msg_bytes>\n", argv[0]);
	serv_req s_req;
	strcpy(s_req.type, (argv[3]));
	if (!strcmp(argv[3], "LS"))
	{
		// ENVIA AL SOCKET EL REQUERIMIENTO
		fixed_write(sockfd, &s_req, sizeof(serv_req), 0);
	}
	else if (!strcmp(argv[3], "RM"))
	{
		strcpy(s_req.fname, argv[4]);
		// ENVIA AL SOCKET EL REQUERIMIENTO
		fixed_write(sockfd, &s_req, sizeof(serv_req), 0);
	}

	// 1:hostname 2:port 3:[GET|PUT] 4:src_filename 5:<bytes> 6:<dst_filename> 7:<msg_bytes>\n", argv[0]);
	else if (!strcmp(argv[3], "GET"))
	{
		// ENVIA UN MENSAJE AL SOCKET CON LA LONGITUD A ENVIAR
		strcpy(s_req.fname, argv[4]);
		s_req.total_len = atol(argv[5]);
		s_req.msg_len = atol(argv[(argc > 7 ? 7 : 5)]);
		fixed_write(sockfd, &s_req, sizeof(serv_req), 0);
		printf("Here is the length to receive: %lu\n", (unsigned long)s_req.total_len);

		// ABRE EL ARCHIVO PARAMETRO #6:dst_filename
		FILE *f;
		f = fopen(argv[(argc > 6 ? 6 : 4)], "wb");
		if (f == NULL)
			error("Error opening file");

		// DEFINE VARIABLES PARA CONTAR LOS BYTES RECIBIDOS, EL TAMAÑO MAXIMO POR ENVIO, Y TIEMPOS
		uint32_t received_size = 0;
		uint32_t msg_size;
		// double timetick, rtt;
		printf("inicia apertura de archivos\n");

		char buffer[s_req.msg_len];
		while (received_size < s_req.total_len)
		{
			msg_size = min(s_req.msg_len, s_req.total_len - received_size);
			fixed_read(sockfd, buffer, msg_size, 1);
			received_size += msg_size;
			fixed_write(sockfd, &received_size, sizeof(received_size), 0);
			fwrite(buffer, sizeof(char), msg_size, f);
		}
		// n = read(sockfd,msg_buff,sizeof(buffer));
		printf("\nTotal received bytes: %lu\n", (unsigned long)received_size);
		fclose(f);
	}

	// 1:hostname 2:port 3:[GET|PUT] 4:src_filename 5:<bytes> 6:<dst_filename> 7:<msg_bytes>\n", argv[0]);
	else if (strcmp(argv[3], "PUT") == 0)
	{
		// ARMA EL REQUEST A ENVIAR AL SERVER CON LA OPERACION A REALIZAR
		strcpy(s_req.fname, argv[(argc > 6 ? 6 : 4)]);
		s_req.total_len = atol(argv[5]);
		s_req.msg_len = atol(argv[(argc > 7 ? 7 : 5)]);
		// ENVIA EL REQUEST printf("\nserv_req:'%s %s %lu %lu'\n", s_req.type, s_req.fname, (unsigned long)s_req.total_len, (unsigned long)s_req.msg_len);
		fixed_write(sockfd, &s_req, sizeof(serv_req), 0);

		// ABRE EL ARCHIVO PARAMETRO #4:src_filename
		printf("request sent, opening source file\n");
		FILE *f, *f_rtt;
		f = fopen(argv[4], "rb");
		if (f == NULL)
			error("Error opening file");

		// DEFINE VARIABLES PARA CONTAR LOS BYTES RECIBIDOS, EL TAMAÑO MAXIMO POR ENVIO, Y TIEMPOS
		uint32_t sent_size = 0;
		uint32_t msg_size;
		double timetick, rtt;

		// ABRE ARCHIVOS PARA ESCRIBIR LO QUE ENVIA Y LOS rtt, Y PREPARA EL BUFFER
		f_rtt = fopen("rtt.txt", "w");
		char buffer[s_req.msg_len];

		// MIENTRAS EL SERVIDOR NO RECIBIO TODO
		printf("start sending \n");
		while (sent_size < s_req.total_len)
		{
			msg_size = min(s_req.msg_len, s_req.total_len - sent_size);
			fread(buffer, sizeof(char), msg_size, f);
			// TOMA EL TIEMPO DE INICIO
			timetick = dwalltime();
			fixed_write(sockfd, buffer, msg_size, 1);
			fixed_read(sockfd, &sent_size, sizeof(sent_size), 0);
			// TOMA EL TIEMPO DESDE INICIO EN MILISEGUNDOS
			rtt = (dwalltime() - timetick) * 1000;
			// GUARDA LOS BYTES QUE RECIBIO EL SERVIDOR Y EL TIEMPO
			fprintf(f_rtt, "%f - %ld \n", rtt, (unsigned long)msg_size);
			printf("\nSent bytes count: %lu\n", (unsigned long)sent_size);
		}
		fclose(f_rtt);
		fclose(f);
	}

	close(sockfd);
	return 0;
}
