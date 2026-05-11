---
description: Rich Trading Bot order trimming and hedging — apply when working with CheckTrimming, InpTrimEnable, InpTrimHedge, partial trim, day profit trim, or drawdown recovery logic.
globs: ["Custumer/RichTradingBot/**"]
alwaysApply: false
---

# Rich Trading Bot — Cơ Chế Tỉa Lệnh (Order Trimming)

Dùng để giảm drawdown khi rổ lệnh lớn, bằng cách đóng có chọn lọc.

## Tham Số

| Tham số | Mặc định | Mô tả |
| :--- | :--- | :--- |
| `InpTrimEnable` | false | Bật/tắt Tỉa lệnh |
| `InpTrimHedge` | false | Tỉa chéo: đóng lệnh lời + lệnh lỗ cùng lúc |
| `InpTrimTrigger` | 5 | Kích hoạt khi **tổng** số lệnh ≥ X |
| `InpTrimTarget` | 10.0 | Mục tiêu lợi nhuận ròng sau tỉa ($) |
| `InpPartialTrim` | false | Bật Tỉa Một Phần theo DD% |
| `InpPartialTrimDD` | 20.0 | Ngưỡng DD% kích hoạt Tỉa Một Phần |
| `InpTrimByDayProfit` | false | Tỉa khi lãi ngày > \|lỗ lệnh tệ nhất\| |

## Thứ Tự Ưu Tiên Thực Thi

Mỗi lần `CheckTrimming()` chạy, chỉ thực hiện **1 hành động** (return sau khi xử lý):

1. **Partial Trim** *(ưu tiên cao nhất)*
   - Điều kiện: `InpPartialTrim=true` **VÀ** DD% hiện tại > `InpPartialTrimDD`
   - Hành động: đóng lệnh có floating loss lớn nhất (`WorstTicket()`)

2. **Day Profit Trim**
   - Điều kiện: `InpTrimByDayProfit=true` **VÀ** `DayProfit > |worstFloatLoss|`
   - Hành động: đóng lệnh tệ nhất (lấy lãi ngày bù lỗ floating)

3. **Hedge Trim** *(tỉa chéo)*
   - Điều kiện: `InpTrimHedge=true` **VÀ** `(bestProfit + worstProfit) >= InpTrimTarget`
   - Hành động: đóng **cả** lệnh tốt nhất lẫn lệnh tệ nhất cùng lúc

4. **Target Trim** *(tỉa theo profit)*
   - Điều kiện: `totalFloatProfit >= InpTrimTarget`
   - Hành động: đóng lệnh tệ nhất nếu phần còn lại vẫn ≥ target

## Helper Functions

- `WorstTicket()` → ticket của lệnh có floating profit âm nhất
- `BestTicket()` → ticket của lệnh có floating profit dương nhất
- `FloatProfit(posType)` → tổng profit + swap của tất cả lệnh (hoặc theo type)
