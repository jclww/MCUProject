
void delay_ms(unsigned int y)
{
    unsigned char x;
    for (; y > 0; y--)
    {
        for (x = 120; x > 0; x--)
            ;
    }
}
void delay1s(void)
{
    unsigned char a, b, c;
    for (c = 95; c > 0; c--)
        for (b = 26; b > 0; b--)
            for (a = 185; a > 0; a--)
                ;
}