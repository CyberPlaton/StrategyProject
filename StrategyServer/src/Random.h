#pragma once

#include <random>

class Random {
public:
    Random()
    {
        s_RandomEngine.seed(std::random_device()());
    }
    ~Random()
    {
    }

    float AlternatingOneFloat()
    {
        return (s_Distribution(s_RandomEngine) % 2 == 0) ? -1.0f : 1.0f;
    }

    uint32_t AlternatingOneInt()
    {
        return (s_Distribution(s_RandomEngine) % 2 == 0) ? -1 : 1;
    }

    float Float()
    {
        return (float)s_Distribution(s_RandomEngine) / std::numeric_limits<uint32_t>::max();
    }

    uint32_t Int()
    {
        return (uint32_t)s_Distribution(s_RandomEngine);
    }

    float InRangeFloat(float x, float y)
    {
        return (x + Float() * (y - x));
    }


private:
    std::mt19937 s_RandomEngine;
    std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
};
