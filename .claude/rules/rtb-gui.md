---
description: Rich Trading Bot GUI panel — apply when working with UpdateGUI, DrawHLine, Lbl, GUI labels, Day P/L display, or OnTradeTransaction for realtime updates.
globs: ["Custumer/RichTradingBot/**"]
alwaysApply: false
---

# Rich Trading Bot — Giao Diện (GUI)

Panel tự động vẽ góc trên-trái màn hình, cập nhật mỗi giây qua `OnTimer()` và ngay lập tức qua `OnTradeTransaction()`.

## Thông Tin Hiển Thị

| Label | Mô tả | Màu động |
| :--- | :--- | :--- |
| RICH TRADING BOT v1.0 | Tiêu đề | `C'80,160,255'` (xanh nhạt) |
| Time | Giờ địa phương `YYYY/MM/DD HH:MM:SS` | Silver |
| Signal | Tên chiến lược tín hiệu đang dùng | Yellow |
| Direct | Hướng giao dịch | Xanh lá=Buy / Đỏ=Sell / Xanh dương=Both |
| Balance | Số dư tài khoản ($) | Silver |
| Initial | Balance khi khởi động EA ($) | Silver |
| Day P/L | Lãi/lỗ đã chốt trong ngày ($) | Xanh lá/Đỏ |
| Float | Tổng floating P/L + % so với balance ban đầu | Xanh lá/Đỏ |
| DD Now | Drawdown hiện tại (%) | Cam khi >15% |
| DD Max | Drawdown lớn nhất từ khi khởi động (%) | Đỏ khi >25% |
| Spread | Spread hiện tại (points) | Silver |
| Buy P/L | Floating P/L tất cả lệnh Buy ($) | Xanh lá/Đỏ |
| Buy Ord | Số lệnh Buy + tổng lot | Silver |
| Sell P/L | Floating P/L tất cả lệnh Sell ($) | Xanh lá/Đỏ |
| Sell Ord | Số lệnh Sell + tổng lot | Silver |
| Total | Tổng số lệnh đang mở | Silver |

## Quy Tắc Màu Sắc

| Màu | Điều kiện |
| :--- | :--- |
| `LimeGreen` | Lãi dương / hướng Buy Only |
| `Tomato` | Lỗ / hướng Sell Only / DD cảnh báo |
| `OrangeRed` | DD Now > 15% |
| `Yellow` | Tên chiến lược tín hiệu |
| `DodgerBlue` | Hướng Both / Either |

## Cập Nhật Realtime — Day P/L

```cpp
void OnTradeTransaction(...) {
    if(trans.type == TRADE_TRANSACTION_DEAL_ADD) {
        UpdateDayProfit();  // tính lại ngay
        UpdateGUI();        // vẽ lại ngay
    }
}
```

**`UpdateDayProfit()`** lọc lịch sử deal từ 00:00 hôm nay theo `DEAL_SYMBOL` (không filter magic):
- Bao gồm: tất cả deal `DEAL_ENTRY_OUT` / `DEAL_ENTRY_OUT_BY` trên symbol này
- **Không filter DEAL_MAGIC** vì lệnh đóng thủ công qua MT5 terminal có `DEAL_MAGIC=0`

## Object Prefix

Tất cả GUI object dùng prefix `"RTB_"` (biến `GUI`):
- `RTB_BG` — background panel
- `RTB_T`, `RTB_Tim`, `RTB_Sig`, ... — các label
- `RTB_TrailBuy`, `RTB_TrailSell` — đường trail (DrawHLine)

`RemoveGUI()` = `ObjectsDeleteAll(0, "RTB_")` — dọn sạch khi EA thoát.
