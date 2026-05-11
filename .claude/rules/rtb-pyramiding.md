---
description: Rich Trading Bot pyramiding (nhồi dương) — apply when working with CheckPyramiding, InpPyraEnable, PyraCount, or add-to-winner logic.
globs: ["Custumer/RichTradingBot/**"]
alwaysApply: false
---

# Rich Trading Bot — Chiến Lược Nhồi Dương (Pyramiding)

Mở thêm lệnh **cùng chiều** khi lệnh hiện tại đang **có lãi** (giá đi đúng hướng).
Ngược với DCA (mở khi lỗ), Pyramiding tận dụng đà giá để gia tăng vị thế.

## Tham Số

| Tham số | Mặc định | Mô tả |
| :--- | :--- | :--- |
| `InpPyraEnable` | false | Bật/tắt Pyramiding |
| `InpPyraDist` | 500 | Khoảng cách nhồi (points) kể từ lệnh **cuối** cùng chiều |
| `InpPyraMaxLevels` | 8 | Số tầng nhồi tối đa |
| `InpPyraLotMult` | 1.0 | Hệ số Lot nhồi (× `InpLotSize`) |

## Điều Kiện Kích Hoạt

```
BUY Pyra:  (bid - lastOpenPrice) >= InpPyraDist * point  [giá tăng đủ từ lệnh cuối]
SELL Pyra: (lastOpenPrice - ask) >= InpPyraDist * point  [giá giảm đủ từ lệnh cuối]
```

## Hành Vi

- Chạy trong `OnTimer()` (mỗi giây), sau DCA.
- Pyramiding order dùng cùng `InpTP_Points` / `InpSL_Points` như lệnh gốc.
- Comment lệnh pyramiding: `"RTB|0|0"` (giống lệnh gốc).
- Bộ đếm `PyraCountBuy` / `PyraCountSell` theo dõi số tầng đã nhồi.
- **Auto reset:** Khi không còn lệnh cùng chiều (`CountBuy()==0` hay `CountSell()==0`), `PyraCount` và `TrailBuy/Sell` tự reset về 0 trong `OnTimer()`.

## Lưu Ý Tương Tác

- Pyramiding và DCA **cùng dùng** `LastOrderTime` → nếu DCA vừa mở, pyramiding phải chờ `InpOrderDelay` giây.
- `InpMaxBuy` / `InpMaxSell` giới hạn tổng lệnh gộp cả pyramiding.
