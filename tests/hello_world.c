int get_two(int a, int* b, int c)
{
    // SINGLE OPERATORS
    a = 1+0-0*0%2;
    a = !0 & 0 | b[0];
    a = a < 0 + a > 0 / 2;
    return a;

}

// TODO: remove this hack to make this file compilable
char * print(char*);
int input();

int main()
{
    int number = 1355;
    char* my_str = "expected_string";
    char a = 'f';
    void* my_ptr;

    print("welcome");

    int other_number = input();

    while(0)
    {
        break;
        continue;
    }

    // COMPOUND OPERATORS

    if(number == 1) {
        return 1;
    }
    else if(number != 2 && number <= 3){
        // number equals three
        return 2;
    }
    else if(number >= 4 || number <= -1)
    {
        return 3;
    }
    else {
        // number equals zero
        return 0;
    }
}
