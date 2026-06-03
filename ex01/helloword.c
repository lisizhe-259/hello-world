#include <stdio.h>
#include <stdio.h>

int main() {
    char name[50];
    
    printf("Hello, World!\n");
        
    printf("ÇëÊäÈëÄãµÄÃû×Ö: ");
     
    fgets(name, sizeof(name), stdin);
    
    printf("Hello, ");
    printf("%s", name);
    
    return 0;
}

