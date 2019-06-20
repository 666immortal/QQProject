#include "UserList.h"

int main()
{
    userList list;
    char str[100];

    initUserList(&list);
    addUser(&list, "laowang", 12);
    addUser(&list, "xiaoliu", 25);
    addUser(&list, "yadan", 31);
    addUser(&list, "daye", 46);
    
    showUserList(list);

    removeUser(&list, 3);
    printf("-----------------------\n");
    showUserList(list);
    listToString(list, str);
    printf("-----------------------\n");
    printf("%s\n", str);
    userList tmp;
    initUserList(&tmp);
    makeUserList(str, &tmp);
    showUserList(tmp);
    deleteUserList(&tmp);
    deleteUserList(&list);

    return 0;
}