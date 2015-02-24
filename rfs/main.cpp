/* 
 * File:   main.cpp
 * Author: alok vardhan
 *
 * Created on September 14, 2014, 11:51 AM
 */
#include <cstdlib>

#include <cctype>
#include <iostream>
#include <string>
#include<cstdlib>
#include <vector>
#include<sstream>
#include "Server.h"
#include "Client.h"
#include <cstdlib>
#include "Common.h"
using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    int PORT;
    Common common;
    if (argc != 3) {
        cout << "Invalid input...usage : s/c port#";
        exit(1);
    }
    if (string(argv[2]).length() < 4) {
        cout << "enter port greator than 1000...";
        exit(1);
    }
    istringstream(string(argv[2])) >> PORT;
    if (PORT==0) {
        cout << "invalid port..." << endl;
        exit(1);
    }
    
    string sc = string(argv[1]);
    if (sc == "c" || sc == "C") {
        Client client;

        client.shell(PORT);
    } else if (sc == "s" || sc == "S") {

        Server server;
        server.shell_s(PORT);

    } else {
        cout << "Invalid i/p...usage : s/c port#";
        exit(1);
    }

    if (string(argv[2]).length() < 4) {
        cout << "enter port greator than 1000...";
        exit(1);
    }
    return 0;
}

