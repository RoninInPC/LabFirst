
#include"regular.h"

std::ostream& operator<<(std::ostream& out, RegularExpression& arg)
{
	std::string copy;
	std::remove_copy(arg.body_.begin(), arg.body_.end(), std::back_inserter(copy), '.');
	out << copy;
	return out;
}

std::vector<std::string> split(const std::string& s, char delim) {
	std::vector<std::string> elems;
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::string sumSet(const std::set<std::string>& s)
{
	if (s.size() == 0 || (s.size()==1 && (*s.begin()).size() == 0))return std::string();
	std::string ans;
	for (auto i : s)
	{
		ans += i + "+";
	}
	ans.pop_back();
	if (ans.size() == 1) {
		return ans;
	}
	return "("+ans+")";
}

std::vector<Term> RegularExpression::getTermsFromBody_()
{
	std::vector<Term> res;
	auto it = body_.begin();
	while (it != body_.end())
	{
		if (*it == ' ') { ++it; continue; }
		auto st = std::find_first_of(it, body_.end(), delim_.begin(), delim_.end());
		if (it == st) ++st;
		res.push_back({ std::string(it,st),TermType::t_op });
		it = st;
	}
	for (size_t i = 0; i < res.size(); i++)
	{
		char c = res[i].name[0];
		if (isdigit(c) || isalpha(c))
			res[i].type = TermType::t_const;
		else if (c == '(')
			res[i].type = TermType::t_opbr;
		else if (c == ')')
			res[i].type = TermType::t_clbr;
	}
	return res;
}

std::vector<Term> RegularExpression::getPostfixFromTerms_(const std::vector<Term>& terms)
{
	std::vector<Term> res;
	std::stack<Term> ops;
	for (auto t : terms)
	{
		switch (t.type)
		{
		case TermType::t_const: res.push_back(t); break;
		case TermType::t_opbr: ops.push(t); break;
		case TermType::t_clbr:
			while (ops.top().type != TermType::t_opbr)
			{
				res.push_back(ops.top()); ops.pop();
			}
			ops.pop();
			break;
		case TermType::t_op:
			while (ops.size() && (ops.top().type == TermType::t_op && priority_[ops.top().name] >= priority_[t.name]))
			{
				res.push_back(ops.top()); ops.pop();
			}
			ops.push(t);
		}
	}
	while (ops.size())
	{
		res.push_back(ops.top()); ops.pop();
	}
	return res;
}

void RegularExpression::newBody_(const std::string& str)
{
	body_.clear();
	body_ = str[0];
	if (delim_.find(str[0]) == delim_.end())
		alphabet_.insert(str[0]);
	for (int i = 1; i < str.size(); i++) {
		auto finding1 = delim_.find(str[i]),finding2= delim_.find(str[i - 1]);
		if (finding1 == delim_.end())
			alphabet_.insert(str[i]);
		if (finding1 == delim_.end() && finding2 == delim_.end())
		{
			body_.push_back('.');
		}
		if (str[i - 1] == '*' && (str[i] == '(' || finding1 == delim_.end()))
		{
			body_.push_back('.');
		}
		if ((finding2 == delim_.end() && str[i] == '(') || (finding1 == delim_.end() && str[i - 1] == ')'))
		{
			body_.push_back('.');
		}
		if(str[i] == '(' && str[i - 1] == ')')
		{
			body_.push_back('.');
		}
		body_.push_back(str[i]);
	}
}

void RegularExpression::modificationAnd_(std::map<std::string, std::map<char, std::set<std::string>>>& body, std::stack<std::string>& stacking, int& count)
{
	auto second = stacking.top();
	stacking.pop();
	auto first = stacking.top();
	stacking.pop();
	bool f1 = false, f2 = false;
	if (first.size() != 1) {
		f1 = true;
	}
	if (second.size() != 1) {
		f2 = true;
	}
	if (!f1 && !f2)
	{	
		int start = ++count;
		body.insert({ "Q" + std::to_string(count),{ { first[0], {"Q" + std::to_string(++count)} } } });
		body.insert({ "Q" + std::to_string(count),{ { 'e', {"Q" + std::to_string(++count)}}}});
		body.insert({ "Q" + std::to_string(count),{ { second[0], {"Q" + std::to_string(++count)}}} });
		body.insert({ "Q" + std::to_string(count),{} });
		stacking.push("Q" + std::to_string(start) + "+" + "Q" + std::to_string(count));
		return;
	}
	if (f1 && !f2)
	{
		auto splitting = split(first, '+');
		body[splitting[1]]['e'].insert("Q" + std::to_string(++count));
		body.insert({ "Q" + std::to_string(count),{ { second[0], {"Q" + std::to_string(++count)}}} });
		body.insert({ "Q" + std::to_string(count),{} });
		stacking.push(splitting[0] + "+" + "Q" + std::to_string(count));
		return;
	}
	if (!f1 && f2)
	{
		auto splitting = split(second, '+');
		int start = ++count;
		body.insert({ "Q" + std::to_string(count),{ { first[0], {"Q" + std::to_string(++count)}}} });
		body.insert({ "Q" + std::to_string(count),{ { 'e', {splitting[0]}}} });
		stacking.push("Q" + std::to_string(start) + "+" + splitting[1]);
		return;
	}
	auto splitting = split(first, '+');
	auto splitting2 = split(second, '+');
	body[splitting[1]]['e'].insert(splitting2[0]);
	stacking.push(splitting[0] + "+" + splitting2[1]);
}

void RegularExpression::modificationOr_(std::map<std::string, std::map<char, std::set<std::string>>>& body, std::stack<std::string>& stacking, int& count)
{
	auto second = stacking.top();
	stacking.pop();
	auto first = stacking.top();
	stacking.pop();
	bool f1 = false, f2 = false;
	if (first.size() != 1) {
		f1 = true;
	}
	if (second.size() != 1) {
		f2 = true;
	}
	if (!f1 && !f2)
	{
		int start = ++count;
		body.insert({ "Q" + std::to_string(count),{ { 'e', {"Q" + std::to_string(++count),"Q" + std::to_string(++count)} } } });
		body.insert({ "Q" + std::to_string(count - 1),{ { first[0], {"Q" + std::to_string(++count)} } } });
		body.insert({ "Q" + std::to_string(count - 1),{ { second[0], {"Q" + std::to_string(++count)} } } });
		body.insert({ "Q" + std::to_string(count - 1),{ { 'e', {"Q" + std::to_string(++count)} } } });
		body.insert({ "Q" + std::to_string(count - 1),{ { 'e', {"Q" + std::to_string(count)}}}});
		body.insert({ "Q" + std::to_string(count),{} });
		stacking.push("Q" + std::to_string(start) + "+" + "Q" + std::to_string(count));
		return;
	}
	if (f1 && !f2)
	{
		auto splitting = split(first, '+');
		int start = ++count;
		body.insert({ "Q" + std::to_string(count),{ { 'e', {splitting[0],"Q" + std::to_string(++count)} } } });
		body.insert({ "Q" + std::to_string(count),{ { second[0], {"Q" + std::to_string(++count)} } } });
		body[splitting[1]]['e'] = { "Q" + std::to_string(++count) };
		body.insert({ "Q" + std::to_string(count - 1),{ { 'e', {"Q" + std::to_string(count)}}} });
		body.insert({ "Q" + std::to_string(count),{} });
		stacking.push("Q" + std::to_string(start) + "+" + "Q" + std::to_string(count));
		return;
	}
	if (!f1 && f2)
	{
		auto splitting = split(second, '+');
		int start = ++count;
		body.insert({ "Q" + std::to_string(count),{ { 'e', {splitting[0],"Q" + std::to_string(++count)} } } });
		body.insert({ "Q" + std::to_string(count),{ { first[0], {"Q" + std::to_string(++count)} } } });
		body[splitting[1]]['e'] = { "Q" + std::to_string(++count) };
		body.insert({ "Q" + std::to_string(count - 1),{ { 'e', {"Q" + std::to_string(count)}}} });
		body.insert({ "Q" + std::to_string(count),{} });
		stacking.push("Q" + std::to_string(start) + "+" + "Q" + std::to_string(count));
		return;
	}
	auto splitting = split(first, '+');
	auto splitting2 = split(second, '+');
	int start = ++count;
	body.insert({ "Q" + std::to_string(count),{ { 'e', {splitting[0],splitting2[0]} } } });
	body[splitting[1]]['e'].insert("Q" + std::to_string(++count));
	body[splitting2[1]]['e'].insert("Q" + std::to_string(count));
	body.insert({ "Q" + std::to_string(count),{} });
	stacking.push("Q" + std::to_string(start) + "+" + "Q" + std::to_string(count));
}

void RegularExpression::modificationR_(std::map<std::string, std::map<char, std::set<std::string>>>& body, std::stack<std::string>& stacking, int& count)
{
	auto first = stacking.top();
	stacking.pop();
	bool f1 = false;
	if (first.size() != 1) {
		f1 = true;
	}
	if (!f1)
	{
		int start = ++count;
		body.insert({ "Q" + std::to_string(count),{ { 'e', {"Q" + std::to_string(count+3),"Q" + std::to_string(++count)} } } });
		body.insert({ "Q" + std::to_string(count),{ { first[0], {"Q" + std::to_string(++count)} } } });
		body.insert({ "Q" + std::to_string(count),{ { 'e', {"Q" + std::to_string(count-1),"Q" + std::to_string(++count)} } } });
		body.insert({ "Q" + std::to_string(count),{} });
		stacking.push("Q" + std::to_string(start) + "+" + "Q" + std::to_string(count));
		return;
	}
	int start = ++count;
	auto splitting = split(first, '+');
	body.insert({ "Q" + std::to_string(count),{ { 'e', {splitting[0],"Q" + std::to_string(++count)} } } });
	body[splitting[1]]['e'] ={splitting[0],"Q" + std::to_string(count)};
	body.insert({ "Q" + std::to_string(count),{} });
	stacking.push("Q" + std::to_string(start) + "+" + "Q" + std::to_string(count));
}

void RegularExpression::deleteConditionToRegular_(std::map<std::string, std::map<std::string, std::set<std::string>>>& body_arr, std::set<std::string>& conditions, const std::set<std::string>& end_cond, const std::string& begin_cond)
{
	auto deleting = conditions;
	deleting.erase(begin_cond);
	for (auto i : end_cond)
	{
		deleting.erase(i);
	}
	for (auto i : deleting)
	{

		for (auto j : body_arr)
		{
			if (j.first != i) {
				modificateCycle_(body_arr, j.first, i);
				for (auto k : body_arr)
				{
					if (k.first != i && k.first!=j.first)
					{
						modificateTo_(body_arr, j.first, i, k.first);
					}
				}
				body_arr[j.first].erase(i);
			}
		}
		body_arr.erase(i);
	}
}

std::string RegularExpression::gatherAns_(std::map<std::string, std::map<std::string, std::set<std::string>>>& body_arr, const std::string& begin_cond, const std::string& end_cond)
{
	auto sumAtoA = sumSet(body_arr[begin_cond][begin_cond]);
	auto sumAtoB = sumSet(body_arr[begin_cond][end_cond]);
	auto sumBtoB = sumSet(body_arr[end_cond][end_cond]);
	auto sumBtoA = sumSet(body_arr[end_cond][begin_cond]);
	std::string ans,sumAtoBtoA,sumAtoBtoB;
	if (!sumBtoB.empty())
	{
		sumBtoB = "(" + sumBtoB + ")*";
	}
	if (!sumAtoB.empty())
	{
		sumAtoBtoB = sumAtoB;
		if (!sumBtoB.empty())
		{
			sumAtoBtoB += "." + sumBtoB;
		}
	}
	else {
		if (!sumAtoA.empty()) 
		{
			return "(" + sumAtoA + ")*";
		}
		else sumAtoA;
	}

	if (!sumBtoA.empty())
	{
		sumAtoBtoA = sumAtoBtoB + "." + sumBtoA;
	}
	if (!sumAtoA.empty())
	{
		ans = sumAtoA;
		if (!sumAtoBtoA.empty())
		{
			ans += "+" + sumAtoBtoA;
		}
		ans = "(" + ans + ")*";
		if (!sumAtoBtoB.empty())
		{
			ans += "." + sumAtoBtoB;
		}
	}
	else
	{
		ans = sumAtoBtoA;
		if (!ans.empty())
		{
			ans = "(" + ans + ")*";
		}
		if (!ans.empty())
		{
			ans += "." + sumAtoBtoB;
		}
		else ans = sumAtoBtoB;
	}
	return ans;
	/*if (ans.size() != 0)
	{
		ans += "*";
	}
	if (sumAtoB.size() == 0)
		return ans;
	if (ans.size() != 0) {
		ans += "." + sumAtoB;
	}
	else ans = sumAtoB;
	if (ans.size() != 0 && sumBtoB.size() != 0) {
		ans += "." + sumBtoB + "*";
	}
	if (sumBtoA.size() != 0)
	{
		ans = ans + "+(" + ans + "." + sumBtoA + ")*."+sumAtoB;
	}
	if (sumBtoB.size() != 0)
	{
		ans += "." + sumBtoB + "*";
	}*/
	
	return ans;
}

void RegularExpression::modificateCycle_(std::map<std::string, std::map<std::string, std::set<std::string>>>& body_arr, const std::string& condition, const std::string& deleting)
{
	auto sumCtoC = sumSet(body_arr[condition][condition]);
	auto sumCtoD = sumSet(body_arr[condition][deleting]);
	auto sumDtoD = sumSet(body_arr[deleting][deleting]);
	auto sumDtoC = sumSet(body_arr[deleting][condition]);
	std::string modCtoC = sumCtoC;
	if (!sumCtoD.empty() && !sumDtoC.empty())
	{
		if (!modCtoC.empty())
		{
			modCtoC += "+" + sumCtoD;
		}
		else modCtoC = sumCtoD;
		if (!sumDtoD.empty()) {
			modCtoC += ".(" + sumDtoD + ")*";
		}
		modCtoC += "." + sumDtoC;
	}
	if (!modCtoC.empty())
		body_arr[condition][condition] = { modCtoC };
	//std::cout << condition << " " << deleting << " " << modCtoC << "\n";
}

void RegularExpression::modificateTo_(std::map<std::string, std::map<std::string, std::set<std::string>>>& body_arr, const std::string& condition, const std::string& deleting, const std::string& connected)
{
	auto sumDtoD = sumSet(body_arr[deleting][deleting]);
	auto sumCtoD = sumSet(body_arr[condition][deleting]);
	auto sumCtoCon = sumSet(body_arr[condition][connected]);
	auto sumDtoCon = sumSet(body_arr[deleting][connected]);
	std::string modCtoCon = sumCtoCon;
	if (!sumCtoD.empty() && !sumDtoCon.empty())
	{
		if (!modCtoCon.empty())
		{
			modCtoCon += "+" + sumCtoD;
		}
		else modCtoCon = sumCtoD;
		if (!sumDtoD.empty()) {
			modCtoCon += ".(" + sumDtoD + ")*";
		}
		modCtoCon += "." + sumDtoCon;
	}
	if(!modCtoCon.empty())
		body_arr[condition][connected] = { modCtoCon };
	//std::cout << condition << " " << deleting << " " << connected << " " << modCtoCon << "\n";
}

bool RegularExpression::getRegularCorrect_(const std::string& str, const std::string& str2)
{
	auto part = split(str2, '+');
	std::string copy;
	std::remove_copy(body_.begin(), body_.end(), std::back_inserter(copy), '.');
	std::replace(copy.begin(), copy.end(), '+', '|');
	std::regex r(copy);
	return regex_match(str, r);
}

RegularExpression::RegularExpression(const std::string& str)
{
	newBody_(str);
}

RegularExpression::RegularExpression(Automat A)
{
	if (!A.is_dka_)
		A.toDeterminal();
	A.minimizeDeterminal();
	std::map<std::string, std::map<std::string, std::set<std::string>>> body_arr;
	std::set<std::string> conditions;
	for (auto i : A.body_)
	{
		for (auto j : i.second)
		{
			for (auto k : j.second)
			{
				body_arr[i.first][k].insert({ j.first });
			}
		}
		conditions.insert(i.first);
	}
	alphabet_ = A.alphabet_;
	if (A.end_condition_.size() + 1 != conditions.size())
		deleteConditionToRegular_(body_arr, conditions, A.end_condition_, A.begin_condition_);
	if (A.end_condition_.size() == 1)
	{
		body_ = gatherAns_(body_arr, A.begin_condition_, *A.end_condition_.begin());
	}
	else
	{
		body_.clear();
		for (auto i : A.end_condition_)
		{
			auto copy = body_arr;
			deleteConditionToRegular_(copy, conditions, { i }, A.begin_condition_);
			body_ += gatherAns_(copy, A.begin_condition_, i) + "+";
		}
		body_.pop_back();
	}
	std::string copy;
	std::remove_copy(body_.begin(), body_.end(), std::back_inserter(copy), '.');
	stripBrackets(copy);
	//std::cout << copy << "\n";
	newBody_(copy);
	//std::cout << body_ << "\n";
}

bool RegularExpression::isCorrect(const std::string& str)
{
	auto f = true;
	auto pstfx = getPostfixFromTerms_(getTermsFromBody_());
	std::stack<std::string> stacking;
	int i = 0;
	for (auto p : pstfx)
	{
		switch (p.type)
		{
		case TermType::t_const: stacking.push(p.name); break;
		case TermType::t_op:
			if (p.name == ".")
			{
				if (stacking.size() < 2)return false;
				if (delim_.find(stacking.top()[0]) != delim_.end() && stacking.top().size() == 1)return false;
				auto first = stacking.top();
				stacking.pop();
				if (delim_.find(stacking.top()[0]) != delim_.end() && stacking.top().size() == 1)return false;
				auto second = stacking.top();
				stacking.pop();
				stacking.push(first+second);
			}
			if (p.name == "+")
			{
				if (stacking.size() < 2)return false;
				if (delim_.find(stacking.top()[0]) != delim_.end()&& stacking.top().size()==1)return false;
				auto first = stacking.top();
				stacking.pop();
				if (delim_.find(stacking.top()[0]) != delim_.end() && stacking.top().size() == 1)return false;
				auto second = stacking.top();
				stacking.pop();
				stacking.push(first + "+" + second);
			}
			if (p.name == "*")
			{
				if (stacking.size() < 1)return false;
				if (delim_.find(stacking.top()[0]) != delim_.end() && stacking.top().size() == 1)return false;
				auto first = stacking.top();
				stacking.pop();
				auto str = split(first, '+');
				std::string ans;
				for (int i = 0; i < str.size(); i++)
				{
					if (ans.size() != 0) {
						if (ans.back() == '*')ans.pop_back();
					}
					ans += str[i] + "*+";
				}
				ans.pop_back();
				stacking.push(ans);
			}
			break;
		default:
			break;
		}
	}
	auto ans = stacking.top();
	return getRegularCorrect_(str,ans);
}

Automat RegularExpression::makeAutomat(const std::string& str)
{
	newBody_(str);
	alphabet_.insert('e');
	auto pstfx = getPostfixFromTerms_(getTermsFromBody_());
	std::cout << "\n";
	for (auto p : pstfx)
	{
		std::cout << p.name;
	}
	std::cout << "\n";
	std::stack<std::string> stacking;
	std::map<std::string, std::map<char, std::set<std::string>>> body;
	int count = 0;
	for (auto p : pstfx)
	{
		switch (p.type)
		{
		case TermType::t_const: stacking.push(p.name); break;
		case TermType::t_op:
			if (p.name == ".")
			{
				modificationAnd_(body, stacking, count);
			}
			if (p.name == "+")
			{
				modificationOr_(body, stacking, count);
			}
			if (p.name == "*")
			{
				modificationR_(body, stacking, count);
			}
			break;
		default:
			break;
		}
	}
	auto begin_end = stacking.top();
	auto splitting = split(begin_end, '+');
	return Automat(alphabet_, body, splitting[0], { splitting[1] });
}
Automat RegularExpression::makeAutomat()
{
	if (!body_.empty()) {
		std::string copy;
		std::remove_copy(body_.begin(), body_.end(), std::back_inserter(copy), '.');
		return makeAutomat(copy);
	}
	return Automat();
}
void stripBrackets(std::string& S)
{
	std::map<int, bool> pmap;
	for (int i = 0; i < S.size(); i++) {
		std::map<int, bool>::iterator it;
		if (S.at(i) == '(') {
			pmap[i] = true;
		}
		else if (S.at(i) == ')') {
			it = pmap.end();
			it--;
			if (!(*it).second) {
				pmap.erase(it);
			}
			else {
				S.erase(S.begin() + i);
				S.erase(S.begin() + (*it).first);
				pmap.erase(it);
				i = i - 2;
			}
		}
		else {
			if (!pmap.empty()) {
				it = pmap.end();
				it--;
				(*it).second = false;
			}
		}
	}
}
