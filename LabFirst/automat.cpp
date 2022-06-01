#include<fstream>
#include<iostream>
#include<stack>
#include<algorithm>
#include<queue>
#include"automat.h"
void Automat::modernEE_(std::string begin)
{
	std::set<std::string> going;
	std::queue<std::string> base;
	base.push(begin);
	going.insert(begin);
	while (base.size() != 0)
	{
		auto str = base.front();
		base.pop();
		/*if (str != begin)*/
		auto finding = body_[str].find('e');
		if (finding != body_[str].end()) {
			for (auto i : (*finding).second)
			{
				if (going.find(i) == going.end()) {
					going.insert(i);
					base.push(i);
					body_[begin]['e'].insert(i);
					//std::cout <<begin<<" " << i << "\n";
				}
			}
		}
	}
}
void Automat::newEndCondition_()
{
	std::set<std::string> newEndCondition;
	for (auto i : body_)
	{
		auto finding = body_[i.first].find('e');
		if (finding != body_[i.first].end())
		{
			for (auto j : end_condition_)
			{
				auto finding_end = (*finding).second.find(j);
				if (finding_end != (*finding).second.end())
				{
					newEndCondition.insert(i.first);
				}
			}
		}
	}
	for (auto j : end_condition_)
	{
		newEndCondition.insert(j);
	}
	end_condition_ = std::move(newEndCondition);
}
void Automat::newConnection_()
{
	for (auto i : body_)
	{
		auto finding = body_[i.first].find('e');
		if (finding != body_[i.first].end())
		{
			for (auto j : (*finding).second)
			{
				for (auto k : body_[j])
				{
					for (auto t : body_[j][k.first])
					{
						if (body_[i.first].find(k.first)!= body_[i.first].end()) {
							body_[i.first][k.first].insert(t);
						}
						else
							body_[i.first].insert({ k.first,{t} });
					}
				}
			}
		}
	}
}
void Automat::deleteE_()
{
	alphabet_.erase('e');
	for (auto i = body_.begin(); i != body_.end();i++) {
		(*i).second.erase('e');
	}
}
void Automat::deleteNotConnection_()
{
	std::map<std::string, int> counting;
	for (auto i : body_)
	{
		counting.insert({ i.first, 0 });
	}
	for (auto i : body_)
	{
		for (auto j : i.second)
		{
			for (auto k : j.second)
			{
				counting[k]++;
			}
		}
	}
	for (auto i : counting)
	{
		if (i.second == 0 && i.first!=begin_condition_) {
			body_.erase(i.first);
			end_condition_.erase(i.first);
		}
	}
}
std::pair<std::set<std::string>, std::set<std::string>> Automat::spliting(const std::set<std::string>& R, const std::pair<std::set<std::string>, char>& C)
{
	std::set<std::string> R1, R2;
	for (auto i : R)
	{
		auto finding = body_[i].find(C.second);
		if (finding != body_[i].end())
		{
			std::set<std::string> un;
			std::set_intersection(C.first.begin(), C.first.end(), (*finding).second.begin(), (*finding).second.end(), std::inserter(un, un.begin()));
			if (un.size() != 0) {
				R1.insert(i);
			}
			else
				R2.insert(i);
		}
		else {
			R2.insert(i);
		}
	}
	return {R1,R2};
}
void Automat::doFinishBuildDeterminal_()
{
	int alpha_size = alphabet_.size();
	int count = body_.size() + 1;
	std::string nameZero = "D" + std::to_string(count);
	std::map<char, std::set<std::string>> part;
	for (auto i : alphabet_)
	{
		part.insert({ i,{nameZero} });
	}
	int count_notD = 0;
	for (auto i : body_)
	{
		if (i.second.size() != alpha_size)
		{
			count_notD++;
			for (auto j : alphabet_)
			{
				if (body_[i.first].find(j) == body_[i.first].end())
				{
					body_[i.first].insert({ j,{nameZero} });
				}
			}
		}
	}
	if (count_notD != 0)
	{
		body_.insert({ nameZero,part });
	}
}
Automat::Automat(const std::set<char>& alpabet, const std::map<std::string, std::map<char, std::set<std::string>>>& body, const std::string& begin, const std::set<std::string>& end)
{
	body_ = body;
	alphabet_ = alpabet;
	begin_condition_ = begin;
	end_condition_ = end;
	if (alphabet_.find('e') != alphabet_.end())
	{
		is_eps_ = true;
	}
	is_dka_ = false;
}
inline int Automat::size()const
{
	return body_.size();
}

void Automat::readFromFile(const std::string& file_name)
{
	std::ifstream F(file_name);
	int countCondition;
	F >> countCondition;
	char letter;
	F >> letter;
	while (letter != ']')
	{
		F >> letter;
		if (letter == 'e') {
			is_eps_ = true;
			is_dka_ = false;
		}
		if (letter != ',' && letter != ']')
			alphabet_.insert(letter);
	}
	for (int i = 0; i < countCondition; i++)
	{
		std::string condition;
		while (true)
		{
			F >> letter;
			if (letter == '=')break;
			condition.push_back(letter);
		}
		F >> letter;
		std::map<char, std::set<std::string>> part;
		while (letter != '}')
		{
			F >> letter;
			char c = letter;
			F >> letter >> letter;
			std::set<std::string> s;
			std::string str;
			while (letter != ']')
			{
				F >> letter;
				if (letter == ',' || letter == ']')
				{
					s.insert(str);
					str.clear();
				}
				else
					str.push_back(letter);
			}
			part.insert({ c,s });
			s.clear();
			F >> letter;
		}
		body_.insert({ condition,part });
		part.clear();
		condition.clear();
	}
	F >> letter;
	begin_condition_.push_back(letter);
	while (letter != '[')
	{
		F >> letter;
		begin_condition_.push_back(letter);
	}
	begin_condition_.pop_back();
	std::string str;
	while (letter != ']')
	{
		F >> letter;
		if (letter == ',' || letter == ']')
		{
			end_condition_.insert(str);
			str.clear();
		}
		else
			str.push_back(letter);
	}
	F.close();
	is_dka_ = isDeterminal();
}

void Automat::toNoEpsial()
{
	if (!is_eps_ || is_dka_)return;
	is_eps_ = false;
	for (auto i : body_) {
		modernEE_(i.first);
	}
	newEndCondition_();
	newConnection_();
	deleteE_();
	deleteNotConnection_();
}

void Automat::toDeterminal()
{
	if (is_dka_ == true)return;
	toNoEpsial();
	alphabet_.erase('e');
	is_dka_ = true;
	int count = 1;
	std::map<std::string, std::map<char, std::set<std::string>>> new_body;
	std::map<std::string,std::string> unique;
	std::stack<std::set<std::string>> stacking;
	std::set<std::string> new_end_condition;
	unique.insert({ begin_condition_,"D" + std::to_string(count) });
	count++;
	stacking.push({ begin_condition_ });
	
	while (stacking.size() != 0) {
		auto top_set = stacking.top();
		stacking.pop();
		std::map<char, std::set<std::string>> part;
		for (auto i : top_set)
		{
			for (auto j : body_[i]) {
				if (part.find(j.first) != part.end()) {
					part[j.first] = Union<std::string>(part[j.first], j.second);
				}
				else 
				{
					part.insert(j);
				}
			}	
		}
		for (auto i : part)
		{
			std::set<std::string> cond_end;
			std::set_intersection(end_condition_.begin(), end_condition_.end(), i.second.begin(), i.second.end(), std::inserter(cond_end,cond_end.begin()));
			auto sum_set = MakeStr(i.second);
			if (unique.find(sum_set) == unique.end())
			{
				unique.insert({ sum_set, "D" + std::to_string(count) });
				count++;
				stacking.push(i.second);
			}
			part[i.first] = { unique[sum_set] };
			if (cond_end.size() != 0) {
				new_end_condition.insert(unique[sum_set]);
			}
		}
		new_body.insert({ unique[MakeStr(top_set)],part });
	}
	end_condition_ = std::move(new_end_condition);
	begin_condition_ = unique[begin_condition_];
	body_ = std::move(new_body);
	doFinishBuildDeterminal_();
}

bool Automat::isDeterminal() const
{
	for (auto i : body_)
	{
		for (auto j : i.second)
		{
			if (j.second.size() != 1) {
				return false;
			}
		}
	}
	return true;
}

void Automat::minimizeDeterminal()
{
	std::set<std::string> no_end_condition;
	for (auto i : body_)
	{
		if (end_condition_.find(i.first) == end_condition_.end())
			no_end_condition.insert(i.first);
	}
	std::set < std::set<std::string>> P = { no_end_condition,end_condition_ };
	std::queue < std::pair<std::set<std::string>, char>> stacking;
	for (auto i : alphabet_)
	{
		stacking.push({ no_end_condition,i });
		stacking.push({ end_condition_,i });
	}
	while (stacking.size() != 0)
	{
		auto top_stack = stacking.front();
		stacking.pop();
		auto copy_P = P;
		for (auto part : P)
		{
			auto two_parts = spliting(part, top_stack);
			if (two_parts.first.size() != 0 && two_parts.second.size() != 0)
			{
				copy_P.erase(part);
				copy_P.insert(two_parts.first);
				copy_P.insert(two_parts.second);
				for (auto i : alphabet_)
				{
					stacking.push({ two_parts.first, i });
					stacking.push({ two_parts.second,i });
				}
			}
		}
		P = std::move(copy_P);
	}

	//блок переделки автомата из сжатого множества P
	int count = 1;
	std::map < std::string, std::string > unique;
	for (auto i : P)
	{
		for (auto j : i) 
		{	
			unique.insert({ j,"MD" + std::to_string(count) });
		}
		count++;
	}
	std::map<std::string, std::map<char, std::set<std::string>>> new_body;
	std::set<std::string> new_end_condition;
	for (auto i : P)
	{
		std::map<char, std::set<std::string>> part;
		for (auto j : i)
		{
			for (auto k : body_[j])
			{
				
				if (part.find(k.first) != part.end()) {
					part[k.first] = Union<std::string>(part[k.first], k.second,unique);

				}else
					part.insert({ k.first, Union<std::string>(k.second, k.second,unique) });
			}
			if (end_condition_.find(j) != end_condition_.end())
			{
				new_end_condition.insert(unique[j]);
			}
		}
		new_body.insert({ unique[*i.begin()],part });
	}
	end_condition_ = std::move(new_end_condition);
	body_ = std::move(new_body);
	begin_condition_ = unique[begin_condition_];
}

void Automat::printInFile(const std::string& file_name) const
{
	std::ofstream F(file_name);
	F << size();
	F << "\n[";
	for (auto i=alphabet_.begin();i!=alphabet_.end();i++) 
	{
		F << *i;
		if (++i != alphabet_.end())
			F << ',';
		i--;
	}
	F << "]\n";
	for (auto i : body_)
	{
		F << i.first << "={";
		if (i.second.size() == 0) {
			F << "}" << std::endl;
			continue;
		}
		for (auto j = i.second.begin(); j != i.second.end(); j++)
		{
			F << (*j).first << ":[";
			for (auto k = (*j).second.begin(); k != (*j).second.end(); k++)
			{
				F << *k;
				if (++k != (*j).second.end())
					F << ',';
				k--;
			}
			if (++j != i.second.end())
				F << "],";
			j--;
		}
		F << "]}\n";
	}
	F << begin_condition_ << std::endl << '[';
	for (auto i = end_condition_.begin(); i != end_condition_.end(); i++)
	{
		F << *i;
		if (++i != end_condition_.end())
			F << ',';
		i--;
	}
	F << ']';
	F.close();
}

bool Automat::readString(const std::string& str)
{
	auto b = begin_condition_;
	if (is_dka_)
	{
		for (auto i : str)
		{
			if (body_[b].find(i) == body_[b].end())
			{
				return false;
			}
			b = *body_[b][i].begin();
		}
		if (end_condition_.find(b) == end_condition_.end())
			return false;
		return true;
	}
	/*char part;
	int i = 0;
	std::set<std::string> conditions = { b };
	while (true)
	{
		std::set<std::string> new_conditions;
		if (i < str.size())
		{
			part = str[i];
			i++;
		}
		else part = 'e';
		for (auto i : conditions)
		{
			auto finding = body_[i].find(part);
			if (finding != body_[i].end() )
			{
				new_conditions = Union<std::string>(new_conditions, (*finding).second);
			}
		}
		if (new_conditions.size() == 0 && part != 'e')
		{
			return false;
		}
		if (part == 'e' || i == str.size())
		{
			for (auto j : end_condition_)
			{
				if (new_conditions.find(j) != new_conditions.end())
					return true;
			}
		}
		if (part == 'e' && new_conditions==conditions)
		{
			return false;
		}
		conditions = new_conditions;
	}
	return true;*/

	int i = 0;
	std::set<std::pair<std::string, int>> stacking;
	stacking.insert({ begin_condition_,i });
	while (true)
	{
		std::set<std::pair<std::string, int>> new_stacking;
		for (auto i : stacking)
		{
			if (i.second < str.size()) {
				auto finding = body_[i.first].find(str[i.second]);
				if (finding != body_[i.first].end())
				{
					for (auto j : (*finding).second)
					{
						if (end_condition_.find(j) != end_condition_.end() && i.second + 1 == str.size())
							return true;
						int k = i.second + 1;
						new_stacking.insert({ j,k });
					}
				}
			}
			auto finding = body_[i.first].find('e');
			if (finding != body_[i.first].end())
			{
				for (auto j : (*finding).second)
				{
					if (end_condition_.find(j) != end_condition_.end() && i.second + 1 >= str.size())
						return true;
					new_stacking.insert({ j,i.second });
				}
			}
		}
		if (new_stacking.size() == 0)
			return false;
		if (stacking == new_stacking)
			return false;
		stacking = new_stacking;
	}
	return true;
}

std::string MakeStr(const std::set<std::string>& arg)
{
	std::string ans;
	for (auto i : arg) 
	{
		ans += i;
	}
	return ans;
}
