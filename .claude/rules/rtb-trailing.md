---
description: Rich Trading Bot trailing stop — apply when working with CheckTrailing, InpTrailEnable, TRAIL_BASKET, TRAIL_SINGLE, TrailBuy/TrailSell state, or moving stop loss logic.
globs: ["Custumer/RichTradingBot/**"]
alwaysApply: false
---

# Rich Trading Bot — Chế Độ Trailing Stop

## Tham Số

| Tham số | Mặc định | Mô tả |
| :--- | :--- | :--- |
| `InpTrailEnable` | false | Bật/tắt Trailing |
| `InpTrailMode` | `TRAIL_BASKET` | `Basket` = trail theo rổ lệnh \| `Single` = trail từng lệnh |
| `InpTrailMinOrds` | 1 | Số lệnh tối thiểu để bắt đầu trail |
| `InpTrailActivate` | 500 | Giá phải chạy dương ≥ X points mới bắt đầu trail |
| `InpTrailStep` | 200 | Bước nhảy tối thiểu để dời SL (points) |
| `InpTrailInit` | 300 | SL đặt cách giá hiện tại X points khi lần đầu trail |
| `InpTrailShowLine` | true | Vẽ đường trail (nét đứt) trên chart |

## Chế Độ Basket (`TRAIL_BASKET`)

Trail dựa trên **giá trung bình có trọng số** (`AvgOpenPrice`) của tất cả lệnh cùng chiều.

**BUY Basket:**
```
Kích hoạt khi: bid - avgBuyPrice >= InpTrailActivate * point
newSL = bid - InpTrailInit * point
Dời SL khi: newSL >= TrailBuy + InpTrailStep * point
→ Áp dụng newSL cho TẤT CẢ lệnh Buy cùng lúc
```

**SELL Basket:**
```
Kích hoạt khi: avgSellPrice - ask >= InpTrailActivate * point
newSL = ask + InpTrailInit * point
Dời SL khi: newSL <= TrailSell - InpTrailStep * point
→ Áp dụng newSL cho TẤT CẢ lệnh Sell cùng lúc
```

## Chế Độ Single (`TRAIL_SINGLE`)

Trail độc lập cho **từng lệnh** dựa trên giá mở lệnh đó:

```
BUY:  profitPts = (bid - openPrice) / point
      Kích hoạt khi profitPts >= InpTrailActivate
      newSL = bid - InpTrailInit * point
      Dời khi newSL >= curSL + InpTrailStep * point

SELL: profitPts = (openPrice - ask) / point
      Kích hoạt khi profitPts >= InpTrailActivate
      newSL = ask + InpTrailInit * point
      Dời khi newSL <= curSL - InpTrailStep * point
```

## State & Reset

- `TrailBuy` / `TrailSell`: lưu mức SL trail hiện tại cho từng hướng.
- **Auto reset:** Khi `CountBuy()==0` → `TrailBuy=0`; khi `CountSell()==0` → `TrailSell=0`.
- `ApplyTrailToPos(tk, posType, newSL)`: chỉ dời SL nếu newSL tốt hơn curSL (BUY: cao hơn; SELL: thấp hơn).
