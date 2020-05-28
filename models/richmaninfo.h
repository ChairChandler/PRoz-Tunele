#ifndef RICHMANINFO_H
#define RICHMANINFO_H



class RichmanInfo
{
    int counter = 0;
    int id;
public:
    RichmanInfo() = default;
    RichmanInfo(int id);

    RichmanInfo& incrementCounter();
    RichmanInfo& incrementCounter(int val);

    void setCounter(int val);
    int getCounter() const;
    int getId() const;
    bool operator==(const RichmanInfo &a) const;
};

#endif // RICHMANINFO_H
