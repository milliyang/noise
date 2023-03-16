#include "noise/noise.h"
#include "noise/util.h"
#include "noise/log.h"
#include "noise/args.h"
#include "glob.h"
#include <armadillo>

#include "noise/feed.h"

#ifdef CONFIG_QPLOT
#include "qplot.h"
#endif

using namespace noise;

#define MAX_CODE_NUM   (20)

#define PERIOD (120)

struct select {
    int     window; // period
    float   ratio0; // min  / max0      # usually  < 1.0, means max loss drawdown
    float   ratio1; // max1 / min       # usually >= 1.0, means max profit ratio
};

struct record {
    //key
    std::string     code;
    int             date_begin;
    int             date_end;
    //
    struct select   sel;
    arma::frowvec   data;
};

float interpolate(float src, float src_l, float src_r, float target_l, float target_r)
{
    float value;
    if (src <= src_l) {
        value = target_l;
    } else if (src >= src_r) {
        value = target_r;
    } else {
        //    a      M - L     x - l
        //  ----- = ------  = -------
        //    b      R - M     r - x
        float a = src - src_l;
        float b = src_r - src;
        value = (a * target_r + b * target_l) / (a + b);
    }
    return value;
}

arma::frowvec linear_resize(const arma::frowvec &src, int size)
{
    arma::frowvec target = arma::frowvec(size);
    const int SRC_SIZE = (int) src.n_cols-1;
    for (int i = 0; i < size; i++) {
        if (i == 0) {
            target[i] = src[i];
            continue;
        } else if (i == (size-1)) {
            target[i] = src[src.n_cols-1];
            continue;
        }
        float ratio = (float) (i * SRC_SIZE) / (float)(size-1);
        int ii = (int)ratio;
        //   ii   ratio  ii+1
        //   10    ?     12
        target[i] = interpolate(ratio, (float)ii, (float)(ii+1), src[ii], src[ii+1]);
    }
    return target;
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

PtrSeries get_series(const std::string figuire, arma::frowvec &src)
{
    auto s0 = std::make_shared<struct series>();
    for (int i = 0; i < src.n_cols; i++) {
        s0->data.push_back(src.at(i));
    }
    s0->figure = figuire;
    s0->name = figuire;
    return s0;
}

int32_t main(int32_t argc, char** argv)
{
    noise::init();
    if (uargs::args0_parse(__FILE__, argc, argv) != 0) {
        return 0;
    }

    const struct select SEL = {PERIOD, 0.8f, 1.6f};

    struct feed_config cfg;
    cfg.code = "300015.SZ";
    // cfg.root_path = "P:/tmpfs/root.20230311";
    cfg.filename = "D:/quant/noise/build/Debug/000026.SZ.csv";

    noise::FeedCSV feed;
    feed.init(cfg);
    auto bars = feed.get_bars();

    arma::frowvec data = get_closes(bars);

#ifdef CONFIG_QPLOT
    noise::QPlot ploter;
#endif
    std::vector<PtrSeries> series;

    int end = (int) data.n_cols - PERIOD;
    for (int i = 0; i < end; i++) {
        arma::frowvec data0 = data.cols(i, i+PERIOD);
        auto min_idx = data0.index_min();
        if (min_idx <= 0 || min_idx >= (PERIOD-1)) {
            continue;
        }

        float min = data0.min();
        arma::frowvec data_1st = data0.cols(0, min_idx);
        arma::frowvec data_2nd = data0.cols(min_idx, PERIOD-1);

        float max0 = data_1st.max();
        float max1 = data_2nd.max();

        auto max0_idx =  data_1st.index_max();
        auto max1_idx =  data_2nd.index_max();
        max1_idx = (data_1st.n_cols-1)+max1_idx;   //to data0[max1_idx]

        // if ((max1 / min ) <= SEL.ratio1 ||
        //     (min  / max0) >= SEL.ratio0 ||
        //     (max0 > max1) ) {
        //     continue;
        //     // NG record ->
        // } else {
        //     // OK record ->
        // }
        float mmax = std::max(max0, max1);
        // data0.print();
        arma::frowvec data1 = data0 /mmax;

        LOGI(" [{}]  [{}]{} ->[{}]{} ->[{}]{}  data_idx:[{}]->[{}]->[{}]",
            i, max0_idx, max0, min_idx, min, max1_idx, max1,
            bars[max0_idx+i].date, bars[min_idx+i].date, bars[max1_idx+i].date);

        arma::frowvec data_sel = data1.cols(max0_idx, max1_idx);
        arma::frowvec data_sel_scaled = linear_resize(data_sel, PERIOD);

        data_sel.resize(PERIOD);

        series.clear();
        series.push_back(get_series("data1", data1));
        series.push_back(get_series("data_sel", data_sel));
        series.push_back(get_series("data_sel_rescale", data_sel_scaled));

#ifdef CONFIG_QPLOT
        ploter.plotChart("Result", series);
#endif
        // break;
    }
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
