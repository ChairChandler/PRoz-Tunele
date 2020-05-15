#ifndef RICHMANINFO_H
#define RICHMANINFO_H


class RichmanInfo
{
    int counter = 0;
    const int id;
public:
    RichmanInfo(int id);
    RichmanInfo& incrementCounter();
    RichmanInfo& incrementCounter(int val);
    int getCounter() const;
    int getId() const;
};

#endif // RICHMANINFO_H
