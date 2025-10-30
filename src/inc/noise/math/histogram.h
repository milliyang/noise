#pragma once

#include "noise/def.h"
#include "noise/position.h"

namespace noise {
namespace math {

#define OVERFLOW_BIN_NUM  (5)

class Histogram {

public:
    Histogram(int bin_size = 64, bool limited = false);
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

    bool                limited_;
    int                 overflow_bin_[OVERFLOW_BIN_NUM];   //100%~500%

    float   min_;
    float   max_;

    float   base_;
    float   step_;
    int     type_;
};

} // namespace math
} // namespace noise

