#pragma once
#include "ThostTraderApi\ThostFtdcTraderApi.h"
#include "glog/logging.h" 
#include "GlobalVars.h"
#include  <map> 
#include <vector>
#include <thread> 
#include <chrono>
#include <mutex>
using namespace std;
class CTraderSpi : public CThostFtdcTraderSpi
{
public:	
	//���캯��
	CTraderSpi(CThostFtdcTraderApi* api, CThostFtdcReqUserLoginField req, 
		CThostFtdcReqAuthenticateField  Authenticate) :pTraderUserApi(api), reqLoginField(req), Authenticate(Authenticate){};
	
	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();

	///�ͻ�����֤��Ӧ
	virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///�����ѯ��Լ��Ӧ
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯ�ʽ��˻���Ӧ
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯͶ���ֲ߳���Ӧ
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//ί�в�ѯ��Ӧ
	virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///����¼��������Ӧ
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///ִ������¼��������Ӧ
	virtual void OnRspExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///ѯ��¼��������Ӧ
	virtual void OnRspForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///����¼��������Ӧ
	virtual void OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///��������������Ӧ
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///ִ���������������Ӧ
	virtual void OnRspExecOrderAction(CThostFtdcInputExecOrderActionField *pInputExecOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///���۲���������Ӧ
	virtual void OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInputQuoteAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///����Ӧ��
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ���������
	virtual void OnFrontDisconnected(int nReason);
		
	///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	virtual void OnHeartBeatWarning(int nTimeLapse);
	
	///����֪ͨ
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	///ִ������֪ͨ
	virtual void OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder);

	///ѯ��֪ͨ
	virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);

	///����֪ͨ
	virtual void OnRtnQuote(CThostFtdcQuoteField *pQuote);
	
	///�ɽ�֪ͨ
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);


	///������ʵ��û������
    //******************************************************************************************************

	///����¼�����ر�
	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);

	///������������ر�
	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);


public:
	///�û���¼����
	void ReqUserLogin();
	///Ͷ���߽�����ȷ��
	void ReqSettlementInfoConfirm();
	///�����ѯ��Լ
	void ReqQryInstrument();
	///�����ѯ�ʽ��˻�
	void ReqQryTradingAccount();
	///�����ѯͶ���ֲ߳�
	void ReqQryInvestorPosition();
	///�����ѯί��
	void ReqQryOrder();
	///����¼������
	void ReqOrderInsert(TThostFtdcDirectionType DIRECTION, TThostFtdcPriceType LIMIT_PRICE, string InstrumentID, int volume);
	///ִ������¼������
	void ReqExecOrderInsert();
	///ѯ��¼������
	void ReqForQuoteInsert();	
	//����¼������
	string ReqQuoteInsert(TThostFtdcOrderSysIDType ForQuoteSysID, TThostFtdcPriceType LIMIT_PRICE, string InstrumentID, int volume, double spreed);
		//ReqQuoteInsert(TThostFtdcOrderSysIDType ForQuoteSysID, TThostFtdcPriceType LIMIT_PRICE, string InstrumentID, int volume, double spreed);
	///������������
	void ReqOrderAction(CThostFtdcOrderField *pOrder);
	///ִ�������������
	void ReqExecOrderAction(CThostFtdcExecOrderField *pExecOrder);
	///���۲�������
	void ReqQuoteAction(CThostFtdcQuoteField *pQuote);
	// �Ƿ��յ��ɹ�����Ӧ
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
	// �Ƿ��ҵı����ر�
	bool IsMyOrder(CThostFtdcOrderField *pOrder);
	// �Ƿ��ҵ�ִ������ر�
	bool IsMyExecOrder(CThostFtdcExecOrderField *pExecOrder);
	// �Ƿ��ҵı���
	bool IsMyQuote(CThostFtdcQuoteField *pQuote);
	// �Ƿ����ڽ��׵ı���
	bool IsTradingOrder(CThostFtdcOrderField *pOrder);	
	// �Ƿ�δ������ִ������
	bool IsTradingExecOrder(CThostFtdcExecOrderField *pExecOrder);
	// �Ƿ�δ�����ı���
	bool IsTradingQuote(CThostFtdcQuoteField *pQuote);

	void CTraderSpi::ReqAuthenticate();
	TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��
	TThostFtdcSessionIDType	SESSION_ID;	//�Ự���
private:

	CThostFtdcReqUserLoginField reqLoginField;

	CThostFtdcReqAuthenticateField  Authenticate;

	CThostFtdcTraderApi* pTraderUserApi;

};