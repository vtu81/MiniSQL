//
//  
//  catalog by 俞晓锋 
//

#include "catalog_manager.h"
#include<cstring>


std::vector <std::string> CatalogManager::GetAllTable(){

	return tablelist;

}

void CatalogManager::CreateTable(std::string name, Attribute Attr, int primary, Index index){
    //repeat error
    if(IsTable(name)){
        throw table_exist();
    }
	tablenum++;
    Attr.repeat[primary]=true;
    std::string tempstring="0000 ";
    tempstring+=name;
    tempstring=tempstring+" "+numtostr(Attr.num, 2);
    //if repeat?
    for(int i=0;i<Attr.num;i++)
        tempstring=tempstring+" "+numtostr(Attr.type[i], 3)+" "+Attr.name[i]+" "+(Attr.repeat[i]==true?"1":"0");
    tempstring=tempstring+" "+numtostr(primary, 2);
    //index number
    tempstring=tempstring+" ;"+numtostr(index.num, 2);
    //index information
    for(int i=0;i<index.num;i++)
        tempstring=tempstring+" "+numtostr(index.location[i], 2)+" "+index.indexname[i];
    //# represents end of each record
    tempstring=tempstring+"\n"+"#";
    //update record
    std::string str_len=numtostr((int)tempstring.length()-1, 4);
    tempstring=str_len+tempstring.substr(4,tempstring.length()-4);
    //number of block
    int blocknumber=fetchBlockNum(TABLEPATH)/PAGESIZE;
    //if block number =0
    if(blocknumber<=0)
        blocknumber=1;
    for(int current_block=0;current_block<blocknumber;current_block++){
        char* buffer = BM.fetchPage(TABLEPATH , current_block);
        int page_id = BM.fetchPageID(TABLEPATH , current_block);
     
        int length=0;
        for(length=0;length<PAGESIZE&&buffer[length]!='\0'&&buffer[length]!='#';length++){}
        //page size not overflow
        if(length+(int)tempstring.length()<PAGESIZE){
            if(length&&buffer[length-1]=='#')
                buffer[length-1]='\0';
            else if(buffer[length]=='#')
                buffer[length]='\0';
            
            strcat(buffer , tempstring.c_str());
            //mark the page and refresh
            BM.markPageDirty(page_id);
            return;
        }
    }
    //build a new block for more space
    char* buffer = BM.fetchPage(TABLEPATH , blocknumber);
    int page_id = BM.fetchPageID(TABLEPATH , blocknumber);
    strcat(buffer , tempstring.c_str());
    BM.markPageDirty(page_id);
	while (tablelist.size()<tablenum)
	{
		tablelist.push_back(name);
	}
}

void CatalogManager::DropTable(std::string name){
    //no such table
    if(!IsTable(name)){
        throw table_not_exist();
    }
    
    int blockadress;
    int startindex=GetTableAdress(name,blockadress);
    
    char* buffer = BM.fetchPage(TABLEPATH , blockadress);
    int pageid = BM.fetchPageID(TABLEPATH , blockadress);
    std::string buffer_check(buffer);

    int end_index=startindex+strtonum(buffer_check.substr(startindex,4));
    int index=0,currentindex=0;;
    do{
        if(index<startindex||index>=end_index)
            buffer[currentindex++]=buffer[index];
        index++;
    }while(buffer[index]!='#');
    buffer[currentindex++]='#';
    buffer[currentindex]='\0';
    //mark page
    BM.markPageDirty(pageid);
/*delete record in tablelist*/
    std::vector <std::string>::iterator i;
    for(i=tablelist.begin();i!=tablelist.end();){
        if(*i==name) tablelist.erase(i);
        else ++i;
    }
}

Attribute CatalogManager::GetAttribute(std::string name){
    //no such attribute
    if(!IsTable(name)){
        throw table_not_exist();
    }
    //the corresponding block in page
    int blockadress;
    int startindex=GetTableAdress(name,blockadress);
    char* buffer = BM.fetchPage(TABLEPATH , blockadress);
    std::string checkbuffer(buffer);
    int endindex=0;
    std::string tablename=GetTableName(checkbuffer, startindex, endindex);
    Attribute tableattr;
    startindex=endindex+1;
    //get attribute number 
    std::string attr_num=checkbuffer.substr(startindex,2);
    tableattr.num=strtonum(attr_num);
    startindex+=3;
    for(int index=0;index<tableattr.num;index++){
        if(checkbuffer[startindex]=='-'){
            tableattr.type[index]=-1;
            startindex+=5;
            while(checkbuffer[startindex]!=' '){
                tableattr.name[index]+=checkbuffer[startindex++];
            }
            startindex+=1;
            tableattr.repeat[index]=(checkbuffer[startindex]=='1'?true:false);
        }
        else if(strtonum(checkbuffer.substr(startindex,3))==0){
            tableattr.type[index]=0;
            startindex+=4;
            while(checkbuffer[startindex]!=' '){
                tableattr.name[index]+=checkbuffer[startindex++];
            }
            startindex+=1;
            tableattr.repeat[index]=(checkbuffer[startindex]=='1'?true:false);
        }
        else{
            tableattr.type[index]=strtonum(checkbuffer.substr(startindex,3));
            startindex+=4;
            while(checkbuffer[startindex]!=' '){
                tableattr.name[index]+=checkbuffer[startindex++];
            }
            startindex+=1;
            tableattr.repeat[index]=(checkbuffer[startindex]=='1'?true:false);
        }
        startindex+=2;
    }
    //record primaty key
    if(checkbuffer[startindex]=='-')
        tableattr.primarykey=-1;
    else
        tableattr.primarykey=strtonum(checkbuffer.substr(startindex,2));
    Index index_record=GetIndex(tablename);
    for(int i=0;i<32;i++)
        tableattr.isindex[i]=false;
    for(int i=0;i<index_record.num;i++)
        tableattr.isindex[index_record.location[i]]=true;
    
    return tableattr;
}

bool CatalogManager::IsAttribute(std::string t_name , std::string a_name){
    //no such table
    if(!IsTable(t_name)){
        throw table_not_exist();
    }
    //look through all the attribute by index
    Attribute find_attr=GetAttribute(t_name);
    for(int index=0;index<find_attr.num;index++){
        if(a_name==find_attr.name[index])
            return true;
    }
    return false;
}

//return attribute name according to table name
std::string CatalogManager::IndextoAttr(std::string t_name, std::string i_name){
    if(!IsTable(t_name))
        throw table_not_exist();
    Index index_record=GetIndex(t_name);
    int hasfind=-1;
    for(int i=0;i<index_record.num;i++){
        if(index_record.indexname[i]==i_name){
            hasfind=i;
            break;
        }
    }
    if(hasfind==-1)
        throw index_not_exist();
    Attribute attr_record=GetAttribute(t_name);
    return attr_record.name[index_record.location[hasfind]];
}

void CatalogManager::CreateIndex(std::string t_name,std::string a_name,std::string i_name){
    //no such table
    if(!IsTable(t_name))
        throw table_not_exist();
    //no such attribute
    if(!IsAttribute(t_name, a_name))
        throw attribute_not_exist();
    Index indexrecord=GetIndex(t_name);
	//max size of index
    if(indexrecord.num>=10)
        throw index_full();
    Attribute findattr=GetAttribute(t_name);
    for(int i=0;i<indexrecord.num;i++){
        if(indexrecord.indexname[i]==i_name)
            throw index_exist();
        if(findattr.name[indexrecord.location[i]]==a_name)
            throw index_exist();
    }
    indexrecord.indexname[indexrecord.num]=i_name;
    for(int index=0;index<findattr.num;index++){
        if(a_name==findattr.name[index])
        {
            indexrecord.location[indexrecord.num]=index;
            break;
        }
    }
    indexrecord.num++;
    //Delete the table in the original table and insert it again to realize the brush
    DropTable(t_name);
    CreateTable(t_name, findattr, findattr.primarykey, indexrecord);
}

void CatalogManager::DropIndex(std::string table_name,std::string index_name){
    //no such index, return error
    if(!IsTable(table_name)){
        throw table_not_exist();
    }
    Index indexrecord=GetIndex(table_name);
    Attribute attrrecord=GetAttribute(table_name);
    //if index repeat?
    int isindex=-1;
    for(int index=0;index<indexrecord.num;index++){
        if(indexrecord.indexname[index]==index_name){
            isindex=index;
            break;
        }
    }
    if(isindex==-1){
        throw index_not_exist();
    }
    indexrecord.indexname[isindex]=indexrecord.indexname[indexrecord.num-1];
    indexrecord.location[isindex]=indexrecord.location[indexrecord.num-1];
    indexrecord.num--;
    //Delete the table in the original table and then insert it to refresh it
    DropTable(table_name);
    CreateTable(table_name, attrrecord, attrrecord.primarykey, indexrecord);
    
}

void CatalogManager::PrintTable(std::string table_name){
    //no such table,return error
    if(!IsTable(table_name)){
        throw table_not_exist();
    }
    //print table name
    std::cout<<"Table name:"<<table_name<<std::endl;
    Attribute attrrecord=GetAttribute(table_name);
    Index indexrecord=GetIndex(table_name);
    //get longest attribute name
    int longest=-1;
    for(int index=0;index<attrrecord.num;index++){
        if((int)attrrecord.name[index].length()>longest)
            longest=(int)attrrecord.name[index].length();
    }
    //print attribute
    std::string type;
    std::cout<<"Attribute:"<<std::endl;
    std::cout<<"Num|"<<"Name"<<std::setw(longest+2)<<"|Type"<<type<<std::setw(6)<<"|"<<"Unique|Primary Key"<<std::endl;
    for(int indexcount=0;indexcount<longest+35;indexcount++)
        std::cout<<"-";
    std::cout<<std::endl;
    for(int index=0;index<attrrecord.num;index++){
        switch (attrrecord.type[index]) {
            case -1:
                type="int";
                break;
            case 0:
                type="float";
                break;
            default:
                type="char("+numtostr(attrrecord.type[index]-1, 3)+")";
                break;
        }
        std::cout<<index<<std::setw(3-index/10)<<"|"<<attrrecord.name[index]<<std::setw(longest-(int)attrrecord.name[index].length()+2)<<"|"<<type<<std::setw(10-(int)type.length())<<"|";
        if(attrrecord.repeat[index])
            std::cout<<"unique"<<"|";
        else
            std::cout<<std::setw(7)<<"|";
        if(attrrecord.primarykey==index)
            std::cout<<"primary key";
        std::cout<<std::endl;
    }
    
    for(int indexcount=0;indexcount<longest+35;indexcount++)
        std::cout<<"-";
    
    std::cout<<std::endl;
    
    //print index
    std::cout<<"Index:"<<std::endl;
    std::cout<<"Num|Location|Name"<<std::endl;
    longest=-1;
    for(int indexcount=0;indexcount<indexrecord.num;indexcount++){
        if((int)indexrecord.indexname[indexcount].length()>longest)
            longest=(int)indexrecord.indexname[indexcount].length();
    }
    for(int indexcount=0;indexcount<((longest+14)>18?(longest+14):18);indexcount++)
        std::cout<<"-";
    std::cout<<std::endl;
    for(int indexcount=0;indexcount<indexrecord.num;indexcount++){
        std::cout<<indexcount<<std::setw(3-indexcount/10)<<"|"<<indexrecord.location[indexcount]<<std::setw(8-indexrecord.location[indexcount]/10)<<"|"<<indexrecord.indexname[indexcount]<<std::endl;
    }
    for(int indexcount=0;indexcount<((longest+14)>18?(longest+14):18);indexcount++)
        std::cout<<"-";
    std::cout<<std::endl<<std::endl;
}

//if table name repeat
bool CatalogManager::IsTable(std::string table_name){
    int blocknumber=fetchBlockNum(TABLEPATH)/PAGESIZE;
    if(blocknumber<=0)
        blocknumber=1;
        for(int curblock=0;curblock<blocknumber;curblock++){
        char* buffer = BM.fetchPage(TABLEPATH , curblock);
        std::string checkbuffer(buffer);
        std::string str_tmp="";
        int startindex=0,endindex=0;
        do{
            //# means empty file
            if(checkbuffer[0]=='#')
                break;
            //name repeat
            else if(GetTableName(buffer, startindex, endindex)==table_name){
                return true;
            } 
            else{
                //update table index by length
                startindex+=strtonum(checkbuffer.substr(startindex,4));
                //the file is empty
                if(!startindex)
                    break;
            }
        }while(checkbuffer[startindex]!='#');  //end
    }
    return false;
}

std::string CatalogManager::numtostr(int num,short bit){
    std::string str="";
    if(num<0){
        num=-num;
        str+="-";
    }
    int divisor=pow(10,bit-1);
    for(int i=0;i<bit;i++){
        str+=(num/divisor%10+'0');
        divisor/=10;
    }
    return str;
}

int CatalogManager::strtonum(std::string str){
    return atoi(str.c_str());
}

//Get the table name of the row
std::string CatalogManager::GetTableName(std::string buffer,int start,int &rear){
    std::string str_tmp="";
    rear=0;
    if(buffer=="")
        return buffer;
    while(buffer[start+rear+5]!=' '){
        rear++;
    }
    str_tmp=buffer.substr(start+5,rear);
    rear=start+5+rear;
    return str_tmp;
}

	/*Get the location of the table, 
	reference the location of the block where the table is outgoing, 
	and return the location in the block. If not found, return -1
	*/
int CatalogManager::GetTableAdress(std::string name,int &blockplace){
    int blocknum=fetchBlockNum(TABLEPATH);
    if(blocknum<=0)
        blocknum=1;
    //look through all blocks
    for(blockplace=0;blockplace<blocknum;blockplace++){
        char* buffer = BM.fetchPage(TABLEPATH , blockplace);
        std::string buffercheck(buffer);
        std::string str_tmp="";
        int start=0,rear=0;
        do{
            //# means end
            if(buffercheck[0]=='#')
                break;
            if(GetTableName(buffer, start, rear)==name){
                return start;
            }
            else{
                //update start by length
                start+=strtonum(buffercheck.substr(start,4));
                if(!start)
                    break;
            }
        }while(buffercheck[start]!='#');  //end
    }
    return -1;
}

Index CatalogManager::GetIndex(std::string t_name){
    Index indexrecord;
    //get table address and corresponding block
    int blockplace;
    int startindex=GetTableAdress(t_name,blockplace);
    char* buffer = BM.fetchPage(TABLEPATH , blockplace);
    //align startindex and now position
    std::string checkbuffer(buffer);
    while(checkbuffer[startindex]!=';')
        startindex++;
    startindex++;
    //the number of index
    indexrecord.num=strtonum(checkbuffer.substr(startindex,2));
    //Get the information of all index
    for(int times=0;times<indexrecord.num;times++){
        startindex+=3;
        indexrecord.location[times]=strtonum(checkbuffer.substr(startindex,2));
        startindex+=3;
        while(checkbuffer[startindex]!=' '&&checkbuffer[startindex]!='#'&&checkbuffer[startindex]!='\n'){
            indexrecord.indexname[times]+=checkbuffer[startindex++];
        }
        startindex-=2;
    }
    return indexrecord;
}

/*return number of the block in the Page*/
int CatalogManager::fetchBlockNum(std::string table_name) {
    char* p;
    int blocknum = -1;
    do {
        p = BM.fetchPage(table_name , blocknum + 1);
        blocknum++;
    } while(p[0] != '\0');
    return blocknum;
}
