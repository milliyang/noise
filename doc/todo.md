
TODO LIST (20230111)
-------------------
```
- spdlog/fmt                                                        =>done
- talib more series: ma,stddev,boll                                 =>done
- add pnl figure                                                    =>done
- handy noise::plot()                                               =>done
- Plot stock name, symbol                                           =>done
- select area for pattern detection                                 =>skip
- sum total pnl & trade                                             =>done
- support without preload  (avoid peek data ahead)                  =>done
- unitest for preload                                               =>done
- verify ma/stddev utest; compare to armadillo,ta-lib,              =>done

- yml or ini config
- show buy,sell check point ( special series: start,arrow,triangle,text)
- update data when not support preload

```


TODO LIST (20220529)
-------------------
```
- time / duration / exposure days ( formating, calculation )        =>done
- sys log                                                           =>done
- openmp                                                            =>done
- utalib                                                            =>done
- 3rd talib                                                         -> skip, later
- predefine series (_date, open, _high, _low, _close, _volume )  =>done
- gnuplot
- subdir: .qline
- qplot
    - main figure                                                   =>done
    - volume figure                                                 =>done
    - connect mutiple figure                                        =>done
    - show cur bar info (on the left side)                          =>done
    - move cursor in all figure                                     =>done
    - bugfix: center line in bar when using keyboard (left,right)   =>done
    - mouse wheel (scaler up/down, move window)                     =>done
        - scale up/down ->done
        - sync cur_idx on mouse pos after scale
        - move window when mouse left button down
    - show lagel; (show/hide)                                       =>done
        - draw
        - on_click ( hide/show )                                    ->skip
```


Others:
-------------------
- 3rd:
    - talib  ?
    - opencv ?

- search, calc drawdown ( max,avg on closed trades )
- unitest
- strategy interface rewrite ( user, system isolation )
- ploting tool (terminal, gnuplot, flutter ... )
    - blur type=(nanf, replicate, mirror), kernel=5, anchor=(left,middle,right)

- pure terminal control console (tmux)

- data feed (tushare,FTNN, ...)
- real world trading broker (FTNN) ?
- database for history data ?


### Note:
```


```



###
```
tushare

https://github.com/cttn/Stockex
https://github.com/milliyang/yfinance

http://baostock.com/baostock/index.php/%E9%A6%96%E9%A1%B5

https://www.akshare.xyz/index.html


pip install akshare mplfinance

import akshare as ak
import mplfinance as mpf  # Please install mplfinance as follows: pip install mplfinance
stock_us_daily_df = ak.stock_us_daily(symbol="AAPL", adjust="qfq")


image hosting:  https://zippyimage.com/


```

