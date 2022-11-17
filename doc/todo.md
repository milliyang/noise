

TODO LIST (20220529)
-------------------
```

- time / duration / exposure days ( formating, calculation )        => done
- sys log                                                           -> skip, later
- openmp                                                            =>done
- utalib                                                            =>done
- 3rd talib                                                         -> skip, later
- predefine indicator (_date, open, _high, _low, _close, _volume )  => done
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

    - add pnl figure

    - show buy,sell check point ( special indicator: start,arrow,triangle,text)


- update data when not support preload

- 3rd:
    - talib  ?
    - opencv ?

- search, calc drawdown ( max,avg on closed trades )
- yml or ini config
- unitest
- strategy interface rewrite ( user, system isolation )
- ploting tool (terminal, gnuplot, flutter ... )
    - blur type=(nanf, replicate, mirror), kernel=5, anchor=(left,middle,right)


- pure terminal control console (tmux)

- data feed (tushare,FTNN, ...)
- real world trading broker (FTNN) ?
- database for history data ?

```


### Note:
```
    Stat:
        - 计算每个阶段的Stat股票价值+现金 （用于数据Plotting分析）
            - 在每个Bar后计算，还是所有Bar完成后计算

        - 


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



```