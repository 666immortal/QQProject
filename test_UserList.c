#include "UserList.h"

int main()
{
    userList list;
    char str[100];

    initUserList(&list);
    addUser(&list, "laowang", NULL);
    addUser(&list, "xiaoliu", NULL);
    addUser(&list, "yadan", NULL);
    addUser(&list, "daye", NULL);
    
    showUserList(list);

    removeUser(&list, 3);
    printf("-----------------------\n");
    showUserList(list);
    listToString(list, str);
    printf("-----------------------\n");
    printf("%s\n", str);
    deleteUserList(&list);

    return 0;
}