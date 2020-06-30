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
    else if (s[index] == '=') // 遇到操作符"="
    {
        index++;
        word = "=";
    }
    else if (s[index] == '<') // 遇到以"<"开头的操作符
    {
        index++;
        switch (s[index])
        {
            case '=': word = "<="; index++; break; // 小于等于
            case '>': word = "<>"; index++; break; // 不等于
            default:  word = "<" ; break; // 后面不是操作符的一部分，大于
        }
    }
    else if (s[index] == '>') // 遇到以">"开头的操作符
    {
        index++;
        switch (s[index])
        {
            case '=': word = ">="; index++; break; // 大于等于
            default:  word = ">"; break; // 后面不是操作符的一部分，大于
        }
    }
    else
    { // 读取单词
        while (s[index] != ' ' &&s[index] != '(' && s[index] != '\x0a' && s[index] != 0 && s[index] != ')' && s[index] != ','
               && s[index] != '=' && s[index] != '>' && s[index] != '<' && s[index] != '\t'
                )
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
            Attribute table_attribute;
            int num = 0;
            string tablename;
            string primaryKeyName;
            word = getWord(temp, index);
            //创建表的名字
            if (!word.empty())
            {
                tablename = word;
            }
            else
            {
                //没有表名
                std::cout << "empty table!" << std::endl;
//                table_not_exist e;
//                throw e;
                return 0;
            }

            word = getWord(temp, index);
            if (word.compare("(") == 0)
            {
                word = getWord(temp, index);
                while (word.compare(")") != 0 && word.compare("primary") != 0 && word.compare("") != 0)
                {
                    string attribute_name = word;
                    int type;
                    bool unique = true;
                    word = getWord(temp, index);
                    if (word.compare("int") == 0)
                        type = -1; // TYPE_INT
                    else if (word.compare("float") == 0)
                        type = 0; // TYPE_FLOAT
                    else if (word.compare("char") == 0)
                    {
                        word = getWord(temp, index);
                        if (word.compare("(") != 0)
                        {
                            //char后没有(
//                            create_table_syntax_error e;
//                            throw e;
                            std::cout << "create table syntax error!" << std::endl;
                            return 0;
                        }
                        word = getWord(temp, index);
                        type = string2num(word);
                        if (type == 0)
                        {
                            //发生溢出即n不在1-255内
//                            char_size_overflow e;
//                            throw e;
                            std::cout << "char size overflow!" << std::endl;
                            return 0;
                        }
                        word = getWord(temp, index);
                        if (word.compare(")") != 0)
                        {
                            //char后没有(
//                            create_table_syntax_error e;
//                            throw e;
                            std::cout << "create table syntax error!" << std::endl;
                            return 0;
                        }
                    }
                    else
                    {
                        //未知类型
//                        unknown_data_type e;
//                        throw e;
                        std::cout << "unknown data type" << std::endl;
                        return 0;
                    }
                    //查看是否有unique
                    word = getWord(temp, index);
                    if (word.compare("unique") == 0)
                    {
                        unique = false;
                        word = getWord(temp, index);
                    }
                    table_attribute.name[num] = attribute_name;
                    table_attribute.type[num] = type;
                    table_attribute.repeat[num] = unique;
                    table_attribute.num = ++num;
                    if (word.compare(",") != 0)
                    {
                        if (word.compare(")") != 0)
                        {
                            //缺少,或)
//                            create_table_syntax_error e;
//                            throw e;
                            std::cout << "create table syntax error!" << std::endl;
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
//                        create_table_syntax_error e;
//                        throw e;
                        std::cout << "create table syntax error!" << std::endl;
                        return 0;
                    }
                    word = getWord(temp, index);
                    if (word.compare("(") != 0)
                    {
                        //key后面没跟()
//                        create_table_syntax_error e;
//                        throw e;
                        std::cout << "create table syntax error!" << std::endl;
                        return 0;
                    }
                    word = getWord(temp, index);
                    //检查是否存在
                    int i;
                    for (i = 0;i < num;i++)
                    {
                        if (table_attribute.name[i] == word)
                        {
                            table_attribute.repeat[i] = false;
                            break;
                        }
                    }
                    if (i == num)
                    {
                        //主键属性不存在
//                        primary_key_not_exist e;
//                        throw e;
                        std::cout << "primary key not exist!" << std::endl;
                        return 0;
                    }
                    primaryKeyLocation = i;
                    primaryKeyName = table_attribute.name[i];
                    word = getWord(temp, index);
                    if (word.compare(")") != 0)
                    {
                        //属性后面没跟)
//                        create_table_syntax_error e;
//                        throw e;
                        std::cout << "create table syntax error!" << std::endl;
                        return 0;
                    }
                    word = getWord(temp, index);
                }
                if (word.compare(")") != 0)
                {
                    //缺少)
//                    create_table_syntax_error e;
//                    throw e;
                    std::cout << "create table syntax error!" << std::endl;
                    return 0;
                }
                Index table_index;
                table_index.num = 0;
                //连接api
                api->createTable(tablename, table_attribute, primaryKeyLocation, table_index);
            }
            else
            {
                //如果没有出现(
                std::cout << "create table synxtax error!" << std::endl;
                return 0;
//                create_table_syntax_error e;
//                throw e;
//                return 0;
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
//                index_not_exist e;
//                throw e;
                std::cout << "index not exist error!" << std::endl;
                return 0;
            }
            word = getWord(temp, index);
            if (word.compare("on") != 0)
            {
                //缺少on
//                create_index_syntax_error e;
//                throw e;
                std::cout << "create index syntax error!" << std::endl;
                return 0;
            }
            word = getWord(temp, index);
            if (word.compare("") == 0)
            {
                //插入index没有对应属性
//                create_index_syntax_error e;
//                throw e;
                std::cout << "create index syntax error!" << std::endl;
                return 0;
            }
            else table_name = word;
            word = getWord(temp, index);
            if (word.compare("(") != 0)
            {
                //on后面缺少(
//                create_index_syntax_error e;
//                throw e;
                std::cout << "create index syntax error!" << std::endl;
                return 0;
            }
            word = getWord(temp, index);
            if (word.compare("") == 0)
            {
                //插入index没有对应属性
//                create_index_syntax_error e;
//                throw e;
                std::cout << "create index syntax error!" << std::endl;
                return 0;
            }
            else attribute_name = word;
            word = getWord(temp, index);
            if (word.compare(")") != 0)
            {
                //缺少)
//                create_index_syntax_error e;
//                throw e;
                std::cout << "create index syntax error!" << std::endl;
                return 0;
            }
            //引用api创建index
            api->createIndex(index_name, table_name, attribute_name);
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
//                table_not_exist e;
//                throw e;
                std::cout << "empty table!" << std::endl;
                return 0;
            }
            else
            {
                api->dropTable(word);
            }
        }
        else if (word.compare("index") == 0)
        {
            string table_name,index_name,
                    word = getWord(temp, index);
            //创建索引的名字
            if (!word.empty())
                index_name = word;
            else
            {
                //没有索引名
//                index_not_exist e;
//                throw e;
                std::cout << "index not exist error!" << std::endl;
                return 0;
            }
            word = getWord(temp, index);
            if (word.compare("on") != 0)
            {
                //缺少on
//                create_index_syntax_error e;
//                throw e;
                std::cout << "drop index syntax error!" << std::endl;
                return 0;
            }
            word = getWord(temp, index);
            if (word.compare("") == 0)
            {
                //插入index没有对应属性
//                create_index_syntax_error e;
//                throw e;
                std::cout << "drop index syntax error!" << std::endl;
                return 0;
            }
            else table_name = word;
            //删除索引
            api->dropIndex(table_name, index_name);
        }
        else
        {
            //drop后语法错误
            return 0;
        }
    }
    else if (word.compare("select") == 0)
    {
        //处理select句段
        word = getWord_no_symbol(temp, index);
        auto* select_attribute_name_list = new vector<string>(); // 被选择的属性数组，储存的是名字
        string table_name = ""; // 选择的表格名称
        if (word.compare("*") == 0)
        { // 全选
            // 全选由API实现，把属性指针置为nullptr即可
            select_attribute_name_list = nullptr;
            do
            { // 忽略后面出现的属性，找到from语句
                word = getWord_no_symbol(temp, index);
            } while (word.compare("from") != 0);
        }
        else // select后面出现其他属性
        {
            do
            { // 添加属性
                select_attribute_name_list->push_back(word);
                word = getWord_no_symbol(temp, index);
//                cout << word <<endl;
            } while (!word.empty() && word.compare("from") != 0);
        }
        word = getWord_no_symbol(temp, index);
        /* 结束select处理 */

        /* 处理from句段 */
        if (word.empty())
        { // 没有from语句
            // 表格为空，抛出异常
            std::cout << "empty table!" << std::endl;
            return 0;
            //            table_not_exist e;
            //            throw e;
        }
        table_name = word;
        word = getWord_no_symbol(temp, index);
        /* 结束from处理 */

        /* 处理where句段 */
        if (!word.empty())
        { // from语句结束非空，说明还有条件
            if (word.compare("where") != 0)
            { // from后面不只一个表格，试图访问多个表格
                std::cout << "query on multiple tables is not allowed!" << std::endl;
                return 0;
                //                multiple_tables e;
                //                throw e;
            }
            vector<Condition>* conditions = new vector<Condition>();

            do
            {
                word = getWord_no_symbol(temp, index);
//                cout << word << "  ";
                if (word.compare("and") == 0)
                { // 条件之间的逻辑与
                    word = getWord_no_symbol(temp, index);
                }

                // 条件涉及的属性
                string attribute_name = word;
                word = getWord(temp, index);
//                cout << word << endl;
                if (word.empty()) break;
                // 操作符
                int operate = -1;
                if (word.compare("=") == 0)
                {
                    operate = 0;
                }
                else if (word.compare("<>") == 0)
                {
                    operate = 1;
                }
                else if (word.compare("<") == 0)
                {
                    operate = 2;
                }
                else if (word.compare(">") == 0)
                {
                    operate = 3;
                }
                else if (word.compare("<=") == 0)
                {
                    operate = 4;
                }
                else if (word.compare(">=") == 0)
                {
                    operate = 5;
                }
                else
                {

                }

                if (operate < 0)
                { // 操作符不合法，抛出异常
//                    invalid_operator e;
//                    throw e;
                    std::cout << "invalid operator!" << std::endl;
                    return 0;
                }
                word = getWord_no_symbol(temp, index);
                string value = word;
                // 插入条件
                conditions->push_back(Condition(attribute_name, value, operate));
            } while (!word.empty());
            try
            {
                std::cout << "select result: #################" << std::endl;
                int select_count = api->showRecord(table_name, select_attribute_name_list, conditions);
                if (select_count <= 0)
                {
                    std::cout << "No record is selected" << std::endl;
                }
                else
                {   
                    std::cout << "select " << select_count << " record(s) on " << table_name << " successfully!" << std::endl;
                }
                std::cout << "################################" << std::endl;
            }
            catch (table_not_exist e)
            {
                std::cout << "table \"" << table_name << "\" not exist!" << std::endl;
                return 0;
            }
            catch (attribute_not_exist e)
            {
                std::cout << "attributes being queried not exist!" << std::endl;
                return 0;
            }
        }
        else
        { // 没有where语句，元组全选
            try
            {
                std::cout << "select result: #################" << std::endl;
                int select_count = api->showRecord(table_name, select_attribute_name_list);
                if (select_count <= 0)
                {
                    std::cout << "No record is selected" << std::endl;
                }
                else
                {   
                    std::cout << "select " << select_count << " record(s) on " << table_name << " successfully!" << std::endl;
                }
                std::cout << "################################" << std::endl;
            }
            catch (table_not_exist e)
            {
                std::cout << "table \"" << table_name << "\" not exist!" << std::endl;
            }
            catch (attribute_not_exist e)
            {
                std::cout << "attributes being queried not exist!" << std::endl;
            }
        }
    }
    else if (word.compare("insert") == 0)
    { // 处理插入
        word = getWord(temp, index);
        int insert_count = 0; // 插入的记录数目
        if (word.compare("into") != 0)
        { // 只有insert后面没有into
            std::cout << "insert syntax error!" << std::endl;
            return 0;
            //            insert_syntax_error e;
            //            throw e;
        }
        word = getWord(temp, index);
        string table_name = word;
        word = getWord(temp, index);
        if (word.compare("values") != 0)
        { // insert into + 表名后面没有values
            std::cout << "insert syntax error!" << std::endl;
            return 0;
            //            insert_syntax_error e;
            //            throw e;
        }
        do
        {
        	vector<string>* record_content = new vector<string>(); // 插入的内容
            word = getWord(temp, index);
            if (word.compare("(") != 0)
            { // 值之间没有括号扩起来
                std::cout << "insert syntax error!" << std::endl;
                return 0;
                //                insert_syntax_error e;
                //                throw e;
            }
            word = getWord(temp, index);
            while (word.compare(")") != 0)
            { // 碰到右括号时，这部分插入停止
                if (word.empty())
                { // 不存在右括号
                    std::cout << "insert syntax error!" << std::endl;
                    return 0;
                    //                    insert_syntax_error e;
                    //                    throw e;
                }
                record_content->push_back(word); // 插入的值
//                cout << word << endl;
                word = getWord(temp, index);
                if (word.compare(",") == 0)
                { // 略去逗号
                    word = getWord(temp, index);
                }
            }
            // 执行插入语句
            try
            {
                api->insertRecord(table_name, record_content);
                insert_count ++;
            }
            catch (table_not_exist e)
            {
                std::cout << "table \"" << table_name << "\" not exist!" << std::endl;
                insert_count = -1;
            }
            catch (attribute_not_exist e)
            {
                std::cout << "attributes not exist!" << std::endl;
                insert_count = -1;
            }
            
            word = getWord(temp, index); // 略去插入值之间的逗号
        } while (!word.empty());
        if (insert_count >= 0)
        {
            std::cout << "insert result:##################" << std::endl;
            std::cout << "insert " << insert_count << " record(s) on " << table_name << " successfully!" << std::endl;
            std::cout << "################################" << std::endl;
        }
        else
        {
            std::cout << "insert result:##################" << std::endl;
            std::cout << "insert on " << table_name <<  " failed" << std::endl;
            std::cout << "################################" << std::endl;
        }
    }
    else if (word.compare("delete") == 0)
    {
        word = getWord(temp, index);
        if (word.compare("from") != 0)
        { // delete from语句错误
            std::cout << "delete syntax error!" << std::endl;
            return 0;
            //            delete_syntax_error e;
            //            throw e;
        }
        word = getWord(temp, index);
        string table_name = word;
        word = getWord(temp, index);
        if (word.compare("where") == 0)
        { // 有条件的删除
            int delete_count = 0;
            vector<Condition>* conditions = new vector<Condition>();
            do
            {
                word = getWord_no_symbol(temp, index);
                if (word.empty()) break;
                if (word.compare("and") == 0)
                { // 条件之间的逻辑与
                    word = getWord_no_symbol(temp, index);
                }
                // 条件涉及的属性
                string attribute_name = word;
                word = getWord(temp, index);
                // 操作符
                int operate = -1;
                if (word.compare("=") == 0)
                {
                    operate = 0;
                }
                else if (word.compare("<>") == 0)
                {
                    operate = 1;
                }
                else if (word.compare("<") == 0)
                {
                    operate = 2;
                }
                else if (word.compare(">") == 0)
                {
                    operate = 3;
                }
                else if (word.compare("<=") == 0)
                {
                    operate = 4;
                }
                else if (word.compare(">=") == 0)
                {
                    operate = 5;
                }
                else
                {

                }
                if (operate < 0)
                { // 操作符不合法，抛出异常
                    std::cout << "Invalid operator: \"" << word << "\"!" << std::endl;
                    return 0;
                    //                    invalid_operator e;
                    //                    throw e;
                }
                word = getWord_no_symbol(temp, index);
                string value = word;
                // 插入条件
                conditions->push_back(Condition(attribute_name, value, operate));
            } while (!word.empty());
            try
            {
                delete_count = api->deleteRecord(table_name, conditions);
            }
            catch (table_not_exist e)
            {
                std::cout << "table \"" << table_name << "\" not exist!" << std::endl;
                delete_count = -1;
            }

            if (delete_count >= 0)
            {
                std::cout << "delete result:##################" << std::endl;
                std::cout << "delete " << delete_count << " record(s) on " << table_name <<" successfully!" << std::endl;
                std::cout << "################################" << std::endl;
            }
            else
            {
                std::cout << "delete result:##################" << std::endl;
                std::cout << "delete on " << table_name << " failed" << std::endl;
                std::cout << "################################" << std::endl;
            }
        }
        else if (word.empty())
        { // 全表删除
            try
            {
                int delete_count = api->deleteRecord(table_name);
                std::cout << "delete result:##################" << std::endl;
                std::cout << "delete " << delete_count << " record(s) on "<< table_name << " successfully!" << std::endl;
                std::cout << "################################" << std::endl;
            }
            catch (table_not_exist e)
            {
                std::cout << "table \"" << table_name << "\" not exist!" << std::endl;

                std::cout << "delete result:##################" << std::endl;
                std::cout << "delete on " << table_name << " failed" << std::endl;
                std::cout << "################################" << std::endl;
            }
        }
        else
        { // 语法错误
            std::cout << "Syntax Error!" << std::endl;
            return 0;
            //            delete_syntax_error e;
            //            throw e;
        }
    }
    else if (word.compare("quit") == 0)
    {
        //退出SQL的操作
        return 2;
    }
    else if (word.compare("execfile") == 0)
    {
        word = getWord(temp, index);
        string query;
        ifstream in(word);
        while (!in.eof())
        {
            getline(in, temp);
            query = query + temp;
            int pos = query.find(";");
            if (pos != query.npos)
            {
                query.erase(pos);
                int condition = interpreter(query);
                if (condition == 0)
                {
                    return 0;
                }
                else if (condition == 2)
                {
                    return 2;
                }
                query = "";
            }
        }
        in.close();
    }
	else
	{
	      std::cout << "syntax error!" << std::endl;
	      return 0;
    }
    //success
    return 1;
}


/* 只需要读取单词，忽略掉标点符号 */
string Interpreter::getWord_no_symbol(string s, int &index)
{
    string ans;
    do
    {
        ans = getWord(s, index);
    }
    while (ans.compare("(") == 0 || ans.compare(")") == 0 || ans.compare(",") == 0);
    return ans;
}