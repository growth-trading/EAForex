//+------------------------------------------------------------------+
//|                                        RichTradingBot.mq5        |
//|                        Rich Trading Bot v1.0 (MQL5)              |
//|                  Spec: Custumer/CLAUDE.md                        |
//+------------------------------------------------------------------+
#property copyright "Rich Trading Bot v1.0"
#property version   "1.00"
#property strict

#include <Trade\Trade.mqh>
#include <Trade\PositionInfo.mqh>

CTrade    Trade;

//+------------------------------------------------------------------+
//| ENUMS                                                            |
//+------------------------------------------------------------------+
enum ENUM_SIGNAL_MODE  { SIG_EMA, SIG_BZ_ZONE, SIG_ICHIMOKU, SIG_BB, SIG_SIMULATED };
enum ENUM_DIRECTION    { DIR_BOTH, DIR_ONLY_BUY, DIR_ONLY_SELL, DIR_EITHER };
enum ENUM_DCA_MODE     { DCA_STOP, DCA_STEP, DCA_STEP_TF };
enum ENUM_TRAIL_MODE   { TRAIL_BASKET, TRAIL_SINGLE };

//+------------------------------------------------------------------+
//| INPUT: BASE SETTINGS                                             |
//+------------------------------------------------------------------+
sinput string  _s0         = "══════ CÀI ĐẶT CƠ BẢN ══════"; //
input  double  InpLotSize      = 0.01;    // Lots ban đầu
input  bool    InpUseTakeProfit= true;    // Dùng Take Profit (Use_TP)
input  bool    InpUseStopLoss  = false;   // Dùng Stop Loss (Use_SL)
input  bool    InpStealthMode  = false;   // Ẩn TP/SL trên chart (Stealth Mode)
input  int     InpOrderDelay   = 5;       // Độ trễ mở lệnh (giây)
input  ulong   InpMagic        = 202601;  // Magic Number
input  double  InpTP_Points    = 3000.0;  // TP mỗi lệnh (points)
input  double  InpSL_Points    = 0.0;     // SL mỗi lệnh (points, 0=tắt)

//+------------------------------------------------------------------+
//| INPUT: ENTRY SIGNAL                                              |
//+------------------------------------------------------------------+
sinput string  _s1         = "══════ TÍN HIỆU VÀO LỆNH ══════"; //
input  ENUM_SIGNAL_MODE InpSignalMode = SIG_EMA;      // Chiến lược tín hiệu
input  ENUM_DIRECTION   InpDirection  = DIR_BOTH;     // Hướng giao dịch
input  ENUM_TIMEFRAMES  InpSignalTF   = PERIOD_H1;    // Khung thời gian tín hiệu

//+------------------------------------------------------------------+
//| INPUT: EMA                                                       |
//+------------------------------------------------------------------+
sinput string  _s2         = "══════ EMA FILTER (34+89) ══════"; //
input  int     InpEMAFast  = 34;   // EMA nhanh
input  int     InpEMASlow  = 89;   // EMA chậm
input  double  InpEMAPullbackPts = 100.0; // Khoảng pullback về EMA34 (points)

//+------------------------------------------------------------------+
//| INPUT: BOLLINGER BANDS                                           |
//+------------------------------------------------------------------+
sinput string  _s3         = "══════ BOLLINGER BANDS ══════"; //
input  int     InpBBPeriod = 20;   // BB Period
input  double  InpBBDev    = 2.0;  // BB Deviation

//+------------------------------------------------------------------+
//| INPUT: ICHIMOKU                                                  |
//+------------------------------------------------------------------+
sinput string  _s4         = "══════ ICHIMOKU ══════"; //
input  int     InpIchiTenkan = 9;   // Tenkan-sen
input  int     InpIchiKijun  = 26;  // Kijun-sen
input  int     InpIchiSenkou = 52;  // Senkou Span B

//+------------------------------------------------------------------+
//| INPUT: GLOBAL FILTERS                                            |
//+------------------------------------------------------------------+
sinput string  _s5         = "══════ BỘ LỌC CHUNG ══════"; //
input  int     InpMaxBuy   = 10;   // Số lệnh Buy tối đa
input  int     InpMaxSell  = 10;   // Số lệnh Sell tối đa

//+------------------------------------------------------------------+
//| INPUT: DCA (8 LEVELS)                                            |
//+------------------------------------------------------------------+
sinput string  _s6         = "══════ DCA - TẦNG 1 ══════"; //
input  ENUM_DCA_MODE InpDCA1Mode = DCA_STEP; // DCA T1: Chế độ
input  double  InpDCA1Mult = 1.5;    // DCA T1: Hệ số Lot
input  int     InpDCA1Max  = 2;      // DCA T1: Max lệnh tổng tại tầng này
input  double  InpDCA1Dist = 1000.0; // DCA T1: Khoảng cách (points)
input  double  InpDCA1TP   = 500.0;  // DCA T1: TP (points)
input  double  InpDCA1SL   = 0.0;    // DCA T1: SL (points, 0=tắt)

sinput string  _s7         = "══════ DCA - TẦNG 2 ══════"; //
input  ENUM_DCA_MODE InpDCA2Mode = DCA_STEP;
input  double  InpDCA2Mult = 2.0;
input  int     InpDCA2Max  = 2;
input  double  InpDCA2Dist = 1500.0;
input  double  InpDCA2TP   = 500.0;
input  double  InpDCA2SL   = 0.0;

sinput string  _s8         = "══════ DCA - TẦNG 3 ══════"; //
input  ENUM_DCA_MODE InpDCA3Mode = DCA_STEP;
input  double  InpDCA3Mult = 2.5;
input  int     InpDCA3Max  = 2;
input  double  InpDCA3Dist = 2000.0;
input  double  InpDCA3TP   = 500.0;
input  double  InpDCA3SL   = 0.0;

sinput string  _s9         = "══════ DCA - TẦNG 4 ══════"; //
input  ENUM_DCA_MODE InpDCA4Mode = DCA_STEP;
input  double  InpDCA4Mult = 3.0;
input  int     InpDCA4Max  = 2;
input  double  InpDCA4Dist = 2500.0;
input  double  InpDCA4TP   = 500.0;
input  double  InpDCA4SL   = 0.0;

sinput string  _s10        = "══════ DCA - TẦNG 5 ══════"; //
input  ENUM_DCA_MODE InpDCA5Mode = DCA_STEP;
input  double  InpDCA5Mult = 3.5;
input  int     InpDCA5Max  = 2;
input  double  InpDCA5Dist = 3000.0;
input  double  InpDCA5TP   = 500.0;
input  double  InpDCA5SL   = 0.0;

sinput string  _s11        = "══════ DCA - TẦNG 6 ══════"; //
input  ENUM_DCA_MODE InpDCA6Mode = DCA_STEP;
input  double  InpDCA6Mult = 4.0;
input  int     InpDCA6Max  = 2;
input  double  InpDCA6Dist = 3500.0;
input  double  InpDCA6TP   = 500.0;
input  double  InpDCA6SL   = 0.0;

sinput string  _s12        = "══════ DCA - TẦNG 7 ══════"; //
input  ENUM_DCA_MODE InpDCA7Mode = DCA_STEP;
input  double  InpDCA7Mult = 5.0;
input  int     InpDCA7Max  = 2;
input  double  InpDCA7Dist = 4000.0;
input  double  InpDCA7TP   = 500.0;
input  double  InpDCA7SL   = 0.0;

sinput string  _s13        = "══════ DCA - TẦNG 8 ══════"; //
input  ENUM_DCA_MODE InpDCA8Mode = DCA_STOP;
input  double  InpDCA8Mult = 6.0;
input  int     InpDCA8Max  = 1;
input  double  InpDCA8Dist = 5000.0;
input  double  InpDCA8TP   = 500.0;
input  double  InpDCA8SL   = 0.0;

//+------------------------------------------------------------------+
//| INPUT: PYRAMIDING                                                |
//+------------------------------------------------------------------+
sinput string  _s14        = "══════ NHỒI DƯƠNG (PYRAMIDING) ══════"; //
input  bool    InpPyraEnable    = false;  // Bật Nhồi Dương
input  double  InpPyraDist      = 500.0;  // Khoảng cách nhồi (points)
input  int     InpPyraMaxLevels = 8;      // Số tầng nhồi tối đa
input  double  InpPyraLotMult   = 1.0;    // Hệ số Lot nhồi

//+------------------------------------------------------------------+
//| INPUT: ORDER TRIMMING                                            |
//+------------------------------------------------------------------+
sinput string  _s15        = "══════ TỈA LỆNH (TRIMMING) ══════"; //
input  bool    InpTrimEnable     = false;  // Bật Tỉa Lệnh
input  bool    InpTrimHedge      = false;  // Tỉa chéo (Hedging mode)
input  int     InpTrimTrigger    = 5;      // Kích hoạt khi số lệnh >= X
input  double  InpTrimTarget     = 10.0;   // Mục tiêu lợi nhuận sau tỉa ($)
input  bool    InpPartialTrim    = false;  // Bật Tỉa Một Phần
input  double  InpPartialTrimDD  = 20.0;   // Kích hoạt khi DD% >
input  bool    InpTrimByDayProfit= false;  // Tỉa theo Lãi Ngày

//+------------------------------------------------------------------+
//| INPUT: TRAILING STOP                                             |
//+------------------------------------------------------------------+
sinput string  _s16        = "══════ TRAILING STOP ══════"; //
input  bool          InpTrailEnable   = false;        // Bật Trailing
input  ENUM_TRAIL_MODE InpTrailMode   = TRAIL_BASKET; // Basket hoặc Đơn lẻ
input  int           InpTrailMinOrds  = 1;            // Số lệnh tối thiểu kích hoạt
input  double        InpTrailActivate = 500.0;        // Points kích hoạt Trail
input  double        InpTrailStep     = 200.0;        // Bước nhảy SL (points)
input  double        InpTrailInit     = 300.0;        // SL đầu tiên cách giá (points)
input  bool          InpTrailShowLine = true;         // Vẽ đường Trail

//+------------------------------------------------------------------+
//| INPUT: EXIT LOGIC                                                |
//+------------------------------------------------------------------+
sinput string  _s17        = "══════ ĐÓNG LỆNH TỔNG ══════"; //
input  double  InpCloseProfit  = 0.0;  // Chốt lời khi tổng lãi đạt ($, 0=tắt)
input  double  InpCloseLoss    = 0.0;  // Cắt lỗ khi tổng lỗ đạt ($, 0=tắt)
input  double  InpClosePerPips = 0.0;  // Đóng từng lệnh khi đạt (points, 0=tắt)

//+------------------------------------------------------------------+
//| GLOBAL STATE                                                     |
//+------------------------------------------------------------------+
int      hEMAFast   = INVALID_HANDLE;
int      hEMASlow   = INVALID_HANDLE;
int      hBB        = INVALID_HANDLE;
int      hIchi      = INVALID_HANDLE;

// DCA config arrays (index 0-7 = level 1-8)
ENUM_DCA_MODE DCA_Mode[8];
double        DCA_Mult[8];
int           DCA_MaxOrd[8];
double        DCA_Dist[8];
double        DCA_TP[8];
double        DCA_SL[8];

datetime LastOrderTime  = 0;
double   InitBalance    = 0.0;
double   MaxDrawdownPct = 0.0;
double   DayProfit      = 0.0;
int      LastDay        = -1;

// Basket trail levels
double   TrailBuy  = 0.0;
double   TrailSell = 0.0;

// Pyramiding order counts per direction
int      PyraCountBuy  = 0;
int      PyraCountSell = 0;
datetime PyraLastBuy   = 0;
datetime PyraLastSell  = 0;

// GUI prefix
const string GUI = "RTB_";

//+------------------------------------------------------------------+
//| UTILITY FUNCTIONS                                                |
//+------------------------------------------------------------------+

int CountPos(int posType) {
    int n = 0;
    for(int i = PositionsTotal()-1; i >= 0; i--) {
        ulong tk = PositionGetTicket(i);
        if(!PositionSelectByTicket(tk)) continue;
        if(PositionGetInteger(POSITION_MAGIC) != (long)InpMagic) continue;
        if(PositionGetString(POSITION_SYMBOL) != _Symbol) continue;
        if((int)PositionGetInteger(POSITION_TYPE) == posType) n++;
    }
    return n;
}

int CountBuy()  { return CountPos(POSITION_TYPE_BUY);  }
int CountSell() { return CountPos(POSITION_TYPE_SELL); }
int CountAll()  { return CountBuy() + CountSell(); }

double FloatProfit(int posType = -1) {
    double p = 0;
    for(int i = PositionsTotal()-1; i >= 0; i--) {
        ulong tk = PositionGetTicket(i);
        if(!PositionSelectByTicket(tk)) continue;
        if(PositionGetInteger(POSITION_MAGIC) != (long)InpMagic) continue;
        if(PositionGetString(POSITION_SYMBOL) != _Symbol) continue;
        if(posType >= 0 && (int)PositionGetInteger(POSITION_TYPE) != posType) continue;
        p += PositionGetDouble(POSITION_PROFIT) + PositionGetDouble(POSITION_SWAP);
    }
    return p;
}

double TotalLot(int posType) {
    double lot = 0;
    for(int i = PositionsTotal()-1; i >= 0; i--) {
        ulong tk = PositionGetTicket(i);
        if(!PositionSelectByTicket(tk)) continue;
        if(PositionGetInteger(POSITION_MAGIC) != (long)InpMagic) continue;
        if(PositionGetString(POSITION_SYMBOL) != _Symbol) continue;
        if((int)PositionGetInteger(POSITION_TYPE) != posType) continue;
        lot += PositionGetDouble(POSITION_VOLUME);
    }
    return lot;
}

// Last opened price for a direction (most recently opened position)
double LastOpenPrice(int posType) {
    double   price = 0;
    datetime latest = 0;
    for(int i = PositionsTotal()-1; i >= 0; i--) {
        ulong tk = PositionGetTicket(i);
        if(!PositionSelectByTicket(tk)) continue;
        if(PositionGetInteger(POSITION_MAGIC) != (long)InpMagic) continue;
        if(PositionGetString(POSITION_SYMBOL) != _Symbol) continue;
        if((int)PositionGetInteger(POSITION_TYPE) != posType) continue;
        datetime t = (datetime)PositionGetInteger(POSITION_TIME);
        if(t > latest) { latest = t; price = PositionGetDouble(POSITION_PRICE_OPEN); }
    }
    return price;
}

// Weighted average open price
double AvgOpenPrice(int posType) {
    double totalLot = 0, totalCost = 0;
    for(int i = PositionsTotal()-1; i >= 0; i--) {
        ulong tk = PositionGetTicket(i);
        if(!PositionSelectByTicket(tk)) continue;
        if(PositionGetInteger(POSITION_MAGIC) != (long)InpMagic) continue;
        if(PositionGetString(POSITION_SYMBOL) != _Symbol) continue;
        if((int)PositionGetInteger(POSITION_TYPE) != posType) continue;
        double lot   = PositionGetDouble(POSITION_VOLUME);
        double price = PositionGetDouble(POSITION_PRICE_OPEN);
        totalLot  += lot;
        totalCost += lot * price;
    }
    return (totalLot > 0) ? totalCost / totalLot : 0;
}

// Ticket of position with worst (most negative) floating profit
ulong WorstTicket() {
    ulong  tk_worst = 0;
    double worst    = 0;
    for(int i = PositionsTotal()-1; i >= 0; i--) {
        ulong tk = PositionGetTicket(i);
        if(!PositionSelectByTicket(tk)) continue;
        if(PositionGetInteger(POSITION_MAGIC) != (long)InpMagic) continue;
        if(PositionGetString(POSITION_SYMBOL) != _Symbol) continue;
        double p = PositionGetDouble(POSITION_PROFIT);
        if(p < worst) { worst = p; tk_worst = tk; }
    }
    return tk_worst;
}

// Best (most positive) ticket
ulong BestTicket() {
    ulong  tk_best = 0;
    double best    = 0;
    for(int i = PositionsTotal()-1; i >= 0; i--) {
        ulong tk = PositionGetTicket(i);
        if(!PositionSelectByTicket(tk)) continue;
        if(PositionGetInteger(POSITION_MAGIC) != (long)InpMagic) continue;
        if(PositionGetString(POSITION_SYMBOL) != _Symbol) continue;
        double p = PositionGetDouble(POSITION_PROFIT);
        if(p > best) { best = p; tk_best = tk; }
    }
    return tk_best;
}

void CloseAll(int posType = -1) {
    for(int i = PositionsTotal()-1; i >= 0; i--) {
        ulong tk = PositionGetTicket(i);
        if(!PositionSelectByTicket(tk)) continue;
        if(PositionGetInteger(POSITION_MAGIC) != (long)InpMagic) continue;
        if(PositionGetString(POSITION_SYMBOL) != _Symbol) continue;
        if(posType >= 0 && (int)PositionGetInteger(POSITION_TYPE) != posType) continue;
        Trade.PositionClose(tk);
    }
}

double NormLot(double lot) {
    double minL  = SymbolInfoDouble(_Symbol, SYMBOL_VOLUME_MIN);
    double maxL  = SymbolInfoDouble(_Symbol, SYMBOL_VOLUME_MAX);
    double stepL = SymbolInfoDouble(_Symbol, SYMBOL_VOLUME_STEP);
    lot = MathRound(lot / stepL) * stepL;
    return MathMax(minL, MathMin(maxL, lot));
}

//+------------------------------------------------------------------+
//| OPEN ORDER                                                       |
//+------------------------------------------------------------------+
bool OpenOrder(int ordType, double lot, double tp_pts = 0, double sl_pts = 0) {
    double ask   = SymbolInfoDouble(_Symbol, SYMBOL_ASK);
    double bid   = SymbolInfoDouble(_Symbol, SYMBOL_BID);
    double point = SymbolInfoDouble(_Symbol, SYMBOL_POINT);

    double price, tp = 0, sl = 0;

    if(ordType == ORDER_TYPE_BUY) {
        price = ask;
        if(InpUseTakeProfit && tp_pts > 0 && !InpStealthMode)
            tp = NormalizeDouble(price + tp_pts * point, _Digits);
        if(InpUseStopLoss && sl_pts > 0 && !InpStealthMode)
            sl = NormalizeDouble(price - sl_pts * point, _Digits);
    } else {
        price = bid;
        if(InpUseTakeProfit && tp_pts > 0 && !InpStealthMode)
            tp = NormalizeDouble(price - tp_pts * point, _Digits);
        if(InpUseStopLoss && sl_pts > 0 && !InpStealthMode)
            sl = NormalizeDouble(price + sl_pts * point, _Digits);
    }

    lot = NormLot(lot);
    bool ok;
    if(ordType == ORDER_TYPE_BUY)
        ok = Trade.Buy(lot, _Symbol, price, sl, tp, "RTB");
    else
        ok = Trade.Sell(lot, _Symbol, price, sl, tp, "RTB");

    if(ok) {
        LastOrderTime = TimeCurrent();
        Print("RTB: Open ", (ordType == ORDER_TYPE_BUY ? "BUY" : "SELL"),
              " lot=", lot, " tp=", tp, " sl=", sl);
    } else {
        Print("RTB: OpenOrder FAILED type=", ordType, " err=", GetLastError());
    }
    return ok;
}

//+------------------------------------------------------------------+
//| ENTRY SIGNALS                                                    |
//+------------------------------------------------------------------+

// Returns +1 = BUY, -1 = SELL, 0 = no signal
int SignalEMA() {
    double fast[], slow[];
    ArraySetAsSeries(fast, true);
    ArraySetAsSeries(slow, true);
    if(CopyBuffer(hEMAFast, 0, 0, 3, fast) < 3) return 0;
    if(CopyBuffer(hEMASlow, 0, 0, 3, slow) < 3) return 0;

    double point = SymbolInfoDouble(_Symbol, SYMBOL_POINT);
    double price = (double)iClose(_Symbol, InpSignalTF, 0);

    // Golden / Death cross
    bool crossUp   = fast[2] < slow[2] && fast[1] > slow[1];
    bool crossDown = fast[2] > slow[2] && fast[1] < slow[1];

    // Pullback to EMA34: price within InpEMAPullbackPts of EMA34
    bool trendUp   = fast[0] > slow[0];
    bool trendDown = fast[0] < slow[0];
    bool pullBuy   = trendUp   && MathAbs(price - fast[0]) <= InpEMAPullbackPts * point;
    bool pullSell  = trendDown && MathAbs(price - fast[0]) <= InpEMAPullbackPts * point;

    if(crossUp  || pullBuy)  return  1;
    if(crossDown || pullSell) return -1;
    return 0;
}

int SignalBZZone() {
    MqlRates r[];
    ArraySetAsSeries(r, true);
    // Read 3 closed candles (index 1-3)
    if(CopyRates(_Symbol, InpSignalTF, 1, 3, r) < 3) return 0;

    bool allGreen = r[0].close > r[0].open && r[1].close > r[1].open && r[2].close > r[2].open;
    bool allRed   = r[0].close < r[0].open && r[1].close < r[1].open && r[2].close < r[2].open;

    if(allGreen) return  1;
    if(allRed)   return -1;
    return 0; // Gray = no signal
}

int SignalIchimoku() {
    double tenkan[], kijun[], spanA[], spanB[], chikou[];
    ArraySetAsSeries(tenkan,  true);
    ArraySetAsSeries(kijun,   true);
    ArraySetAsSeries(spanA,   true);
    ArraySetAsSeries(spanB,   true);
    ArraySetAsSeries(chikou,  true);

    if(CopyBuffer(hIchi, 0, 0, 3, tenkan)  < 3) return 0;
    if(CopyBuffer(hIchi, 1, 0, 3, kijun)   < 3) return 0;
    if(CopyBuffer(hIchi, 2, 0, 3, spanA)   < 3) return 0;
    if(CopyBuffer(hIchi, 3, 0, 3, spanB)   < 3) return 0;
    if(CopyBuffer(hIchi, 4, 0, 3, chikou)  < 3) return 0;

    double price    = (double)iClose(_Symbol, InpSignalTF, 0);
    double pastPrice= (double)iClose(_Symbol, InpSignalTF, InpIchiKijun); // Chikou lag
    double kumoTop  = MathMax(spanA[0], spanB[0]);
    double kumoBot  = MathMin(spanA[0], spanB[0]);

    // Tenkan cross Kijun (golden = buy, death = sell) on bar[2] vs [1]
    bool tkCrossUp  = tenkan[2] < kijun[2] && tenkan[1] > kijun[1];
    bool tkCrossDown= tenkan[2] > kijun[2] && tenkan[1] < kijun[1];

    bool buyOK  = price > kumoTop  && tkCrossUp   && chikou[0] > pastPrice;
    bool sellOK = price < kumoBot  && tkCrossDown  && chikou[0] < pastPrice;

    if(buyOK)  return  1;
    if(sellOK) return -1;
    return 0;
}

int SignalBB() {
    double upper[], lower[], mid[];
    ArraySetAsSeries(upper, true);
    ArraySetAsSeries(lower, true);
    ArraySetAsSeries(mid,   true);
    if(CopyBuffer(hBB, 0, 0, 2, mid)   < 2) return 0;
    if(CopyBuffer(hBB, 1, 0, 2, upper) < 2) return 0;
    if(CopyBuffer(hBB, 2, 0, 2, lower) < 2) return 0;

    double closeBar1 = (double)iClose(_Symbol, InpSignalTF, 1);

    if(closeBar1 <= lower[1]) return  1;  // Touch/breach lower band → BUY
    if(closeBar1 >= upper[1]) return -1;  // Touch/breach upper band → SELL
    return 0;
}

int SignalSimulated() {
    // Immediate market order – direction determined by InpDirection
    if(InpDirection == DIR_ONLY_BUY)  return  1;
    if(InpDirection == DIR_ONLY_SELL) return -1;
    return 1; // Default buy for Both/Either in simulated mode
}

int GetSignal() {
    int sig = 0;
    switch(InpSignalMode) {
        case SIG_EMA:       sig = SignalEMA();       break;
        case SIG_BZ_ZONE:   sig = SignalBZZone();    break;
        case SIG_ICHIMOKU:  sig = SignalIchimoku();  break;
        case SIG_BB:        sig = SignalBB();        break;
        case SIG_SIMULATED: sig = SignalSimulated(); break;
    }
    if(InpDirection == DIR_ONLY_BUY  && sig < 0) return 0;
    if(InpDirection == DIR_ONLY_SELL && sig > 0) return 0;
    return sig;
}

//+------------------------------------------------------------------+
//| INITIAL ENTRY (OnTick)                                           |
//+------------------------------------------------------------------+
void CheckEntry() {
    if(TimeCurrent() - LastOrderTime < InpOrderDelay) return;

    int sig = GetSignal();
    if(sig == 0) return;

    if(sig > 0) {
        if(CountBuy() > 0) return; // DCA will handle scaling
        if(CountBuy() >= InpMaxBuy) return;
        OpenOrder(ORDER_TYPE_BUY, InpLotSize, InpTP_Points, InpSL_Points);
    }
    if(sig < 0) {
        if(CountSell() > 0) return;
        if(CountSell() >= InpMaxSell) return;
        OpenOrder(ORDER_TYPE_SELL, InpLotSize, InpTP_Points, InpSL_Points);
    }
}

//+------------------------------------------------------------------+
//| DCA LOGIC                                                        |
//+------------------------------------------------------------------+
void CheckDCA(int posType) {
    int count = CountPos(posType);
    if(count == 0) return;

    // DCA level = current count (0-based). Level 0 = first DCA after initial entry.
    int lvl = count - 1; // which DCA config row applies
    if(lvl < 0 || lvl >= 8) return;
    if(DCA_Mode[lvl] == DCA_STOP) return;

    // Cap: total positions must not exceed (DCA_MaxOrd[lvl] + 1) for this level
    // DCA_MaxOrd means max total orders allowed when at this DCA level
    if(count > DCA_MaxOrd[lvl]) return;

    int maxOrds = (posType == POSITION_TYPE_BUY) ? InpMaxBuy : InpMaxSell;
    if(count >= maxOrds) return;

    double lastPrice = LastOpenPrice(posType);
    if(lastPrice == 0) return;

    double ask   = SymbolInfoDouble(_Symbol, SYMBOL_ASK);
    double bid   = SymbolInfoDouble(_Symbol, SYMBOL_BID);
    double point = SymbolInfoDouble(_Symbol, SYMBOL_POINT);
    double dist  = DCA_Dist[lvl] * point;

    bool trigger = false;
    if(posType == POSITION_TYPE_BUY)
        trigger = (lastPrice - bid) >= dist;  // Price fell below entry by dist
    else
        trigger = (ask - lastPrice) >= dist;  // Price rose above entry by dist

    if(!trigger) return;

    // Step+TF: require same-direction signal confirmation
    if(DCA_Mode[lvl] == DCA_STEP_TF) {
        int sig = GetSignal();
        if(posType == POSITION_TYPE_BUY  && sig != 1)  return;
        if(posType == POSITION_TYPE_SELL && sig != -1) return;
    }

    if(TimeCurrent() - LastOrderTime < InpOrderDelay) return;

    double lot = NormLot(InpLotSize * DCA_Mult[lvl]);
    int    ord = (posType == POSITION_TYPE_BUY) ? ORDER_TYPE_BUY : ORDER_TYPE_SELL;
    Print("RTB: DCA level ", lvl+1, " triggered. count=", count);
    OpenOrder(ord, lot, DCA_TP[lvl], DCA_SL[lvl]);
}

//+------------------------------------------------------------------+
//| PYRAMIDING (NHỒI DƯƠNG)                                          |
//+------------------------------------------------------------------+
void CheckPyramiding(int posType) {
    if(!InpPyraEnable) return;

    int count = CountPos(posType);
    if(count == 0) return;

    int pyraCount = (posType == POSITION_TYPE_BUY) ? PyraCountBuy : PyraCountSell;
    if(pyraCount >= InpPyraMaxLevels) return;

    double lastPrice = LastOpenPrice(posType);
    if(lastPrice == 0) return;

    double ask   = SymbolInfoDouble(_Symbol, SYMBOL_ASK);
    double bid   = SymbolInfoDouble(_Symbol, SYMBOL_BID);
    double point = SymbolInfoDouble(_Symbol, SYMBOL_POINT);
    double dist  = InpPyraDist * point;

    bool inProfit = false;
    if(posType == POSITION_TYPE_BUY)
        inProfit = (bid - lastPrice) >= dist;  // Price moved up by dist
    else
        inProfit = (lastPrice - ask) >= dist;  // Price moved down by dist

    if(!inProfit) return;
    if(TimeCurrent() - LastOrderTime < InpOrderDelay) return;

    double lot = NormLot(InpLotSize * InpPyraLotMult);
    int    ord = (posType == POSITION_TYPE_BUY) ? ORDER_TYPE_BUY : ORDER_TYPE_SELL;
    if(OpenOrder(ord, lot, InpTP_Points, InpSL_Points)) {
        if(posType == POSITION_TYPE_BUY) { PyraCountBuy++;  PyraLastBuy  = TimeCurrent(); }
        else                             { PyraCountSell++; PyraLastSell = TimeCurrent(); }
        Print("RTB: Pyramiding level ", pyraCount + 1);
    }
}

//+------------------------------------------------------------------+
//| ORDER TRIMMING                                                   |
//+------------------------------------------------------------------+
void CheckTrimming() {
    if(!InpTrimEnable) return;
    if(CountAll() < InpTrimTrigger) return;

    double balance = AccountInfoDouble(ACCOUNT_BALANCE);
    double equity  = AccountInfoDouble(ACCOUNT_EQUITY);

    // Partial trim: by drawdown%
    if(InpPartialTrim && balance > 0) {
        double ddPct = (balance - equity) / balance * 100.0;
        if(ddPct > InpPartialTrimDD) {
            ulong worstTk = WorstTicket();
            if(worstTk > 0) {
                Print("RTB: Partial Trim DD=", ddPct, "%");
                Trade.PositionClose(worstTk);
                return;
            }
        }
    }

    // Trim by day profit: if today's closed profit > |worst floating loss|
    if(InpTrimByDayProfit) {
        ulong worstTk = WorstTicket();
        if(worstTk > 0 && PositionSelectByTicket(worstTk)) {
            double worstP = PositionGetDouble(POSITION_PROFIT);
            if(DayProfit > MathAbs(worstP) && DayProfit > 0) {
                Print("RTB: Trim by DayProfit=", DayProfit, " worst=", worstP);
                Trade.PositionClose(worstTk);
                return;
            }
        }
    }

    // Hedging trim: close profitable opposite position to cover worst
    if(InpTrimHedge) {
        ulong worstTk = WorstTicket();
        ulong bestTk  = BestTicket();
        if(worstTk > 0 && bestTk > 0 && worstTk != bestTk) {
            if(PositionSelectByTicket(bestTk)) {
                double bestP  = PositionGetDouble(POSITION_PROFIT);
                if(PositionSelectByTicket(worstTk)) {
                    double worstP = PositionGetDouble(POSITION_PROFIT);
                    double net = bestP + worstP;
                    if(net >= InpTrimTarget) {
                        Trade.PositionClose(worstTk);
                        Trade.PositionClose(bestTk);
                        Print("RTB: Hedge trim, net=", net);
                        return;
                    }
                }
            }
        }
    }

    // Same-direction trim: use aggregate floating profit vs target
    double totalProfit = FloatProfit();
    if(InpTrimTarget > 0 && totalProfit >= InpTrimTarget) {
        ulong worstTk = WorstTicket();
        if(worstTk > 0 && PositionSelectByTicket(worstTk)) {
            double worstP = PositionGetDouble(POSITION_PROFIT);
            // Only close worst if remaining positions still meet target
            if((totalProfit + worstP) >= InpTrimTarget) {
                Print("RTB: Trim target met, closing worst=", worstP);
                Trade.PositionClose(worstTk);
            }
        }
    }
}

//+------------------------------------------------------------------+
//| TRAILING STOP                                                    |
//+------------------------------------------------------------------+
void ApplyTrailToPos(ulong tk, int posType, double newSL) {
    if(!PositionSelectByTicket(tk)) return;
    double curSL = PositionGetDouble(POSITION_SL);
    double curTP = PositionGetDouble(POSITION_TP);
    bool   move  = false;
    if(posType == POSITION_TYPE_BUY  && (curSL == 0 || newSL > curSL)) move = true;
    if(posType == POSITION_TYPE_SELL && (curSL == 0 || newSL < curSL)) move = true;
    if(move) Trade.PositionModify(tk, NormalizeDouble(newSL, _Digits), curTP);
}

void CheckTrailing() {
    if(!InpTrailEnable) return;
    if(CountAll() < InpTrailMinOrds) return;

    double ask   = SymbolInfoDouble(_Symbol, SYMBOL_ASK);
    double bid   = SymbolInfoDouble(_Symbol, SYMBOL_BID);
    double point = SymbolInfoDouble(_Symbol, SYMBOL_POINT);

    if(InpTrailMode == TRAIL_BASKET) {
        // --- BUY BASKET ---
        if(CountBuy() > 0) {
            double avgBuy = AvgOpenPrice(POSITION_TYPE_BUY);
            if(bid - avgBuy >= InpTrailActivate * point) {
                double newSL = bid - InpTrailInit * point;
                bool   moved = false;
                if(TrailBuy == 0 || newSL >= TrailBuy + InpTrailStep * point) {
                    TrailBuy = newSL;
                    moved = true;
                }
                if(moved) {
                    for(int i = PositionsTotal()-1; i >= 0; i--) {
                        ulong tk = PositionGetTicket(i);
                        if(!PositionSelectByTicket(tk)) continue;
                        if(PositionGetInteger(POSITION_MAGIC) != (long)InpMagic) continue;
                        if(PositionGetString(POSITION_SYMBOL) != _Symbol) continue;
                        if((int)PositionGetInteger(POSITION_TYPE) != POSITION_TYPE_BUY) continue;
                        ApplyTrailToPos(tk, POSITION_TYPE_BUY, TrailBuy);
                    }
                }
            }
        } else { TrailBuy = 0; }

        // --- SELL BASKET ---
        if(CountSell() > 0) {
            double avgSell = AvgOpenPrice(POSITION_TYPE_SELL);
            if(avgSell - ask >= InpTrailActivate * point) {
                double newSL = ask + InpTrailInit * point;
                bool   moved = false;
                if(TrailSell == 0 || newSL <= TrailSell - InpTrailStep * point) {
                    TrailSell = newSL;
                    moved = true;
                }
                if(moved) {
                    for(int i = PositionsTotal()-1; i >= 0; i--) {
                        ulong tk = PositionGetTicket(i);
                        if(!PositionSelectByTicket(tk)) continue;
                        if(PositionGetInteger(POSITION_MAGIC) != (long)InpMagic) continue;
                        if(PositionGetString(POSITION_SYMBOL) != _Symbol) continue;
                        if((int)PositionGetInteger(POSITION_TYPE) != POSITION_TYPE_SELL) continue;
                        ApplyTrailToPos(tk, POSITION_TYPE_SELL, TrailSell);
                    }
                }
            }
        } else { TrailSell = 0; }

        // Draw lines
        if(InpTrailShowLine) {
            if(TrailBuy  > 0) DrawHLine("TrailBuy",  TrailBuy,  clrLimeGreen);
            if(TrailSell > 0) DrawHLine("TrailSell", TrailSell, clrTomato);
        }

    } else {
        // --- SINGLE TRAILING PER POSITION ---
        for(int i = PositionsTotal()-1; i >= 0; i--) {
            ulong tk = PositionGetTicket(i);
            if(!PositionSelectByTicket(tk)) continue;
            if(PositionGetInteger(POSITION_MAGIC) != (long)InpMagic) continue;
            if(PositionGetString(POSITION_SYMBOL) != _Symbol) continue;

            int    pt        = (int)PositionGetInteger(POSITION_TYPE);
            double openPrice = PositionGetDouble(POSITION_PRICE_OPEN);

            if(pt == POSITION_TYPE_BUY) {
                double profitPts = (bid - openPrice) / point;
                if(profitPts >= InpTrailActivate) {
                    double newSL = bid - InpTrailInit * point;
                    double curSL = PositionGetDouble(POSITION_SL);
                    if(curSL == 0 || newSL >= curSL + InpTrailStep * point)
                        ApplyTrailToPos(tk, POSITION_TYPE_BUY, newSL);
                }
            } else {
                double profitPts = (openPrice - ask) / point;
                if(profitPts >= InpTrailActivate) {
                    double newSL = ask + InpTrailInit * point;
                    double curSL = PositionGetDouble(POSITION_SL);
                    if(curSL == 0 || newSL <= curSL - InpTrailStep * point)
                        ApplyTrailToPos(tk, POSITION_TYPE_SELL, newSL);
                }
            }
        }
    }
}

//+------------------------------------------------------------------+
//| EXIT LOGIC                                                       |
//+------------------------------------------------------------------+
void CheckExit() {
    // 1. Per-position exit by pips target
    if(InpClosePerPips > 0) {
        double ask   = SymbolInfoDouble(_Symbol, SYMBOL_ASK);
        double bid   = SymbolInfoDouble(_Symbol, SYMBOL_BID);
        double point = SymbolInfoDouble(_Symbol, SYMBOL_POINT);

        for(int i = PositionsTotal()-1; i >= 0; i--) {
            ulong tk = PositionGetTicket(i);
            if(!PositionSelectByTicket(tk)) continue;
            if(PositionGetInteger(POSITION_MAGIC) != (long)InpMagic) continue;
            if(PositionGetString(POSITION_SYMBOL) != _Symbol) continue;

            int    pt    = (int)PositionGetInteger(POSITION_TYPE);
            double opn   = PositionGetDouble(POSITION_PRICE_OPEN);
            double ppts  = (pt == POSITION_TYPE_BUY) ? (bid - opn) / point : (opn - ask) / point;

            if(ppts >= InpClosePerPips)
                Trade.PositionClose(tk);
        }
    }

    // 2. Stealth TP/SL management (when stealth mode on, check manually)
    if(InpStealthMode) {
        double ask   = SymbolInfoDouble(_Symbol, SYMBOL_ASK);
        double bid   = SymbolInfoDouble(_Symbol, SYMBOL_BID);
        double point = SymbolInfoDouble(_Symbol, SYMBOL_POINT);

        for(int i = PositionsTotal()-1; i >= 0; i--) {
            ulong tk = PositionGetTicket(i);
            if(!PositionSelectByTicket(tk)) continue;
            if(PositionGetInteger(POSITION_MAGIC) != (long)InpMagic) continue;
            if(PositionGetString(POSITION_SYMBOL) != _Symbol) continue;

            int    pt  = (int)PositionGetInteger(POSITION_TYPE);
            double opn = PositionGetDouble(POSITION_PRICE_OPEN);

            if(pt == POSITION_TYPE_BUY) {
                if(InpUseTakeProfit && InpTP_Points > 0 && bid >= opn + InpTP_Points * point)
                    Trade.PositionClose(tk);
                if(InpUseStopLoss  && InpSL_Points > 0 && bid <= opn - InpSL_Points * point)
                    Trade.PositionClose(tk);
            } else {
                if(InpUseTakeProfit && InpTP_Points > 0 && ask <= opn - InpTP_Points * point)
                    Trade.PositionClose(tk);
                if(InpUseStopLoss  && InpSL_Points > 0 && ask >= opn + InpSL_Points * point)
                    Trade.PositionClose(tk);
            }
        }
    }

    // 3. Basket total profit target
    if(InpCloseProfit > 0) {
        if(FloatProfit() >= InpCloseProfit) {
            Print("RTB: CloseProfit target reached. Closing all.");
            CloseAll();
            return;
        }
    }

    // 4. Basket total loss cut
    if(InpCloseLoss > 0) {
        if(FloatProfit() <= -InpCloseLoss) {
            Print("RTB: CloseLoss limit hit. Closing all.");
            CloseAll();
            return;
        }
    }
}

//+------------------------------------------------------------------+
//| DAY PROFIT TRACKING                                              |
//+------------------------------------------------------------------+
void UpdateDayProfit() {
    MqlDateTime dt;
    TimeToStruct(TimeCurrent(), dt);
    if(dt.day != LastDay) {
        DayProfit = 0;
        LastDay   = dt.day;
    }

    datetime dayStart = StringToTime(StringFormat("%04d.%02d.%02d 00:00:00",
                         dt.year, dt.mon, dt.day));
    if(!HistorySelect(dayStart, TimeCurrent())) return;

    double closed = 0;
    for(int i = 0; i < HistoryDealsTotal(); i++) {
        ulong dTk = HistoryDealGetTicket(i);
        if(HistoryDealGetInteger(dTk, DEAL_MAGIC) != (long)InpMagic) continue;
        if(HistoryDealGetString(dTk, DEAL_SYMBOL) != _Symbol) continue;
        ENUM_DEAL_ENTRY de = (ENUM_DEAL_ENTRY)HistoryDealGetInteger(dTk, DEAL_ENTRY);
        if(de == DEAL_ENTRY_OUT || de == DEAL_ENTRY_OUT_BY)
            closed += HistoryDealGetDouble(dTk, DEAL_PROFIT) +
                      HistoryDealGetDouble(dTk, DEAL_SWAP);
    }
    DayProfit = closed;
}

//+------------------------------------------------------------------+
//| GUI                                                              |
//+------------------------------------------------------------------+
void DrawHLine(string name, double price, color clr) {
    string obj = GUI + name;
    if(ObjectFind(0, obj) < 0)
        ObjectCreate(0, obj, OBJ_HLINE, 0, 0, price);
    ObjectSetDouble(0,  obj, OBJPROP_PRICE, price);
    ObjectSetInteger(0, obj, OBJPROP_COLOR, clr);
    ObjectSetInteger(0, obj, OBJPROP_STYLE, STYLE_DASH);
    ObjectSetInteger(0, obj, OBJPROP_WIDTH, 1);
}

void Lbl(string name, string text, int x, int y, color clr = clrSilver, int sz = 9) {
    string obj = GUI + name;
    if(ObjectFind(0, obj) < 0) {
        ObjectCreate(0, obj, OBJ_LABEL, 0, 0, 0);
        ObjectSetInteger(0, obj, OBJPROP_CORNER,    CORNER_LEFT_UPPER);
        ObjectSetInteger(0, obj, OBJPROP_XDISTANCE, x);
        ObjectSetInteger(0, obj, OBJPROP_YDISTANCE, y);
        ObjectSetString(0,  obj, OBJPROP_FONT, "Consolas");
        ObjectSetInteger(0, obj, OBJPROP_BACK, false);
        ObjectSetInteger(0, obj, OBJPROP_SELECTABLE, false);
    }
    ObjectSetString(0,  obj, OBJPROP_TEXT,     text);
    ObjectSetInteger(0, obj, OBJPROP_COLOR,    clr);
    ObjectSetInteger(0, obj, OBJPROP_FONTSIZE, sz);
}

void UpdateGUI() {
    double balance   = AccountInfoDouble(ACCOUNT_BALANCE);
    double equity    = AccountInfoDouble(ACCOUNT_EQUITY);
    double spread    = (double)SymbolInfoInteger(_Symbol, SYMBOL_SPREAD);
    double totalProfit = FloatProfit();
    double buyProfit   = FloatProfit(POSITION_TYPE_BUY);
    double sellProfit  = FloatProfit(POSITION_TYPE_SELL);
    int    nBuy  = CountBuy();
    int    nSell = CountSell();
    double lotBuy  = TotalLot(POSITION_TYPE_BUY);
    double lotSell = TotalLot(POSITION_TYPE_SELL);
    double pnlPct  = (InitBalance > 0) ? totalProfit / InitBalance * 100.0 : 0;
    double ddPct   = (balance > 0 && equity < balance) ? (balance - equity) / balance * 100.0 : 0;
    if(ddPct > MaxDrawdownPct) MaxDrawdownPct = ddPct;

    string sigName = "";
    switch(InpSignalMode) {
        case SIG_EMA:       sigName = "EMA 34+89";      break;
        case SIG_BZ_ZONE:   sigName = "BZ Zone";        break;
        case SIG_ICHIMOKU:  sigName = "Ichimoku";       break;
        case SIG_BB:        sigName = "Bollinger Band"; break;
        case SIG_SIMULATED: sigName = "Simulated";      break;
    }

    MqlDateTime dt;
    TimeToStruct(TimeLocal(), dt);
    string tStr = StringFormat("%04d/%02d/%02d  %02d:%02d:%02d",
                   dt.year, dt.mon, dt.day, dt.hour, dt.min, dt.sec);

    color cProfit = (totalProfit >= 0) ? clrLimeGreen : clrTomato;
    color cBuyP   = (buyProfit   >= 0) ? clrLimeGreen : clrTomato;
    color cSellP  = (sellProfit  >= 0) ? clrLimeGreen : clrTomato;
    color cDayP   = (DayProfit   >= 0) ? clrLimeGreen : clrTomato;

    // Background
    string bg = GUI + "BG";
    if(ObjectFind(0, bg) < 0) {
        ObjectCreate(0, bg, OBJ_RECTANGLE_LABEL, 0, 0, 0);
        ObjectSetInteger(0, bg, OBJPROP_CORNER,    CORNER_LEFT_UPPER);
        ObjectSetInteger(0, bg, OBJPROP_XDISTANCE, 5);
        ObjectSetInteger(0, bg, OBJPROP_YDISTANCE, 18);
        ObjectSetInteger(0, bg, OBJPROP_XSIZE,     248);
        ObjectSetInteger(0, bg, OBJPROP_YSIZE,     365);
        ObjectSetInteger(0, bg, OBJPROP_BGCOLOR,   C'15,18,28');
        ObjectSetInteger(0, bg, OBJPROP_BORDER_TYPE, BORDER_FLAT);
        ObjectSetInteger(0, bg, OBJPROP_COLOR,     C'40,50,90');
        ObjectSetInteger(0, bg, OBJPROP_WIDTH,     1);
        ObjectSetInteger(0, bg, OBJPROP_BACK,      false);
        ObjectSetInteger(0, bg, OBJPROP_SELECTABLE,false);
    }

    int x = 12, y = 23, s = 16;
    Lbl("T",    " RICH TRADING BOT  v1.0",   x, y, C'80,160,255', 10); y += s+2;
    Lbl("L0",   "────────────────────────",   x, y, C'40,50,90'  );     y += s-2;
    Lbl("Tim",  "Time   : " + tStr,           x, y, clrSilver     );    y += s;
    Lbl("Sig",  "Signal : " + sigName,        x, y, clrYellow     );    y += s;
    Lbl("L1",   "────────────────────────",   x, y, C'40,50,90'  );     y += s-2;
    Lbl("Bal",  StringFormat("Balance: $%.2f", balance),   x, y, clrSilver);  y += s;
    Lbl("Ini",  StringFormat("Initial: $%.2f", InitBalance),x, y, clrSilver); y += s;
    Lbl("DayP", StringFormat("Day P/L: $%.2f", DayProfit), x, y, cDayP);      y += s;
    Lbl("FP",   StringFormat("Float  : $%.2f  (%.2f%%)", totalProfit, pnlPct),
                x, y, cProfit);                                                y += s;
    Lbl("L2",   "────────────────────────",   x, y, C'40,50,90'  );     y += s-2;
    Lbl("DD",   StringFormat("DD Now : %.2f%%", ddPct),    x, y,
                ddPct > 15 ? clrOrangeRed : clrSilver);                       y += s;
    Lbl("MDD",  StringFormat("DD Max : %.2f%%", MaxDrawdownPct), x, y,
                MaxDrawdownPct > 25 ? clrTomato : clrSilver);                 y += s;
    Lbl("Sprd", StringFormat("Spread : %.0f pts", spread), x, y, clrSilver);  y += s;
    Lbl("L3",   "────────────────────────",   x, y, C'40,50,90'  );     y += s-2;
    Lbl("BuyP", StringFormat("Buy P/L: $%.2f", buyProfit),  x, y, cBuyP);     y += s;
    Lbl("BuyC", StringFormat("Buy Ord: %d   Lot: %.2f", nBuy,  lotBuy),  x, y, clrSilver); y += s;
    Lbl("SelP", StringFormat("Sel P/L: $%.2f", sellProfit), x, y, cSellP);    y += s;
    Lbl("SelC", StringFormat("Sel Ord: %d   Lot: %.2f", nSell, lotSell), x, y, clrSilver); y += s;
    Lbl("Tot",  StringFormat("Total  : %d orders", nBuy + nSell), x, y, clrSilver);        y += s;

    ChartRedraw(0);
}

void RemoveGUI() { ObjectsDeleteAll(0, GUI); }

//+------------------------------------------------------------------+
//| INIT DCA ARRAYS                                                  |
//+------------------------------------------------------------------+
void InitDCA() {
    DCA_Mode[0]=InpDCA1Mode; DCA_Mult[0]=InpDCA1Mult; DCA_MaxOrd[0]=InpDCA1Max;
    DCA_Dist[0]=InpDCA1Dist; DCA_TP[0]=InpDCA1TP;     DCA_SL[0]=InpDCA1SL;

    DCA_Mode[1]=InpDCA2Mode; DCA_Mult[1]=InpDCA2Mult; DCA_MaxOrd[1]=InpDCA2Max;
    DCA_Dist[1]=InpDCA2Dist; DCA_TP[1]=InpDCA2TP;     DCA_SL[1]=InpDCA2SL;

    DCA_Mode[2]=InpDCA3Mode; DCA_Mult[2]=InpDCA3Mult; DCA_MaxOrd[2]=InpDCA3Max;
    DCA_Dist[2]=InpDCA3Dist; DCA_TP[2]=InpDCA3TP;     DCA_SL[2]=InpDCA3SL;

    DCA_Mode[3]=InpDCA4Mode; DCA_Mult[3]=InpDCA4Mult; DCA_MaxOrd[3]=InpDCA4Max;
    DCA_Dist[3]=InpDCA4Dist; DCA_TP[3]=InpDCA4TP;     DCA_SL[3]=InpDCA4SL;

    DCA_Mode[4]=InpDCA5Mode; DCA_Mult[4]=InpDCA5Mult; DCA_MaxOrd[4]=InpDCA5Max;
    DCA_Dist[4]=InpDCA5Dist; DCA_TP[4]=InpDCA5TP;     DCA_SL[4]=InpDCA5SL;

    DCA_Mode[5]=InpDCA6Mode; DCA_Mult[5]=InpDCA6Mult; DCA_MaxOrd[5]=InpDCA6Max;
    DCA_Dist[5]=InpDCA6Dist; DCA_TP[5]=InpDCA6TP;     DCA_SL[5]=InpDCA6SL;

    DCA_Mode[6]=InpDCA7Mode; DCA_Mult[6]=InpDCA7Mult; DCA_MaxOrd[6]=InpDCA7Max;
    DCA_Dist[6]=InpDCA7Dist; DCA_TP[6]=InpDCA7TP;     DCA_SL[6]=InpDCA7SL;

    DCA_Mode[7]=InpDCA8Mode; DCA_Mult[7]=InpDCA8Mult; DCA_MaxOrd[7]=InpDCA8Max;
    DCA_Dist[7]=InpDCA8Dist; DCA_TP[7]=InpDCA8TP;     DCA_SL[7]=InpDCA8SL;
}

//+------------------------------------------------------------------+
//| EVENT HANDLERS                                                   |
//+------------------------------------------------------------------+
int OnInit() {
    Trade.SetExpertMagicNumber(InpMagic);
    Trade.SetDeviationInPoints(50);
    Trade.SetTypeFilling(ORDER_FILLING_RETURN);

    InitDCA();

    // Create indicator handles
    hEMAFast = iMA(_Symbol, InpSignalTF, InpEMAFast, 0, MODE_EMA, PRICE_CLOSE);
    hEMASlow = iMA(_Symbol, InpSignalTF, InpEMASlow, 0, MODE_EMA, PRICE_CLOSE);
    hBB      = iBands(_Symbol, InpSignalTF, InpBBPeriod, 0, InpBBDev, PRICE_CLOSE);
    hIchi    = iIchimoku(_Symbol, InpSignalTF, InpIchiTenkan, InpIchiKijun, InpIchiSenkou);

    if(hEMAFast == INVALID_HANDLE || hEMASlow == INVALID_HANDLE ||
       hBB == INVALID_HANDLE      || hIchi    == INVALID_HANDLE) {
        Print("RTB: ERROR — failed to create indicator handles!");
        return INIT_FAILED;
    }

    InitBalance    = AccountInfoDouble(ACCOUNT_BALANCE);
    MaxDrawdownPct = 0;
    TrailBuy       = 0;
    TrailSell      = 0;
    PyraCountBuy   = 0;
    PyraCountSell  = 0;
    LastDay        = -1;

    EventSetTimer(1);
    Print("RTB: Initialized. Magic=", InpMagic, " Signal=", EnumToString(InpSignalMode));
    return INIT_SUCCEEDED;
}

void OnDeinit(const int reason) {
    EventKillTimer();
    RemoveGUI();
    IndicatorRelease(hEMAFast);
    IndicatorRelease(hEMASlow);
    IndicatorRelease(hBB);
    IndicatorRelease(hIchi);
}

void OnTick() {
    // Entry signals only fire when no position exists for that direction
    CheckEntry();
    // Stealth TP/SL check runs on every tick for precision
    if(InpStealthMode) CheckExit();
}

void OnTimer() {
    UpdateDayProfit();

    // Reset pyramiding counters when positions are closed
    if(CountBuy()  == 0) { PyraCountBuy  = 0; TrailBuy  = 0; }
    if(CountSell() == 0) { PyraCountSell = 0; TrailSell = 0; }

    // Exit checks (basket close conditions)
    if(!InpStealthMode) CheckExit();

    // Trimming
    CheckTrimming();

    // Trailing stop
    CheckTrailing();

    // DCA scale-in
    if(CountBuy()  > 0) CheckDCA(POSITION_TYPE_BUY);
    if(CountSell() > 0) CheckDCA(POSITION_TYPE_SELL);

    // Pyramiding (add to winners)
    if(CountBuy()  > 0) CheckPyramiding(POSITION_TYPE_BUY);
    if(CountSell() > 0) CheckPyramiding(POSITION_TYPE_SELL);

    // GUI refresh every second
    UpdateGUI();
}
//+------------------------------------------------------------------+
