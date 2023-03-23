#pragma once

#include "noise/def.h"
#include "noise/position.h"

namespace noise {
namespace math {

class Histogram {

public:
    Histogram(int bin_size = 64);
    ~Histogram(void);

    void push(float value);
    void set(std::vector<float> &data);
    void print(void);

private:
    void process_data(void);
    bool processed_;

private:
    std::vector<float>  values_;
    std::vector<int>    bin_;
    int                 idx_zero_;
    int                 bin_size_;

    float   min_;
    float   max_;

    float   base_;
    float   step_;
    int     type_;
};

} // namespace math
} // namespace noise

