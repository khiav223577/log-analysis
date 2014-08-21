void test_FormatterDate(){
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
void test_InputFormatter(){
    InputFormatter formatter2;
    formatter2.formatList.push_back(new FormatterDate("MMM d HH:mm:ss "));
    formatter2.formatList.push_back(new FormatterString(" ", MAX_STRING_SIZE));
    formatter2.formatList.push_back(new FormatterInteger("10"));
    formatter2.formatList.push_back(new FormatterDiscard(","));
    formatter2.formatList.push_back(new FormatterDate("yyyy/MM/dd HH:mm:ss,"));
    formatter2.formatList.push_back(new FormatterString(",", MAX_STRING_SIZE));
    formatter2.formatList.push_back(new FormatterString(",", MAX_STRING_SIZE));
    formatter2.formatList.push_back(new FormatterString(",", MAX_STRING_SIZE));
    formatter2.formatList.push_back(new FormatterInteger("10"));
    formatter2.formatList.push_back(new FormatterDiscard(","));
    formatter2.formatList.push_back(new FormatterDate("yyyy/MM/dd HH:mm:ss,"));
    formatter2.formatList.push_back(new FormatterIPaddr(NULL));
    formatter2.formatList.push_back(new FormatterDiscard(","));
    formatter2.formatList.push_back(new FormatterIPaddr(NULL));
    formatter2.formatList.push_back(new FormatterDiscard(","));
    formatter2.formatList.push_back(new FormatterIPaddr(NULL));
    formatter2.formatList.push_back(new FormatterDiscard(","));
    formatter2.formatList.push_back(new FormatterIPaddr(NULL));
    formatter2.formatList.push_back(new FormatterDiscard(","));
    formatter2.execute("Dec  3 04:00:01 iisfw 1,2013/12/03 04:00:01,0011C101825,TRAFFIC,end,1,2013/12/03 04:00:00,140.109.23.120,140.109.254.5,0.0.0.0,0.0.0.0");
}
