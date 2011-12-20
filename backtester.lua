-- The tested strategy.
strategy = "MaCross"

-- Path to the parameters file for the strategy.
strategyParams = "data/params/MaCross-backtest.lua"

-- Ask user confirmation before launching the backtest.
confirmLaunch = true

-- If false, only the start parameters will be used with one thread.
optimizationMode = false

-- What paramters generator to use ("genetic" coming later) (optmization mode only).
-- Choices: "complete"
paramsGenerator = "complete"

-- How to sort the results and find the best generated parameters.
-- Choices: "profit"
resultRanking = "profit"

-- If true, show the details of each trade at the end (non-optimization mode only).
showTradeDetails = true

-- If true, log every trade action in real time (buy/sell/adjust/close).
showTradeActions = true

-- Number of threads used for the test (optimization mode only).
threads = 3

-- Source bars.
history = "data/history/EURUSD_MetaQuotes_2011-10-31_2011-11-04.csv"
pair = "EURUSD" -- Two 3 characters currencies ISO 4217.
digits = 5
maxGapSize = 60 -- Generate up to X 1 minute bars before creating a gap in history.

-- Deposit in units of the counter currency.
deposit = 10000

-- Period in minutes.
period = 1

-- Spread in pips.
spread = 1.3

-- Minimal offset in pips between current price and target SL/TP price for opening and adjusting positions.
minPriceOffset = 5

-- false -> Maximum tick generation (up to 12 per 1-minute bar).
-- true -> Simple and faster tick generation (up to 4 per 1-minute bar).
fewerTicks = false

-- If true, the 2 plot files will be generated (non-optimization mode only).
plotOutput = true

-- Path to the plot data file.
plotDataFile = "backtest-result.dat"

-- Path to the plot settings file.
plotSettingsFile = "backtest-result.plt"
