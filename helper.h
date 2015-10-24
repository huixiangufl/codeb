/*
 * helper.h
 *
 *  Created on: Oct 23, 2015
 *      Author: jiade
 */

#ifndef HELPER_H_
#define HELPER_H_

#include <string>
#include <unordered_map>

using namespace std;


struct CompanyAttributes
{
	double max_bid;
	double min_ask;
	int max_bid_share;
	int min_ask_share;
	double div_ratio;
	double net_worth;
	//my data
	double my_div_ratio = 0;
	int my_shares = 0;

	double total_share;
};

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
/**
 * send command, return result
 */
string SendCommand (const string& cmd);

unordered_map<string, CompanyAttributes>
FetchCompanies ();

bool FetchCompanyAttributes (CompanyAttributes& attributes, const string& company);

bool Buy ();
bool Sell ();

#endif /* HELPER_H_ */
