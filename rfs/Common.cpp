/* 
 * File:   Common.cpp
 * Author: alok vardhan
 * this file controls all the operations common to both client and server
 * Created on September 14, 2014, 12:16 PM
 */
#include "Common.h"
#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <cstring>
#include <arpa/inet.h>
#include<iostream>
#include <time.h>
#include <sys/time.h>
#include<assert.h>
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
#include<sstream>
#include<cstring>
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
using namespace std;

Common::Common() {
}
/**
 * getfilename function
 *this function receives a fully qualified name and returns the actual name in the string
 * @param  s : fully qualified name
 * @return sb1 :actual filename which was embedded in the fully qualified name
 */
string Common::getfilename(string s) {
    char tab2[s.length()];
    strcpy(tab2, s.c_str());
    int k = 0;
    for (int i = 0; i < s.length(); i++) {
        if (tab2[i] == '/')
            k = i;
    }

    string sub1 = s.substr(k + 1, s.length() - 1);
    cout << sub1 << '\n';
    return sub1;
}
/**
 * notostring function
 *this function returns string representation of a int
 * @param n : int no.
 * @return st :string
 */
string Common::notostring(int n) {
    stringstream s1;
    string st;
    s1 << n;
    st = s1.str();
    return st;
}
/**
 * splitstring function
 *this function intakes a string and a delimeter and splits the string based on the 
 * delimeter and returns a vector of string containing tokens
 * @param  input_string
 * @param  delim :delimeter
 * @return tokens : vector containing all the tokens
 */
vector<string> Common::splitstring(string input_string, string delim) {
    vector<string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = input_string.find(delim)) != std::string::npos) {
        token = input_string.substr(0, pos);
        tokens.push_back(token);
        input_string.erase(0, pos + delim.length());
    }
    tokens.push_back(input_string);
    return tokens;
}
/**
 * gettime function
 *this function returns the current time
 * @return t1 : current time
 */
double Common::gettime() {
    struct timeval tim;
    gettimeofday(&tim, NULL);
    double t1 = tim.tv_sec + (tim.tv_usec / 1000000.0);
    return t1;
}
/**
 * main function
 *this funct. does a case insensitive comparison of two strings and returns boolean value depending on comparison
 * @param  a first string
 * @param  b  second string
 * @return boolean value 
 */
bool Common::iequals_ignorecase(string a, string b) {
    int size_a = a.size();
    int size_b = b.size();
    if (size_a != size_b)
        return false;
    for (int i = 0; i < size_a; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}
/**
 * stringtoint function
 *this function returns the int format of a no. in string format
 * @param no :no in string format
 * @return numb integer format of the no. in string format
 */
int Common::stringtoint(string no) {

    int numb;
    istringstream(no) >> numb;
    return numb;
}
/**
 * creator function
 * this function displays the creator info with policy acknowledgement
 */
void Common::creator() {
    cout << endl<<"Name         	  : Alok Vardhan" << endl;
    cout << "UBIT NAME    	  : alokvard" << endl;
    cout << "UB email address   : alokvard@buffalo.edu" << endl;
    printf( "I  have  read  and  understood  the   course   academic  integrity  policy  located  at http://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/index.html#integrity\n");
}
/**
 * help function
 *this function displays help
 */
void Common::help() {
    cout << endl<<"1.CREATOR :" << endl;
    cout << "Display  your  full  name,  your  UBIT  name,  and  your  UB  email  address,  followed  by  the" << endl
            << "sentence  “I  have  read  and  understood  the   course   academic  integrity  policy  located  at" << endl
            << "http://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/index.html#integrity”." << endl;
    cout << endl<<"2. HELP  :" << endl;
    cout<<"Display help for all commands..."<<endl;
    cout << endl<<"3. MYIP   :" << endl;
    cout<<"Display the actual IP address of this process."<<endl;
    cout << endl<<"4. MYPORT   :" << endl;
    cout<<"Display the port on which this process is listening for incoming connections."<<endl;
    cout << endl<<"5.REGISTER  <server  IP>  <port_no>   :" << endl;
    cout << "This command  is  used  by  the  client  to  register itself with the server  and  to  get"<<endl;
    cout<<"the  IP  and  listening  port  numbers of all the peers currently registered with the server" << endl
            << "sentence  “I  have  read  and  understood  the   course   academic  integrity  policy  " << endl
            << "The  REGISTER  command  takes  2 arguments. The  first  argument  is  the  IP  address "<<endl
            <<"of  the server and the second argument is the listening port of the server."<<endl;
    
    cout << endl<<"5.CONNECT  <destination>  <port  no>    :" << endl;
    cout << "This   command  establishes  a  new  TCP  connection  to  the specified  <destination>"<<endl
         <<" at  the  specified  < port  no>. The <destination> can be either an IP address or a" << endl
         << "hostname,  e.g.,  CONNECT  euston.cse.buffalo.edu  3456  or  CONNECT  192.168.45.55  3456  " << endl;
    
    cout << endl<<"7.LIST  :" << endl;
    cout << "This   command  Display  a   numbered  list  of  all  the  connections  this  process  is  part  of."<<endl
         <<" This  numbered  list  will include  connections initiated " << endl
         << " by  this  process  and  connections  initiated  by  other  processes.  " << endl;

    cout << endl<<"8.   TERMINATE  <connection  id>  :" << endl;
    cout << "This  command  will  terminate  the  connection  listed  under  the"<<endl
         <<" specified  number  when  LIST  is  used  to  display  all  connections " << endl
         << " e.g., TERMINATE  2 should  terminate the  connection  which  has  the  connection  id  2. " << endl;
    
    cout << endl<<"9.  EXIT   :" << endl;
    cout << "Close  all  connections  and  terminate  the  process."<<endl
         <<" When  a  client  exits,  the server de­registers " << endl
         << "the  client  and  sends  the  updated  “Server­IP­List”  to  all  the  clients. " << endl
         <<" Clients on receiving the updated list from the server should display the updated list."<<endl;   
    
    cout << endl<<"10.  UPLOAD  <connection  id>  <file  name>    :" << endl;
    cout << " For  example,  ‘UPLOAD  3  /local/Fall_2014/dimitrio/a.txt’will  upload  the  file  a.txt  which"<<endl
         <<" When  a  client  exits,  the server de­registers " << endl
         << "is  located  in  /local/Fall_2014/dimitrio/,  to  the  host  on  the  connection  that  " << endl
         <<" has  connection  id  3.  An  error  message  is  displayed  if   the  file   was  inaccessible  or  if  3  does  not"<<endl
         <<"represent  a  valid  connection.  The  remote  machine  will  automatically  accept  the  file and  save it"<<endl
         <<"under  the  original  name   in  the  same  directory  where  your  program  is. "<<endl
         <<"At  the  end  of each successful upload, you  will print  the rate at which the Transmitter (Sender/Uploader) uploaded  the file."<<endl
         <<"At Tx end :"<<endl
         <<"Tx: embankment  ­> euston, File Size: x Bytes, Time Taken: y seconds, Tx Rate: z bits/second "<<endl   
         <<"At Rx end :"<<endl
         <<"Rx: embankment  ­> euston, File Size: x Bytes, Time Taken: y seconds, Rx Rate: z bits/second "<<endl  ; 
            
    cout << endl<<"11.  DOWNLOAD  <connection id  1>  <file1> <connection  id  2> <file2> <connection id 3> <file3>   :" << endl;
    cout << " This  command  will  download a file from each host specified in  the command. "<<endl
         <<" Please note that the total number  of  hosts  can  be  1,  2  or  3. " << endl
         <<"  The  hosts  will  be  part  of  the  list  of  hosts  displayed  with  the  LIST command."<<endl
         <<"E.g.,  if  a  command DOWNLOAD  2 file1 3 file2 4 file3 is entered for a process  running"<<endl
         <<"on underground,  then this  process  will  request  file1  from  embankment, "<<endl
         <<"file2  from  highgate  and  file3  euston.  All  these"<<endl
         <<"At  the  end  of each successful download, you  will print  the rate at which the Transmitter (Sender/Uploader) uploaded  the file."<<endl
         <<"At Tx end :"<<endl
         <<"Tx: embankment  ­> euston, File Size: x Bytes, Time Taken: y seconds, Tx Rate: z bits/second "<<endl   
         <<"At Rx end :"<<endl
         <<"Rx: embankment  ­> euston, File Size: x Bytes, Time Taken: y seconds, Rx Rate: z bits/second "<<endl   ;
    
     cout << endl<<"12.  STATISTICS    :" << endl;
     cout<<"Each   client  should  maintain  the  following  statistics  about  each  of  the  clients"<<endl
         <<"displayed  with the LIST command: number of uploads  to that client, average upload speed (in bits/sec), "<<endl    
         <<"number  of  downloads  from  that  client,  average  download  speed (in  bits/sec)."<<endl    
         <<"The  server  should  also  maintain  the   same  statistics  for all  registered  clients"<<endl
         <<"When  the  command  STATISTICS is  executed  on  the  server,  the  server  should  contact  each  client  in"<<endl
         <<"the  Server­IP­List,  get  updated  statistics,  and  display  all  of  them"<<endl;        
}
/**
 * gethostname function
 *This function returns hostname corresponding to its ip
 * @param  ip ip to be converted
 * @return ser_name :name of host corresponding to ip
 * ref:beejs guide
 */
string Common::gethostname(string ip) {
    struct hostent *he, *he1;
    struct in_addr ipv4addr;
    struct in_addr **addr_list;
    inet_pton(AF_INET, ip.c_str(), &ipv4addr);
    he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
    if (he == NULL) {
        return "invalid";
    }
    char *name = he->h_name;
    string ser_name = (string) name;
    return ser_name;
}
/**
 * longtostring function
 *This function returns string format of corresponding long no
 * @param  no long no to be converted to string
 * @return string format of no 
 */
string Common::longtostring(long no) {
    stringstream s1;
    string st;
    s1 << no;
    st = s1.str();
    return st;
}
/**
 * gethostip function
 *This function returns hostip corresponding to its name
 * @param  name :name of the host
 * @return IP of host corresponding to its name
 * reference beejs guide
 */
string Common::gethostip(string name) {
    struct hostent *he, *he1;
    struct in_addr ipv4addr;
    struct in6_addr ipv6addr;
    struct in_addr **addr_list;
    if ((he = gethostbyname(name.c_str())) == NULL) { // get the host info
        herror("gethostbyname");
        return "invalid";
    }
    addr_list = (struct in_addr **) he->h_addr_list;
    char *ip;
    string name_ip;
    for (int i = 0; addr_list[i] != NULL; i++) {
        ip = inet_ntoa(*addr_list[i]);
        name_ip = (string) ip;
        //        printf("%s ", name_ip);
        break;
    }
    return name_ip;
}
/**
 * myIp function
 *This function returns the actual ip of local machine
 * reference :http://stackoverflow.com/questions/212528/get-the-ip-address-of-the-machine
 * @return IP 
 */
string Common::myIp() {
   char buffer[200];
    memset(buffer,0,200);
    size_t buflen=50;
    assert(buflen >= 16);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sock != -1);

    const char* kGoogleDnsIp = "8.8.8.8";
    uint16_t kDnsPort = 53;
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(kGoogleDnsIp);
    serv.sin_port = htons(kDnsPort);

    int err = connect(sock, (const sockaddr*) &serv, sizeof(serv));
    assert(err != -1);

    sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (sockaddr*) &name, &namelen);
    assert(err != -1);

    const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, buflen);
    assert(p);
    string ip=(string)p;
//      close(sock);
    return ip;

}