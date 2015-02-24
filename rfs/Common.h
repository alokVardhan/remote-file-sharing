/* 
 * File:   Common.h
 * Author: nexus
 *
 * Created on September 14, 2014, 12:16 PM
 */

#ifndef COMMON_H
#define	COMMON_H
#include <string>
#include<map>
#include<vector>
using namespace std;
class Common {
public:
//    map to store host ip
    static map<string, string >host_ip;
//map to store ip host
    static map<string, string >ip_host;
    Common();
    void creator();
    void help();
    string myIp();
    bool iequals_ignorecase(string,string );
    vector<string> splitstring(string input_string,string delim);
    string gethostname(string ip);
    string gethostip(string name);
    string notostring(int n);
    int stringtoint(string no);
    double gettime();
    string getfilename(string);
    string longtostring(long no);
private:

};

#endif	/* COMMON_H */

