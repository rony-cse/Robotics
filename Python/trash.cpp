#define InpMagicNumber 1234
#property strict
#include <Trade\Trade.mqh>
CTrade trade;

bool cond1, cond2, cond3, cond4, cond5, cond6, cond7, buyTriggered, tradingRangeAllowed;
double LotSize, LotSizeMin = 0.05, LotSizeMax = 0.1, TP = 50.0, SL = 5000.0, prevBuyPrice = 0, prevSellPrice = 0, tradeGap = 60.0, Gap = 0.0, trailingStop = 50.0;
static double oldTop = 0., oldBot = 0., internalBuySL = 0.0, internalSellSL = 0.0, BuySL = 0.0, SellSL = 0.0;
int maxPositionsAllowed = 20, maxBuyPositionsAllowed = 10, maxSellPositionsAllowed = 10, spreadAllowed = 20; // points
int nosBuyPos, nosSellPos;
string buy = "", commentStr = "";
static bool topInitialised = false, botInitialised = false;
double upperTradeRange, lowerTradeRange;
input double upperLimit = 0.0;
input double lowerLimit = 0.0;

struct MTrades {long ticketNo; int stopLoss; };
static MTrades old_Trades[], cur_Trades[];

void OnInit() {
    trade.SetExpertMagicNumber(InpMagicNumber);
}

void OnTick()
{
    if ( !isNewBar()) return;
    if (PositionsTotal() < 6) {
        double Ask = NormalizeDouble(SymbolInfoDouble(_Symbol, SYMBOL_ASK), _Digits);
        double Bid = NormalizeDouble(SymbolInfoDouble(_Symbol, SYMBOL_BID), _Digits);
        trade.Buy(0.1, NULL, Ask, 0.0, 0.0, NULL);
        trade.Sell(0.1, NULL, Bid, 0.0, 0.0, NULL);
    }
    if (PositionsTotal() > 0) SetInternalTrailingStop();

} // End OnTick()


void SetInternalTrailingStop()
{
    //string mySpreadsheet = "Spreadsheet.csv";
    //int mySpreadsheetHandle = FileOpen(mySpreadsheet,FILE_READ|FILE_WRITE|FILE_CSV|FILE_ANSI);
    int  count   =   PositionsTotal(); double tempSL = 0.0, BuySL = 0.0, SellSL = 0.0, temp = 0.0, oldSL = 0.0;
    int   sizeOld = ArraySize(old_Trades);
    ArrayResize(cur_Trades, count); for (int i = 0; i < count; i++) {cur_Trades[i].ticketNo = 0; cur_Trades[i].stopLoss = 0.0;}

    for (int i = 0; i < count; i++)
    {   string symbol = PositionGetSymbol(i);
        ulong   ticket  =   PositionGetTicket(i);
        if (ticket > 0) {
            double   openPrice = PositionGetDouble(POSITION_PRICE_OPEN);
            double   currentPrice = PositionGetDouble(POSITION_PRICE_CURRENT);
            for (int j = 0; j < sizeOld; j++)  { if ( ticket == old_Trades[j].ticketNo)  oldSL = old_Trades[j].stopLoss; break; }
            //double   oldSL=PositionGetDouble(POSITION_SL); // << Read oldSL from old array

            if (PositionGetString(POSITION_SYMBOL) == symbol && PositionGetInteger(POSITION_MAGIC) == InpMagicNumber && PositionGetInteger(POSITION_TYPE) == POSITION_TYPE_BUY)
            {   temp = openPrice + TP * _Point + trailingStop * _Point;
                if (currentPrice > temp)
                {   tempSL = currentPrice - trailingStop * _Point;
                    if (oldSL == 0.0) BuySL = tempSL;
                    else if (tempSL > oldSL) BuySL = tempSL;
                    if (BuySL > 0.0) {cur_Trades[i].ticketNo = ticket; cur_Trades[i].stopLoss = BuySL;}
                    else if (oldSL > 0.0 && currentPrice < oldSL) trade.PositionClose(PositionGetSymbol(i));
                    //else  {cur_Trades[i].ticketNo = ticket; cur_Trades[i].stopLoss = 0.0;}
                }
            }

            else if (PositionGetString(POSITION_SYMBOL) == symbol && PositionGetInteger(POSITION_MAGIC) == InpMagicNumber && PositionGetInteger(POSITION_TYPE) == POSITION_TYPE_SELL)
            {   temp = openPrice - TP * _Point - trailingStop * _Point;
                if (currentPrice < temp) {
                    tempSL = currentPrice + trailingStop * _Point;
                    if (oldSL == 0.0) SellSL = tempSL;
                    else if (tempSL < oldSL) SellSL = tempSL;
                    if (SellSL > 0.0) {cur_Trades[i].ticketNo = ticket; cur_Trades[i].stopLoss = SellSL;}
                    else if (oldSL > 0.0 && currentPrice > oldSL) trade.PositionClose(PositionGetSymbol(i));
                    //else  {cur_Trades[i].ticketNo = ticket; cur_Trades[i].stopLoss = 0.0;}
                }
            }
        }
    }
    for (int j = 0; j < count; j++) {
        //FileSeek(mySpreadsheetHandle,0,SEEK_END);
        //FileWrite(mySpreadsheetHandle, cur_Trades[i].ticketNo, cur_Trades[i].stopLoss, "\n");}
        cur_Trades[j].ticketNo = cur_Trades[j].ticketNo; cur_Trades[j].stopLoss = cur_Trades[j].stopLoss ;
    }
    //FileClose(mySpreadsheetHandle);
    ArrayFree(old_Trades); ArrayResize(old_Trades, count);
    ArrayCopy(old_Trades, cur_Trades, 0, 0, WHOLE_ARRAY);


}

// *****************************************************************************************

//+------------------------------------------------------------------+
//|  Return 'true' when a new bar appears as per Orchard Forex                           |
//+------------------------------------------------------------------+
bool isNewBar()
{
    static datetime prevTime = 0;
    datetime currentTime = iTime(_Symbol, _Period, 0);
    if (prevTime != currentTime) //--- if open time changes, a new bar has arrived
    {
        prevTime = currentTime;
        return (true);   //--- new bar
    }
    return (false);  //--- no new bar
}
