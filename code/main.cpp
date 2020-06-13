#include "Catalog_Manager.h"
#include "Index_Manager.h"
#include "Record_Manager.h"
#include "API.h"

#include <iostream>

BufferManager buffer_manager;
API api;
CatalogManager catalog_manager;
RecordManager record_manager;
IndexManager index_manager(&api);




int main(int argc,char * argv[])
{

    //因为构造3个manager的时候用到了api，因此在此处才初始化api的3个manager指针
    api.cm = &catalog_manager;
    api.rm = &record_manager;
    api.im = &index_manager;
}