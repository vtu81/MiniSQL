#include"Interpreter.h"

string Interpreter::getWord(string s, int& index)
{
	string  word;
	//left和right代表截止位置,用来截取substr
	int left, right;
	while ((s[index] == ' ' || s[index] == '\t' || s[index] == '\x0a') && s[index] != '\0')
	{
		index++;
	}
	left = index;
	if (s[index] == '\0') return "";
	//遇到(,)字符，直接返回字符
	if (s[index] == '(' || s[index] == ',' || s[index] == ')')
	{
		index++;
		right = index;
		word = s.substr(left, right - left);
	}
	//遇到引号，则返回内部字符不包括引号
	else if (s[index] == '\x27')
	{
		index++;
		while (s[index] != '\x27'&&s[index] != '\0')
			index++;
		if (s[index] == '\x27')
		{
			right = index;
			index++;
			word = s.substr(left + 1, right - left - 1);
		}
		else if (s[index] == '\0')
		{
			word = "";
		}
	}
	else
	{
		while (s[index] != ' ' &&s[index] != '(' && s[index] != '\x0a' && s[index] != 0 && s[index] != ')' && s[index] != ',')
			index++;
		right = index;
		if (left != right)
			word = s.substr(left, right - left);
		else word = "";
	}
	return word;
}

int Interpreter::string2num(string s)
{
	int result = atoi(s.c_str());
	if (result > 0 && result < 256)
		return result;
	else return 0;
}

int Interpreter::interpreter(string s)
{
	int index = 0;
	string word, temp = s;
	//变为小写
	transform(temp.begin(), temp.end(), temp.begin(), ::tolower);

	word = getWord(temp, index);
	//creat情况
	if (word.compare("create") == 0)
	{
		word = getWord(temp, index);
		if (word.compare("table") == 0)
		{
			string tablename;
			string primaryKeyName;
			word = getWord(temp, index);
			//创建表的名字
			if (!word.empty())
				tablename = word;
			else
			{
				//没有表名
				return 0;
			}

			word = getWord(temp, index);
			if (temp.compare("(") == 0)
			{
				vector<Attribute> attribute_list;
				word = getWord(temp, index);
				while (word.compare(")") != 0 && word.compare("primary") != 0 && word.compare("") != 0)
				{
					string attribute_name = word;
					int type;
					bool unique = false;
					word = getWord(temp, index);
					if (word.compare("int") == 0)
						type = 0; // TYPE_INT
					else if (word.compare("float") == 0)
						type = -1; // TYPE_FLOAT
					else if (word.compare("char") == 0)
					{
						word = getWord(temp, index);
						if (word.compare("(") != 0)
						{
							//char后没有(
							return 0;
						}
						word = getWord(temp, index);
						type = string2num(word);
						if (type == 0)
						{
							//发生溢出即n不在1-255内
							return 0;
						}
						word = getWord(temp, index);
						if (word.compare(")") != 0)
						{
							//char后没有(
							return 0;
						}
					}
					else
					{
						//未知类型
						return 0;
					}
					//查看是否有unique
					word = getWord(temp, index);
					if (word.compare("unique") == 0)
					{
						unique = true;
						word = getWord(temp, index);
					}
					Attribute temp_attribute(attribute_name, type, unique);
					attribute_list.push_back(temp_attribute);
					if (word.compare(",") != 0)
					{
						if (word.compare(")") != 0)
						{
							//缺少,或)
							return 0;
						}
						else break;
					}
					word = getWord(temp, index);
				}
				int primaryKeyLocation;
				string primaryKeyName;
				if (word.compare("primary") == 0)
				{
					word = getWord(temp, index);
					if (word.compare("key") != 0)
					{
						//primary后面没跟key
						return 0;
					}
					word = getWord(temp, index);
					if (word.compare("(") != 0)
					{
						//key后面没跟()
						return 0;
					}
					//检查是否存在
					int i;
					for (i = 0;i < attribute_list.size();i++)
					{
						if (attribute_list[i].name == word)
							attribute_list[i].ifUnique == true;
					}
					if (i == attribute_list.size())
					{
						//主键属性不存在
						return 0;
					}
					primaryKeyLocation = i;
					primaryKeyName = attribute_list[i].name;
					word = getWord(temp, index);
					if (word.compare(")") != 0)
					{
						//属性后面没跟)
						return 0;
					}
					word = getWord(temp, index);
				}
				if (word.compare(")") != 0)
				{
					//缺少)
					return 0;
				}
				//连接api
				/*api->tableCreate(tablename, &attribute_list, primaryKeyName, primaryKeyLocation);*/
			}
			else
			{
				//如果没有出现(
				return 0;
			}
		}
		else if (word.compare("index") == 0)
		{
			string table_name, attribute_name, index_name;
			word = getWord(temp, index);
			//创建索引的名字
			if (!word.empty())
				index_name = word;
			else
			{
				//没有索引名
				return 0;
			}
			word = getWord(temp, index);
			if (word.compare("on") != 0)
			{
				//缺少on
				return 0;
			}
			word = getWord(temp, index);
			if (word.compare("(") != 0)
			{
				//on后面缺少(
				return 0;
			}
			word = getWord(temp, index);
			if (word.compare("") == 0)
			{
				//插入index没有对应属性
				return 0;
			}
			else attribute_name = word;
			word = getWord(temp, index);
			if (word.compare(")") != 0)
			{
				//缺少)
				return 0;
			}
			//引用api创建index
			//
		}
		else
		{
			//create对象错误
			return 0;
		}
	}
	else if (word.compare("drop") == 0)
	{
	    word = getWord(temp, index);
		if (word.compare("table") == 0)
		{
			word = getWord(temp, index);
			if (word.compare("") == 0)
			{
				//删除时缺省表名
				return 0;
			}
			else 
			{
				//调用api函数删除表
			}
		}
		else if (word.compare("index") == 0)
		{
			word = getWord(temp, index);
			if (word.compare("") == 0)
			{
				//删除时缺省表名
				return 0;
			}
			else
			{
				//调用api函数删除索引
			}
		}
		else
		{
			//drop后语法错误
			return 0;
		}
     }
	else if (word.compare("quit") == 0)
	{
	     //退出SQL的操作
    }
}