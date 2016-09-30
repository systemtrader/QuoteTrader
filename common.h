#ifndef COMMON_H_
#define COMMON_H_

#include "ThostTraderApi\ThostFtdcMdApi.h"
#include "ThostTraderApi\ThostFtdcTraderApi.h"
#include <map> 
#include <list>
#include <queue> 
#include <mutex>
#include <fstream>
#include "stdafx.h"
#include  <map> 
#include  <queue> 
#include <mutex>
#include <condition_variable> 

enum MsgType
{
	RtnOrder = 0,		// ί�лر�

	RtnTrade = 1,       // �ɽ��ر�

	InputOrder = 2,       //����¼��

	InputOrderAction = 3,   // ��������¼��

	RtnQuote = 4  //    Quote

	//CThostFtdcInputOrderField
};

struct Msg
{
	MsgType Msg_Type;

	CThostFtdcOrderField  RtnOrder; //ί�лر�

	CThostFtdcQuoteField  RtnQuote;// Quote�ر�
	
	CThostFtdcTradeField  RtnTrade; //�ɽ��ر�

	CThostFtdcInputOrderField InputOrder;//����¼��

	CThostFtdcInputOrderActionField InputOrderAction;//��������¼��

	CThostFtdcRspInfoField RspInfo;

};


//*************ȫ�ֱ���*************************************************************

using namespace std;

extern int iRequestID; // ������

extern vector<pair<string, double> > Instrumentlist; //���ĺ�Լlist

extern map<string, double> InstrumentMap; //���ĺ�ԼMap

extern queue<CThostFtdcForQuoteRspField> ForQuoteRspField;//ѯ�۶���

extern map<string, CThostFtdcDepthMarketDataField> MarketDataField;//��Լ����

extern map<string, double> payoff; //��Լ����

extern map<string, string> BidORDER_REF_present;//�򱨼�����

extern map<string, string> AskORDER_REF_present;//����������

extern map<string, string> Bid_refill;//�򱨼�����

extern map<string, string> Ask_refill;//����������

extern std::mutex   g_lockqueue;

extern std::condition_variable cv; // ȫ����������.

extern double spreed ;//= 2;

extern int volume;//= 2;

extern int iNextOrderRef;  ///����Ӧ�ñ�� 

extern queue<Msg> MsgQueue;   ///��Ϣ����

extern map<string, CThostFtdcOrderField> OrderMap;//ί���б�

extern map<string, CThostFtdcQuoteField> QuoteMap;//Q���б�

extern vector<CThostFtdcInputOrderField> InputOrderList;//ί��¼��

extern vector<CThostFtdcInputOrderActionField> InputOrderActionList;//ί�в����б�

extern vector<CThostFtdcInvestorPositionField> InvestorPositionList;//�ֲ��б�

extern vector<CThostFtdcTradeField> TradeList; //�ɽ��б�;

extern bool InitFinished ;

extern int CheckEnClose(string InstrumentID, TThostFtdcDirectionType Direction);

extern void PositionFrozen(string InstrumentID, TThostFtdcDirectionType Direction, int Volume);

extern void PositionChange(string InstrumentID, TThostFtdcDirectionType Direction, TThostFtdcOffsetFlagType OffsetFlag, int Volume);

extern vector<string> Strsplit(string str, string separator);
#endif