#define LOG_TAG "histo"

#include "noise/def.h"
#include "noise/noise.h"

#include "noise/math/histogram.h"

namespace noise {
namespace math {

#define SPECIAL_BIN  (3)

Histogram::Histogram(int bin_size)
{
    bin_size_ = bin_size;

    min_ = 99999;
    max_ = -99999;
}

Histogram::~Histogram(void)
{
}

void Histogram::push(float value)
{
    values_.push_back(value);

    min_ = std::min(value, min_);
    max_ = std::max(value, max_);
    processed_ = false;
}

void Histogram::set(std::vector<float> &data)
{
    values_.clear();
    min_ = 99999;
    max_ = -99999;
    for (int i = 0; i < data.size(); i++) {
        push(data[i]);
    }
}

void Histogram::process_data(void)
{
    if (processed_) {
        return;
    }

    float max = std::max(std::abs(max_), std::abs(min_));
    idx_zero_ = bin_size_ / 2;
    step_ = max / idx_zero_;
    //LOGI("STEP:{} max:{}  max_:{} min_:{}", step_, max, max_, min_);

    //init
    bin_.resize(bin_size_ + SPECIAL_BIN);
    for (size_t i = 0; i < bin_.size(); i++) {
        bin_[i] = 0;
    }

    //accumulate
    for (size_t i = 0; i < values_.size(); i++) {
        int idx = idx_zero_ + (int)(values_[i] / step_);
        if (values_[i] >= 0) {
            idx+=2;
        }
        bin_[idx]++;
        assert(idx < bin_.size());
    }
    processed_ = true;
}

void Histogram::print(void)
{
    process_data();

    int max_num = -1;
    int tot_num = 0;
    for (size_t i = 0; i < bin_.size(); i++) {
        max_num = std::max(bin_[i], max_num);
        tot_num += bin_[i];
    }

    #define MAX_BIN_LEN (40)
    char buffer[MAX_BIN_LEN+1];
    memset(buffer, ' ', sizeof(buffer));
    buffer[MAX_BIN_LEN] = 0;

    LOGI("Histogram:");
    LOGI("{}   ratio |  win  / total | profit(%)", buffer);

    bool skip_empty_bin_ahead = true;
    for (int i = 0; i < (int) bin_.size(); i++) {
        if (skip_empty_bin_ahead && bin_[i] <= 0) {
            continue;
        }
        skip_empty_bin_ahead = false;

        memset(buffer, ' ', sizeof(buffer));
        int val = MAX_BIN_LEN * bin_[i] / max_num;

        // bugfix rounding, at least one
        if (bin_[i] > 0 && val <= 0) {
            val = 1;
        }
        if (val > 0) {
            memset(buffer, '*', val);
        }
        buffer[MAX_BIN_LEN] = 0;

        LOGI("{}  {:5.2f}%  {:6d} /{:6d}  {:+0.2f}", buffer, 100.0f * bin_[i] / tot_num, bin_[i], tot_num, ( (i - idx_zero_ - 1) * step_));
    }
}

}
}