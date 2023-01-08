#include "noise/def.h"
#include "noise/strategy.h"
#include "noise/broker.h"
#include "noise/util.h"
#include "math.h"
#include <cmath>

namespace noise {

int umath::sum(const std::vector<int> &values)
{
    int sum = 0;
    if (values.size() <= 0) { return 0; }
    for (auto it = values.begin(); it != values.end(); ++it) {
        sum += *it;
    }
    return sum;
}

float umath::mean(const std::vector<int> &values)
{
    if (values.size() <= 0) { return 0; }
    return (float)umath::sum(values) / (float) values.size();
}

float umath::sum(const std::vector<float> &values)
{
    float sum = 0;
    if (values.size() <= 0) { return 0; }
    for (auto it = values.begin(); it != values.end(); ++it) {
        sum += *it;
    }
    return sum;
}

float umath::mean(const std::vector<float> &values)
{
    if (values.size() <= 0) { return 0; }
    return umath::sum(values) / values.size();
}

float umath::stddev(const std::vector<float> &values)
{
    if (values.size() <= 0) { return 0; }
    float mean = umath::mean(values);

    float delta = 0;
    for (auto it = values.begin(); it != values.end(); ++it) {
        delta += powf(*it - mean, 2);
    }
    return sqrtf(delta / values.size());
}

void umath::test_nanf(void)
{
    float value = nanf("");
    int *pp = (int*) &value;
    printf("pp:%0x value:%f\n", *pp, value);
    if (isnanf(value)) {
        printf("isnanf true  nanf-100: %f\n", value-100);
    }
}

}