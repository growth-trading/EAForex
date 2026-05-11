# Tài Liệu Kỹ Thuật: Rich Trading Bot (MQL5)

## I. Tổng Quan (General Overview)
- **Tên Bot:** Rich Trading Bot
- **Nền tảng:** MetaTrader 5 (MQL5)
- **Cặp tiền ưu tiên:** XAUUSD (Vàng)
- **Khung thời gian:** Đa khung (H1, M15, M1...)

## II. Thông Số Cài Đặt Cơ Bản (Base Settings)
- **Lots ban đầu:** 0.01 (Mặc định)
- **Chế độ TP/SL:** - `Use_TP`: Boolean (True/False)
    - `Use_SL`: Boolean (True/False)
- **Ẩn TP/SL (Stealth Mode):** Nếu bật, các đường TP/SL sẽ không hiển thị trên biểu đồ để tránh bị sàn quét (chốt bằng lệnh xóa script).
- **Độ trễ mở lệnh (Order Delay):** X giây (Khoảng cách thời gian tối thiểu giữa 2 lần vào lệnh).

---

## III. Chiến Lược Vào Lệnh (Entry Logic)
Hệ thống cho phép bật/tắt riêng biệt các điều kiện và chọn hướng giao dịch (`Only Buy`, `Only Sell`, `Both`, `Either`).

### 1. Bộ lọc EMA (34 + 89)
- **BUY:** EMA34 cắt lên EMA89 HOẶC Giá hồi về chạm EMA34 sau đó bật lên.
- **SELL:** EMA34 cắt xuống EMA89 HOẶC Giá hồi về chạm EMA34 sau đó giảm tiếp.

### 2. BZ Zone Trader (Xác nhận nến)
- **BUY:** Xuất hiện cụm 3 cây nến **XANH LÁ** liên tiếp (Lực mua mạnh/Demand Zone).
- **SELL:** Xuất hiện cụm 3 cây nến **ĐỎ** liên tiếp (Lực bán mạnh/Supply Zone).
- **Nến XÁM:** Đứng ngoài (Thị trường tích lũy/Momentum yếu).

### 3. Ichimoku Kinko Hyo
- **Điều kiện BUY:**
    - Giá nằm TRÊN mây Kumo.
    - Đường Tenkan-sen cắt LÊN trên Kijun-sen.
    - Chikou Span nằm TRÊN giá tại thời điểm quá khứ.
- **Điều kiện SELL:**
    - Giá nằm DƯỚI mây Kumo.
    - Tenkan-sen cắt XUỐNG dưới Kijun-sen.
    - Chikou Span nằm DƯỚI giá tại thời điểm quá khứ.

### 4. Bollinger Bands (Đánh đảo chiều)
- **BUY:** Giá đóng cửa chạm hoặc vượt ra ngoài dải dưới (Lower Band).
- **SELL:** Giá đóng cửa chạm hoặc vượt ra ngoài dải trên (Upper Band).
### 5. Simulated (Vào lệnh ngay theo giá thị trường)
- Vào lệnh theo giá thị trường: Khi bật bot sẽ tùy theo chiến lược vào lệnh để tham gia thị trường ngay lập tức (không chờ tín hiệu kỹ thuật thêm).
- **Mục đích:** Dùng để test nhanh hoặc vào lệnh thủ công có hỗ trợ quản lý DCA/Trailing tự động.

---

## IV. Bộ Lọc Chung & Quản Lý Lệnh (Global Filters)
- **Giới hạn số lệnh:**
    - `Max_Buy_Orders`: Giới hạn tổng số lệnh Buy đang chạy.
    - `Max_Sell_Orders`: Giới hạn tổng số lệnh Sell đang chạy.
- **Timeframe Filter:** Cho phép chọn khung thời gian quét tín hiệu (M1 đến D1).

---

## V. Chiến Lược DCA (Martingale/Averaging)
Hỗ trợ 8 tầng DCA tùy chỉnh. 
- **Chế độ:** `Stop` (Dừng), `Step` (Cố định khoảng cách), `Step + TF` (Khoảng cách + Xác nhận nến).

| Tầng | Chế độ       | Hệ số nhân Lot | Số lệnh tối đa | Khoảng cách (Pips) | TP (Pips) | SL (Pips) |
| :--- | :---         | :---           | :---           | :---               | :---      | :---      |
| 1    | Step         | 1.0            | 2              | 1000               | Tùy chỉnh | Tùy chỉnh |
| 2    | Step         | Tùy chỉnh      | Tùy chỉnh      | Tùy chỉnh          | Tùy chỉnh | Tùy chỉnh |
| 3    | Step         | Tùy chỉnh      | Tùy chỉnh      | Tùy chỉnh          | Tùy chỉnh | Tùy chỉnh |
| 4    | Step         | Tùy chỉnh      | Tùy chỉnh      | Tùy chỉnh          | Tùy chỉnh | Tùy chỉnh |
| 5    | Step         | Tùy chỉnh      | Tùy chỉnh      | Tùy chỉnh          | Tùy chỉnh | Tùy chỉnh |
| 6    | Step         | Tùy chỉnh      | Tùy chỉnh      | Tùy chỉnh          | Tùy chỉnh | Tùy chỉnh |
| 7    | Step         | Tùy chỉnh      | Tùy chỉnh      | Tùy chỉnh          | Tùy chỉnh | Tùy chỉnh |
| 8    | Step/Stop    | Tùy chỉnh      | Tùy chỉnh      | Tùy chỉnh          | Tùy chỉnh | Tùy chỉnh |

*(Khoảng cách DCA được tính so với lệnh cuối cùng được vào. Mỗi tầng có thể đặt chế độ riêng: `Stop` = không mở thêm, `Step` = mở theo khoảng cách cố định, `Step + TF` = mở theo khoảng cách + xác nhận nến).*

---

## VI. Chiến Lược Nhồi Dương (Pyramiding)
- **Khoảng cách nhồi:** X pips (Khi lệnh hiện tại đang có lãi).
- **Số lệnh nhồi tối đa:** 8 tầng.
- **Khối lượng (Lots):** Tùy chỉnh cho từng tầng nhồi.

---

## VII. Cơ Chế Tỉa Lệnh (Order Trimming/Hedging)
Dùng để cứu tài khoản khi gặp Drawdown lớn.
- **Chế độ:** Tỉa cùng chiều hoặc Tỉa chéo (Hedging).
- **Điều kiện kích hoạt:** Khi số lệnh đạt ngưỡng `X`.
- **Logic:** - Lấy lợi nhuận của các lệnh dương để bù cho `N` lệnh âm lớn nhất.
    - Mục tiêu: Tổng lợi nhuận sau tỉa = `Sô tiền lời mong muốn`.
- **Tỉa một phần (Partial Trim):** - Kích hoạt khi % Âm trạng thái (Drawdown) > `20%`.
    - Tự động cắt bỏ lệnh âm lớn nhất.
- **Tỉa theo Lãi ngày:** Nếu `Bật`, bot kiểm tra tổng Profit đã chốt trong ngày. Nếu `Profit Ngày > Số âm của lệnh lớn nhất` -> Tự động cắt lệnh âm đó.

---

## VIII. Chế Độ Trailing (Trailing Stop)
- **Phân loại:** Trailing Tổng (Basket) hoặc Trailing Đơn từng lệnh.
- **Số lệnh kích hoạt:** Tối thiểu bao nhiêu lệnh thì bắt đầu Trail.
- **Pips kích hoạt:** Giá chạy dương `X` pips mới bắt đầu Trail.
- **Bước nhảy (Step):** Khoảng cách dịch chuyển SL.
- **Điểm đặt SL đầu tiên:** Cách giá hiện tại `Y` pips.
- **Hiển thị:** Vẽ đường Line Trail trên chart.

---

## IX. Đóng Lệnh & Thoát Vị Thế (Exit Logic)
- **Chốt lời tổng (Floating Profit):** Đóng toàn bộ khi tổng lãi trạng thái đạt số tiền `$`.
- **Cắt lỗ tổng (Floating Loss):** Đóng toàn bộ khi tổng lỗ trạng thái đạt số tiền `$`.
- **Đóng theo Pips:** (Optional) Đóng riêng lẻ từng lệnh khi đạt số pips quy định.

---

## X. Giao Diện (GUI)
- Hiển thị thông tin: 
    - Trạng thái Bot (ON/OFF)
    - Tổng Profit ngày
    - Tổng Profit tất cả các ngày
    - Tổng Profit Drawdown hiện tại
    - Phần trăm lợi nhuận theo vốn ban đầu
    - Local time(Ngày, tháng, năm, giờ)
    - Balance ban đầu
    - Drawdown lớn nhất trong toàn bộ thời gian
    - Spread
    - Tổng trạng thái hiện tại
    - Tổng trạng thái lệnh buy
    - Tổng lệnh buy
    - Tổng khối lượng buy (lot)
    - Tổng trạng thái lệnh sell
    - Tổng lệnh sell
    - Tổng khối lượng sell (lot)
    - Tín hiệu sử dụng (Đang dùng tín hiệu nào thì hiện thị tín hiệu đó)
