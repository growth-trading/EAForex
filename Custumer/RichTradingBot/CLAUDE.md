# Tài Liệu Kỹ Thuật: Rich Trading Bot (MQL5)

> **File code:** `RichTradingBot.cpp` (đổi extension thành `.mq5` khi nạp vào MetaEditor)
> **Phiên bản:** v1.0 | **Nền tảng:** MetaTrader 5

---

## I. Tổng Quan (General Overview)
- **Tên Bot:** Rich Trading Bot
- **Nền tảng:** MetaTrader 5 (MQL5)
- **Cặp tiền ưu tiên:** XAUUSD (Vàng)
- **Khung thời gian:** Đa khung (H1, M15, M1...)
- **Magic Number mặc định:** 202601

---

## II. Thông Số Cài Đặt Cơ Bản (Base Settings)

| Tham số            | Kiểu   | Mặc định | Mô tả                                        |
| :---               | :---   | :---     | :---                                         |    
| `InpLotSize`       | double | 0.01     | Lots ban đầu cho mỗi lệnh gốc                |
| `InpUseTakeProfit` | bool   | true     | Bật Take Profit                              |
| `InpUseStopLoss`   | bool   | false    | Bật Stop Loss                                |
| `InpStealthMode`   | bool   | false    | Ẩn TP/SL trên chart (chốt thủ công qua code) |
| `InpOrderDelay`    | int    |  5       | Thời gian tối thiểu giữa 2 lệnh (giây)       |
| `InpTP_Points`     | double | 3000     | TP lệnh gốc (points)                         |
| `InpSL_Points`     | double | 0        | SL lệnh gốc (points, 0 = tắt)                |
| `InpMagic`         | ulong  | 202601   | Magic Number phân biệt lệnh của EA           |

> **Stealth Mode:** Khi bật, TP/SL không đặt trên server — bot tự kiểm tra và đóng lệnh bằng code mỗi tick, tránh bị sàn quét.

---

## III. Chiến Lược Vào Lệnh (Entry Logic)

Hệ thống cho phép chọn **một** chiến lược tín hiệu (`InpSignalMode`) và **một** hướng giao dịch (`InpDirection`).

### Hướng giao dịch (`InpDirection`)

| Giá trị | Hành vi |
| :--- | :--- |
| `Both` | Cho phép cả BUY lẫn SELL theo tín hiệu |
| `Only Buy` | Chỉ vào BUY, bỏ qua tín hiệu SELL |
| `Only Sell` | Chỉ vào SELL, bỏ qua tín hiệu BUY |
| `Either` | Tương tự `Both` (dùng với Simulated) |

### 1. Bộ lọc EMA (34 + 89)
- **BUY:** EMA34 cắt lên EMA89 **HOẶC** giá hồi về chạm EMA34 (trong vùng `InpEMAPullbackPts` points) khi EMA34 > EMA89.
- **SELL:** EMA34 cắt xuống EMA89 **HOẶC** giá hồi về chạm EMA34 khi EMA34 < EMA89.

### 2. BZ Zone Trader (Xác nhận nến)
- **BUY:** 3 nến **XANH LÁ** liên tiếp (Demand Zone — lực mua mạnh).
- **SELL:** 3 nến **ĐỎ** liên tiếp (Supply Zone — lực bán mạnh).
- **Nến XÁM:** Đứng ngoài thị trường (momentum yếu).

### 3. Ichimoku Kinko Hyo
- **BUY:** Giá trên Kumo + Tenkan cắt lên Kijun + Chikou trên giá quá khứ (26 bar).
- **SELL:** Giá dưới Kumo + Tenkan cắt xuống Kijun + Chikou dưới giá quá khứ.

### 4. Bollinger Bands (Đánh đảo chiều)
- **BUY:** Giá đóng cửa chạm hoặc vượt dải **dưới** (Lower Band).
- **SELL:** Giá đóng cửa chạm hoặc vượt dải **trên** (Upper Band).

### 5. Simulated (Vào lệnh ngay theo giá thị trường)
- Vào lệnh ngay lập tức khi bot khởi động, không chờ tín hiệu kỹ thuật.
- **Mục đích:** Test nhanh hoặc vào lệnh thủ công có hỗ trợ DCA/Trailing/Trim tự động.
- **Hành vi theo hướng:**

| `InpDirection`    | Hành vi                                                                       |
| :---              | :---                                                                          |
| `Only Buy`        | Chỉ mở 1 lệnh BUY                                                             |
| `Only Sell`       | Chỉ mở 1 lệnh SELL                                                            |
| `Both` / `Either` | Mở đồng thời **cả BUY lẫn SELL**, mỗi hướng quản lý DCA/Trail/Trim độc lập    |

---

## IV. Bộ Lọc Chung & Quản Lý Lệnh (Global Filters)

| Tham số | Mặc định | Mô tả |
| :--- | :--- | :--- |
| `InpMaxBuy` | 10 | Tổng số lệnh Buy tối đa được phép mở |
| `InpMaxSell` | 10 | Tổng số lệnh Sell tối đa được phép mở |
| `InpSignalTF` | H1 | Khung thời gian quét tín hiệu (M1 đến D1) |

---

## V. Chiến Lược DCA (Martingale/Averaging)

Hỗ trợ **8 tầng DCA** tùy chỉnh độc lập. Khoảng cách tính từ **lệnh cuối cùng được mở** (không phải lệnh gốc).

### Chế độ DCA (`ENUM_DCA_MODE`)
- `Stop`: Dừng DCA tại tầng này.
- `Step`: Mở DCA khi giá cách lệnh cuối đủ `Dist` points.
- `Step + TF`: Như `Step` nhưng thêm xác nhận tín hiệu chiến lược hiện tại.

### Bảng cấu hình (mặc định)

| Tầng | Chế độ | Hệ số Lot | Số lệnh tối đa(*) | Khoảng cách (points) | TP (points) | SL (points) |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| 1 | Step | 1.5x | 2 | 1000 | 500 | 0 |
| 2 | Step | 2.0x | 2 | 1500 | 500 | 0 |
| 3 | Step | 2.5x | 2 | 2000 | 500 | 0 |
| 4 | Step | 3.0x | 2 | 2500 | 500 | 0 |
| 5 | Step | 3.5x | 2 | 3000 | 500 | 0 |
| 6 | Step | 4.0x | 2 | 3500 | 500 | 0 |
| 7 | Step | 5.0x | 2 | 4000 | 500 | 0 |
| 8 | Stop | 6.0x | 1 | 5000 | 500 | 0 |

> **(*) Số lệnh tối đa** = tổng số lệnh Buy (hoặc Sell) được phép tồn tại đồng thời khi đang ở tầng đó. Ví dụ: T1 MaxOrd=2 nghĩa là lệnh gốc (1) + 1 DCA = tối đa 2 lệnh trước khi chuyển sang T2.

---

## VI. Chiến Lược Nhồi Dương (Pyramiding)

Mở thêm lệnh **cùng chiều** khi lệnh hiện tại đang **có lãi** (giá đi đúng hướng).

| Tham số | Mặc định | Mô tả |
| :--- | :--- | :--- |
| `InpPyraEnable` | false | Bật/tắt Pyramiding |
| `InpPyraDist` | 500 | Khoảng cách nhồi (points) kể từ lệnh cuối |
| `InpPyraMaxLevels` | 8 | Số tầng nhồi tối đa |
| `InpPyraLotMult` | 1.0 | Hệ số Lot nhồi (nhân với `InpLotSize`) |

> Bot tự reset bộ đếm pyramiding khi tất cả lệnh cùng chiều đã đóng.

---

## VII. Cơ Chế Tỉa Lệnh (Order Trimming/Hedging)

Dùng để cứu tài khoản khi gặp Drawdown lớn.

| Tham số | Mặc định | Mô tả |
| :--- | :--- | :--- |
| `InpTrimEnable` | false | Bật/tắt Tỉa lệnh |
| `InpTrimHedge` | false | Tỉa chéo: đóng lệnh lời + lệnh lỗ cùng lúc (Hedging) |
| `InpTrimTrigger` | 5 | Kích hoạt khi tổng số lệnh ≥ X |
| `InpTrimTarget` | 10.0 | Mục tiêu lợi nhuận sau tỉa ($) |
| `InpPartialTrim` | false | Bật Tỉa Một Phần (theo DD%) |
| `InpPartialTrimDD` | 20.0 | DD% ngưỡng kích hoạt Tỉa Một Phần |
| `InpTrimByDayProfit` | false | Tỉa khi Lãi Ngày > \|Lỗ lệnh âm nhất\| |

### Logic ưu tiên thực thi (theo thứ tự):
1. **Partial Trim:** Nếu DD% hiện tại > `InpPartialTrimDD` → đóng lệnh lỗ nặng nhất.
2. **Day Profit Trim:** Nếu lãi ngày > |lỗ lệnh tệ nhất| → đóng lệnh đó.
3. **Hedge Trim:** Nếu (lợi nhuận lệnh tốt nhất + lỗ lệnh tệ nhất) ≥ `InpTrimTarget` → đóng cả hai.
4. **Target Trim:** Nếu tổng floating profit ≥ `InpTrimTarget` → đóng lệnh lỗ nặng nhất.

---

## VIII. Chế Độ Trailing (Trailing Stop)

| Tham số | Mặc định | Mô tả |
| :--- | :--- | :--- |
| `InpTrailEnable` | false | Bật/tắt Trailing |
| `InpTrailMode` | Basket | `Basket` = trail theo nhóm lệnh \| `Single` = trail từng lệnh |
| `InpTrailMinOrds` | 1 | Số lệnh tối thiểu để kích hoạt trail |
| `InpTrailActivate` | 500 | Giá phải chạy dương X points mới bắt đầu trail |
| `InpTrailStep` | 200 | Bước nhảy tối thiểu để dời SL (points) |
| `InpTrailInit` | 300 | SL đặt cách giá hiện tại X points khi bắt đầu trail |
| `InpTrailShowLine` | true | Vẽ đường trail (nét đứt) trên chart |

> **Basket mode:** Tính giá trung bình các lệnh cùng chiều. Khi giá vượt ngưỡng, dời SL toàn bộ nhóm đồng thời. Bot tự reset `TrailBuy`/`TrailSell` khi không còn lệnh.

---

## IX. Đóng Lệnh & Thoát Vị Thế (Exit Logic)

| Tham số | Mặc định | Mô tả |
| :--- | :--- | :--- |
| `InpCloseProfit` | 0 | Đóng toàn bộ khi tổng lãi trạng thái ≥ $ (0 = tắt) |
| `InpCloseLoss` | 0 | Đóng toàn bộ khi tổng lỗ trạng thái ≥ $ (0 = tắt) |
| `InpClosePerPips` | 0 | Đóng từng lệnh riêng khi lãi ≥ X points (0 = tắt) |

> Khi **Stealth Mode** bật: bot tự kiểm tra TP/SL mỗi tick dựa trên `InpTP_Points` / `InpSL_Points` và đóng lệnh bằng code thay vì đặt giá trên server.

---

## X. Giao Diện (GUI)

Panel tự động vẽ góc trên-trái màn hình, cập nhật mỗi giây.

| Thông tin hiển thị | Mô tả |
| :--- | :--- |
| Tiêu đề | `RICH TRADING BOT v1.0` |
| Time | Giờ địa phương (năm/tháng/ngày giờ:phút:giây) |
| Signal | Chiến lược tín hiệu đang dùng |
| **Direct** | **Hướng giao dịch đang sử dụng** (màu: xanh lá=Buy, đỏ=Sell, xanh dương=Both/Either) |
| Balance | Số dư tài khoản hiện tại ($) |
| Initial | Balance ban đầu khi khởi động EA ($) |
| Day P/L | Tổng lợi nhuận đã chốt trong ngày ($) |
| Float | Tổng lãi/lỗ trạng thái + phần trăm so với Balance ban đầu |
| DD Now | Drawdown hiện tại (%) |
| DD Max | Drawdown lớn nhất trong toàn bộ thời gian chạy (%) |
| Spread | Spread hiện tại (points) |
| Buy P/L | Tổng lãi/lỗ trạng thái các lệnh Buy ($) |
| Buy Ord | Số lệnh Buy đang mở + tổng khối lượng (lot) |
| Sell P/L | Tổng lãi/lỗ trạng thái các lệnh Sell ($) |
| Sell Ord | Số lệnh Sell đang mở + tổng khối lượng (lot) |
| Total | Tổng số lệnh đang mở (Buy + Sell) |

### Màu sắc trạng thái
- **Xanh lá** (`LimeGreen`): Lãi dương / Buy Only direction
- **Đỏ** (`Tomato`): Lỗ / Sell Only direction / DD cảnh báo
- **Cam** (`OrangeRed`): DD Now > 15%
- **Vàng** (`Yellow`): Tên chiến lược tín hiệu
- **Xanh dương** (`DodgerBlue`): Hướng Both / Either
