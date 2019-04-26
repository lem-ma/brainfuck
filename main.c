#include <stdio.h>
#include <stdlib.h>

/*
 * Brainfuck syntax:
 * > pointer++
 * < pointer--
 * + entry++
 * - entry--
 * . output
 * , input
 * [ jump to ']' if the entry is zero
 * ] jump to '[' if the entry is nonzero
 */

struct linked_list
{
    unsigned char entry;
    struct linked_list *previous;
    struct linked_list *next;
} *tape;

int init(void)
{
    tape=malloc(sizeof(*tape));
    if(!tape) return 1;
    tape->entry=0;
    tape->previous=tape->next=NULL;
    return 0;
}

void clean(void)
{
    struct linked_list *tmp;
    while(tape->previous) tape=tape->previous;
    while(tape)
    {
        tmp=tape;
        tape=tape->next;
        free(tmp);
    }
}

int pointer_forward(void)
{
    if(tape->next) tape=tape->next;
    else
    {
        tape->next=malloc(sizeof(*tape));
        if(!tape->next) return 1;
        tape->next->previous=tape;
        tape=tape->next;
        tape->entry=0;
        tape->next=NULL;
    }
    return 0;
}

int pointer_backward(void)
{
    if(tape->previous) tape=tape->previous;
    else
    {
        tape->previous=malloc(sizeof(*tape));
        if(!tape->previous) return 1;
        tape->previous->next=tape;
        tape=tape->previous;
        tape->entry=0;
        tape->previous=NULL;
    }
    return 0;
}

void run(char s[], size_t len)
{
    int nleft, nright;
    for(size_t i=0;i<len;++i)
    {
        switch(s[i])
        {
            case '>':
                if(pointer_forward()) puts("RAM Error!"),i=len;
                break;
            case '<':
                if(pointer_backward()) puts("RAM Error!"),i=len;
                break;
            case '+':
                ++tape->entry;
                break;
            case '-':
                --tape->entry;
                break;
            case '.':
                putchar(tape->entry);
                break;
            case ',':
                tape->entry=getchar();
            case '[':
                if(tape->entry==0)
                {
                    for(nleft=nright=0,++i;i<len;++i)
                    {
                        if(s[i]=='[') ++nleft;
                        else if(s[i]==']')
                        {
                            if(nleft==nright) break;
                            else ++nright;
                        }
                    }

                }
                break;
            case ']':
                if(tape->entry!=0)
                {
                    for(nleft=nright=0,--i;i>=0;--i)
                    {
                        if(s[i]==']') ++nright;
                        else if(s[i]=='[')
                        {
                            if(nleft==nright) break;
                            else ++nleft;
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
}

int main(int argc, char **argv)
{
    if(init()) puts("Initialization error"),exit(1);
    if(argc==1)
    {
        puts("Fatal error: no file input");
        return 1;
    }
    char *filename=argv[1];
    FILE *fp;
    size_t lSize;
    fp=fopen(filename, "rb");
    if(!fp) perror("error"),exit(1);
    fseek(fp, 0L, SEEK_END);
    lSize=ftell(fp);
    rewind(fp);
    char buffer[lSize];
    if(lSize!=fread(buffer, sizeof(char), lSize, fp))
        fclose(fp),fputs("entire read fails",stderr),exit(1);
    fclose(fp);
    run(buffer, lSize);
    clean();
    return 0;
}
