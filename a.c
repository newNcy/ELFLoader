void foo()
{
}
int main()
{
    int cs [100];
    typedef void (*pfoo)();
    for (int i = 0; i<100; i++) {
        cs[i] = *((int*)(&foo+i));
    }
    pfoo fp = (pfoo)&cs[0];
    fp();
}
