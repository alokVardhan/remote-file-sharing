/**
 * @alokvard_assignment1
 * @author  alok vardhan <alokvard@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
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
#include <iostream>
#include "../include/global.h"

using namespace std;

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
	int PORT;

	if(argc!=3){
		cout<<"Invalid input...usage : s/c port#";
		exit(1);
	}
	if(string(argv[2]).length()<4){
		cout<<"enter port greator than 1000...";
		exit(1);
	}
	istringstream ( string(argv[2]) ) >> PORT;
	
	string sc=string(argv[1]);
	if(sc=="c"||sc=="C"){
            Client client;
            client.shell(PORT);
	}
	else if(sc=="s"||sc=="S"){
        
		Server server;
		server.shell_s(PORT);

	}
	else{
		cout<<"Invalid i/p...usage : s/c port#";
		exit(1);
	}

	if(string(argv[2]).length()<4){
		cout<<"enter port greator than 1000...";
		exit(1);
	}

	return 0;
}
