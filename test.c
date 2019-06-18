#include "MsgParser.h"

//#define TEST_ONLINELIST
//#define TEST_REG_LOG
#define TEST_CMD

#ifdef TEST_ONLINELIST
int main()
{
    char str[] = "Hello I am Jack";
    int num, i;
    char *name[20];
    for(i = 0; i < 20; i++)
    {
        name[i] = (char*)malloc(sizeof(char)*20);
    }

    analysisOnlineListCmd(str, name, &num);

    for(i = 0; i < num; i++)
    {
        printf("%s\n", name[i]);
    }

    for(i = 0; i < 20; i++)
    {
        free(name[i]);
    }

    return 0;
}
#endif

#ifdef TEST_REG_LOG
int main()
{
    char str[] = "老王 123456";
    char name[10];
    char pwd[10];

    analysisRegisterCmd(str, name, pwd);
    //analysisLoginCmd(str, name, pwd);
    printf("name: %s\npassword: %s\n", name, pwd);
    return 0;
}

#endif

#ifdef TEST_CMD
int main()
{
    int i;
    MsgEntity tmp;
    tmp.object = CMD_GETLIST;
    tmp.flag = SEND_FLAG;
    strcpy(tmp.details, "老王 小六 鸭蛋 大头");

    int type;
    char **namelist = NULL;
    int *num = NULL;

    analysisCmdEty(&tmp, &type, namelist, num);

    int n = *num;
    printf("member num: %d", n);
    for(i = 0; i < n; i++)
    {
        printf("%s\n", namelist[i]);
    }

    free(num);
    for(i = 0; i < USER_MAX_NUM; i++)
    {
        free(namelist[i]);
    }

    return 0;
}
#endif



