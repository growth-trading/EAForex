---
description: Rich Trading Bot input parameters — apply when modifying, explaining, or adding settings (InpLotSize, InpMagic, InpMaxBuy, InpSignalTF, stealth mode, TP/SL flags, etc.)
globs: ["Custumer/RichTradingBot/**"]
alwaysApply: false
---

# Rich Trading Bot — Thông Số Cài Đặt

## Cài Đặt Cơ Bản

| Tham số | Kiểu | Mặc định | Mô tả |
| :--- | :--- | :--- | :--- |
| `InpLotSize` | double | 0.01 | Lots ban đầu cho mỗi lệnh gốc |
| `InpUseTakeProfit` | bool | true | Bật server TP cho lệnh gốc và pyramiding |
| `InpUseStopLoss` | bool | false | Bật server SL cho lệnh gốc và pyramiding |
| `InpStealthMode` | bool | false | Ẩn TP/SL trên chart — bot chốt bằng code mỗi tick |
| `InpOrderDelay` | int | 5 | Thời gian tối thiểu giữa 2 lệnh bất kỳ (giây) |
| `InpTP_Points` | double | 3000 | TP lệnh gốc (points) |
| `InpSL_Points` | double | 0 | SL lệnh gốc (points, 0 = tắt) |
| `InpMagic` | ulong | 202601 | Magic Number phân biệt lệnh của EA |

## Bộ Lọc Chung

| Tham số | Mặc định | Mô tả |
| :--- | :--- | :--- |
| `InpMaxBuy` | 10 | Tổng số lệnh Buy tối đa (gộp gốc + DCA + pyramiding) |
| `InpMaxSell` | 10 | Tổng số lệnh Sell tối đa |
| `InpSignalTF` | H1 | Khung thời gian quét tín hiệu (M1 đến D1) |

## Hành Vi TP/SL Theo Loại Lệnh

| Loại lệnh | `InpUseTakeProfit=true` | `InpUseTakeProfit=false` |
| :--- | :--- | :--- |
| Gốc / Pyramiding | Server TP đặt trên broker | Không có server TP; Stealth Mode cần bật để đóng bằng code |
| DCA | Server TP **luôn** đặt nếu `DCA_TP > 0` | Server TP **luôn** đặt nếu `DCA_TP > 0` |

> **DCA bỏ qua `InpUseTakeProfit`:** DCA orders luôn đặt server TP/SL độc lập với flag toàn cục để đảm bảo thoát chính xác ngay cả khi thị trường biến động nhanh.

## Stealth Mode

- **OFF (mặc định):** TP/SL hiển thị trên chart broker; broker tự đóng lệnh khi chạm mức.
- **ON:** Không đặt TP/SL trên server; `CheckExit()` chạy mỗi tick kiểm tra và đóng bằng code — tránh bị sàn "quét" stop.
