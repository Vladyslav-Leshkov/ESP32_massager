#ifndef MODES_H
#define MODES_H

// Структура для зберігання сигналу
struct Signal
{
    int pin;                 // Номер піна (1–5)
    unsigned long startTime; // Час початку (мс, нормалізований)
    unsigned long duration;  // Тривалість (мс)
};

// Mode#1: Нормалізовані дані (66615 → 0 мс)
const Signal mode1[] = {
    {2, 0, 6017},      // PIN2: Start time: 66615 → 0, Duration: 6017
    {1, 0, 18249},     // PIN1: Start time: 66615 → 0, Duration: 18249
                       //  {5, 6116, 6017},   // PIN5: Start time: 72731 → 6116, Duration: 6017
    {4, 14267, 3982},  // PIN4: Start time: 80882 → 14267, Duration: 3982
    {1, 20383, 3971},  // PIN1: Start time: 86998 → 20383, Duration: 3971
    {2, 20383, 3971},  // PIN2: Start time: 86998 → 20383, Duration: 3971
    {5, 20383, 3972},  // PIN5: Start time: 86998 → 20383, Duration: 3972
    {1, 25475, 3982},  // PIN1: Start time: 92090 → 25475, Duration: 3982
    {2, 25475, 3982},  // PIN2: Start time: 92090 → 25475, Duration: 3982
    {1, 31591, 3971},  // PIN1: Start time: 98206 → 31591, Duration: 3971
    {2, 31591, 3971},  // PIN2: Start time: 98206 → 31591, Duration: 3971
    {4, 31591, 3972},  // PIN4: Start time: 98206 → 31591, Duration: 3972
    {2, 38718, 6017},  // PIN2: Start time: 105333 → 38718, Duration: 6017
    {5, 38718, 6017},  // PIN5: Start time: 105333 → 38718, Duration: 6017
    {1, 38718, 18249}, // PIN1: Start time: 105333 → 38718, Duration: 18249
    {4, 46869, 3982},  // PIN4: Start time: 113484 → 46869, Duration: 3982
    {5, 52985, 3982},  // PIN5: Start time: 119600 → 52985, Duration: 3982
    {2, 59101, 3982},  // PIN2: Start time: 125716 → 59101, Duration: 3982
    {1, 59101, 19261}, // PIN1: Start time: 125716 → 59101, Duration: 19261
    {4, 63182, 7535},  // PIN4: Start time: 129797 → 63182, Duration: 7535
    {5, 72862, 8558},  // PIN5: Start time: 139477 → 72862, Duration: 8558
};

// Кількість сигналів у Mode#1
const int mode1Size = sizeof(mode1) / sizeof(mode1[0]);

// Період циклу Mode#1 (мс)
const unsigned long mode1Period = 72862 + 8558; // 72862 (останній startTime) + 8558 (тривалість останнього сигналу) = 81420 мс

#endif