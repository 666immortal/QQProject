#include "MsgStruct.h"

void showMsgEntity(MsgEntity ety)
{
    printf("object: %d\ndetails: %s\nflag:%d\n", 
                    ety.object, ety.details, ety.flag);
}

void showMsgContainer(MsgContainer ctn)
{
    printf("type:%d\n", ctn.type);
    showMsgEntity(ctn.content);
}