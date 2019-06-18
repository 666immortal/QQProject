#include "MsgPackager.h"
#include "UserList.h"
#include "MsgParser.h"

int main()
{
    MsgEntity tmp;
    // userList list;

    // initUserList(&list);
    // addUser(&list, "laowang", NULL);
    // addUser(&list, "xiaoliu", NULL);
    // addUser(&list, "yadan", NULL);
    // addUser(&list, "daye", NULL);
    setCommand(&tmp, CMD_LOGIN, "老王 123456", SEND_FLAG);
    //setDialogue(&tmp, 2, "你好啊", SEND_FLAG);
    //configExitEty(&tmp);
    //configLoginEty(&tmp, "laowang", "123456");
    //configRegisterEty(&tmp, "xiaoliu", "123456");
    //configUserListEty(&tmp, &list);
    showMsgEntity(tmp);

    int type;
    Stnparser cnt;
    analysisCmdEty(&tmp, &type, &cnt);
    if(type == CMD_GETLIST)
    {
        showUserList(cnt.list);
    }
    else if(type == CMD_LOGIN || type == CMD_REGISTER)
    {
        showUserVerify(cnt.nameAndPwd);
    }
    endAnalysis(type, &cnt);
    //deleteUserList(&list);
    return 0;
}