/* 
 * File:   Server.cpp
 * Author: alok vardhan
 * this file controls all the operation pertaining to server
 * Created on September 14, 2014, 12:07 PM
 */
#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include<signal.h>
#include<iostream>

#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <cstring>
#include <arpa/inet.h>
#include<iostream>
#include "Server.h"
#include<vector>
#include<string>
#include<stdio.h>
#include<cstring>
#ifndef _SSTREAM_TCC
#include<sstream>
#endif
#include<iostream>
#include "Common.h"
#define MAXDATASIZE 1000
#define BACKLOG 10 
#define STDIN 0
typedef std::map<string, int >::iterator it_type;
using namespace std;

int PORT_s;
Common comm;
/*sigchld_handler function
 * ref:beejs guide
 */
void sigchld_handler(int s) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

/*get_in_addr function
 * function to get sockaddr, IPv4 or IPv6:
 * ref:beejs guide
 */
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}
/*
 * shell function
 * this function binds ,listens and accepts for server 
 * it uses select to accept new connections
 * as well as print console and takes input from keyboard
 * and also receives messages from connected clients
 * @param port : port on which client is running
 * ref:beejs guide
 */
void Server::shell_s(int port) {
    PORT_s = port;
    int rv;
    int yes = 1;
    socklen_t sin_size;
    int sockfd, numbytes, new_fd;
    char buf[MAXDATASIZE];
    char s[INET6_ADDRSTRLEN];
    struct addrinfo hints, *servinfo, *p;
    struct sigaction sa;
    struct sockaddr_storage their_addr;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    stringstream s1;
    string st;
    s1 << PORT_s;
    st = s1.str();
    char *PORT_CHAR = &st[0];
    rv = getaddrinfo(NULL, PORT_CHAR, &hints, &servinfo);

    if (rv != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof (int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
    freeaddrinfo(servinfo); // all done with this structure
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    string console = "[PA1]$";
    cout << console;
    fflush(stdout);
    fd_set master;
    fd_set read_fds;
    int fdmax;

    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    FD_SET(sockfd, &master);
    FD_SET(STDIN, &master);
    if (sockfd >= STDIN)
        fdmax = sockfd;
    while (1) { // main accept() loop
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            continue;
        }
        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == sockfd) {
                    sin_size = sizeof their_addr;
                    new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
                    if (new_fd == -1) {
                        perror("accept");
                        continue;
                    } else {
                        FD_SET(new_fd, &master);
                        if (new_fd > fdmax) {
                            fdmax = new_fd;
                        }
                    }
                    inet_ntop(their_addr.ss_family,
                            get_in_addr((struct sockaddr *) &their_addr),
                            s, sizeof s);
                    printf("server: got connection from %s", s);
                    fflush(stdout);
//                    keyboard input
                } else if (i == STDIN) {
                    string console = "[PA1]$";
                    string commands[12] = {"CREATOR", "HELP", "MYIP", "MYPORT", "REGISTER", "CONNECT", "LIST", "TERMINATE", "EXIT", "UPLOAD", "DOWNLOAD", "STATISTICS"};
                    string in_command;
                    string input;
                    vector<string> tokens;
                    printf("%s",console.c_str());
                    fflush(stdout);
                    getline(cin, input);
                    if (input.length() == 0)
                        continue;
                    char *s;
                    char* ca = new char[input.size() + 1];
                    copy(input.begin(), input.end(), ca);
                    ca[input.size()] = '\0';
                    s = strtok(ca, " ");
                    string token;

                    while (s != NULL) {
                        tokens.push_back(s);
                        s = strtok(NULL, " ");
                    }
                    in_command = tokens.at(0);
//checking command line input
                    if (comm.iequals_ignorecase(in_command, commands[0])) {
                        comm.creator();
                    } else if (comm.iequals_ignorecase(in_command, commands[1])) {
                        comm.help();
                    } else if (comm.iequals_ignorecase(in_command, commands[2])) {
                        string ip = comm.myIp();
                        char *ip_addr = &ip[0];
                        printf("IP address:%s", ip_addr);
                        fflush(stdout);
                    } else if (comm.iequals_ignorecase(in_command, commands[3])) {
                        int port_num = PORT_s;
                        printf("Port number:%d", port_num);
                        fflush(stdout);
                    } else if (comm.iequals_ignorecase(in_command, commands[4])) {
                        printf("this is a client side command...");
                        fflush(stdout);
                    } else if (comm.iequals_ignorecase(in_command, commands[5])) {
                        printf("this is a client side command...");
                        fflush(stdout);
                    } else if (comm.iequals_ignorecase(in_command, commands[6])) {
                        list();
                    } else if (comm.iequals_ignorecase(in_command, commands[7])) {
                        terminate();
                    } else if (comm.iequals_ignorecase(in_command, commands[8])) {
                        exit_1();
                    } else if (comm.iequals_ignorecase(in_command, commands[9])) {
                        printf("this is a client side command...");
                        fflush(stdout);
                    } else if (comm.iequals_ignorecase(in_command, commands[10])) {
                        printf("this is a client side command...cannot download from server");
                        fflush(stdout);
                    } else if (comm.iequals_ignorecase(in_command, commands[11])) {
                        int rc=statistics();
                    } else {
                        printf("WRONG COMMAND....please enter the command again...");
                        fflush(stdout);
                        continue;
                    }
//                    receive messages
                } else {
                    memset(buf, 0, MAXDATASIZE);
                    numbytes = recv(i, buf, MAXDATASIZE, 0);

                    if (numbytes <= 0) {
                        if (numbytes == 0) {
                            printf("selectserver :socket %d hung up", i);
                        } else {
                            perror("recv");
                        }

                        FD_CLR(i, &master);
                        if (i == fdmax) {
                            while (FD_ISSET(fdmax, &master) == false)
                                fdmax -= 1;
                        }
                        close(i);
                        continue;
                    }

                    string msg = (string) buf;
                    string rec_msg = (string) buf;
                    char c = rec_msg.at(0);
                    vector<string> tokens;
                    string rec_msg1 = rec_msg.substr(2, rec_msg.length() - 1);
                    tokens = comm.splitstring(rec_msg1, " ");
//              checks  registration message and sends the serv ip list stored in fdstore 
                    if (c == 'r') {
                        string rec_msg1 = rec_msg.substr(2, rec_msg.length() - 1);
                        tokens = comm.splitstring(rec_msg1, " ");
                        fdstore.insert(map<string, int>::value_type(tokens.at(0) + " " + tokens.at(2), i));
                        string ret_msg = "s";
                        int k = 0;
                        for (it_type iterator = fdstore.begin(); iterator != fdstore.end(); iterator++) {
                            if (k == 0)
                                ret_msg = ret_msg + "+" + iterator->first;
                            else
                                ret_msg = ret_msg + "+" + iterator->first;
                            k++;
                        }
                        k = 0;
                        for (it_type iterator = fdstore.begin(); iterator != fdstore.end(); iterator++) {

                            k = iterator->second;

                            int len = send(k, ret_msg.c_str(), 500, 0);
                            if (len == -1)
                                perror("send");
                        }
                        k = 0;
                        fflush(stdout);
//                        
                    } else if (c == 'c') {
//                        string rec_msg1 = rec_msg.substr(2, rec_msg.length() - 1);
//                        tokens = comm.splitstring(rec_msg1, " ");
//                        fdstore.insert(map<string, int>::value_type(tokens.at(0) + " " + tokens.at(2), i));
//                        string ret_msg = "d";
//                        ret_msg = ret_msg + "+" + "conndone";
//                        int len = send(i, ret_msg.c_str(), 500, 0);
//                        if (len == -1)
//                            perror("send");
                        cout<<"can't connect to server....";
                        fflush(stdout);
                    } else if (c == 't') {
                        string rec_msg1 = rec_msg.substr(2, rec_msg.length() - 1);
                        tokens = comm.splitstring(rec_msg1, " ");
                        int rc = fdstore.erase(tokens.at(0) + " " + tokens.at(1));
                        if (rc < 0) {
                            cout << "ter unsucc..";
                        }
                        cout << "name of host exiting :" << comm.gethostname(tokens.at(0)) << " ip :" << tokens.at(0)<<endl;
                        fflush(stdout);
                        string ret_msg = "s";
                        int k = 0;
                        if (fdstore.size() == 0) {
                            continue;
                        }
                        for (it_type iterator = fdstore.begin(); iterator != fdstore.end(); iterator++) {
                            if (k == 0)
                                ret_msg = ret_msg + "+" + iterator->first;
                            else
                                ret_msg = ret_msg + "+" + iterator->first;
                            k++;
                        }
                        k = 0;
                        for (it_type iterator = fdstore.begin(); iterator != fdstore.end(); iterator++) {

                            k = iterator->second;

                            int len = send(k, ret_msg.c_str(), 500, 0);
                            if (len == -1)
                                perror("send");
                        }
                        k = 0;
                        fflush(stdout);
                    } else {

                    }

                }
            }
        }

    }
    close(new_fd); // parent doesn't need this

}
/*
 * list function
 * this function lists all existing connections on server
 */
void Server::list() {
    if (fdstore.size() == 0) {
        printf("no connections...\n");
        fflush(stdout);
        return;
    }
    vector<string> tokens;
    int port_num;
    int host_id = 1;
    const char * hostname;
    const char * ip_addr;
    string temp_list;
    printf("\n");
    printf("%-5s%-35s%-20s%-8s\n", "ID", "HOSTNAME", "IP Address", "Port");
    fflush(stdout);
    for (it_type iterator = fdstore.begin(); iterator != fdstore.end(); iterator++) {
        temp_list = iterator->first;
        tokens = comm.splitstring(temp_list, " ");
        port_num = comm.stringtoint(tokens.at(1));
        hostname = comm.gethostname(tokens.at(0)).c_str();
        ip_addr = tokens.at(0).c_str();
        printf("%-5d%-35s%-20s%-8d\n", host_id, hostname, ip_addr, port_num);
        host_id++;
        fflush(stdout);
        tokens.clear();
    }
}
/*exit_1 function
 * exits the server
 */

void Server::exit_1() {

    printf("exiting system...\n");
    exit(0);

}
/*terminate function
 * handles terminate req on server
 */
void Server::terminate() {
    printf("this is a client side command..\n");
    fflush(stdout);
}
/*statistics function
 * handles statistics req on server
 * 
 */
int Server::statistics() {
    string temp_list;
    vector<string> tokens;
    int fd;
    vector<string> split_tokens;
    vector<string> split_tokens_1;
    vector<string> ipport;
    vector<string> stats_str;
    string myip = comm.myIp();
    string myport = comm.notostring(PORT_s);

    string stat_msg = "z";
    stat_msg = stat_msg + "+" + myip + " " + myport;
    int len;
    char buf[MAXDATASIZE];
    printf("\n%2s%25s%25s%30s%20s \t %s\n", "HOSTNAME1", "HOSTNAME2", "Total  ", "Average upload ", "Total    ", "Average Download ");
    printf("%2s%25s%25s%30s%20s \t %s\n", "        ", "        ", "  Uploads", "  Speed(bps)     ", "  Downloads", "speed(bps)       ");
    fflush(stdout);
//   iterates over all connections and sends req msg and receives local stats from all clients
    for (it_type iterator = fdstore.begin(); iterator != fdstore.end(); iterator++) {
        temp_list = iterator->first;
        tokens = comm.splitstring(temp_list, " ");
        fd = iterator->second;
        len = send(fd, stat_msg.c_str(), 100, 0);
        if (len == -1)
            perror("send");

        len = recv(fd, buf, MAXDATASIZE, 0);
        string rec_msg = (string) buf;
        if (rec_msg.at(0) == 'n') {
            continue;
        }
        split_tokens = comm.splitstring(rec_msg, ":");
        for (int k = 0; k < split_tokens.size(); k++) {
            split_tokens_1 = comm.splitstring(split_tokens.at(k), "+");
            ipport = comm.splitstring(split_tokens_1.at(0), " ");
            stats_str = comm.splitstring(split_tokens_1.at(1), " ");
            printf("%2s%25s%20s%10s%10s \t %s\n", comm.gethostname(ipport.at(0)).c_str(),comm.gethostname(tokens.at(0)).c_str(),  stats_str.at(0).c_str(), stats_str.at(1).c_str(), stats_str.at(2).c_str(), stats_str.at(3).c_str());
            fflush(stdout);
            split_tokens_1.clear();
            ipport.clear();
            stats_str.clear();
        }
        tokens.clear();
    }
    return 0;

}