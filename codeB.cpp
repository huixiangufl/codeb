//============================================================================
// Name        : codeB.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include "galik_socketstream.h"
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <vector>
#include "helper.h"

using namespace std;

void test_hashMap()
{
	unordered_map<string, CompanyAttributes>
	hash_map = FetchCompanies();
	for (auto i : hash_map)
		cout << i.first << endl;
}

void test_sendCommand ()
{
	string res = SendCommand("HELP");
	cout << res;
	cout << "good!" << endl;
}

void test_FetchCompanyAttributes ()
{
	CompanyAttributes attributes;
	if (FetchCompanyAttributes (attributes, "EA"))
	{
		cout << "max bid\t" << attributes.max_bid << endl;
		cout << "max bid share\t" << attributes.max_bid_share << endl;
		cout << "min ask\t" << attributes.min_ask << endl;
		cout << "min ask share\t" << attributes.min_ask_share << endl;
		cout << "total share\t" << attributes.total_share << endl;
	}
	else cout << "Error!" << endl;
}

void test_FetchCompanies ()
{
	unordered_map<string, CompanyAttributes> hash_map = FetchCompanies();
	for (auto i : hash_map)
	{
		cout << i.first << endl;
		cout << "max bid\t" << i.second.max_bid << endl;
		cout << "max bid share\t" << i.second.max_bid_share << endl;
		cout << "min ask\t" << i.second.min_ask << endl;
		cout << "min ask share\t" << i.second.min_ask_share << endl;
		cout << "total share\t" << i.second.total_share << endl;
		cout << "net worth\t" << i.second.net_worth << endl;
		cout << "div ratio\t" << i.second.div_ratio << endl;
		cout << "=======================================" << endl;
	}
}

void test_buy()
{
	if (Buy()) cout << "success" << endl;
	else cout << "Error!" << endl;
}

int main(int argc, char** argv) {

	//test_hashMap();
	//test_FetchCompanyAttributes();
	//test_FetchCompanies();
	//test_buy();
	while (true)
	{
		Buy();
		sleep(1);
		Sell();
		sleep(1);
	}
}
