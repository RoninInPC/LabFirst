#include<regex>
#include<vector>
#include<stack>
#include<algorithm>
#include <sstream>
#include <iostream>
#include <vector>
#ifndef REGULAR_H
#define REGULAR_H
#include"automat.h"
enum class TermType {
	t_const,
	t_opbr,
	t_clbr,
	t_op
};
struct Term {
	std::string name;
	TermType type;
};
class RegularExpression {
private:
	std::string body_;
	std::set<char> alphabet_;
	std::set<char> delim_ = { '.','+',')','(','*' };
	std::map<std::string, int> priority_{
		{"+",1},
		{".",2},
		{"*",3}
	};
	std::vector<Term> getTermsFromBody_();
	std::vector<Term> getPostfixFromTerms_(const std::vector<Term>& terms);
	void newBody_(const std::string& str);
	void modificationAnd_(std::map<std::string, std::map<char, std::set<std::string>>>& body, std::stack<std::string>& stacking, int& count);
	void modificationOr_(std::map<std::string, std::map<char, std::set<std::string>>>& body, std::stack<std::string>& stacking, int& count);
	void modificationR_(std::map<std::string, std::map<char, std::set<std::string>>>& body, std::stack<std::string>& stacking, int& count);
	void deleteConditionToRegular_(std::map<std::string, std::map<std::string, std::set<std::string>>>& body_arr, std::set<std::string>& conditions,const std::set<std::string>& end_cond,const std::string& begin_cond);
	std::string gatherAns_(std::map<std::string, std::map<std::string, std::set<std::string>>>& body_arr, const std::string& begin_cond, const std::string& end_cond);
	void modificateCycle_(std::map<std::string, std::map<std::string, std::set<std::string>>>& body_arr, const std::string& condition, const std::string& deleting);
	void modificateTo_(std::map<std::string, std::map<std::string, std::set<std::string>>>& body_arr, const std::string& condition, const std::string& deleting, const std::string& connected);
	bool getRegularCorrect_(const std::string& str, const std::string& str2);
public:
	RegularExpression() = default;
	RegularExpression(const std::string& str);
	RegularExpression(Automat A);
	bool isCorrect(const std::string& str);
	Automat makeAutomat(const std::string& str);
	Automat makeAutomat();
	friend std::ostream& operator << (std::ostream& out, RegularExpression& arg);
};
std::vector<std::string> split(const std::string& s, char delim);
std::string sumSet(const std::set<std::string>& s);
void stripBrackets(std::string& str);
#endif