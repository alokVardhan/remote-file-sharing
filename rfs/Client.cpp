/* 
 * File:   Client.cpp
 * Author: alok vardhan
 * this file controls all the operations related to client
 * Created on September 14, 2014, 12:07 PM
 */
#include<cstdlib>
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
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <cstring>
#include <arpa/inet.h>
#include<iostream>
#include "Client.h"
#include "Common.h"
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
#define BUFSIZE 1000
using namespace std;
/*declaration of iterators for different types of maps
 * 
 */
typedef std::map<string, int >::iterator it_type;
typedef std::map<int, string >::iterator it_type1;
typedef std::map<string, vector<long> >::iterator it_type_stats;
typedef map<int, bool>::iterator it_fstate;
typedef map<int, string>::iterator it_fname;
typedef map<int, vector<int> >::iterator it_fsize;
int PORT;
//variable for starting of download time
double down_start;
//object of Common class
Common comm_c;
//constructor

Client::Client() {

}

void sigchld_handler_C(int s) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

/**
 * get_in_addr_C function
 * get sockaddr, IPv4 or IPv6:
 * reference beejs guide
 */
void *get_in_addr_C(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

/*
 * shell function
 * this function binds ,listens and accepts for client 
 * it uses select to accept new connections
 * as well as print console ans take input from keyboard
 * and also receives messages from connected clients
 * @param port : port on which client is running
 * ref:beejs guide
 */


void Client::shell(int port) {
    PORT = port;
    int rv;
    int yes = 1;
    socklen_t sin_size;
    int numbytes, new_fd;
    char buf[MAXDATASIZE];
    char s[INET6_ADDRSTRLEN];
    struct addrinfo hints, *servinfo, *p;
    struct sigaction sa;
    struct sockaddr_storage their_addr;
    memset(&hints, 0, sizeof hints);
    //    initializing struct variables
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    stringstream s1;
    string st;
    s1 << PORT;
    st = s1.str();
    char *PORT_CHAR = &st[0];
    rv = getaddrinfo(NULL, PORT_CHAR, &hints, &servinfo);

    if (rv != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        //        creating socket
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
        //        port binding
        //        
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
    sa.sa_handler = sigchld_handler_C; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    string console = "[PA1]$";
    cout << console;
    fflush(stdout);
    //    master list containing all the sockfds for clients
    fd_set master;
    fd_set read_fds;
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    FD_SET(sockfd, &master);
    FD_SET(STDIN, &master);
    //    setting fdmax for initialization
    if (sockfd >= STDIN)
        fdmax = sockfd;
    // main accept() loop using select
    while (1) {
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
                            get_in_addr_C((struct sockaddr *) &their_addr),
                            s, sizeof s);
                    printf("client: got connection from %s\n", s);
                    fflush(stdout);

                } else if (i == STDIN) {
                    string console = "[PA1]$";
                    string commands[12] = {"CREATOR", "HELP", "MYIP", "MYPORT", "REGISTER", "CONNECT", "LIST", "TERMINATE", "EXIT", "UPLOAD", "DOWNLOAD", "STATISTICS"};

                    string in_command;

                    string input;
                    vector<string> tokens;
                    cout << console;
                    fflush(stdout);
//                 receiving command line input
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
//checking command line input ie shell input and invoking corresponding features
                    if (comm_c.iequals_ignorecase(in_command, commands[0])) {
                        comm_c.creator();
                    } else if (comm_c.iequals_ignorecase(in_command, commands[1])) {
                        comm_c.help();
                    } else if (comm_c.iequals_ignorecase(in_command, commands[2])) {
                        string ip = comm_c.myIp();
                        char *ip_addr = &ip[0];
                        printf("IP address:%s", ip_addr);
                        fflush(stdout);
                    } else if (comm_c.iequals_ignorecase(in_command, commands[3])) {
                        int port_num = PORT;
                        printf("Port number:%d", port_num);
                        fflush(stdout);
                    } else if (comm_c.iequals_ignorecase(in_command, commands[4])) {
                        if (tokens.size() != 3) {
                            printf("Invalid command aruments...usage Register serverIP Port#");
                            fflush(stdout);
                            continue;
                        }
                        if (tokens.size() == 3) {
                            string ip = comm_c.gethostip(tokens.at(1));
                            if (comm_c.iequals_ignorecase(ip, "invalid")) {
                                printf("invalid address...");
                                fflush(stdout);
                                continue;
                            }
                            tokens.at(1) = ip;
                        }
                        int reg_fd = register_c(tokens);
                        if (reg_fd <= 0) {
                            cout << "reg unsucessful";
                            fflush(stdout);
                            continue;
                        }
                        FD_SET(reg_fd, &master);
                        if (reg_fd > fdmax) {
                            fdmax = reg_fd;
                        }
                        continue;
                    } else if (comm_c.iequals_ignorecase(in_command, commands[5])) {
                        if (tokens.size() != 3) {
                            printf("Invalid command aruments...usage connect IP Port#");
                            continue;
                        }
                        if (tokens.size() == 3) {
                            string ip = comm_c.gethostip(tokens.at(1));
                            if (comm_c.iequals_ignorecase(ip, "invalid")) {
                                printf("invalid address...\n");
                                fflush(stdout);
                                continue;
                            }
                            tokens.at(1) = ip;
                        }

                        if (fdstore.size() > 4) {
                            cout << "requesting more than 3 connections...can't connect";
                            fflush(stdout);
                            continue;
                        }
                        int reg_fd = connect_client(tokens.at(1), tokens.at(2));
                        if (reg_fd <= 0) {
                            cout << "connection unsucessful";
                            fflush(stdout);
                            continue;
                        }
                        FD_SET(reg_fd, &master);
                        if (reg_fd > fdmax) {
                            fdmax = reg_fd;
                        }
                        continue;
                    } else if (comm_c.iequals_ignorecase(in_command, commands[6])) {
                        int rc = list();
                    } else if (comm_c.iequals_ignorecase(in_command, commands[7])) {
                        if (tokens.size() != 2) {
                            printf("Invalid command aruments...usage terminate connection#\n");
                            fflush(stdout);
                            continue;
                        }
                        if (tokens.at(1).length() != 1) {
                            printf("invalid connection id..please enter a valid connection id\n");
                            fflush(stdout);
                            continue;
                        }
                        if (comm_c.stringtoint(tokens.at(1)) == 1) {
                            cout << "cannot terminate server..." << endl;
                            fflush(stdout);
                            continue;
                        }
                        int ret_term = terminate(comm_c.stringtoint(tokens.at(1)));

                        if (ret_term <= 0) {
                            cout << "term unsuccfl..." << endl;
                            fflush(stdout);
                            continue;
                        } else {
                            cout << "term successful...." << endl;
                            fflush(stdout);
                        }
                        FD_CLR(ret_term, &master);
                        if (ret_term == fdmax) {
                            while (FD_ISSET(fdmax, &master) == false)
                                fdmax -= 1;
                        }
                        close(ret_term);
                    } else if (comm_c.iequals_ignorecase(in_command, commands[8])) {
                        int rc = exit_1();
                        cout << "exiting system..." << endl;
                        exit(2);
                    } else if (comm_c.iequals_ignorecase(in_command, commands[9])) {
                        int rc = upload(tokens);
                        if (rc < 0) {
                            printf("upload failed...\n");
                        }
                    } else if (comm_c.iequals_ignorecase(in_command, commands[10])) {
                        int rc = download(tokens);
                    } else if (comm_c.iequals_ignorecase(in_command, commands[11])) {
                        int rc = statistics();
                    } else {

                        cout << "WRONG COMMAND....please enter the command again...";
                        fflush(stdout);
                        continue;
                    }
                } else {
                    memset(buf, 0, MAXDATASIZE);
                    numbytes = recv(i, buf, MAXDATASIZE, 0);
                    //checks for client close
                    if (numbytes <= 0) {
                        if (numbytes == 0) {
                            printf("socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i);
                        FD_CLR(i, &master);
                        if (i == fdmax) {
                            while (FD_ISSET(fdmax, &master) == false)
                                fdmax -= 1;
                        }
                        continue;
                    }
                    string rec_msg = (string) buf;
                    //checks if download is in process
                    if (fd_filename.find(i) != fd_filename.end() && download_flag) {
                        rec_download(i, buf, numbytes);
                        continue;
                    }
                    char c = rec_msg.at(0);
                    vector<string> tokens;
                    string rec_msg1 = rec_msg.substr(2, rec_msg.length() - 1);
                    //                    server msg containing updated serv ip list
                    if (c == 's') {
                        tokens = comm_c.splitstring(rec_msg1, "+");
                        serv_list.clear();
                        for (int k = 0; k < tokens.size(); k++) {
                            serv_list.insert(map<string, int>::value_type(tokens.at(k), k));
                        }
                        int port_num;
                        int host_id = 1;
                        const char * hostname;
                        const char * ip_addr;
                        printf("\n");
                        printf("%-5s%-35s%-20s%-8s\n", "ID", "HOSTNAME", "IP Address", "Port");
                        fflush(stdout);
                        string temp_string;
                        string my_string;
                        vector <string> tok;
                        string m_ip = comm_c.myIp();
                        string m_port = comm_c.notostring(PORT);
                        my_string = m_ip + " " + m_port;
                        for (it_type iterator1 = serv_list.begin(); iterator1 != serv_list.end(); iterator1++) {
                            temp_string = iterator1->first;
                            tok = comm_c.splitstring(temp_string, " ");
                            port_num = comm_c.stringtoint(tok.at(1));
                            hostname = comm_c.gethostname(tok.at(0)).c_str();
                            ip_addr = tok.at(0).c_str();
                            printf("%-5d%-35s%-20s%-8d\n", host_id, hostname, ip_addr, port_num);
                            host_id++;
                            fflush(stdout);
                        }
                        fflush(stdout);
                        //connect msg
                    } else if (c == 'c') {
                        if (fdstore.size() == 3) {
                            printf("cannot receive any new connection...already have 3 connections...\n");
                            string ret_msg = "x";
                            ret_msg = ret_msg + "+" + "connection rejected";
                            int len = send(i, ret_msg.c_str(), 100, 0);
                            if (len == -1)
                                perror("send");
                            continue;
                        }
                        tokens = comm_c.splitstring(rec_msg1, " ");
                        fdstore.insert(map<string, int>::value_type(tokens.at(0) + " " + tokens.at(2), i));
                        string ret_msg = "d";
                        ret_msg = ret_msg + "+" + "conndone";
                        int len = send(i, ret_msg.c_str(), 500, 0);
                        if (len == -1)
                            perror("send");
                        //             sucessful connection
                    } else if (c == 'd') {

                        cout << "conn done";
                        fflush(stdout);
                        //checks for msg for download
                    } else if (c == 'p') {
                        tokens = comm_c.splitstring(rec_msg1, " ");
                        int rc = handle_download(tokens, i);
                        //                        termination msg
                    } else if (c == 't') {
                        string rec_msg1 = rec_msg.substr(2, rec_msg.length() - 1);
                        tokens = comm_c.splitstring(rec_msg1, " ");
                        int rc = fdstore.erase(tokens.at(0) + " " + tokens.at(1));
                        if (rc < 0) {
                            cout << "ter unsucc..";
                        }
                        cout << "terminated connection from :" << comm_c.gethostname(tokens.at(0)) << " ip :" << tokens.at(0) << "name :" << comm_c.gethostname(tokens.at(0));
                        fflush(stdout);
                        //                        registration msg
                    } else if (c == 'r') {
                        cout << "can't register the  client..." << endl;
                        string send_msg = "q";
                        int len = send(i, send_msg.c_str(), 20, 0);

                        fflush(stdout);
                    }//                  checks for server msg to send stats  
                    else if (c == 'z') {
                        string stat_msg;
                        string temp_msg;
                        if (stat_map.size() == 0) {
                            stat_msg = "n";
                            int len1 = send(i, stat_msg.c_str(), 100, 0);
                            if (len1 <= 0)
                                perror("send");
                            continue;
                        }
                        string temp_string;
                        vector<string> ipport;
                        vector<long> stats;
                        double avg_up = 0;
                        double avg_down = 0;
                        long avg_up_sp;
                        long avg_down_sp;
                        int count = 0;
                        for (it_type_stats iterator = stat_map.begin(); iterator != stat_map.end(); iterator++) {
                            temp_string = iterator->first;
                            stats = iterator->second;
                            if (stats.at(0) == 0) {
                                avg_up = 0;
                            } else {
                                avg_up = stats.at(1) / stats.at(0);
                            }
                            avg_up_sp = static_cast<long> (avg_up);
                            if (stats.at(2) == 0) {
                                avg_down = 0;
                            } else {
                                avg_down = stats.at(3) / stats.at(2);
                            }
                            avg_down_sp = static_cast<long> (avg_down);
                            temp_msg = temp_string + "+" + comm_c.longtostring(stats.at(0)) + " " + comm_c.longtostring(avg_up_sp);
                            temp_msg = temp_msg + " " + comm_c.longtostring(stats.at(2)) + " " + comm_c.longtostring(avg_down_sp);
                            if (count == 0) {
                                stat_msg = temp_msg;
                            } else {
                                stat_msg = stat_msg + ":" + temp_msg;
                            }
                            count++;
                            temp_msg.clear();
                        }
                        cout<<"stats msg : "<<stat_msg<<endl;
                        //                        sends local stats to server
                        int len1 = send(i, stat_msg.c_str(), MAXDATASIZE, 0);
                        if (len1 <= 0)
                            perror("send");
                        //                        upload msg
                    } else if (c == 'u') {
                        string rec_msg1 = rec_msg.substr(2, rec_msg.length() - 1);
                        tokens = comm_c.splitstring(rec_msg1, " ");
                        int rc = rec_upload(tokens, i);
                    }
                }
            }
        }

    }
    close(new_fd); // parent doesn't need this
}

/*rec_download function
 * this function receives incoming bytes and writes them to the correspnding file
 * @param fd_down :file descriptor of client sending file
 * @param down_msg :actual received msg
 * @param msglen :length of msg received
 */


int Client::rec_download(int fd_down, char * down_msg, int msglen) {
    FILE *received_file;
    string filename = (fd_filename.find(fd_down))->second;
    //    opening  file in append mode
    received_file = fopen(filename.c_str(), "a");
    if (received_file == NULL) {
        fprintf(stderr, "Failed to open file --> %s\n", strerror(errno));
        fflush(stdout);
    }
    vector<int> f_size = (fd_filesize.find(fd_down))->second;
    int file_size = f_size.at(0);
    //    writing to file
    fwrite(down_msg, sizeof (char), msglen, received_file);
    //    closing file
    int rc = fclose(received_file);
    
    f_size.at(0) = f_size.at(0);
    f_size.at(1) = f_size.at(1) + msglen;
    (fd_filesize.find(fd_down))->second = f_size;
    string temp_string;
    vector<string> ipport;
    for (it_type iterator = fdstore.begin(); iterator != fdstore.end(); iterator++) {
        if (iterator->second == fd_down) {
            temp_string = iterator->first;
            ipport = comm_c.splitstring(temp_string, " ");
            break;
        }
    }
    //    checking if file download is complete
    if (f_size.at(1) >= f_size.at(0)) {
        //        erasing the corrsponding entries
        fd_filename.erase(fd_down);
        fd_filesize.erase(fd_down);
        (fd_filestate.find(fd_down))->second = true;

        double end = comm_c.gettime();
        double speed = file_size / (end - down_start);
        speed = speed * 8;
        long down_speed = static_cast<long> (speed);
        string from_ser, to_ser;
        to_ser = comm_c.gethostname(comm_c.myIp());
        from_ser = comm_c.gethostname(ipport.at(0));
        vector<long> stats;
        printf("\nRx:%s->%s,File Size:%d Bytes,Time Taken:%lf seconds,Rx Rate:%lubits/second\n", from_ser.c_str(), to_ser.c_str(), file_size, end - down_start, down_speed);
        //    populating statistics map with values
        if (stat_map.find(ipport.at(0) + " " + ipport.at(1)) == stat_map.end()) {
            stats.push_back(0);
            stats.push_back(0);
            stats.push_back(1);
            stats.push_back(down_speed);
            stat_map.insert(map<string, vector<long> >::value_type(ipport.at(0) + " " + ipport.at(1), stats));
            stats.clear();
        } else {
            stats = stat_map.find(ipport.at(0) + " " + ipport.at(1))->second;
            stats.at(2) = stats.at(2) + 1;
            stats.at(3) = stats.at(3) + down_speed;
            stat_map.find(ipport.at(0) + " " + ipport.at(1))->second = stats;
            stats.clear();
        }
    }
    //    setting download flag to false if no entry is left
    if (fd_filename.size() == 0) {
        download_flag = false;
        fd_filestate.clear();
    }
    return 0;
}

/*handle_download function 
 * this function handles download request on other client and sends file
 * @param tokens : vector containing requestors ip ,port and filename
 * @param down_fd :requestors file descriptor
 */
int Client::handle_download(vector<string> tokens, int down_fd) {
    struct stat file_stat;
    bool flag = false;
    off_t offset = 0;
    int sent_bytes = 0;
    string FILETOSEND = tokens.at(2);
    double start = comm_c.gettime();
    //    opening file
    int fd = open(FILETOSEND.c_str(), O_RDONLY);
    bool flag_file = true;
    if (fd == -1) {
        fprintf(stderr, "Error opening file --> %s\n", strerror(errno));
        fflush(stdout);
        flag_file = false;
    }
    if (fstat(fd, &file_stat) < 0) {
        printf("Cant open file...\n");
        fflush(stdout);
    }
    string size;
    stringstream strstream;
    char buf[MAXDATASIZE];
    string filename = comm_c.getfilename(FILETOSEND);
    strstream << file_stat.st_size;
    strstream >> size;
    int file_size = comm_c.stringtoint(size);
    fflush(stdout);
    string myip = comm_c.myIp();
    string myport = comm_c.notostring(PORT);
    string ack_msg = "f";
    if (flag_file) {
        ack_msg = ack_msg + " " + myip + " " + myport + " " + size;
    } else {
        ack_msg = "n";
    }
    //    sending ack msg with myip myport filesize if file is found else sends negative msg
    int ack_len = send(down_fd, ack_msg.c_str(), 100, 0);
    if (ack_msg == "n") {
        return 0;
    }
    if (ack_len < 0) {
        perror("send");
        printf("err in send of ack");
    }
    ack_len = 0;
    //    third msg in three way handshake
    ack_len = recv(down_fd, buf, MAXDATASIZE, 0);
    if (ack_len < 0) {
        perror("recv");
        printf("err in rec of ack");
    }
    string rec_msg = (string) buf;
    int remain_data = file_stat.st_size;
    //    sending file
    while ((remain_data > 0)&&((sent_bytes = sendfile(down_fd, fd, &offset, BUFSIZE)) > 0)) {
        remain_data -= sent_bytes;
    }
    close(fd);
    double end = comm_c.gettime();
    double speed = file_size / (end - start);
    speed = speed * 8;
    long down_speed = static_cast<long> (speed);
    vector<long> stats;
    string from_ser, to_ser;
    from_ser = comm_c.gethostname(comm_c.myIp());
    to_ser = comm_c.gethostname(tokens.at(0));
    printf("\nTx:%s->%s,File Size:%d Bytes,Time Taken:%lf seconds,Tx Rate:%lubits/second\n", from_ser.c_str(), to_ser.c_str(), file_size, end - start, down_speed);
    //    populating local stats map
    if (stat_map.find(tokens.at(0) + " " + tokens.at(1)) == stat_map.end()) {
        stats.push_back(1);
        stats.push_back(down_speed);
        stats.push_back(0);
        stats.push_back(0);
        stat_map.insert(map<string, vector<long> >::value_type(tokens.at(0) + " " + tokens.at(1), stats));
        stats.clear();
    } else {
        stats = stat_map.find(tokens.at(0) + " " + tokens.at(1))->second;
        stats.at(0) = stats.at(0) + 1;
        stats.at(1) = stats.at(1) + down_speed;
        stat_map.find(tokens.at(0) + " " + tokens.at(1))->second = stats;
        stats.clear();
    }
}

/*download function
 * this function validates the download command and the arguments
 * and after validating sends down request msgs to the corresponding clients
 * @param vector containg list of all the arguments 0th argument is download command
 * returns 0 on successful execution
 */
int Client::download(vector<string> tokens) {
    download_flag = false;
    down_start=0.0000;
    fd_filename.clear();
    fd_filesize.clear();
    fd_filestate.clear();
    int k = 0;
    int key = 0;
    int no_valid_args = 0;
    if (tokens.size() % 2 == 0 || tokens.size() < 3) {
        cout << "invalid no. of arguments" << endl;
        return -1;
    }
    //    map to store conn id and file name corresponding to it
    map<int, string> down_map;
    for (k = 1; k < tokens.size();) {
        down_map.insert(map<int, string>::value_type(comm_c.stringtoint(tokens.at(k)), tokens.at(k + 1)));
        k = k + 2;
    }

    k = 1;
    string file_to_download;
    string filename;
    string myip = comm_c.myIp();
    string myport = comm_c.notostring(PORT);
    string temp_msg = "p";
    temp_msg = temp_msg + "+" + myip + " " + myport;
    string down_msg;
    int down_fd;
    int l = 1;
    vector<string> ack_tokens;
    for (it_type1 itr = down_map.begin(); itr != down_map.end(); itr++) {
        down_msg = temp_msg;
        key = itr->first;
        if (key == 0 || key > fdstore.size() + 1 || key == 1) {
            printf("invalid connection id : %d..for %d pair of arguments...\n", key, k);
            fflush(stdout);
            continue;
        }
        k++;
        file_to_download = itr->second;
        filename = comm_c.getfilename(file_to_download);
        l = 1;
        down_fd = 0;
        for (it_type iterator = fdstore.begin(); iterator != fdstore.end(); iterator++) {
            down_fd = iterator->second;
            if (l == key - 1) {
                break;
            }
            l++;
        }
        if (down_fd <= 0) {
            printf("something wrong...");
            continue;
        }
        down_msg = down_msg + " " + file_to_download;
        //        sending download req
        int len = send(down_fd, down_msg.c_str(), 100, 0);
        if (len == -1) {
            perror("send");
            printf("error in first send of upload\n");
        }
        char buf[MAXDATASIZE];
        int numbytes = recv(down_fd, buf, MAXDATASIZE, 0);
        string ack_msg = (string) buf;
        char c = ack_msg.at(0);
        //        negative ack msg if file doesn't exist on other client
        if (c == 'n') {
            printf("\nfile not found on client...\n");
            continue;
        } else if (c == 'f') {
            ack_tokens = comm_c.splitstring(ack_msg, " ");
            int file_size = comm_c.stringtoint(ack_tokens.at(3));
            FILE *received_file;
            char buffer[BUFSIZE];
            int remain_data = file_size;
            fd_filename.insert(map<int, string>::value_type(down_fd, filename));
            vector<int> f_size;
            f_size.push_back(file_size);
            f_size.push_back(0);
            fd_filesize.insert(map<int, vector<int> >::value_type(down_fd, f_size));
            bool st = false;
            fd_filestate.insert(map<int, bool>::value_type(down_fd, st));
            string ack_msg;
            ack_msg = "ready for receiving packets...start sending...";
            int down_len = send(down_fd, ack_msg.c_str(), 100, 0);
        } else {
            printf("damn!!...shldn't have reached here...\n");
        }
    }
    download_flag = true;
    down_start = comm_c.gettime();
    return 0;
}

/*rec_upload function 
 * this function receives the file 
 * @param tokens :vector containing requestors ip,port filename and filesize
 * @param fd of requestor
 * returns 0: on sucessful exec
 */
int Client::rec_upload(vector<string> tokens, int up_fd) {
    string FILENAME = tokens.at(2);
    int file_size = comm_c.stringtoint(tokens.at(3));
    FILE *received_file;
    char buffer[BUFSIZE];
    int remain_data = 0;
    int len = 0;
    bool flag = false;
    
    received_file = fopen(FILENAME.c_str(), "w");
    if (received_file == NULL) {
        fprintf(stderr, "Failed to open file foo --> %s\n", strerror(errno));
        return -1;
    }
    string ack_msg = "received connection for upload...";
    len = send(up_fd, ack_msg.c_str(), 100, 0);
    remain_data = file_size;
    len = 0;
    double start = comm_c.gettime();
    //    receiving and writing file
    while (((len = recv(up_fd, buffer, BUFSIZE, 0)) > 0) && (remain_data > 0)) {
        if (len < 0) {
            flag = true;
            break;
        }
        fwrite(buffer, sizeof (char), len, received_file);
        remain_data -= len;
        if (remain_data <= 0) {
            break;
        }
    }

    int rc = fclose(received_file);
    if (rc < 0) {
        cout << "cant close file";
    }
    if (flag == true) {
        printf("upload failed on receiving end...\n");
        return -1;
    }
    string from_ser, to_ser;
    to_ser = comm_c.gethostname(comm_c.myIp());
    from_ser = comm_c.gethostname(tokens.at(0));
    double end = comm_c.gettime();
    double speed = file_size / (end - start);
    speed = speed * 8;
    long down_speed = static_cast<long> (speed);
    printf("\nRx:%s->%s,File Size:%d Bytes,Time Taken:%lf seconds,Rx Rate:%lubits/second\n", from_ser.c_str(), to_ser.c_str(), file_size, end - start, down_speed);
    vector<long> stats;
    //populating stats map
    if (stat_map.find(tokens.at(0) + " " + tokens.at(1)) == stat_map.end()) {
        stats.push_back(0);
        stats.push_back(0);
        stats.push_back(1);
        stats.push_back(down_speed);
        stat_map.insert(map<string, vector<long> >::value_type(tokens.at(0) + " " + tokens.at(1), stats));
        stats.clear();
    } else {
        stats = stat_map.find(tokens.at(0) + " " + tokens.at(1))->second;
        stats.at(2) = stats.at(2) + 1;
        stats.at(3) = stats.at(3) + down_speed;
        stat_map.find(tokens.at(0) + " " + tokens.at(1))->second = stats;
        stats.clear();
    }
    return 0;
}

/*upload function
 * this function sends file which is mentioned in upload command
 * @param:tokens containing upload command,conn_id,filename
 * returns 0 :successful execution
 */
int Client::upload(vector<string> tokens) {
    struct stat file_stat;
    bool flag = false;
    off_t offset = 0;
    int sent_bytes = 0;
    if (tokens.size() != 3) {
        printf("invalid arguments...\n");
        fflush(stdout);
        return -1;
    }
    if (comm_c.stringtoint(tokens.at(1)) == 1) {
        printf("can't upload file to server...\n");
        fflush(stdout);
        return -1;
    }
    if (comm_c.stringtoint(tokens.at(1))>(fdstore.size() + 1)) {
        printf("invalid connection id...\n");
        fflush(stdout);
        return -1;
    }
    string FILETOSEND = tokens.at(2);
    int conn_no = comm_c.stringtoint(tokens.at(1));
    double start = comm_c.gettime();
    int fd = open(FILETOSEND.c_str(), O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Error opening file --> %s\n", strerror(errno));
        fflush(stdout);
        return -1;
    }
    if (fstat(fd, &file_stat) < 0) {
        fprintf(stderr, "Error fstat --> %s\n", strerror(errno));

        fflush(stdout);
        return -1;
    }
    string size;
    stringstream strstream;
    string filename = comm_c.getfilename(FILETOSEND);
    strstream << file_stat.st_size;
    strstream >> size;
    int file_size = comm_c.stringtoint(size);
    fflush(stdout);

    string upload_msg = "u";
    string temp;
    vector<string> ipstore;
    upload_msg = upload_msg + "+" + comm_c.myIp() + " " + comm_c.notostring(PORT) + " " + filename + " " + size;
    int l = 1;
    int up_fd;
    for (it_type iterator = fdstore.begin(); iterator != fdstore.end(); iterator++) {
        temp = iterator->first;
        ipstore = comm_c.splitstring(temp, " ");
        up_fd = iterator->second;
        if (l == conn_no - 1) {
            break;
        }
        l++;
        temp.clear();
        ipstore.clear();
        up_fd = 0;
    }
    //   initiating upload req
    int len = send(up_fd, upload_msg.c_str(), 100, 0);
    if (len == -1) {
        perror("send");
        printf("error in first send of upload\n");
    }
    char buf[MAXDATASIZE];
    //    receiving ack
    int numbytes = recv(up_fd, buf, MAXDATASIZE, 0);

    int remain_data = file_stat.st_size;
    //    sending file
  
    while (((sent_bytes = sendfile(up_fd, fd, &offset, BUFSIZE)) > 0) && (remain_data > 0)) {
        remain_data -= sent_bytes;
    }
    close(fd);
    double end = comm_c.gettime();
    double speed = file_size / (end - start);
    speed = speed * 8;
    long y = static_cast<long> (speed);
//    if(y<0)
//        y=y*(-1);
    string from_ser, to_ser;
    from_ser = comm_c.gethostname(comm_c.myIp());
    to_ser = comm_c.gethostname(ipstore.at(0));
    vector<long> stats;
    //populating stats map
    if (stat_map.find(ipstore.at(0) + " " + ipstore.at(1)) == stat_map.end()) {
        stats.push_back(1);
        stats.push_back(y);
        stats.push_back(0);
        stats.push_back(0);
        stat_map.insert(map<string, vector<long> >::value_type(ipstore.at(0) + " " + ipstore.at(1), stats));
        stats.clear();
    } else {

        stats = stat_map.find(ipstore.at(0) + " " + ipstore.at(1))->second;
        stats.at(0) = stats.at(0) + 1;
        stats.at(1) = stats.at(1) + y;
        stat_map.find(ipstore.at(0) + " " + ipstore.at(1))->second = stats;
        stats.clear();
    }
    printf("\nTx:%s->%s,File Size:%d Bytes,Time Taken:%lf seconds,Tx Rate:%lubits/second\n", from_ser.c_str(), to_ser.c_str(), file_size, end - start, y);
    return 0;
}

/*connect_client function
 * this function sends connect request to another client
 * @ip : ip of other client
 * @port : port of the other client
 * @returns socket file descriptor of other client so that it could be added to master list
 * @returns -1 is anything goes wrong 
 * ref:beejs guide
 */
int Client::connect_client(string ip, string port) {
    string myip = comm_c.myIp();
    string myport = (string) comm_c.notostring(PORT);
    //check for self connect
    if (ip == myip && port == myport) {
        cout << "trying to do a self connect...";
        fflush(stdout);
        return -1;
    }
    //checking for more than 3 conns
    if (fdstore.size() == 3) {
        printf("already have 3 connections...cannot establish new connection...\n ");
        fflush(stdout);
        return -1;
    }
    if (fdstore.find(ip + " " + port) == fdstore.end()) {

    } else {
        //        duplicate conn
        printf("already connected to this client....\n");
        fflush(stdout);
        return -1;
    }
    if (comm_c.stringtoint(port) == 0) {
        printf("invalid port....\n ");
        fflush(stdout);
        return -1;

    }
    vector<string> temp;
    bool flag = false;
    for (it_type iterator1 = serv_list.begin(); iterator1 != serv_list.end(); iterator1++) {
        temp = comm_c.splitstring(iterator1->first, " ");
        if (myip == temp.at(0) && myport == temp.at(1)) {
            continue;
        }
        if (ip == temp.at(0) && port == temp.at(1)) {
            flag = true;
        }
        temp.clear();
    }
    //   ip not found in serv list
    //trying to connect to an unregiatered client
    if (flag == false) {
        cout << "ip not found in the serv list...please enter a valid ip..." << endl;
        fflush(stdout);
        return 0;
    }
    int sockfd_con, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    string serv_name = comm_c.gethostname(ip);

    string myname = comm_c.gethostname(myip);
    if ((rv = getaddrinfo(ip.c_str(), port.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        cout << "enter the right address...";
        fflush(stdout);
        return 0;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd_con = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            return 0;
        }
        if (connect(sockfd_con, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd_con);
            perror("client: connect");
            return 0;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        cout << "could not connect" << endl;
        fflush(stdout);
        return 0;
    }

    inet_ntop(p->ai_family, get_in_addr_C((struct sockaddr *) p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);
    fflush(stdout);
    freeaddrinfo(servinfo); // all done with this structure
    //connection message to other client
    string conn_msg = "c";
    conn_msg = conn_msg + "+" + myip + " " + comm_c.gethostname(myip) + " " + myport;
    int len = send(sockfd_con, conn_msg.c_str(), 100, 0);
    if (len == -1)
        perror("send");
    char ack_buf[MAXDATASIZE];
    int ack_len = recv(sockfd_con, ack_buf, MAXDATASIZE, 0);
    string ack_msg = (string) ack_buf;
    char c = ack_msg.at(0);
    //   check if other client already has 3 connections
    if (c == 'x') {
        printf("cannot connect to client...client already has 3 connections...\n");
        fflush(stdout);
        return -1;
    } else {
        printf("successful connection...\n");
        fflush(stdout);
    }
    //    storing the fd of other client for future use
    fdstore.insert(map<string, int>::value_type(ip + " " + port, sockfd_con));

    return sockfd_con;
}

/*list function
 * this function lists all the clients that this client is connected to
 * returns 0 on successful execution
 */
int Client::list() {
    vector<string> tokens;
    int k = 2;
    string temp_list;
    if (fdstore_ser.size() == 0) {
        printf("no connections...");
        fflush(stdout);
        return -1;
    }
    it_type it = fdstore_ser.begin();
    int port_num;
    int host_id = 1;
    const char * hostname;
    const char * ip_addr;
    printf("\n");
    printf("%-5s%-35s%-20s%-8s\n", "ID", "HOSTNAME", "IP Address", "Port");
    fflush(stdout);
    temp_list = it->first;
    tokens = comm_c.splitstring(temp_list, " ");
    port_num = comm_c.stringtoint(tokens.at(1));
    hostname = comm_c.gethostname(tokens.at(0)).c_str();
    ip_addr = tokens.at(0).c_str();

    printf("%-5d%-35s%-20s%-8d\n", host_id, hostname, ip_addr, port_num);
    fflush(stdout);
    if (fdstore.size() == 0) {
        return 0;
    }
    host_id++;
    for (it_type iterator = fdstore.begin(); iterator != fdstore.end(); iterator++) {
        temp_list = iterator->first;
        tokens = comm_c.splitstring(temp_list, " ");
        port_num = comm_c.stringtoint(tokens.at(1));
        hostname = comm_c.gethostname(tokens.at(0)).c_str();
        ip_addr = tokens.at(0).c_str();
        printf("%-5d%-35s%-20s%-8d\n", host_id, hostname, ip_addr, port_num);
        fflush(stdout);
        host_id++;
        tokens.clear();
    }
    return 0;
}

/*exit_1 function
 * this function closes all the connections and then terminates the server connection
 * returns 0: successful exec
 */
int Client::exit_1() {
    int ex_fd;
    int k = 2;
    vector<string> tokens;
    for (it_type iterator = fdstore.begin(); iterator != fdstore.end(); iterator++) {
        ex_fd = iterator->second;
        terminate(k);
        k++;
        close(ex_fd);
    }
    //    printf("all clients disconnected...now sending msg to serv\n");
    fflush(stdout);
    if (fdstore_ser.size() == 0) {
        return 0;
    }
    it_type iterator = fdstore_ser.begin();
    int fd_serv = iterator->second;
    string myip = comm_c.myIp();
    string myport = comm_c.notostring(PORT);

    string ter_msg = "t";
    ter_msg = ter_msg + "+" + myip + " " + myport;
    int len = send(fd_serv, ter_msg.c_str(), 100, 0);
    if (len == -1)
        perror("send");
    fdstore.clear();
    fdstore_ser.clear();
    close(fd_serv);

    //    close(sockfd);

    return 0;
}

/*register_c function
 * this function handles the register request
 * ref:bejs guide
 */
int Client::register_c(vector<string> tokens) {
    if (fdstore_ser.size() == 1) {
        cout << "can't register two times..." << endl;
        fflush(stdout);
        return -1;
    }

    string myip = comm_c.myIp();
    string myport = comm_c.notostring(PORT);


    if (myip == tokens.at(1) && myport == tokens.at(2)) {
        printf("trying to do a self register....\n ");
        fflush(stdout);
        return -1;
    }
    if (comm_c.stringtoint(tokens.at(2)) == 0) {
        printf("invalid port....\n ");
        fflush(stdout);
        return -1;
    }

    int sockfd_r, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    string serv_name = comm_c.gethostname(tokens.at(1));

    string myname = comm_c.gethostname(myip);
    if ((rv = getaddrinfo(tokens.at(1).c_str(), tokens.at(2).c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        printf("enter the right address...\n");
        fflush(stdout);
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd_r = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        if (connect(sockfd_r, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd_r);
            perror("client: connect");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        fflush(stdout);
    }

    inet_ntop(p->ai_family, get_in_addr_C((struct sockaddr *) p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);
    fflush(stdout);
    freeaddrinfo(servinfo); // all done with this structure
    //    fdstore_ser.insert(map<string, int>::value_type(tokens.at(1) + " " + tokens.at(2), sockfd_r));
    string msg = "r";
    msg = msg + "+" + myip + " " + myname + " " + myport;
    int len = send(sockfd_r, msg.c_str(), 100, 0);
    if (len == -1)
        perror("send");
    len = recv(sockfd_r, buf, 1000, 0);
    string rec_msg = (string) buf;
    if (rec_msg.at(0) == 'q') {
        printf("can't register on a client...\n");
        return -1;
    }
    //    
    string rec_msg1 = rec_msg.substr(2, rec_msg.length() - 1);
    vector<string> tokens_1;
    tokens_1 = comm_c.splitstring(rec_msg1, "+");
    serv_list.clear();
    for (int k = 0; k < tokens_1.size(); k++) {
        serv_list.insert(map<string, int>::value_type(tokens_1.at(k), k));
    }
    int port_num;
    int host_id = 1;
    const char * hostname;
    const char * ip_addr;
    printf("\n");
    printf("%-5s%-35s%-20s%-8s\n", "ID", "HOSTNAME", "IP Address", "Port");
    fflush(stdout);
    string temp_string;
    string my_string;
    vector <string> tok;
    string m_ip = comm_c.myIp();
    string m_port = comm_c.notostring(PORT);
    my_string = m_ip + " " + m_port;
    for (it_type iterator1 = serv_list.begin(); iterator1 != serv_list.end(); iterator1++) {
        temp_string = iterator1->first;
        tok = comm_c.splitstring(temp_string, " ");
        port_num = comm_c.stringtoint(tok.at(1));
        hostname = comm_c.gethostname(tok.at(0)).c_str();
        ip_addr = tok.at(0).c_str();
        printf("%-5d%-35s%-20s%-8d\n", host_id, hostname, ip_addr, port_num);
        host_id++;
        fflush(stdout);
    }
    fflush(stdout);
    //    
    fdstore_ser.insert(map<string, int>::value_type(tokens.at(1) + " " + tokens.at(2), sockfd_r));
    return sockfd_r;
}

/*terminate function
 * this function terminates the requested connection
 * @param conn_id : connection id
 * returns fd corresponding to conn id 
 * else returns -1 for unsuccessful exec 
 */
int Client::terminate(int conn_id) {
    conn_id--;
    if (conn_id == 0 || conn_id > fdstore.size()) {
        cout << "invalid connection id...please enter a valid connection id..." << endl;
        fflush(stdout);
        return -1;
    }
    string myip = comm_c.myIp();
    string myport = comm_c.notostring(PORT);
    string ter_msg = "t";
    string ip_toterm, port_toterm;
    int fd_toterm;
    string temp_list;
    vector<string> tokens;
    int k = 1;

    for (it_type iterator = fdstore.begin(); iterator != fdstore.end(); iterator++) {
        temp_list = iterator->first;
        fd_toterm = iterator->second;
        tokens = comm_c.splitstring(temp_list, " ");
        fflush(stdout);
        if (k == conn_id) {
            ip_toterm = tokens.at(0);
            port_toterm = tokens.at(1);
            break;
        }
        k++;
        tokens.clear();
    }
    //    it_type it = fdstore.find(ip_toterm + "+" + port_toterm);
    //    fdstore.erase(it);

    fflush(stdout);
    ter_msg = ter_msg + "+" + myip + " " + myport;

    int len = send(fd_toterm, ter_msg.c_str(), 100, 0);
    if (len == -1)
        perror("send");
    int rc = fdstore.erase(ip_toterm + " " + port_toterm);
    if (rc < 0)
        cout << "ter...unsucc";
    return fd_toterm;
}

/*statistics function
 * this function prints the local statistics
 * returns 0 or -1 depending upon the execution(successful or unsuccessful)
 */
int Client::statistics() {
    if (stat_map.size() == 0) {
        printf("no uploads or downloads from this client...");
        fflush(stdout);
        return -1;
    }
    vector<string> tokens;
    string temp_str;
    vector<long> stats;
    printf("\n%2s%35s%30s%20s \t %s\n", "HOSTNAME", "Total  ", "Average upload ", "Total    ", "Average Download ");
    printf("%2s%35s%30s%20s \t %s\n", "        ", "Uploads", "Speed(bps)     ", "Downloads", "speed(bps)       ");
    double avg_up = 0;
    double avg_down = 0;
    long avg_up_sp;
    long avg_down_sp;
    for (it_type_stats iterator = stat_map.begin(); iterator != stat_map.end(); iterator++) {
        temp_str = iterator->first;
        tokens = comm_c.splitstring(temp_str, " ");
        stats = iterator->second;
        if (stats.at(0) == 0) {
            avg_up = 0;
        } else {
            avg_up = stats.at(1) / stats.at(0);
        }
        avg_up_sp = static_cast<long> (avg_up);
        if (stats.at(2) == 0) {
            avg_down = 0;
        } else {
            avg_down = stats.at(3) / stats.at(2);
        }
        avg_down_sp = static_cast<long> (avg_down);
        printf("%2s%10lu%30lu%20lu \t %5lu\n", comm_c.gethostname(tokens.at(0)).c_str(), stats.at(0), avg_up_sp, stats.at(2), avg_down_sp);
        fflush(stdout);
        avg_up = 0;
        avg_down = 0;
    }
}