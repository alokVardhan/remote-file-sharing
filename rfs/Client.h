/* 
 * File:   Client.h
 * Author: alok vardhan
 *this file controls all the operations pertaining to client
 * Created on September 15, 2014, 2:20 PM
 */

#ifndef CLIENT_H
#define	CLIENT_H
#include<vector>
#include<string>
#include<iostream>
#include<map>

using namespace std;

class Client {
    //    this map stores (IP +" " + PORT) and its corresponding filedesriptor
    map<string, int> fdstore;
    //    this map contains (IP +" " + PORT) and its corresponding filedesriptor just for server
    map<string, int> fdstore_ser;
    //    this map stores (IP +" " + PORT) for 
    map<string, int> serv_list;
    int sockfd;
    //    variable to contain value of max sock in list in select
    int fdmax;
//    map that stores (ip+port ) and stats corresponding to it
    map<string, vector<long> > stat_map;
//    map to store fd and file name of files to be downloaded
    map<int, string> fd_filename;
    //    map to store fd and file state of files to be downloaded in boolean
    map<int, bool> fd_filestate;
    //    map to store fd and <file size,file downloaded> of files to be downloaded
    map<int, vector<int> > fd_filesize;
//    download flag
    bool download_flag;
public:
    Client();
    bool ret_state();
    int rec_download(int, char *, int);
    int connect_client(string, string);
    int list();
    int terminate(int conn_id);
    int exit_1();
    int register_c(vector<string> tokens);
    int upload(vector<string> tokens);
    int statistics();
    int rec_upload(vector<string> tokens, int up_fd);
    void shell(int);
    int download(vector<string> tokens);
    int handle_download(vector<string> tokens, int fd);
private:

};

#endif	/* CLIENT_H */

