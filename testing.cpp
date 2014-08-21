void SetColor2(){
    static int i = 2;
    SetColor(i);
    i = (i == 8 ? 2 : i + 1);
}
void test_date_formatter(){
    FormatterDate formatData("yyyy/MM/dd HH:mm:ss");
    InputFormatter formatter;
    srand(time(NULL));
    for(int i = 0; i < 1000; ++i){
        int year = 2000 - rand() % 61 + 30;
        int month = rand() % 12 + 1;
        int dates[] = {31,28,31,30,31,30,31,31,30,31,30,31};
        if (RDate::is_leapyear(year)) dates[1] = 29;
        int day = rand() % dates[month - 1] + 1;
        int hour = rand() % 24;
        int min = rand() % 60;
        int sec = rand() % 60;
        char data[999];
        const char *inputStream = data;
        sprintf(data,"%d/%02d/%02d %02d:%02d:%02d",year,month,day,hour,min,sec);
        SetColor2();formatData.execute(&inputStream);
    }
    SetColor(7);
}
