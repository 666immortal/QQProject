#include "MsgPackager.h"
#include "UserList.h"

int main()
{
    userList list;
    MsgEntity tmp;

    initUserList(&list);
    addUser(&list, "laowang", NULL);
    addUser(&list, "xiaoliu", NULL);
    addUser(&list, "yadan", NULL);
    addUser(&list, "daye", NULL);
    //setCommand(&tmp, CMD_LOGIN, "老王 123456", SEND_FLAG);
    //setDialogue(&tmp, 2, "你好啊", SEND_FLAG);
    //configExitEty(&tmp);
    //configLoginEty(&tmp, "laowang", "123456");
    //configRegisterEty(&tmp, "xiaoliu", "123456");
    configUserListEty(&tmp, &list);
    showMsgEntity(tmp);
    deleteUserList(&list);
    
    return 0;
}