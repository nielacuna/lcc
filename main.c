#include <c.h>

int main(void)
{

        char *duplicate = string("hello");
        char *duplicate1 = string("olleh");
        char *strnum = stringd(12345);

        printf("%s %s %s\n", duplicate, duplicate1, strnum);

        return 0;
}
