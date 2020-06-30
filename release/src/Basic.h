#ifndef BASIC_H
#define BASIC_H
struct Attribute {
	/*number of attribute*/
	int num;
	/*attribute name*/
	std::string name[32];
	/*type of each attribute: -1 for int ;0 for float ; 1~255 for string (length+1)*/
	short type[32];
	/*if unique*/
	bool repeat[32];
	/*position of primary key :-1 means no primary key and 0~255 means the key's position*/
	short primarykey;
	bool isindex[32];
};

//each table has no more than 10 index
struct Index {
	int num;
	short location[10];
	std::string indexname[10];
};


#endif
