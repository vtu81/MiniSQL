//
//  
//  catalog by 俞晓锋 
//

#ifndef _CATALOG_MANAGER_H_
#define _CATALOG_MANAGER_H_

#include <iostream>
#include <cmath>
#include <cstring>
#include <iomanip>
#include<vector>
#include "Buffer_Manager.h"
#include "Basic.h"
#include "Exception.h"

#ifndef CONST
#define TABLEPATH "./database/catalog"
#define PAGESIZE 4096
#define MAXFRAMESIZE 100
#endif


class CatalogManager{
public:
	BufferManager BM;
	/*
	@parammeter：table_name,attribute_name,primary_key,index
	Function：insert a table in catalog file
	Output：if repeat, output error table_not_exist
	*/
    void CreateTable(std::string t_name, Attribute attribute, int primary, Index index);
	/*
    @parammeter：table_name
    Function：delete a table in catalog file
    Output：if no such table, output error table_not_exist
	*/
    void DropTable(std::string t_name);
	/*
	@parammeter：table_name,attribute_name,index
	Function：Create a index for an atrribute on a table
	Output：if no such table，output error table_not_exist.
	if no such attribute，output error:attribute_not_exist。
	if index repeat，output error index_exist
	*/
	void CreateIndex(std::string t_name, std::string a_name, std::string i_name);
	/*
	@parammeter：table_name,index
	Function：find attribute through index
	Output：if no such table，output error table_not_exist
			if no such index，output error index_not_exist
	*/
	std::string IndextoAttr(std::string t_name, std::string i_name);
	/*
	@parammeter：table_name,index
	Function：Delete index of a table
	output：if no such table，output error table_not_exist
			if no such attribute, output error attribute_not_exist
			if no such index，output error index_not_exist
	*/
	void DropIndex(std::string t_name, std::string i_name);
    /*
	@parammeter：table_name
    Function：check if the table exists, ouput true for yes and false for no
	*/
    bool IsTable(std::string t_name);
    /*
	@parammeter：table_name, attribute_name
    Function：check if the attribute exists, true for yes and false for no
    Output：if no such attribute,output error table_not_exist
	*/
    bool IsAttribute(std::string t_name , std::string a_name);
    /*
	@parammeter：table_name
    Function：Get the attribute from a named table
    Ouput：if no such table，output error table_not_exist
	*/
    Attribute GetAttribute(std::string t_name);
    /*
	@parammeter：table_name
    Function：show table information
    output：if no such table，output error able_not_exist
	*/
    void PrintTable(std::string t_name);
	/*get the index of the table*/
	Index GetIndex(std::string t_name);
	/*Get all table name*/
	std::vector <std::string> GetAllTable();
private:
    /*tranform numer to string，bit is width of number*/
    std::string numtostr(int num,short bit);
    /*string to number*/
    int strtonum(std::string str);
    /*get the table name*/
    std::string GetTableName(std::string buffer,int start,int &rear);
    /*get the address of the table and return the block information */
    int GetTableAdress(std::string t_name,int &suitable_block);
    /*get the size of the file*/
    int fetchBlockNum(std::string t_name);
	/*get table information*/
	std::vector <std::string> tablelist;
	int tablenum = 0;
};

#endif
