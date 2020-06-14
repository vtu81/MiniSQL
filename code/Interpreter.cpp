#include"Interpreter.h"

string Interpreter::getWord(string s, int& index)
{
	string  word;
	//left��right�����ֹλ��,������ȡsubstr
	int left, right;
	while ((s[index] == ' ' || s[index] == '\t' || s[index] == '\x0a') && s[index] != '\0')
	{
		index++;
	}
	left = index;
	if (s[index] == '\0') return "";
	//����(,)�ַ���ֱ�ӷ����ַ�
	if (s[index] == '(' || s[index] == ',' || s[index] == ')')
	{
		index++;
		right = index;
		word = s.substr(left, right - left);
	}
	//�������ţ��򷵻��ڲ��ַ�����������
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
	//��ΪСд
	transform(temp.begin(), temp.end(), temp.begin(), ::tolower);

	word = getWord(temp, index);
	//creat���
	if (word.compare("create") == 0)
	{
		word = getWord(temp, index);
		if (word.compare("table") == 0)
		{
			string tablename;
			string primaryKeyName;
			word = getWord(temp, index);
			//�����������
			if (!word.empty())
				tablename = word;
			else
			{
				//û�б���
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
							//char��û��(
							return 0;
						}
						word = getWord(temp, index);
						type = string2num(word);
						if (type == 0)
						{
							//���������n����1-255��
							return 0;
						}
						word = getWord(temp, index);
						if (word.compare(")") != 0)
						{
							//char��û��(
							return 0;
						}
					}
					else
					{
						//δ֪����
						return 0;
					}
					//�鿴�Ƿ���unique
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
							//ȱ��,��)
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
						//primary����û��key
						return 0;
					}
					word = getWord(temp, index);
					if (word.compare("(") != 0)
					{
						//key����û��()
						return 0;
					}
					//����Ƿ����
					int i;
					for (i = 0;i < attribute_list.size();i++)
					{
						if (attribute_list[i].name == word)
							attribute_list[i].ifUnique == true;
					}
					if (i == attribute_list.size())
					{
						//�������Բ�����
						return 0;
					}
					primaryKeyLocation = i;
					primaryKeyName = attribute_list[i].name;
					word = getWord(temp, index);
					if (word.compare(")") != 0)
					{
						//���Ժ���û��)
						return 0;
					}
					word = getWord(temp, index);
				}
				if (word.compare(")") != 0)
				{
					//ȱ��)
					return 0;
				}
				//����api
				/*api->tableCreate(tablename, &attribute_list, primaryKeyName, primaryKeyLocation);*/
			}
			else
			{
				//���û�г���(
				return 0;
			}
		}
		else if (word.compare("index") == 0)
		{
			string table_name, attribute_name, index_name;
			word = getWord(temp, index);
			//��������������
			if (!word.empty())
				index_name = word;
			else
			{
				//û��������
				return 0;
			}
			word = getWord(temp, index);
			if (word.compare("on") != 0)
			{
				//ȱ��on
				return 0;
			}
			word = getWord(temp, index);
			if (word.compare("(") != 0)
			{
				//on����ȱ��(
				return 0;
			}
			word = getWord(temp, index);
			if (word.compare("") == 0)
			{
				//����indexû�ж�Ӧ����
				return 0;
			}
			else attribute_name = word;
			word = getWord(temp, index);
			if (word.compare(")") != 0)
			{
				//ȱ��)
				return 0;
			}
			//����api����index
			//
		}
		else
		{
			//create�������
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
				//ɾ��ʱȱʡ����
				return 0;
			}
			else 
			{
				//����api����ɾ����
			}
		}
		else if (word.compare("index") == 0)
		{
			word = getWord(temp, index);
			if (word.compare("") == 0)
			{
				//ɾ��ʱȱʡ����
				return 0;
			}
			else
			{
				//����api����ɾ������
			}
		}
		else
		{
			//drop���﷨����
			return 0;
		}
     }
	else if (word.compare("quit") == 0)
	{
	     //�˳�SQL�Ĳ���
    }
}