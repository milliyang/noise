#include "noise/noise.h"
#include "noise/util.h"
#include "noise/log.h"
#include "noise/args.h"
#include "glob.h"
#include <armadillo>

#include <cstdlib>
#include <ctime>
#include <algorithm>

#ifdef CONFIG_QPLOT
#include "qplot.h"
#endif

using namespace noise;

#define MAX_CODE_NUM   (20)

#define PERIOD (120)


#define WIN_SIZE_MIN            (10)
#define WIN_SIZE_MAX            (30)

#define POSITIVE_RATIO_MIN      ( 0.0f)
#define POSITIVE_RATIO_MAX      ( 1.0f)
#define POSITIVE_RATIO_STEP     ( 0.1f)

#define FUTURE_WIN_SIZE         (10)
#define RAISE_THRESHOLD         (0.03f)         // 600RMB / 2W

#define NUM_OF_CODES            (200)

struct select {
    int     window;                 // period
    float   positive_ratio;         // raise / (raise+fall)
    int     future_windows;         //
    float   raise_threshold;        // 5%

    //cache
    int     match_cnt;
    int     winning_cnt;

    //output
    float   winning_rate;
};

PtrSeries create_series(const std::string figuire, arma::frowvec &src)
{
    auto s0 = std::make_shared<struct series>();
    for (int i = 0; i < src.n_cols; i++) {
        s0->data.push_back(src.at(i));
    }
    s0->figure = figuire;
    s0->name = figuire;
    return s0;
}

arma::frowvec get_closes(std::vector<struct bar> &bars)
{
    arma::frowvec closes = arma::frowvec(bars.size());
    int i = 0;
    for (int i = 0; i < bars.size(); i++) {
        closes[i] = bars[i].close;
    }
    return closes;
}

arma::frowvec get_closes(struct bars &bars)
{
    return get_closes(bars.data);
}

void process_code_winning_rate(std::vector<struct select> &conds, const std::string &code)
{
    struct bars bars = mcache::get_bars(code);
    LOGI("read code:{} size:{}", code, bars.data.size());

    arma::frowvec close_v = get_closes(bars);

    for (auto iter = conds.begin(); iter != conds.end(); iter++) {
        auto &sel = *iter;

        int win_size = sel.window + sel.future_windows;
        int max_idx = (int) close_v.n_cols - win_size;
        if (max_idx <= 0) {
            continue;
        }

        for (int i = 0; i < max_idx; i++) {
            arma::frowvec subsample = close_v.cols(i, i+win_size);

            int tot = 0;
            int win = 0;
            for (int j = 1; j <= sel.window; j++) {
                tot++;
                if (subsample[j] > subsample[j-1]) {
                    win++;
                }
            }
            assert(tot == sel.window);
            float positive_ratio = (float) win / tot;

            if (positive_ratio >= sel.positive_ratio) {
                //match
                sel.match_cnt++;

                //float final_value = subsample[win_size-1];
                float final_value = subsample.cols(sel.window, win_size-1).max();

                //match & win
                float raise_ratio = (final_value - subsample[sel.window]) / subsample[sel.window];
                if (raise_ratio >= sel.raise_threshold) {
                    sel.winning_cnt++;
                }
            }
        }

    }
}

bool compare_winning_ratio_raising(struct select i1, struct select i2)
{
    return (i1.winning_rate < i2.winning_rate);
}

int32_t main(int32_t argc, char** argv)
{
    if (noise::init(argc, argv) != 0) {
        return 0;
    }

    std::vector<struct select> conditions;

    for (int i = WIN_SIZE_MIN; i < WIN_SIZE_MAX; i++) {
        for (float j = POSITIVE_RATIO_MIN; j <= POSITIVE_RATIO_MAX; j+=POSITIVE_RATIO_STEP) {
            struct select sel;
            memset(&sel, 0, sizeof(struct select));
            sel.window = i;
            sel.positive_ratio = j;
            sel.future_windows = FUTURE_WIN_SIZE;
            sel.raise_threshold = RAISE_THRESHOLD;
            conditions.push_back(sel);
        }
    }
    LOGI("conditions:{}", conditions.size());

    //select random codes
    std::vector<code_info> codes;
    mcache::get_all_code_info(codes);
    LOGI("ALL CODES:{}", codes.size());

    std::srand( (uint32_t) std::time(nullptr)); // use current time as seed for random generator
    int random_variable = std::rand();
    LOGI("random_variable:{}  RAND_MAX:{}", random_variable, RAND_MAX);

    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, (int)codes.size());

    std::vector<int> nums;
    for (int i = 0; i < NUM_OF_CODES; i++) {
        nums.push_back(dist(rd));
    }
    auto ip = std::unique(nums.begin(), nums.end());
    nums.resize(std::distance(nums.begin(), ip));

    for (int i = 0; i < nums.size(); i++) {
        auto code_info = codes[nums[i]];
        process_code_winning_rate(conditions, code_info.code);
    }

    for (auto it = conditions.begin(); it != conditions.end(); it++) {
        if (it->match_cnt > 0) {
            it->winning_rate = (float)it->winning_cnt / (float)it->match_cnt;
        } else {
            it->winning_rate = 0.0f;
        }
    }

    sort(conditions.begin(), conditions.end(), compare_winning_ratio_raising);

        LOGI("window, positive, future_win, threshold |    winning_rate    win/match");
    for (auto it = conditions.begin(); it != conditions.end(); it++) {
        LOGI("{:5d}    {:6.2f}     {:5d}        {:6.2f}   |   {:6.2f}     {:5d}/{:5d}",
            it->window,
            it->positive_ratio,
            it->future_windows,
            it->raise_threshold,
            it->winning_rate,
            it->winning_cnt,
            it->match_cnt);
    }

    return 0;

#ifdef CONFIG_QPLOT
    noise::QPlot ploter;
#endif
    std::vector<PtrSeries> series;

    // series.clear();
    // series.push_back(create_series("data1", data1));
    // series.push_back(create_series("data_sel", data_sel));
    // series.push_back(create_series("data_sel_rescale", data_sel_scaled));

#ifdef CONFIG_QPLOT
        ploter.plotChart("Result", series);
#endif

    return 0;
}

/**

MSBuild.exe noise.sln -m
MSBuild.exe noise.sln -target:bt -m
MSBuild.exe noise.sln -target:sel -m

.\Debug\bt.exe -c 300014.SZ -m 100000 -p0
.\Debug\bt.exe -f .code.csv -m 100000 -p1

.\Debug\sel.exe -c 000026.SZ -m 100000 -p1
.\Debug\show.exe -c D:\quant\noise\build\Debug\000026.SZ.csv


grep.exe -rin "sqn\|code\|Win Rate" logs\sink.i.log
grep.exe -rin "sqn\|code\|Win Rate" logs\sink.d.log

*/
