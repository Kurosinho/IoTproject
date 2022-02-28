#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#define MAX_BUF 128
#define PORT 4501
#define MAX_PARAMS 4

#define REGISTER 1
#define REPORT 2
#define COMMAND 3

#define HUMIDIFIER 1
#define THERMOMETER 2
#define AIR_CONDITIONER 3
#define HUMLED 4

#define ON 1
#define OFF 2


socklen_t addr_len = sizeof(struct sockaddr);
fd_set readfds;
int sockfd;
int amount=0, maxClients=5;
struct timeval timeout;
unsigned char buffer[MAX_BUF];




int main(){
    // Nieudana próba implementacji rulefile'a. Kod wygląda dobrze, ale z jakiegoś powodu nie działa i żadna z osób, która nań patrzyła, nie wie dlaczego...
    // FILE *in_file = fopen("rulefile.txt", "r");
    // int *params[MAX_PARAMS] = {};
    // char *states[MAX_PARAMS] = {};
    // char nodeStates[MAX_PARAMS];
    // size_t len = 0;
    // size_t buffer_size;
    // char line[150];
    // char *token;
    // int position = 0;
    // int turn = 0;


    // //for (int i = 0; (len = getline(&line, &buffer_size, in_file)) != -1; i++){
    // while (fgets(line, sizeof(line), in_file)){
    //     //printf("%4u: %4lu %s\n", i, len, line);

    //     /* get the first token */
        
    //     if(strstr(line, "THEN") != NULL){
    //         token = strtok(line, " ""); 
            
    //         while(token != NULL) {
                
    //             turn++;
    //             printf("turn: %d\n", turn);
    //             //printf("%s-\n", token);
                
    //             if (turn == 3){
    //                 position++;
    //                 printf("position: %d\n", position);
    //                 states[position] = token;
    //                 printf("token: %s\n", token);
    //                 printf("state on %d position: %s\n", position, states[position]);
                    
    //                 //printf("%s\n", states[position]);
                    
    //             }
    //             for (int a = 0; a < MAX_PARAMS; a++){
    //                 printf("state: %s\n", states[a]);
    //             }
                
    //             //printf("state: %s\0", &(states[position]));
    //             token = strtok(NULL, " ");
                
    //         }
            
    //     }
    //     turn = 0;    
    // }
    struct sockaddr_in servAddr, cliAddr, cliAddrs[5];
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = INADDR_ANY;
    memset(&servAddr.sin_zero, 0, sizeof(servAddr.sin_zero));
    memset(&cliAddr.sin_zero, 0, sizeof(cliAddr.sin_zero));

    printf("Server is up and running... \n");

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < maxClients; i++){
        cliAddrs[i] = servAddr;
    }

    if(bind(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr)) == -1){
        exit(EXIT_FAILURE);
    }

    for(;;){
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;

        int r_select = select(sockfd+1, &readfds, NULL, NULL, &timeout);

        if(r_select > 0)
        {
            if(FD_ISSET(sockfd, &readfds))
            {
                memset(&cliAddr, 0, sizeof(struct sockaddr));
                int pos = recvfrom(sockfd, (char *)buffer, MAX_BUF, 0, (struct sockaddr *)&cliAddr, &addr_len);
                buffer[pos] = '\0';

                int type = ((buffer[0] & 0b11000000) >> 6);
                int id = ((buffer[0] & 0b00111000) >> 3);
                int object = ((buffer[0] & 0b00000111));

                //printf("Received(%s:%d): %x\n", inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port), type);
                if (type == REGISTER){
                    for (int i = 0; i < maxClients; i++){
                        if (cliAddrs[i].sin_port == servAddr.sin_port){
                            cliAddrs[i] = cliAddr;
                            //printf("%d", ntohs(cliAddrs[i].sin_port));
                            amount++;
                            printf("New device added! Device no. %d\n", amount);
                            break;
                        }                    
                    }
                }
                if (type == REPORT){
                    memset(&cliAddr, 0, sizeof(struct sockaddr));
                    
                    //printf("%x\n", buffer[0]);
                    if (object == HUMIDIFIER){
                        printf("Air humidity: %d percent\n", buffer[1]);
                        if (buffer[1] >= 70){
                            printf("Air is too humid. Turn the dryer on!\n");

                            for (int i = 0; i < maxClients; i++){
                                if (ntohs(cliAddrs[i].sin_port) == 4505){
                                    cliAddr = cliAddrs[i];
                                    buffer[0] = (COMMAND << 6) + (ON << 3) + (HUMLED);
                                    sendto(sockfd, buffer, MAX_BUF, 0, (const struct sockaddr *)&cliAddr, addr_len);
                                    break;
                                }
                            }
                        }

                        if (buffer[1] <= 45) {
                            printf("Air is too dry. Turn the air dryer off!\n");

                            for (int i = 0; i < maxClients; i++){
                                if (ntohs(cliAddrs[i].sin_port) == 4505){
                                    cliAddr = cliAddrs[i];
                                    buffer[0] = (COMMAND << 6) + (OFF << 3) + (HUMLED);
                                    sendto(sockfd, buffer, MAX_BUF, 0, (const struct sockaddr *)&cliAddr, addr_len);
                                    break;
                                }
                            }
                        }
                    }
                    if (object == THERMOMETER){
                        printf("Air temperature: %d degrees\n", buffer[1]);
                        if (buffer[1] >= 25){
                            printf("Air is too hot! Turn the air conditiorner on!\n");

                            for (int i = 0; i < maxClients; i++){
                                if (ntohs(cliAddrs[i].sin_port) == 4504){
                                    cliAddr = cliAddrs[i];
                                    buffer[0] = (COMMAND << 6) + (ON << 3) + (AIR_CONDITIONER);
                                    sendto(sockfd, buffer, MAX_BUF, 0, (const struct sockaddr *)&cliAddr, addr_len);
                                    break;
                                }
                            }
                        }
                        if (buffer[1] <= 18){
                            printf("Air is too cold! Turn the air conditiorner off!\n");

                            for (int i = 0; i < maxClients; i++){
                                if (ntohs(cliAddrs[i].sin_port) == 4504){
                                    cliAddr = cliAddrs[i];
                                    buffer[0] = (COMMAND << 6) + (OFF << 3) + (AIR_CONDITIONER);
                                    sendto(sockfd, buffer, MAX_BUF, 0, (const struct sockaddr *)&cliAddr, addr_len);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    close(sockfd);
    return 0;
}