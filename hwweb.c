#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 1024

int access(const char *pathname, int mode);


void createHtml() {
    FILE *fp = fopen("main.html", "w"); // "w" 모드로 파일 생성 또는 열기
    if (fp == NULL) { // 파일 열기에 실패한 경우
        printf("Failed to create or open file.\n");
        return;
    }
    fprintf(fp, "<html>\n<head>\n<title>Hello, world!</title>\n</head>\n<body>\n<h1>Hello, world!</h1>\n</body>\n</html>"); // 파일에 HTML 코드 작성
    fclose(fp); // 파일 닫기
}

void checkHtml() {
	if (access("main.html", F_OK) != -1) {
        printf("main.html file exists.\n");
    } 
	else {
        printf("main.html file does not exist. Running creatHtml() function.\n");
        createHtml();
    }
}

void send_file(int sock, const char *file_path) {
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) {
        perror("Failed to open file");
        exit(1);
    }

    char buffer[BUF_SIZE];
    int n;

    // 파일 내용을 읽어서 클라이언트로 전송
    while ((n = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
        if (send(sock, buffer, n, 0) == -1) {
            perror("Failed to send file");
            exit(1);
        }
    }

    fclose(fp);
}

int main() {
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[BUF_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
	
	checkHtml();

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Failed to open socket");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 80;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Failed to bind socket");
        exit(1);
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("Failed to accept connection");
            exit(1);
        }

        // HTTP 요청을 받아서 무시
        while ((n = recv(newsockfd, buffer, BUF_SIZE - 1, 0)) > 0) {
            if (strstr(buffer, "\r\n\r\n")) {
                break;
            }
        }

        // main.html 파일을 전송
        send_file(newsockfd, "main.html");

        close(newsockfd);
	break;
    }

    close(sockfd);
	
    return 0;
}
