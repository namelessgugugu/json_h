#pragma warning(disable:4996)
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <utility>
#include <stack>
#include <vector>
#include <map>
#include "json.h"

namespace json
{
	bool bracket_with_enter = 0;
	enum type_of_value
	{
		typeNULL,
		typeINT,
		typeFLOAT,
		typeBOOL,
		typeSTRING,
		typeARRAY,
		typeOBJECT,
		ERROR
	};

	class json_value_int :public json_value
	{
	private:
		long long value;
	public:
		json_value_int(long long v)
		{
			value = v;
			return;
		}
		json_value_int(const void* p)
		{
			value = *((long long*)p);
			return;
		}
		json_value_int(const char* p, const int l, const int r)
		{ 
			value = 0;
			int st = l, f = 0;
			if (p[l] == '-')
				st = l + 1, f = 1;
			for (int i = st; i < r; ++i)
				value = (value << 3) + (value << 1) + p[i] - '0';
			value = f ? -value : value;
			return;
		}
		type_of_value get_type(void)
		{
			return typeINT;
		}
		const void* get_value(void)
		{
			return &value;
		}
	};
	class json_value_float :public json_value
	{
	private:
		double value;
	public:
		json_value_float(double v)
		{
			value = v;
			return;
		}
		json_value_float(const void* p)
		{
			value = *((double*)p);
			return;
		}
		json_value_float(const char* p, const int l, const int r)
		{
			value = 0;
			int st = l, f = 0, i;
			if (p[l] == '-')
				st = l + 1, f = 1;
			for (i = st; i < r && p[i] != '.'; ++i)
				value = (value * 10) + p[i] - '0';
			double tmp = 0;
			int cnt = r - i - 1;
			for (i += 1; i < r; ++i)
				tmp = (tmp * 10) + p[i] - '0';
			while (cnt--)
				tmp /= 10;
			value = f ? -value - tmp : value + tmp;
			return;
		}
		type_of_value get_type(void)
		{
			return typeFLOAT;
		}
		const void* get_value(void)
		{
			return &value;
		}
	};
	class json_value_bool :public json_value
	{
	private:
		bool value;
	public:
		json_value_bool(bool v)
		{
			value = v;
			return;
		}
		json_value_bool(const void* p)
		{
			value = *((bool*)p);
			return;
		}
		json_value_bool(const char* p, const int l, const int r)
		{
			value = p[l] == 't' ? 1 : 0;
			return;
		}
		type_of_value get_type(void)
		{
			return typeBOOL;
		}
		const void* get_value(void)
		{
			return &value;
		}
	};
	class json_value_string :public json_value
	{
	private:
		const char* value;
	public:
		json_value_string(const char* v)
		{
			value = v;
			return;
		}
		json_value_string(const void* p)
		{
			value = *((const char**)p);
			return;
		}
		json_value_string(const char* p, const int l, const int r)
		{
			char* tmp = new char[r - l + 2];
			strncpy(tmp, p + l + 1, r - l - 2);
			tmp[r - l - 2] = 0;
			value = tmp;
			delete[] tmp;
			return;
		}
		type_of_value get_type(void)
		{
			return typeSTRING;
		}
		const void* get_value(void)
		{
			return &value;
		}
	};
	class json_value_array :public json_value
	{
	private:
		json_array value;
	public:
		json_value_array(json_array v)
		{
			value = v;
			return;
		}
		json_value_array(const void* p)
		{
			value = *((json_array*)p);
			return;
		}
		json_value_array(const char* p, const int l, const int r)
		{
			char* tmp = new char[r - l + 2];
			strncpy(tmp, p + l, r - l);
			tmp[r - l] = 0;
			value.read(tmp);
			delete[] tmp;
			return;
		}
		type_of_value get_type(void)
		{
			return typeARRAY;
		}
		const void* get_value(void)
		{
			return &value;
		}
	};
	class json_value_object :public json_value
	{
	private:
		json_object value;
	public:
		json_value_object(json_object v)
		{
			value = v;
			return;
		}
		json_value_object(const void* p)
		{
			value = *((json_object*)p);
			return;
		}
		json_value_object(const char* p, int const l ,int const r)
		{
			char* tmp = new char[r - l + 2];
			strncpy(tmp, p + l, r - l);
			tmp[r - l] = 0;
			value.read(tmp);
			delete[] tmp;
			return;
		}
		type_of_value get_type(void)
		{
			return typeOBJECT;
		}
		const void* get_value(void)
		{
			return &value;
		}
	};
	class json_value_null :public json_value
	{
	private:
		static const int value = 0;
	public:
		type_of_value get_type(void)
		{
			return typeNULL;
		}
		const void* get_value(void)
		{
			return &value;
		}
	};

	int* str_info(const char* str)
	{
		int len = strlen(str), * res = new int[len];
		memset(res, 0xff, sizeof(int) * len);
		std::stack<int> bracket[2];
		int quote = -1;
		for (int i = 0; i < len; ++i)
		{
			if (quote == -1)
			{
				if (str[i] == '\"')
					quote = i;
				else if (str[i] == '{' || str[i] == '[')
					bracket[str[i] != '{'].push(i);
				else if (str[i] == '}' || str[i] == ']')
				{
					int k = (str[i] != '}');
					int tmp = bracket[k].top();
					bracket[k].pop();
					res[i] = tmp, res[tmp] = i;
				}
			}
			else
			{
				if (str[i] == '\\')
				{
					if (str[i - 1] != '\\' || res[i - 1] != -2)
						res[i] = -2;
				}
				else if (str[i] == '\"' && (i == 0 || str[i - 1] != '\\' || res[i - 1] != -2))
					res[quote] = i, res[i] = quote, quote = -1;
			}
		}
		return res;
	}
	type_of_value find_type(const char* str, const int left, const int right)
	{
		if (str[left] == '{')
			return typeOBJECT;
		else if (str[left] == '[')
			return typeARRAY;
		else if (str[left] == '\"')
			return typeSTRING;
		else if (str[left] == 'f' || str[left] == 't')
			return typeBOOL;
		else if (str[left] == 'n')
			return typeNULL;
		else if (std::find(str + left, str + right, '.') == str + right)
			return typeINT;
		else
			return typeFLOAT;
		return ERROR;
	}
	void put_tab(std::vector<char>& str, int cnt)
	{
		while (cnt--)
			str.push_back('\t');
		return;
	}

	void value_to_str(std::vector<char>& vec, long long value, const int tab_count, const bool enter);
	void value_to_str(std::vector<char>& vec, double value, const int tab_count, const bool enter);
	void value_to_str(std::vector<char>& vec, bool value, const int tab_count, const bool enter);
	void value_to_str(std::vector<char>& vec, const char* value, const int tab_count, const bool enter);
	void value_to_str(std::vector<char>& vec, json_array value, const int tab_count, const bool enter);
	void value_to_str(std::vector<char>& vec, json_object value, const int tab_count, const bool enter);
	void value_to_str(std::vector<char>& vec, const int tab_count, const bool enter);

	void value_to_str(std::vector<char>& vec, type_of_value tov, const void* value, const int tab_count, const bool enter)
	{
		if (tov == typeINT)
			value_to_str(vec, *((long long*)value), tab_count, enter);
		else if (tov == typeFLOAT)
			value_to_str(vec, *((double*)value), tab_count, enter);
		else if (tov == typeBOOL)
			value_to_str(vec, *((bool*)value), tab_count, enter);
		else if (tov == typeSTRING)
			value_to_str(vec, *((const char**)value), tab_count, enter);
		else if (tov == typeARRAY)
			value_to_str(vec, *((json_array*)value), tab_count, enter);
		else if (tov == typeOBJECT)
			value_to_str(vec, *((json_object*)value), tab_count, enter);
		else if (tov == typeNULL)
			value_to_str(vec, tab_count, enter);
		return;
	}
	void value_to_str(std::vector<char>& vec, long long value, const int tab_count, const bool enter)
	{
		if (value == 0)
			vec.push_back('0');
		int nsiz = vec.size();
		bool flag = 0;
		if (value < 0)
			flag = 1, value = -value, vec.push_back('-');
		while (value)
			vec.push_back(value % 10 + '0'), value /= 10;
		std::reverse(vec.end() - vec.size() + nsiz + flag, vec.end());
		return;
	}
	void value_to_str(std::vector<char>& vec, double value, const int tab_count, const bool enter)
	{
		if (value < 0)
			value = -value, vec.push_back('-');
		long long a = (long long)value;
		bool flag = (value - a >= 0.1);
		long long b = 1000000000000000LL * (flag ? value - a : value - a + 0.1);
		value_to_str(vec, a, 0, 0);
		vec.push_back('.');
		int nsiz = vec.size();
		value_to_str(vec, b, 0, 0);
		*(vec.end() - vec.size() + nsiz) -= (!flag);
		return;
	}
	void value_to_str(std::vector<char>& vec, bool value, const int tab_count, const bool enter)
	{
		static const char* s[2] = { {"false"}, {"true"} };
		static const int l[2] = { 5, 4 };
		vec.insert(vec.end(), s[value], s[value] + l[value]);
	}
	void value_to_str(std::vector<char>& vec, const char* value, const int tab_count, const bool enter)
	{
		vec.push_back('\"');
		vec.insert(vec.end(), value, value + strlen(value));
		vec.push_back('\"');
		return;
	}
	void value_to_str(std::vector<char>& vec, json_array value, const int tab_count, const bool enter)
	{
		if (enter)
		{
			vec.push_back('\n');
			put_tab(vec, tab_count);
		}
		vec.push_back('['), vec.push_back('\n');
		for (int i = 0; i < value.value_list.size(); ++i)
		{
			json_value* p = value.value_list.at(i);
			put_tab(vec, tab_count + 1);
			if (p == NULL)
				value_to_str(vec, tab_count, enter);
			else
				value_to_str(vec, p->get_type(), p->get_value(), tab_count + 1, 0);
			if (i != value.value_list.size() - 1)
				vec.push_back(',');
			vec.push_back('\n');
			
		}
		put_tab(vec, tab_count);
		vec.push_back(']');
		return;
	}
	void value_to_str(std::vector<char>& vec, json_object value, const int tab_count, const bool enter)
	{
		if (enter)
		{
			vec.push_back('\n');
			put_tab(vec, tab_count);
		}
		vec.push_back('{'), vec.push_back('\n');
		if (value.value_list.size())
		{
			auto it = value.value_list.begin(), end = --value.value_list.end();
			for (; it != value.value_list.end(); ++it)
			{
				put_tab(vec, tab_count + 1);
				value_to_str(vec, it->first, tab_count, 1);
				vec.push_back(':');
				json_value* p = it->second;
				value_to_str(vec, p->get_type(), p->get_value(), tab_count + 1, bracket_with_enter);
				if (it != end)
					vec.push_back(',');
				vec.push_back('\n');
			}
		}
		put_tab(vec, tab_count);
		vec.push_back('}');
		return;
	}
	void value_to_str(std::vector<char>& vec, const int tab_count, const bool enter)
	{
		static const char* s = { "null" };
		vec.insert(vec.end(), s, s + 4);
		return;
	}

	json_array::json_array(void)
	{
		clear_vector(value_list);
		return;
	}
	json_array::json_array(json_array& a)
	{
		clear_vector(value_list);
		value_list.resize(a.value_list.size());
		for (int i = 0; i < a.value_list.size(); ++i)
		{
			json_value* value = a.value_list[i];
			json_value* tmp = NULL;
			if (value == NULL)
				continue;
			type_of_value p = value->get_type();
			const void* v = value->get_value();
			if (p == typeINT)
				tmp = new json_value_int(v);
			else if (p == typeFLOAT)
				tmp = new json_value_float(v);
			else if (p == typeBOOL)
				tmp = new json_value_bool(v);
			else if (p == typeSTRING)
				tmp = new json_value_string(v);
			else if (p == typeARRAY)
				tmp = new json_value_array(v);
			else if (p == typeOBJECT)
				tmp = new json_value_object(v);
			else if (p == typeNULL)
				tmp = new json_value_null;
			value_list[i] = tmp;
		}
	}
	json_array::json_array(const char* file)
	{
		read(file);
		return;
	}
	json_array& json_array::operator =(json_array a)
	{
		clear_vector(value_list);
		value_list.resize(a.value_list.size(), NULL);
		for (int i = 0; i < a.value_list.size(); ++i)
		{
			json_value* value = a.value_list[i];
			json_value* tmp = NULL;
			if (value == NULL)
				continue;
			type_of_value p = value->get_type();
			const void* v = value->get_value();
			if (p == typeINT)
				tmp = new json_value_int(v);
			else if (p == typeFLOAT)
				tmp = new json_value_float(v);
			else if (p == typeBOOL)
				tmp = new json_value_bool(v);
			else if (p == typeSTRING)
				tmp = new json_value_string(v);
			else if (p == typeARRAY)
				tmp = new json_value_array(v);
			else if (p == typeOBJECT)
				tmp = new json_value_object(v);
			else if (p == typeNULL)
				tmp = new json_value_null;
			value_list[i] = tmp;
		}
		return *this;
	}
	void json_array::clear_vector(std::vector<json_value*>& a)
	{
		for (int i = 0; i < a.size(); ++i)
			if (a.at(i) != NULL)
			{
				delete a.at(i);
				a[i] = NULL;
			}
		a.clear();
		return;
	}
	void json_array::read_str(const char* str, const int* info, const int left, const int right)
	{
		std::vector<int> edge;
		edge.push_back(0);
		for (int i = left + 1; i < right - 1; ++i)
		{
			if (str[i] == ',')
				edge.push_back(i);
			if (info[i] > i)
				i = info[i];
		}
		edge.push_back(right - 1);
		value_list.resize(edge.size() - 1, NULL);
		for (int i = 0; i < edge.size() - 1; ++i)
		{
			int l = edge.at(i), r = edge.at(i + 1);
			type_of_value p = find_type(str, l + 1, r);
			json_value* tmp = NULL;
			if (value_list[i] != NULL)
				delete value_list.at(i);
			if (p == typeINT)
				tmp = new json_value_int(str, l + 1, r);
			else if (p == typeFLOAT)
				tmp = new json_value_float(str, l + 1, r);
			else if (p == typeBOOL)
				tmp = new json_value_bool(str, l + 1, r);
			else if (p == typeSTRING)
				tmp = new json_value_string(str, l + 1, r);
			else if (p == typeARRAY)
				tmp = new json_value_array(str, l + 1, r);
			else if (p == typeOBJECT)
				tmp = new json_value_object(str, l + 1, r);
			else if (p == typeNULL)
				tmp = new json_value_null;
			value_list[i] = tmp;
		}
		return;
	}
	unsigned int json_array::size(void)
	{
		return value_list.size();
	}
	void json_array::read(const char* file)
	{
		clear_vector(value_list);
		int len = strlen(file), nlen = 0;
		int* info = str_info(file);
		char* str = new char[len + 2];
		for (int i = 0; i < len; ++i)
		{
			if (file[i] != ' ' && file[i] != '\n' && file[i] != '\t' && file[i] != '\0')
				str[nlen++] = file[i];
			if (info[i] > i&& i == '\"')
			{
				for (int j = i + 1; j <= info[i]; ++j)
					str[nlen++] = file[j];
				i = info[i];
			}
		}
		str[nlen] = 0;
		delete[] info;
		info = str_info(str);
		read_str(str, info, 0, nlen);
		delete[] info;
		delete[] str;
		return;
	}
	char* json_array::to_str(void)
	{
		std::vector<char> tmp;
		value_to_str(tmp, *this, 0, 0);
		unsigned int len = tmp.size();
		char* res = new char[len + 1];
		for (int i = 0; i < tmp.size(); ++i)
			res[i] = tmp.at(i);
		res[len] = 0;
		return res;
	}
	std::pair<const void*, type_of_value> json_array::at(int key)
	{
		if (key >= (signed)value_list.size() || value_list[key] == NULL)
			return std::make_pair((void*)NULL, ERROR);
		json_value* p = value_list.at(key);
		return std::make_pair(p->get_value(), p->get_type());
	}
	void json_array::insert(long long value, int key)
	{
		if (key >= (signed)value_list.size())
			value_list.resize(key + 1, NULL);
		else if (value_list[key] != NULL)
			delete value_list.at(key);
		value_list[key] = new json_value_int(value);
		return;
	}
	void json_array::insert(double value, int key)
	{
		if (key >= (signed)value_list.size())
			value_list.resize(key + 1, NULL);
		else if (key != -1 && value_list[key] != NULL)
			delete value_list.at(key);
		json_value* p = new json_value_int(value);
		if (key == -1)
			value_list.push_back(p);
		else
			value_list[key] = p;
		return;
	}
	void json_array::insert(bool value, int key)
	{
		if (key >= (signed)value_list.size())
			value_list.resize(key + 1, NULL);
		else if (key != -1 && value_list[key] != NULL)
			delete value_list.at(key);
		json_value* p = new json_value_bool(value);
		if (key == -1)
			value_list.push_back(p);
		else
			value_list[key] = p;
		return;
	}
	void json_array::insert(const char* value, int key)
	{
		if (key >= (signed)value_list.size())
			value_list.resize(key + 1, NULL);
		else if (key != -1 && value_list[key] != NULL)
			delete value_list.at(key);
		json_value* p = new json_value_float(value);
		if (key == -1)
			value_list.push_back(p);
		else
			value_list[key] = p;
		return;
	}
	void json_array::insert(json_array value, int key)
	{
		if (key >= value_list.size())
			value_list.resize(key + 1, NULL);
		else if (key != -1 && value_list[key] != NULL)
			delete value_list.at(key);
		json_value* p = new json_value_array(value);
		if (key == -1)
			value_list.push_back(p);
		else
			value_list[key] = p;
		return;
	}
	void json_array::insert(json_object value, int key)
	{
		if (key >= value_list.size())
			value_list.resize(key + 1, NULL);
		else if (key != -1 && value_list[key] != NULL)
			delete value_list.at(key);
		json_value* p = new json_value_object(value);
		if (key == -1)
			value_list.push_back(p);
		else
			value_list[key] = p;
		return;
	}
	void json_array::insert(int key)
	{
		if (key >= value_list.size())
			value_list.resize(key + 1, NULL);
		else if (key != -1 && value_list[key] != NULL)
			delete value_list.at(key);
		json_value* p = new json_value_null;
		if (key == -1)
			value_list.push_back(p);
		else
			value_list[key] = p;
		return;
	}
	void json_array::erase(int key)
	{
		if (key == -1)
			key = value_list.size() - 1;
		if (value_list[key] != NULL)
			delete value_list.at(key);
		value_list[key] = NULL;
		return;
	}

	const char* json_object::json_object_value::var::key(void)
	{
		return it->first;
	}
	const void* json_object::json_object_value::var::value(void)
	{
		return it->second->get_value();
	}
	type_of_value json_object::json_object_value::var::typ(void)
	{
		return it->second->get_type();
	}
	json_object::json_object_value::json_object_value(std::map<const char*, json_value*, cmp>::iterator it)
	{
		x.it = it;
		return;
	}
	json_object::json_object_value::json_object_value(void)
	{
		return;
	}
	bool json_object::json_object_value::operator ==(const json_object::json_object_value a) const
	{
		return x.it == a.x.it;
	}
	bool json_object::json_object_value::operator !=(const json_object::json_object_value a) const
	{
		return !(*this == a);
	}
	json_object::json_object_value::var* json_object::json_object_value::operator ->(void)
	{
		return &x;
	}
	json_object::json_object_value& json_object::json_object_value::operator ++(void)
	{
		++x.it;
		return *this;
	}
	json_object::json_object_value json_object::json_object_value::operator ++(int)
	{
		json_object_value tmp = *this;
		++(*this);
		return tmp;
	}
	json_object::json_object_value& json_object::json_object_value::operator --(void)
	{
		--x.it;
		return *this;
	}
	json_object::json_object_value json_object::json_object_value::operator --(int)
	{
		json_object_value tmp = *this;
		--(*this);
		return tmp;
	}
	bool json_object::cmp::operator ()(const char* a, const char* b) const
	{
		return strcmp(a, b) < 0;
	}
	json_object::json_object(void)
	{
		clear_map(value_list);
		return;
	}
	json_object::json_object(json_object& a)
	{
		clear_map(value_list);
		auto it = a.value_list.begin();
		while (it != a.value_list.end())
		{
			const char* key = it->first;
			json_value* value = it->second;
			json_value* tmp = NULL;
			type_of_value p = value->get_type();
			const void* v = value->get_value();
			if (p == typeINT)
				tmp = new json_value_int(v);
			else if (p == typeFLOAT)
				tmp = new json_value_float(v);
			else if (p == typeBOOL)
				tmp = new json_value_bool(v);
			else if (p == typeSTRING)
				tmp = new json_value_string(v);
			else if (p == typeARRAY)
				tmp = new json_value_array(v);
			else if (p == typeOBJECT)
				tmp = new json_value_object(v);
			else if (p == typeNULL)
				tmp = new json_value_null;
			value_list[key] = tmp;
			++it;
		}
		return;
	}
	json_object::json_object(const char* file)
	{
		read(file);
		return;
	}
	json_object::~json_object(void)
	{
		clear_map(value_list);
		return;
	}
	json_object& json_object::operator =(json_object a)
	{
		clear_map(value_list);
		auto it = a.value_list.begin();
		while (it != a.value_list.end())
		{
			const char* key = it->first;
			json_value* value = it->second;
			json_value* tmp = NULL;
			type_of_value p = value->get_type();
			const void* v = value->get_value();
			if (p == typeINT)
				tmp = new json_value_int(v);
			else if (p == typeFLOAT)
				tmp = new json_value_float(v);
			else if (p == typeBOOL)
				tmp = new json_value_bool(v);
			else if (p == typeSTRING)
				tmp = new json_value_string(v);
			else if (p == typeARRAY)
				tmp = new json_value_array(v);
			else if (p == typeOBJECT)
				tmp = new json_value_object(v);
			else if (p == typeNULL)
				tmp = new json_value_null;
			value_list[key] = tmp;
			++it;
		}
		return *this;
	}
	void json_object::clear_map(std::map<const char*, json_value*, cmp>& a)
	{
		auto it = a.begin();
		while (it != a.end())
		{
			if (it->second != NULL)
			{
				delete it->second;
				it->second = NULL;
			}
			a.erase(it++);
		}
		a.clear();
		return;
	}
	void json_object::read_str(const char* str, const int* info, const int left, const int right)
	{
		std::vector<int> edge;
		edge.push_back(0);
		for (int i = left + 1; i < right - 1; ++i)
		{
			if (str[i] == ',')
				edge.push_back(i);
			if (info[i] > i)
				i = info[i];
		}
		edge.push_back(right - 1);
		for (int i = 0; i < edge.size() - 1; ++i)
		{
			int l = edge.at(i), r = edge.at(i + 1);
			int colon = std::find(str + l + 1, str + r, ':') - str;
			char* key = new char[colon - l + 2];
			strncpy(key, str + l + 2, colon - l - 3);
			key[colon - l - 3] = 0;
			type_of_value p = find_type(str, colon + 1, r);
			json_value* tmp = NULL;
			if (value_list.count(key))
				delete value_list.at(key);
			if (p == typeINT)
				tmp = new json_value_int(str, colon + 1, r);
			else if (p == typeFLOAT)
				tmp = new json_value_float(str, colon + 1, r);
			else if (p == typeBOOL)
				tmp = new json_value_bool(str, colon + 1, r);
			else if (p == typeSTRING)
				tmp = new json_value_string(str, colon + 1, r);
			else if (p == typeARRAY)
				tmp = new json_value_array(str, colon + 1, r);
			else if (p == typeOBJECT)
				tmp = new json_value_object(str, colon + 1, r);
			else if (p == typeNULL)
				tmp = new json_value_null;
			value_list[key] = tmp;
			delete key;
		}
		return;
	}
	json_object::iterator json_object::begin(void)
	{
		iterator tmp(value_list.begin());
		return tmp;
	}
	json_object::iterator json_object::end(void)
	{
		iterator tmp(value_list.end());
		return tmp;
	}
	unsigned int json_object::size(void)
	{
		return value_list.size();
	}
	void json_object::read(const char* file)
	{
		clear_map(value_list);
		int len = strlen(file), nlen = 0;
		int* info = str_info(file);
		char* str = new char[len + 2];
		for (int i = 0; i < len; ++i)
		{
			if (file[i] != ' ' && file[i] != '\n' && file[i] != '\t' && file[i] != '\0')
				str[nlen++] = file[i];
			if (info[i] > i && i == '\"')
			{
				for (int j = i + 1; j <= info[i]; ++j)
					str[nlen++] = file[j];
				i = info[i];
			}
		}
		str[nlen] = 0;
		delete[] info;
		info = str_info(str);
		read_str(str, info, 0, nlen);
		delete[] info;
		delete[] str;
		return;
	}
	char* json_object::to_str(void)
	{
		std::vector<char> tmp;
		value_to_str(tmp, *this, 0, 0);
		unsigned int len = tmp.size();
		char* res = new char[len + 1];
		for (int i = 0; i < tmp.size(); ++i)
			res[i] = tmp.at(i);
		res[len] = 0;
		return res;
	}
	std::pair<const void*, type_of_value> json_object::at(const char* key)
	{
		if (!value_list.count(key) || value_list[key] == NULL)
			return std::make_pair((void*)NULL, ERROR);
		json_value* p = value_list.at(key);
		return std::make_pair(p->get_value(), p->get_type());
	}
	void json_object::insert(const char* key, long long value)
	{
		if (value_list.count(key) && value_list[key] != NULL)
			delete value_list.at(key);
		value_list[key] = new json_value_int(value);
		return;
	}
	void json_object::insert(const char* key, double value)
	{
		if (value_list.count(key) && value_list[key] != NULL)
			delete value_list.at(key);
		value_list[key] = new json_value_float(value);
		return;
	}
	void json_object::insert(const char* key, bool value)
	{
		if (value_list.count(key) && value_list[key] != NULL)
			delete value_list.at(key);
		value_list[key] = new json_value_bool(value);
		return;
	}
	void json_object::insert(const char* key, const char* value)
	{
		if (value_list.count(key) && value_list[key] != NULL)
			delete value_list.at(key);
		value_list[key] = new json_value_string(value);
		return;
	}
	void json_object::insert(const char* key, json_array value)
	{
		if (value_list.count(key) && value_list[key] != NULL)
			delete value_list.at(key);
		value_list[key] = new json_value_array(value);
		return;
	}
	void json_object::insert(const char* key, json_object value)
	{
		if (value_list.count(key) && value_list[key] != NULL)
			delete value_list.at(key);
		value_list[key] = new json_value_object(value);
		return;
	}
	void json_object::insert(const char* key)
	{
		if (value_list.count(key) && value_list[key] != NULL)
			delete value_list.at(key);
		value_list[key] = new json_value_null;
		return;
	}
	void json_object::erase(const char* key)
	{
		if (value_list.count(key) && value_list[key] != NULL)
			delete value_list.at(key);
		value_list.erase(key);
		return;
	}
}
