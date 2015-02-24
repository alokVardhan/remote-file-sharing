/* 
 * File:   Server.h
 * Author: nexus
 *
 * Created on September 14, 2014, 12:07 PM
 */

#ifndef SERVER_H
#define	SERVER_H
#include<stdlib.h>
#include<string>
#include<vector>
#include<map>
using namespace std;

class Server {
// map to store (ip+port) and fd    
map<string,int> fdstore;
//map to store stats
map<string,vector<int> > stat_map;
public:
    void sendmsg_s(string msg,string ip,string port);
    void list();
    void terminate();
    void exit_1();
    int statistics();
    void shell_s(int);
private:

};

#endif	/* SERVER_H */

