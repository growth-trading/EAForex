---
description: Rich Trading Bot project overview — apply when working with RichTradingBot files, discussing architecture, or explaining the EA's purpose and conventions.
globs: ["Custumer/RichTradingBot/**"]
alwaysApply: false
---

# Rich Trading Bot — Tổng Quan Dự Án

**File code:** `Custumer/RichTradingBot/RichTradingBot.cpp`
- Đổi extension thành `.mq5` khi nạp vào MetaEditor để biên dịch
- Spec đầy đủ: `Custumer/RichTradingBot/CLAUDE.md`

**Nền tảng:** MetaTrader 5 (MQL5)
**Phiên bản:** v1.0
**Cặp tiền ưu tiên:** XAUUSD (Vàng)
**Khung thời gian:** Đa khung (H1, M15, M1...)
**Magic Number mặc định:** `202601`

## Kiến Trúc Sự Kiện

| Handler | Tần suất | Nhiệm vụ |
| :--- | :--- | :--- |
| `OnTick()` | Mỗi tick | `CheckEntry()` + `CheckExit()` (khi Stealth Mode ON) |
| `OnTimer()` | Mỗi 1 giây | Day P/L, DCA, Pyramiding, Trailing, Trimming, GUI |
| `OnTradeTransaction()` | Ngay khi deal thêm | Realtime Day P/L + GUI khi có lệnh đóng |
| `OnInit()` | Khởi động | Khởi tạo indicator handles, DCA arrays, timer |
| `OnDeinit()` | Thoát | Dọn GUI, giải phóng indicator handles |

## Quy Ước Comment Lệnh

Mỗi lệnh mang comment `"RTB|tp_pts|sl_pts"` để bot phân biệt và xử lý:
- Lệnh gốc / pyramiding: `"RTB|0|0"`
- Lệnh DCA: `"RTB|500|0"` (TP=500pts, SL=0)

## Thư Viện Dùng

```cpp
#include <Trade\Trade.mqh>
#include <Trade\PositionInfo.mqh>
CTrade Trade;
```
