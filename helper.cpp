/*
 * helper.cpp
 *
 *  Created on: Oct 23, 2015
 *      Author: jiade
 */
#include "galik_socketstream.h"
#include <assert.h>
#include <sstream>
#include <vector>
#include <limits>
#include <cmath>
#include <iostream>
#include <unordered_set>

#include "helper.h"


using namespace galik;
using namespace galik::net;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	size_t start = 0, end = 0;
	while ((end = s.find(delim, start)) != string::npos) {
		elems.push_back(s.substr(start, end - start));
		start = end + 1;
	}
	elems.push_back(s.substr(start));
	return elems;
}

string SendCommand (const string& cmd)
{
	assert (!cmd.empty());
	string name("MXA");
	string password("mxa12345");

	socketstream ss;
	if (!ss.open("codebb.cloudapp.net", 17429))
		return "";

	ss << name << " " << password << "\n" << cmd << "\nCLOSE_CONNECTION" << endl;
	stringstream strs;
	while (ss.good() && !ss.eof()) {
		string line;
		getline(ss, line);
		strs << line << endl;
	}
	return strs.str();
}

bool FetchCompanyAttributes (CompanyAttributes& attributes, const string& company)
{
	string str = SendCommand(string("ORDERS ") + company);
	vector<string> elems;
	split (str, ' ', elems);
	if (elems.empty() || elems[0] != "SECURITY_ORDERS_OUT")
		return false;
	double min_ask = numeric_limits<double>::max();
	double max_bid = numeric_limits<double>::min();
	int max_bid_share = 0;
	int min_ask_share = 0;
	int total_share = 0;
	for (int i = 1; i < elems.size(); i += 4)
	{
		if (elems[i] != "BID" && elems[i] != "ASK")
			continue;
		double price = stod(elems[i+2]);
		int cur_share = stoi(elems[i+3]);
		if (elems[i] == "BID")
		{
			if (price > max_bid)
			{
				max_bid = price;
				max_bid_share = cur_share;
			}
			//max_bid = max(max_bid, price);
		}
		else if (elems[i] == "ASK")
		{
			if (price < min_ask)
			{
				min_ask = price;
				min_ask_share = cur_share;
			}
			//min_ask = max(min_ask, stod(elems[i+2]));
			total_share += cur_share;
		}
	}
	attributes.min_ask = min_ask;
	attributes.max_bid = max_bid;
	attributes.max_bid_share = max_bid_share;
	attributes.min_ask_share = min_ask_share;
	attributes.total_share = total_share;

	return true;
}

unordered_map<string, CompanyAttributes>
FetchCompanies ()
{
	unordered_map<string, CompanyAttributes> hash_map;
	string str = SendCommand("SECURITIES");
	vector<string> elems;
	split (str, ' ', elems);
	if (elems.empty() || elems[0] != "SECURITIES_OUT")
		return hash_map;
	for (int i = 1; i < elems.size(); i += 4)
	{
		if (!isalnum(elems[i][0])) continue;
		CompanyAttributes attributes;
		if (FetchCompanyAttributes (attributes, elems[i]))
		{
			// fetch dividend ratio
			attributes.net_worth = stod(elems[i+1]);
			attributes.div_ratio = stod(elems[i+2]);
			hash_map[elems[i]] = attributes;
		}
	}
	return hash_map;
}

bool GetMyCash (double& cash)
{
	string str = SendCommand("MY_CASH");
	vector<string> elems;
	split (str, ' ', elems);
	if (elems.empty() || elems[0] != "MY_CASH_OUT")
		return false;
	cash = stod (elems[1]);
	return true;
}

bool DoBuy (unordered_map<string, CompanyAttributes>& companies, unordered_set<string>& bought)
{
	double val = numeric_limits<double>::min();
	string company;
	double price; double offset = 0.1;
	int shares;
	// find company info
	for (auto i : companies)
	{
		if (bought.find(i.first) != bought.end())
			continue;
		double div = i.second.div_ratio;
		double net_worth = i.second.net_worth;
		double min_ask = i.second.min_ask;
		double max_bid = i.second.max_bid;
		double total_share = i.second.total_share;
		double cur_val = div * net_worth / total_share * (1 / min_ask) * (1 / (min_ask - max_bid));
		//val = max (val, cur_val);
		if (cur_val > val)
		{
			val = cur_val;
			company = i.first;
			price = min_ask + offset;
			shares = i.second.min_ask_share;
		}
	}
	double cash;
	if (!GetMyCash(cash))
		return false;
	// find how many shares to buy
	int n_shares;
	if (cash > price * shares)
	{
		n_shares = min(shares, (int)floor(300/price));
	}
	else
	{
		n_shares = min(floor(cash / price), floor(300/price));
	}
	if (n_shares > 0)
	{
		string cmd = "BID " + company + " " + to_string(price) + " " + to_string(n_shares);
		if (!SendCommand(cmd).empty())
		{
			cout << "Buy:" << endl;
			cout << "cash: " << cash << endl;
			cout << cmd << endl;
			cout << "===============================" << endl;
			bought.insert(company);
			return true;
		}
		return false;

	}
	return false;
}

bool Buy ()
{
	unordered_map<string, CompanyAttributes> hash_map = FetchCompanies ();
	unordered_set<string> bought;
	double cash;
	bool success = false;
	if (GetMyCash(cash))
	{
		success = DoBuy(hash_map, bought);
		while (success && DoBuy(hash_map, bought));
	}
	return success;
}

bool UpdateMyData (unordered_map<string, CompanyAttributes>& my_companies)
{
	string str = SendCommand("MY_SECURITIES ");
	vector<string> elems;
	split (str, ' ', elems);
	if (elems.empty() || elems[0] != "MY_SECURITIES_OUT")
		return false;
	for (int i = 1; i < elems.size(); i += 3)
	{
		if (!isalnum(elems[i][0])) continue;
		my_companies[elems[i]].my_shares = stoi(elems[i+1]);
		my_companies[elems[i]].my_div_ratio = stod(elems[i+2]);
	}
	return true;
}

double GetMaxAsk (const string& company)
{
	string str = SendCommand(string("ORDERS ") + company);
	vector<string> elems;
	split (str, ' ', elems);
	if (elems.empty() || elems[0] != "SECURITY_ORDERS_OUT")
		return false;
	double max_ask = numeric_limits<double>::min();
	for (int i = 1; i < elems.size(); i += 4)
	{
		if (elems[i] != "BID" && elems[i] != "ASK")
			continue;
		if (elems[i] == "ASK")
		{
			max_ask = max(max_ask, stod(elems[i+2]));
		}
	}
	return max_ask;
}

bool DoSell (unordered_map<string, CompanyAttributes>& companies)
{
	double rate = 0.75;
	double price_offset = 0.001;
	string str = SendCommand("MY_SECURITIES ");
	vector<string> elems;
	split (str, ' ', elems);
	if (elems.empty() || elems[0] != "MY_SECURITIES_OUT")
		return false;
	for (int i = 1; i < elems.size(); i += 3)
	{
		if (!isalnum(elems[i][0])) continue;
		if (stoi(elems[i+1]) > 0 && stod(elems[i+2]) < stoi(elems[i+1]) * companies[elems[i]].div_ratio * rate)
		{
			string ticker = elems[i];
			double price = GetMaxAsk(elems[i]) - price_offset;
			int shares = companies[elems[i]].my_shares;
			string cmd = "ASK " + elems[i] + " " + to_string(price) + " " + to_string(shares);
			if (!SendCommand(cmd).empty())
			{
				cout << "Sell:" << endl;
				cout << cmd << endl;
				cout << "===============================" << endl;
			}
		}
	}
	return true;
}

bool Sell ()
{
	unordered_map<string, CompanyAttributes> hash_map = FetchCompanies ();
	UpdateMyData(hash_map);
	DoSell (hash_map);
	return true;
}

