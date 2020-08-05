#ifndef DEBUG_H
#define DEBUG_H

#include <QDebug>
#include <iostream>
#include <chrono>

#define DEBUG_WATCH(var) qDebug() << #var << " = " << var;
#define DEBUG qDebug()

class ScopeTimer
{
public:
    using clock_t = std::chrono::high_resolution_clock;
    using second_t = std::chrono::duration<double, std::ratio<1>>;
    ScopeTimer(QString name)
    {
        m_name = name;
        m_beg = clock_t::now();
    }
    ~ScopeTimer()
    {
        std::cerr << m_name.toStdString() << " took " << std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count() << " seconds";
        std::cerr << std::endl;
    }
private:
    QString m_name;
    std::chrono::time_point<clock_t> m_beg;
};

class Timer
{
public:
    using clock_t = std::chrono::high_resolution_clock;
    using second_t = std::chrono::duration<double, std::ratio<1>>;
    Timer()
    {
        reset();
    }
    void reset()
    {
        m_beg = clock_t::now();
    }
    double elapsed()
    {
        return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
    }
private:
    std::chrono::time_point<clock_t> m_beg;
};


#define TIMER ScopeTimer _scope_timer_(__PRETTY_FUNCTION__)

#endif // DEBUG_H
