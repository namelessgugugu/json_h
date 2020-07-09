#pragma warning(disable:4996)
#pragma once
#ifndef JSON_H
#define JSON_H
#include <utility>
#include <vector>
#include <map>

namespace json
{
	extern bool bracket_with_enter;
	enum type_of_value;
	class json_value
	{
	public:
		virtual type_of_value get_type(void) = 0;
		virtual const void* get_value(void) = 0;
	};
	class json_array;
	class json_object;

	class json_array
	{
	private:
		std::vector<json_value*> value_list;
		void clear_vector(std::vector<json_value*>& a);
		void read_str(const char* str, const int* info, const int left, const int right);

		friend void value_to_str(std::vector<char>& vec, json_array value, const int tab_count, const bool enter);
	public:
		json_array(void);
		json_array(json_array& a);
		json_array(const char* file);
		json_array& operator =(json_array a);
		unsigned int size(void);
		void read(const char* file);
		char* to_str(void);
		std::pair<const void*, type_of_value> at(int key);
		void insert(long long value, int key = -1);
		void insert(double value, int key = -1);
		void insert(bool value, int key = -1);
		void insert(const char* value, int key = -1);
		void insert(json_array value, int key = -1);
		void insert(json_object value, int key = -1);
		void insert(int key = -1);
		void erase(int key = -1);
	};

	class json_object
	{
	private:
		struct cmp
		{
			bool operator ()(const char* a, const char* b) const;
		};
		class json_object_value
		{
		private:
			class var
			{
			private:
				std::map<const char*, json_value*, cmp>::iterator it;
			public:
				friend class json_object;
				const char* key(void);
				const void* value(void);
				type_of_value typ(void);
			}x;
			json_object_value(std::map<const char*, json_value*, cmp>::iterator it);
		public:
			friend class json_object;
			json_object_value(void);
			bool operator ==(const json_object_value a) const;
			bool operator !=(const json_object_value a) const;
			var* operator ->(void);
			json_object_value& operator ++(void);
			json_object_value operator ++(int);
			json_object_value& operator --(void);
			json_object_value operator --(int);
		};
		
		std::map<const char*, json_value*, cmp> value_list;
		void clear_map(std::map<const char*, json_value*, cmp>& a);
		void read_str(const char* str, const int* info, const int left, const int right);

		friend void value_to_str(std::vector<char>& vec, json_object value, const int tab_count, const bool enter);
	public:
		typedef json_object_value iterator;
		typedef const json_object_value const_iterator;
		json_object(void);
		json_object(json_object&a);
		json_object(const char* file);
		~json_object(void);
		json_object& operator =(json_object a);
		iterator begin(void);
		iterator end(void);
		unsigned int size(void);
		void read(const char* file);
		char* to_str(void);
		std::pair<const void*, type_of_value> at(const char* key);
		void insert(const char* key, long long value);
		void insert(const char* key, double value);
		void insert(const char* key, bool value);
		void insert(const char* key, const char* value);
		void insert(const char* key, json_array value);
		void insert(const char* key, json_object value);
		void insert(const char* key);
		void erase(const char* key);
	};
}
#endif
