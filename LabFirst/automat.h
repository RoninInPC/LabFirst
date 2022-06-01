#include<string>
#include<set>
#include<map>
#include"regular.h"
#ifndef AUTOMAT_H
#define AUTOMAT_H
class Automat 
{
private:
	bool is_eps_ = false;
	bool is_dka_ = true;
	std::set<char> alphabet_;
	std::map<std::string, std::map<char, std::set<std::string>>> body_;
	std::string begin_condition_;
	std::set<std::string> end_condition_;
	void modernEE_(std::string begin);
	void newEndCondition_();
	void newConnection_();
	void deleteE_();
	void deleteNotConnection_();
	std::pair< std::set<std::string>, std::set<std::string>> spliting(const std::set<std::string>& R,const std::pair< std::set<std::string>,char>& C);
	void doFinishBuildDeterminal_();
public:
	Automat() = default;
	Automat(const std::set<char>& alpabet, const std::map<std::string, std::map<char, std::set<std::string>>>& body, const std::string& begin, const std::set<std::string>& end);
	int size()const;
	void readFromFile(const std::string& file_name);
	void toNoEpsial();
	void toDeterminal();
	bool isDeterminal()const;
	void minimizeDeterminal();
	void printInFile(const std::string& file_name)const;
	bool readString(const std::string& str);
	friend class RegularExpression;
};
template<typename T>
std::set<T> Union(const std::set<T>& first, const std::set<T>& second)
{
	std::set<T> ans = first;
	for (auto i : second)
	{
		ans.insert(i);
	}
	return ans;
}
template<typename T>
std::set<T> Union(const std::set<T>& first, const std::set<T>& second,std::map<T,T> unique)
{
	std::set<T> ans;
	for (auto i : first)
	{
		if (unique.find(i) != unique.end())
			ans.insert(unique[i]);
		else
			ans.insert(i);
	}
	for (auto i : second)
	{
		if (unique.find(i) != unique.end())
			ans.insert(unique[i]);
		else
			ans.insert(i);
	}
	return ans;
}
std::string MakeStr(const std::set<std::string>& arg);
#endif