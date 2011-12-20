-- Where to listen for clients.
host = "0.0.0.0"
port = 40100
password = "test"

-- If true, a X server is required.
graphicalUi = true

-- Graphical mode:
--  true -> Show the component logs in the UI.
--  false -> Do not show the component logs in the UI.
-- Non-graphical mode:
--  true -> Show the component logs.
--  false -> Do not show the component logs.
coreLogToUi = true
soundLogToUi = true

-- Graphical mode:
--  true -> Print the component logs on std out.
--  false -> Do not print the component logs on std out.
-- Non-graphical mode:
--  Ignored.
coreLogToStdOut = true
soundLogToStdOut = false

-- The strategy name used for the StrategyInstantiator.
strategy = "MaCross"

-- Path to the configuration file of the strategy (StratParamsConf).
strategyParams = "data/params/MaCross.lua"

-- Print packet data to std out.
logPackets = true

-- Start with trading enabled.
enableTrading = true

-- Print more messages from SoundSystem (graphical mode).
debugSound = false

-- Sound files (graphical mode).
soundPathError = "data/sound/error.wav"
soundEnableError = true
soundPathWarning = "data/sound/warning.wav"
soundEnableWarning = true
soundPathUp = "data/sound/up.wav"
soundEnableUp = true
