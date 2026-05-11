---
description: Rich Trading Bot exit logic — apply when working with CheckExit, InpCloseProfit, InpCloseLoss, InpClosePerPips, stealth TP/SL, DCA code exit, or basket close conditions.
globs: ["Custumer/RichTradingBot/**"]
alwaysApply: false
---

# Rich Trading Bot — Đóng Lệnh & Thoát Vị Thế (Exit Logic)

## Tham Số Basket Exit

| Tham số | Mặc định | Mô tả |
| :--- | :--- | :--- |
| `InpCloseProfit` | 0 | Đóng **toàn bộ** khi tổng floating lãi ≥ $ (0 = tắt) |
| `InpCloseLoss` | 0 | Đóng **toàn bộ** khi tổng floating lỗ ≥ $ (0 = tắt) |
| `InpClosePerPips` | 0 | Đóng **từng lệnh riêng** khi lãi ≥ X points (0 = tắt) |

## Thứ Tự Xử Lý Trong `CheckExit()`

### Section 1 — Per-Position ClosePerPips
- Kiểm tra mỗi lệnh riêng lẻ
- Đóng nếu `profitPts >= InpClosePerPips`
- Chạy bất kể Stealth Mode

### Section 2a — DCA Code TP/SL *(luôn chạy)*
Đọc comment `"RTB|tp|sl"` của từng lệnh DCA, đóng khi chạm mức:
```
BUY DCA:  bid >= openPrice + useTP * point  → đóng lấy lãi
          bid <= openPrice - useSL * point  → đóng cắt lỗ
SELL DCA: ask <= openPrice - useTP * point  → đóng lấy lãi
          ask >= openPrice + useSL * point  → đóng cắt lỗ
```
> Đây là safety net — DCA đã có server TP/SL từ `OpenOrder()`, section này xử lý nếu server TP bị miss.

### Section 2b — Stealth TP/SL lệnh gốc *(chỉ khi `InpStealthMode=true`)*
- Chỉ xử lý lệnh có comment `"RTB|0|0"` (lệnh gốc và pyramiding)
- Dùng `InpTP_Points` và `InpSL_Points` để đóng bằng code

### Section 3 — Basket Profit Target
```
FloatProfit() >= InpCloseProfit → CloseAll()
```

### Section 4 — Basket Loss Cut
```
FloatProfit() <= -InpCloseLoss → CloseAll()
```

## Tần Suất Chạy

| Điều kiện | `CheckExit()` chạy ở đâu |
| :--- | :--- |
| `InpStealthMode=true` | `OnTick()` — mỗi tick, tần suất cao nhất |
| `InpStealthMode=false` | `OnTimer()` — mỗi 1 giây |

> Khi Stealth OFF: DCA có server TP/SL (broker xử lý chính xác). Section 2a là safety net 1 giây.
> Khi Stealth ON: Section 2b chạy mỗi tick thay thế server TP/SL cho lệnh gốc.
