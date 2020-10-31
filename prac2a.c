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
    char bom[3]; int check, fl = 0;
    check = fread(bom, sizeof(char), 3, stdin);
    if (check != 3)
    {
        fseek(stdin, 0, SEEK_SET);
        fl = 1;
        bom[1] = 0xfe;
        bom[0] = 0xff;
        fwrite(bom, sizeof(char), 2, stdout);

    }
    else 
        if (bom[0]!=0xef || bom[1]!=0xbb || bom[2]!=0xbf)
        {
                fseek(stdin, 0, SEEK_SET);
                fl = 1;
                bom[1] = 0xfe;
                bom[0] = 0xff;
                fwrite(bom, sizeof(char), 2, stdout);
        } else 
        {
            bom[0] = 0xfe;
            bom[1] = 0xff;
            fwrite(bom, sizeof(char), 2, stdout);
        }

    //чтение и перекодировка символов
    char s[3];
    char temp1, temp2;
    while((s[0]=getchar())!=EOF)
    {
        if ((s[0] & 0x80) == 0x00) //начинается c 0
        {
            s[1] = s[0];
            s[0] = 0x00;
            if(fl == 0)
                fwrite(s, sizeof(char), 2, stdout);
            else 
            {
                temp1 = s[0];
                s[0] = s[1];
                s[1] = temp1;
                fwrite(s, sizeof(char), 2, stdout);
            }
        } else if ((s[0] & 0xe0) == 0xc0) //начинается с 110
                {
                    s[1] = getchar();
                    if((s[1] & 0xc0) != 0x80)
                    {
                        fprintf(stderr, "not correct text");
                        return 3;
                    }
                    temp1 = s[0] & 0x1c;
                    temp2 = s[0] & 0x03;
                    s[0] = temp1 >> 2;
                    s[1] = s[1] & 0x3f;
                    s[1] = s[1] | (temp2 << 6);
                    if(fl == 0)
                        fwrite(s, sizeof(char), 2, stdout);
                    else 
                    {
                        temp1 = s[0];
                        s[0] = s[1];
                        s[1] = temp1;
                        fwrite(s, sizeof(char), 2, stdout);
                    }
                } else if ((s[0] & 0xf0) == 0xe0) //начинается с 1110
                        {
                            s[1] = getchar();
                            if((s[1] & 0xc0) != 0x80)
                            {
                                fprintf(stderr, "not correct text");
                                return 4;
                            }
                            s[2] = getchar();
                            if((s[2] & 0xc0) != 0x80)
                            {
                                fprintf(stderr, "not correct text");
                                return 5;
                            }
                            //первый байт
                            s[0] = s[0] << 4;
                            temp1 = s[1] & 0x3c;
                            temp1 = temp1 >> 2;
                            s[0] = s[0] | temp1;
                            //второй байт
                            s[1] = s[1] << 6;
                            s[2] = s[2] & 0x3f;
                            s[1] = s[1] | s[2];

                            if(fl == 0)
                                fwrite(s, sizeof(char), 2, stdout);
                            else 
                            {
                                temp1 = s[0];
                                s[0] = s[1];
                                s[1] = temp1;
                                fwrite(s, sizeof(char), 2, stdout);
                            }
                        } else
                        {
                            fprintf(stderr, "not correct text");
                            return 5;
                        }
    }
}
