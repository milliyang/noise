Noise
=====
A Backtest or Trading Framework with C++


Why invent the wheel ?
------------
* Inspired by [backtesting.py](https://kernc.github.io/backtesting.py)
* Not invent, just rewrite it in another way
* Need a simpler, faster, ease of replace component framework


Features
--------
* Simple, no-documented API / structure
* Composable Strategies / Broker / Feed / Ploting
* Support different ways of Backtest (SimpleBroker/BrokerMuxer/EventDriven/PreProcessed)
* Cheap Backtest Result
* Cheap Visualization
* Cheap TA-Lib

Requirements
------------
- OpenMP [Optional]     maximize 48+ Cores to run
- Qt [Optional]         for GUI ploting
- GCC
- CMAKE
- 3rd in: noise/3rd
    - [fmt](https://github.com/fmtlib/fmt)
    - [spdlog](https://github.com/gabime/spdlog)
    - [args](https://github.com/Taywee/args)
    - [inih](https://github.com/benhoyt/inih)
    - [Armadillo](https://arma.sourceforge.net)
    - [glob](https://github.com/p-ranav/glob)
    - [hdf5](https://github.com/HDFGroup/hdf5) & [HighFive](https://github.com/BlueBrain/HighFive)


Build on Linux
--------------
```sh
    cd noise
    mkdir build && cd build
    cmake ..
    make && ./bt
```

Usage
-----
```sh
    make && ./bt
```

Results in:

```text
[Stat] summary:
  Start                      [YYYMMDD] 20180102
  End                        [YYYMMDD] 20180213
  Duration                       [days]     112
  Exposure Time                      [%]  99.11
  Equity Final                  [$]    +6277.18
  Equity Peak                   [$]   +10109.52
  PNL                           [$]    -3722.82
  Return                             [%] -37.23
  Buy & Hold Return                  [%]  -8.54
  Return (Ann.)                      [%]   0.00 TBD
  Volatility (Ann.)                  [%]   0.00 TBD
  Sharpe Ratio                             0.00 TBD
  Max. Drawdown                      [%]   0.00 TBD
  Avg. Drawdown                      [%]   0.00 TBD
  Max. Drawdown Duration         [days]    0.00 TBD
  Avg. Drawdown Duration         [days]    0.00 TBD
  Trades                                     57
  Win Rate                           [%]  35.09
  Avg. Trade                         [%]  -3.99
  Best. Trade                        [%]  11.11
  Wors. Trade                        [%] -17.50
  Best. Trade on PNL             [$]     +40.85
  Wors. Trade on PNL             [$]    -246.01
  Max. Trade Duration           [days]   108.00
  Avg. Trade Duration           [days]    51.63
  SQN                                     -5.53

[main] time elapsed: 12.594399 ms
```

Find more usage examples in dir noise/sample/

Build on Windows with GUI Ploting
--------------
```sh
    cd noise
    mkdir build && cd build
    cmake ../sample
    MSBuild.exe noise.sln           # require VisualStudio
    .\Debug\cd.exe
```
![output_image](https://bg-so-1.zippyimage.com/2023/03/12/4f0a4a172cc3761cf68bee597bb7612d.png)

Contibutes
----------
- Always welcomed ( Feature or Bugfix or Unitest )
- Platform Independent
- Simple is the most important thing. No heavy / fancy code. Not much 3rd library I Hope...
- Keep less code

Final
-----
* No Lisence, or [UnLisence](https://unlicense.org/)
* Thanks to [Github](https://github.com/), [kernc](https://github.com/kernc)
* Again, Inspired by [backtesting.py](https://kernc.github.io/backtesting.py)


Additional Remarks
------------------
* I did try not to include too much third-party library. But as project went on, they grows ...
* Some are just tiny cpp header-only:
    - [fmt](https://github.com/fmtlib/fmt)
    - [spdlog](https://github.com/gabime/spdlog)
    - [args](https://github.com/Taywee/args)
    - [inih](https://github.com/benhoyt/inih)

* Some made it easier for me to do unitest, math calculation
    - [Armadillo](https://arma.sourceforge.net)
    - [Eigen](https://gitlab.com/libeigen/eigen)
    - [glob](https://github.com/p-ranav/glob) & [filesystem](https://github.com/gulrak/filesystem)

* Some help to speed up backtesting for large dataset:
    - [hdf5](https://github.com/HDFGroup/hdf5) & [HighFive](https://github.com/BlueBrain/HighFive)

* They may have diff Lisences ...
