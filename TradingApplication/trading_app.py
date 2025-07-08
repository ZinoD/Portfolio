


import streamlit as st
import yfinance as yf
import pandas as pd
import ta

st.title("📈 Trading Backtester GUI")

symbols = st.multiselect(
"Select assets to analyze:",
['AAPL', 'BTC-USD', 'ETH-USD', '^GSPC', 'EURUSD=X'],
default=['AAPL', 'BTC-USD']
)

interval = st.selectbox("Select interval:", ['1d', '1h'])
period = st.selectbox("Select data period:", ['3mo', '6mo', '1y'])

def analyze_asset(symbol):
df = yf.download(symbol, interval=interval, period=period, progress=False)
if df.empty or len(df) < 60:
return f"⚠️ Not enough data for {symbol}"

df.dropna(inplace=True)
df['EMA50'] = ta.trend.ema_indicator(df['Close'], window=50).ema_indicator()
df['RSI'] = ta.momentum.rsi(df['Close'], window=14)
macd = ta.trend.macd(df['Close'])
df['MACD'] = macd.macd_diff()

df['Buy'] = (df['Close'] > df['EMA50']) & (df['RSI'] < 30) & (df['MACD'] > 0) & (df['MACD'].shift(1) < 0)
df['Sell'] = (df['Close'] < df['EMA50']) & (df['RSI'] > 70) & (df['MACD'] < 0) & (df['MACD'].shift(1) > 0)

position = None
entry_price = 0
trades = []

for i in range(1, len(df)):
if df['Buy'].iloc[i] and position is None:
position = 'long'
entry_price = df['Close'].iloc[i]
elif df['Sell'].iloc[i] and position == 'long':
exit_price = df['Close'].iloc[i]
profit = (exit_price - entry_price) / entry_price * 100
trades.append(profit)
position = None

if not trades:
return f"📊 {symbol}: No complete trades found in backtest."

win_rate = sum(1 for x in trades if x > 0) / len(trades) * 100
avg_return = sum(trades) / len(trades)
total_return = sum(trades)

return (
f"📈 {symbol} Backtest Results:\n"
f"Trades: {len(trades)}\n"
f"Win Rate: {win_rate:.2f}%\n"
f"Avg Return: {avg_return:.2f}%\n"
f"Total Return: {total_return:.2f}%\n"
)

if st.button("Run Backtest"):
for sym in symbols:
result = analyze_asset(sym)
st.text(result)
