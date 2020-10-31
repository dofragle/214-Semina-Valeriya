#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    //организация ввода и вывода
    FILE *fp;
    if (argc == 2)
    {
        if((fp = freopen(argv[1], "r", stdin)) == NULL) 
        {
            fprintf(stderr, "cannot open file");
            return 1;
        }

    } else
            if (argc == 3)
            {
                if((fp = freopen(argv[1], "r", stdin)) == NULL) 
                {
                    fprintf(stderr, "cannot open file");
                    return 1;

                }
                if((fp = freopen(argv[2], "w", stdout)) == NULL) 
                {
                    fprintf(stderr, "cannot open file");
                    return 2;
                }
            }

    //чтение BOM(LE - fl=1, BE - fl=0)
    unsigned char bom[2]; int check, fl = 0;
    check = fread(bom, sizeof(unsigned char), 2, stdin);
    if (check != 2)
    {
        fseek(stdin, 0, SEEK_SET);
        fl = 1;
    }
    else 
    {
        if (bom[0]==0xff && bom[1]==0xfe)
        {
            fl = 1;
        } else 
        {
            if(bom[0]==0xfe && bom[1]==0xff)
            {
                fl = 0;
            } else
            {
                fseek(stdin, 0, SEEK_SET);
                fl = 1;
            }
        }
    }
    //чтение и перекодировка символов
    unsigned char s[3];
    int cond;
    unsigned char temp1;
    while((cond = fread(s, sizeof(unsigned char), 2, stdin)) == 2)
    {
        if(fl==1)
        {
            temp1 = s[0];
            s[0] = s[1];
            s[1] = temp1;
        } 
        if(s[0]==0)
            putc(s[1], stdout);
        else
        {
            if(s[0]<0x08) //2 байта
            {
                //1 байт
                s[0] = s[0] << 2;
                s[0] = s[0] | 0xc0;
                temp1 = s[1];
                temp1 = temp1 >> 6;
                s[0] = s[0] | temp1;
                //2 байт
                s[1] = s[1] & 0x3f;
                s[1] = s[1] | 0x80;

                fwrite(s, sizeof(unsigned char), 2, stdout);
            }
            else //3 байта
            {
                //1 байт
                temp1 = s[0];
                s[0] = s[0] >> 4;
                s[0] = s[0] | 0xe0;

                //3 байт
                s[2] = s[1];
                s[2] = s[2] & 0x3f;
                s[2] = s[2] | 0x80;

                //2 байт
                temp1 = temp1 << 4;
                temp1 = temp1 >> 2;
                temp1 = temp1 | 0x80;
                s[1] = s[1] >> 6;
                s[1] = s[1] | temp1;
                s[1] = s[1] | 0x80;

                fwrite(s, sizeof(unsigned char), 3, stdout);
            }
        }
    }
    if (cond == 1)
    {
        fprintf(stderr, "not correct text");
        return 3;
    }
}
