#pragma warning(disable : 4996)
#include "TraderSpi.h"
#include <windows.h>
#include <iostream>
#include "common.h"
#include "Config.h"
using namespace std;

TThostFtdcOrderRefType	ORDER_REF;	//报单引用
TThostFtdcOrderRefType	EXECORDER_REF;	//执行宣告引用
TThostFtdcOrderRefType	FORQUOTE_REF;	//询价引用
TThostFtdcOrderRefType	QUOTE_REF;	//报价引用

// 流控判断
bool IsFlowControl(int iResult)
{
	return ((iResult == -2) || (iResult == -3));
}

void CTraderSpi::OnFrontConnected()
{
	cerr << "--->>> " << "OnFrontConnected" << endl;
	
	///客户端认证请求
	//ReqAuthenticate();
	ReqUserLogin();
}

///CTP认证
void CTraderSpi::ReqAuthenticate()
{
	//CThostFtdcReqAuthenticateField  Authenticate;
	//memset(&req, 0, sizeof(req));
	//strcpy(Authenticate.BrokerID, BROKER_ID);
	//strcpy(Authenticate.UserID, INVESTOR_ID);
	//strcpy(Authenticate.UserProductInfo\, PASSWORD);	
	//int iResult = pTraderUserApi->ReqAuthenticate(&Authenticate, ++iRequestID);

	//cerr << "--->>> 发送用户登录认证请求: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
}
//CTP认证回调函数
void CTraderSpi::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	/*cerr << "OnRspAuthenticate" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		
		cerr << "<认证>||客户端认证成功！" << endl;
		ReqUserLogin();
	}*/
	
}

void CTraderSpi::ReqUserLogin()
{
	//CThostFtdcReqUserLoginField req;
	//memset(&req, 0, sizeof(req));
	//strcpy(req.BrokerID, BROKER_ID);
	//strcpy(req.UserID, INVESTOR_ID);
	//strcpy(req.Password, PASSWORD);
	int iResult = pTraderUserApi->ReqUserLogin(&reqLoginField, ++iRequestID);
	cerr << "--->>> ReqUserLogin:" << iResult << ((iResult == 0) ? ", Success" : ",Fail") << endl;
}



void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		
		cerr << "<Login>||Login successful" << endl;
		// 保存会话参数
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
	    iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		iNextOrderRef++;
		sprintf(ORDER_REF, "%d", iNextOrderRef);
		
		sprintf(EXECORDER_REF, "%d", 1);
		
		sprintf(FORQUOTE_REF, "%d", 1);
		
		sprintf(QUOTE_REF, "%d", 1);
		///获取当前交易日
		cerr << "--->>>GetTradingDay=" << pTraderUserApi->GetTradingDay() << endl;
		///投资者结算结果确认
		ReqSettlementInfoConfirm();
	}
}

void CTraderSpi::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	
	memset(&req, 0, sizeof(req));

	strcpy(req.BrokerID, reqLoginField.BrokerID);
	
	strcpy(req.InvestorID, reqLoginField.UserID);
	
	int iResult = pTraderUserApi->ReqSettlementInfoConfirm(&req, ++iRequestID);
	
	cerr << "--->>> ReqSettlementInfoConfirm: " << iResult << ((iResult == 0) ? ",Success":",Fail") << endl;
}

void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspSettlementInfoConfirm" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///请求查询合约
		ReqQryTradingAccount();
	}
}

void CTraderSpi::ReqQryInstrument()
{
	//CThostFtdcQryInstrumentField req;
	//memset(&req, 0, sizeof(req));
	//strcpy(req.InstrumentID, INSTRUMENT_ID);
	//while (true)
	//{
	//	int iResult = pTraderUserApi->ReqQryInstrument(&req, ++iRequestID);
	//	if (!IsFlowControl(iResult))
	//	{
	//		cerr << "--->>> 请求查询合约: "  << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
	//		break;
	//	}
	//	else
	//	{
	//		cerr << "--->>> 请求查询合约: "  << iResult << ", 受到流控" << endl;
	//		Sleep(1000);
	//	}
	//} // while
}

void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspQryInstrument" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///请求查询合约
		//ReqQryTradingAccount();
	}
}

void CTraderSpi::ReqQryTradingAccount()
{
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, reqLoginField.BrokerID);
	strcpy(req.InvestorID, reqLoginField.UserID);
	while (true)
	{
		int iResult = pTraderUserApi->ReqQryTradingAccount(&req, ++iRequestID);
		if (!IsFlowControl(iResult))
		{
			cerr << "--->>> ReqQryTradingAccount:" << iResult << ((iResult == 0) ? ",Success":",Fail") << endl;
			break;
		}
		else
		{
			cerr << "--->>> ReqQryTradingAccount: "  << iResult << ", FlowControl" << endl;
			Sleep(1000);
		}
	} // while
}

void CTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspQryTradingAccount" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		//请求查询投资者委托
		ReqQryInvestorPosition();		
		 //ReqQryOrder();
	}
}

void CTraderSpi::ReqQryOrder()
{
	CThostFtdcQryOrderField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, reqLoginField.BrokerID);
	strcpy(req.InvestorID, reqLoginField.UserID);
	while (true)
	{
		int iResult = pTraderUserApi->ReqQryOrder(&req, ++iRequestID);
		if (!IsFlowControl(iResult))
		{
			cerr << "--->>> ReqQryOrder:" << iResult << ((iResult == 0) ? ",Success" : ",Fail") << endl;
			break;
		}
		else
		{
			cerr << "--->>> ReqQryOrder: " << iResult << ", FlowControl" << endl;
			Sleep(1000);
		}
	} // while
}
//查询委托
void CTraderSpi::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//cerr << "--->>> " << "OnRspQryOrder" << endl;
	if (!IsErrorRspInfo(pRspInfo))
	{  
		if (pOrder != nullptr&&pOrder->OrderStatus != THOST_FTDC_OST_AllTraded&&pOrder->OrderStatus != THOST_FTDC_OST_Canceled)
		{//更新委托列表
			g_lockqueue.lock();			
			//过滤委托
			//if ((FRONT_ID == pOrder->FrontID) && SESSION_ID == pOrder->SessionID) 
			//{
			stringstream ss;
			ss << pOrder->FrontID;
			string FrontID = ss.str();
			
			ss.clear();
			ss << pOrder->SessionID;
			string SessionID = ss.str();			
			OrderMap[FrontID+"_"+SessionID + "_" + string(pOrder->OrderRef)] = *pOrder;
			cerr << "[Order]|OrderID:" << FrontID + "_" + SessionID + "_" + string(pOrder->OrderRef) << "|<" << pOrder->InstrumentID
					<< ">|(Bid 0 Ask 1):" << pOrder->Direction 
					<< "|CombOffsetFlag:" << pOrder->CombOffsetFlag
					<< "|VolumeTotalOriginal:" << pOrder->VolumeTotalOriginal
					<< "|VolumeTotal:" << pOrder->VolumeTotal
					<< "|VolumeTraded:" << pOrder->VolumeTraded					
					<< "|LimitPrice:" << pOrder->LimitPrice
					<< "|OrderStatus:" << pOrder->OrderStatus				
					<< endl;
			LOG(INFO) << "[Order]|OrderID:" << FrontID + "_" + SessionID + "_" + string(pOrder->OrderRef) << "|<" << pOrder->InstrumentID
					<< ">|(Bid 0 Ask 1):" << pOrder->Direction
					<< "|CombOffsetFlag:" << pOrder->CombOffsetFlag
					<< "|VolumeTotalOriginal:" << pOrder->VolumeTotalOriginal
					<< "|VolumeTotal:" << pOrder->VolumeTotal
					<< "|VolumeTraded:" << pOrder->VolumeTraded
					<< "|LimitPrice:" << pOrder->LimitPrice
					<< "|OrderStatus:" << pOrder->OrderStatus				
					<< endl;
			//}
			g_lockqueue.unlock(); 
		}		
		if (bIsLast)
		{        
			LOG(INFO) << "--------------------------------------------------------------------------------" << endl;
			//请求查询投资者持仓
			ReqQryInvestorPosition();		

		}
	}
	
}

void CTraderSpi::ReqQryInvestorPosition()
{
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, reqLoginField.BrokerID);
	strcpy(req.InvestorID, reqLoginField.UserID);

	//strcpy(req.InstrumentID, INSTRUMENT_ID);
	while (true)
	{
		int iResult = pTraderUserApi->ReqQryInvestorPosition(&req, ++iRequestID);
		if (!IsFlowControl(iResult))
		{
			cerr << "--->>> ReqQryInvestorPosition: " << iResult << ((iResult == 0) ? ",Success" : ",Fail") << endl;
			break;
		}
		else
		{
			cerr << "--->>> ReqQryInvestorPosition: "  << iResult << ", FlowControl" << endl;
			Sleep(1000);
		}
	} // while
}

void CTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//cerr << "--->>> " << "OnRspQryInvestorPosition" << endl;
	if (!IsErrorRspInfo(pRspInfo))
	{
		if (pInvestorPosition != nullptr)
		{
		//更新持仓列表
		g_lockqueue.lock();		
		//存在委托 CTP 会出现持仓为0而冻结持仓不为0的情况，需要清理一下。		
		if (pInvestorPosition->YdPosition > 0)
		{
			/*	pInvestorPosition->LongFrozen = 0;
				pInvestorPosition->ShortFrozen = 0;
				}*/				
			
			///持仓恢复到昨仓
			pInvestorPosition->Position = pInvestorPosition->YdPosition;
			//冻结持仓恢复为0
			pInvestorPosition->ShortFrozen = 0;

			pInvestorPosition->LongFrozen = 0;
			
		    InvestorPositionList.push_back(*pInvestorPosition);			
			
			cerr << "[Position]|<" << pInvestorPosition->InstrumentID << ">|(2.Long 3.Short):"
				<< pInvestorPosition->PosiDirection << "|Position:" << pInvestorPosition->Position
		/*		<< "|Frozen:" 
				<< (pInvestorPosition->PosiDirection == '2' ? pInvestorPosition->ShortFrozen : pInvestorPosition->LongFrozen)*/
				<< "|Enable:" 
				<< pInvestorPosition->Position - (pInvestorPosition->PosiDirection == '2' ? pInvestorPosition->ShortFrozen : pInvestorPosition->LongFrozen)
				<< endl;
			LOG(INFO) << "[Position]|<" << pInvestorPosition->InstrumentID << ">|(2.Long 3.Short):"
				<< pInvestorPosition->PosiDirection << "|Position:" << pInvestorPosition->Position
		/*		<< "|Frozen:"
				<< (pInvestorPosition->PosiDirection == '2' ? pInvestorPosition->ShortFrozen : pInvestorPosition->LongFrozen)*/
				<< "|Enable:"
				<< pInvestorPosition->Position - (pInvestorPosition->PosiDirection == '2' ? pInvestorPosition->ShortFrozen : pInvestorPosition->LongFrozen)				
				<< endl;
		}
		g_lockqueue.unlock();
	    }
	}
	///
	if (bIsLast)
	{
		LOG(INFO) << "--------------------------------------------------------------------------------" << endl;
		/////初始化完成
		//std::unique_lock <std::mutex> lck(g_lockqueue);
		InitFinished = true; // 设置全局标志位为 true.
		//cv.notify_all(); // 唤醒所有线程.		
	}
}

void CTraderSpi::ReqOrderInsert(TThostFtdcDirectionType DIRECTION, TThostFtdcPriceType LIMIT_PRICE, string InstrumentID,int volume)
{
	
		CThostFtdcInputOrderField req;		
		
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, reqLoginField.BrokerID);
		strcpy(req.InvestorID, reqLoginField.UserID);
		///合约代码
		strcpy(req.InstrumentID, (char*)InstrumentID.data());
						
		iNextOrderRef++;
						
		sprintf(ORDER_REF, "%d", iNextOrderRef);
						
		///报单引用
		strcpy(req.OrderRef, ORDER_REF);				
						
		///用户代码
		//	TThostFtdcUserIDType	UserID;
		///报单价格条件: 限价
		req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		///买卖方向: 
		req.Direction = DIRECTION;
		int volumn = CheckEnClose(InstrumentID, DIRECTION);

		if (CheckEnClose(InstrumentID, DIRECTION) > 0)
		{
			///组合开平标志: 平仓
			req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;//THOST_FTDC_OF_Open;
		}
		else
			//组合开平标志: 开仓
			req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
						
						
		///组合投机套保标志
		req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
						
		///价格
		req.LimitPrice = LIMIT_PRICE;
						
		///数量: 1
		req.VolumeTotalOriginal = volume;
		///有效期类型: 当日有效
		req.TimeCondition = THOST_FTDC_TC_GFD;
		///GTD日期
		//	TThostFtdcDateType	GTDDate;
		///成交量类型: 任何数量
		req.VolumeCondition = THOST_FTDC_VC_AV;
		///最小成交量: 1
		req.MinVolume = 1;
		///触发条件: 立即
		req.ContingentCondition = THOST_FTDC_CC_Immediately;
		///止损价
		//	TThostFtdcPriceType	StopPrice;
		///强平原因: 非强平
		req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
		///自动挂起标志: 否
		req.IsAutoSuspend = 0;
		///业务单元
		//	TThostFtdcBusinessUnitType	BusinessUnit;
		///请求编号
		//	TThostFtdcRequestIDType	RequestID;
		///用户强评标志: 否
		req.UserForceClose = 0;//				
						
		int iResult = pTraderUserApi->ReqOrderInsert(&req, ++iRequestID);						
						
		if (DIRECTION == THOST_FTDC_D_Buy)
		{
			cerr << ">>>>>> 买入<" << InstrumentID << ">|价格：" << LIMIT_PRICE << "录入请求: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
			LOG(INFO) << ">>>>>> 买入<" << InstrumentID << ">|价格：" << LIMIT_PRICE << "录入请求: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
		}
		else
		{
			cerr << ">>>>>> 卖出<" << InstrumentID << ">|价格：" << LIMIT_PRICE << "录入请求: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
			LOG(INFO) << ">>>>>> 卖出<" << InstrumentID << ">|价格：" << LIMIT_PRICE << "录入请求: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
						
		}
}

// 报单请求回应（AVAILABLE） 
void CTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

	//如果通过CTP验证，报单录入正确，则不会进入该回调，
	cerr << "--->>> " << "OnRspOrderInsert" << endl;

	IsErrorRspInfo(pRspInfo);

	Msg MsgOrderInsert;

	//MsgType Type = RtnTrade;

	MsgOrderInsert.Msg_Type = InputOrder;

	MsgOrderInsert.InputOrder = *pInputOrder;

	MsgOrderInsert.RspInfo = *pRspInfo;

	g_lockqueue.lock();

	MsgQueue.push(MsgOrderInsert);

	g_lockqueue.unlock();

}

void CTraderSpi::ReqOrderAction(CThostFtdcOrderField *pOrder)
{
	//static bool ORDER_ACTION_SENT = false;		//是否发送了报单

	//if (ORDER_ACTION_SENT)
	//	return;

	CThostFtdcInputOrderActionField req;

	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, reqLoginField.BrokerID);
	strcpy(req.InvestorID, reqLoginField.UserID);

	///报单操作引用
	//	TThostFtdcOrderActionRefType	OrderActionRef;
	///报单引用
	strcpy(req.OrderRef, pOrder->OrderRef);
	///请求编号
	//	TThostFtdcRequestIDType	RequestID;
	///前置编号
	req.FrontID = FRONT_ID;
	///会话编号
	req.SessionID = SESSION_ID;

	///交易所代码
	//	TThostFtdcExchangeIDType	ExchangeID;
	///报单编号
	//	TThostFtdcOrderSysIDType	OrderSysID;

	///操作标志
	req.ActionFlag = THOST_FTDC_AF_Delete;

	///价格
	//	TThostFtdcPriceType	LimitPrice;
	///数量变化
	//	TThostFtdcVolumeType	VolumeChange;
	///用户代码
	//	TThostFtdcUserIDType	UserID;
	///合约代码

	strcpy(req.InstrumentID, pOrder->InstrumentID);

	int iResult = pTraderUserApi->ReqOrderAction(&req, ++iRequestID);

	cerr     << "<委托撤销>|委托编号:<" << pOrder->OrderRef << ">|价格:" << pOrder->LimitPrice << "|方向(0.买1.卖):" << pOrder->Direction<< iResult << ((iResult == 0) ? ",成功" : ",失败") << endl;
	LOG(INFO)<< "<委托撤销>|委托编号:<" << pOrder->OrderRef << ">|价格:" << pOrder->LimitPrice << "|方向(0.买1.卖):" << pOrder->Direction << iResult << ((iResult == 0) ? ",成功" : ",失败") << endl;


	//}///ORDER_ACTION_SENT = true;
}

// 撤单请求回应 （AVAILABLE）
void CTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

	//如果通过CTP验证，撤单请求正确，则不会进入该回调，
	cerr << "--->>> " << "OnRspOrderAction" << endl;
	
	IsErrorRspInfo(pRspInfo);

	Msg MsgInputOrderAction;	

	MsgInputOrderAction.Msg_Type = InputOrderAction;

	if (pInputOrderAction!=nullptr)
	MsgInputOrderAction.InputOrderAction = *pInputOrderAction;
	
	if (pRspInfo != nullptr)
	MsgInputOrderAction.RspInfo = *pRspInfo;

	g_lockqueue.lock();

	MsgQueue.push(MsgInputOrderAction);

	g_lockqueue.unlock();
}

///  成交通知（AVAILABLE） 
void CTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{


	//Msg MsgTrade;

	////MsgType Type = RtnTrade;

	//MsgTrade.Msg_Type = RtnTrade;
	//if (pTrade!=nullptr)
	//MsgTrade.RtnTrade = *pTrade;

	//g_lockqueue.lock();

	//MsgQueue.push(MsgTrade);

	//g_lockqueue.unlock();

	/*if ((pTrade->Direction == THOST_FTDC_D_Buy))
	if (pTrade->OffsetFlag == THOST_FTDC_OF_Open)
	{
	LongPosition[pTrade->InstrumentID]++;
	LongEnClose[pTrade->InstrumentID]++;
	}
	else
	{
	ShortPosition[pTrade->InstrumentID]--;
	}

	else

	if (pTrade->OffsetFlag == THOST_FTDC_OF_Open)
	{
	ShortPosition[pTrade->InstrumentID]++;
	ShortEnClose[pTrade->InstrumentID]++;
	}
	else
	{
	LongPosition[pTrade->InstrumentID]--;
	}

	if ((pTrade->Direction == THOST_FTDC_D_Buy) && (InstrumentStatus[pTrade->InstrumentID] == "ALLNOT"))
	{
	InstrumentStatus[pTrade->InstrumentID] = "BID";

	cerr << "OnRtnTrade | 合约：<" << pTrade->InstrumentID << "> | 方向 ：买入 | 成交价格：" << pTrade->Price<< "" << endl;
	LOG(INFO) << "OnRtnTrade | 合约：<" << pTrade->InstrumentID << "> | 方向 ：买入 | 成交价格：" << pTrade->Price << "" << endl;

	cerr << "OnRtnTrade | 合约: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;
	LOG(INFO) << "OnRtnTrade | 合约: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;

	return;
	}
	if ((pTrade->Direction == THOST_FTDC_D_Sell) && (InstrumentStatus[pTrade->InstrumentID] == "ALLNOT"))
	{

	InstrumentStatus[pTrade->InstrumentID] = "ASK";

	cerr << "OnRtnTrade | 合约：<" << pTrade->InstrumentID << "> | 方向 ：卖出 | 成交价格：" << pTrade->Price << "" << endl;
	LOG(INFO) << "OnRtnTrade | 合约：<" << pTrade->InstrumentID << "> | 方向 ：卖出 | 成交价格：" << pTrade->Price << "" << endl;

	cerr << "OnRtnTrade | 合约: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;
	LOG(INFO) << "OnRtnTrade | 合约: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;

	return;
	}

	if ((pTrade->Direction == THOST_FTDC_D_Buy) && (InstrumentStatus[pTrade->InstrumentID] == "ASK"))
	{
	InstrumentStatus[pTrade->InstrumentID] = "NONE";

	cerr << "OnRtnTrade | 合约：<" << pTrade->InstrumentID << "> | 方向 ：买入 | 成交价格：" << pTrade->Price << "" << endl;
	LOG(INFO) << "OnRtnTrade | 合约：<" << pTrade->InstrumentID << "> | 方向 ：买入 | 成交价格：" << pTrade->Price << "" << endl;

	cerr << "OnRtnTrade | 合约: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;
	LOG(INFO) << "OnRtnTrade | 合约: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;
	return;
	}
	if ((pTrade->Direction == THOST_FTDC_D_Sell) && (InstrumentStatus[pTrade->InstrumentID] == "ASK_Change"))
	{

	InstrumentStatus[pTrade->InstrumentID] = "NONE";

	cerr << "OnRtnTrade | 合约：<" << pTrade->InstrumentID << "> | 方向 ：卖出 | 成交价格：" << pTrade->Price << "" << endl;
	LOG(INFO) << "OnRtnTrade | 合约：<" << pTrade->InstrumentID << "> | 方向 ：卖出 | 成交价格：" << pTrade->Price << "" << endl;

	cerr << "OnRtnTrade | 合约: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;
	LOG(INFO) << "OnRtnTrade | 合约: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;
	return;
	}
	if ((pTrade->Direction == THOST_FTDC_D_Buy) && (InstrumentStatus[pTrade->InstrumentID] == "BID_Change"))
	{

	InstrumentStatus[pTrade->InstrumentID] = "NONE";
	cerr << "OnRtnTrade | 合约：<" << pTrade->InstrumentID << "> | 方向 ：买入 | 成交价格：" << pTrade->Price << "" << endl;
	LOG(INFO) << "OnRtnTrade | 合约：<" << pTrade->InstrumentID << "> | 方向 ：买入 | 成交价格：" << pTrade->Price << "" << endl;

	cerr << "OnRtnTrade | 合约: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;
	LOG(INFO) << "OnRtnTrade | 合约: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;

	return;

	}
	if ((pTrade->Direction == THOST_FTDC_D_Sell) && (InstrumentStatus[pTrade->InstrumentID] == "BID"))
	{
	InstrumentStatus[pTrade->InstrumentID] = "NONE";

	cerr << "OnRtnTrade | 合约：<" << pTrade->InstrumentID << "> | 方向 ：卖出 | 成交价格：" << pTrade->Price << "" << endl;
	LOG(INFO) << "OnRtnTrade | 合约：<" << pTrade->InstrumentID << "> | 方向 ：卖出 | 成交价格：" << pTrade->Price << "" << endl;

	cerr << "OnRtnTrade | 合约: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;
	LOG(INFO) << "OnRtnTrade | 合约: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;

	return;
	}*/

}

///报单通知 AVAILABLE
void CTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	//过滤委托
	//if ((FRONT_ID == pOrder->FrontID) && (SESSION_ID == pOrder->SessionID))
	//{
		g_lockqueue.lock();

		Msg MsgOrder;

		//MsgType Type = RtnTrade;

		MsgOrder.Msg_Type = RtnOrder;

		MsgOrder.RtnOrder = *pOrder;

		MsgQueue.push(MsgOrder);

		g_lockqueue.unlock();
//	}
	
	//	/////////////////////////////////////////////////////////////////////////
	//	///TFtdcOrderStatusType是一个报单状态类型
	//	/////////////////////////////////////////////////////////////////////////
	//	///全部成交
	//#define THOST_FTDC_OST_AllTraded '0'
	//	///部分成交还在队列中
	//#define THOST_FTDC_OST_PartTradedQueueing '1'
	//	///部分成交不在队列中
	//#define THOST_FTDC_OST_PartTradedNotQueueing '2'
	//	///未成交还在队列中
	//#define THOST_FTDC_OST_NoTradeQueueing '3'
	//	///未成交不在队列中
	//#define THOST_FTDC_OST_NoTradeNotQueueing '4'
	//	///撤单
	//#define THOST_FTDC_OST_Canceled '5'
	//	///未知
	//#define THOST_FTDC_OST_Unknown 'a'
	//	///尚未触发
	//#define THOST_FTDC_OST_NotTouched 'b'
	//	///已触发
	//#define THOST_FTDC_OST_Touched 'c'
	//typedef char TThostFtdcOrderStatusType;	
	//	///报单已经提交	
	//cerr << " OnRtnOrder |合约：<" << pOrder->InstrumentID << "> | 方向 ：" << isOpen << "买入  报单状态 ：报单已提交" << endl;
	//string isOpen = "";
	///OrderStatus状态修改为成交前会先再次调用，会再次修改可平仓，//当OrderStatus状态修改为成交修改为成交后，应该将可平仓数复原
	//if (pOrder->OrderStatus == THOST_FTDC_OST_NoTradeQueueing)
	//	//&& (pOrder->OrderSubmitStatus != THOST_FTDC_OSS_Accepted))
	//{
	//	if (pOrder->Direction == THOST_FTDC_D_Buy)
	//	{
	//		BidORDER_REF[pOrder->InstrumentID] = pOrder->OrderRef;
	//		if (pOrder->CombOffsetFlag[0] != THOST_FTDC_OF_Open)
	//		{
	//			ShortEnClose[pOrder->InstrumentID]--;
	//			isOpen = "平仓";
	//		}
	//		cerr << " OnRtnOrder |合约：<" << pOrder->InstrumentID << "> | 方向 ：" << isOpen << "买入  报单状态 ：报单已提交" << endl;
	//		LOG(INFO) << " OnRtnOrder |合约：<" << pOrder->InstrumentID << "> | 方向 ：" << isOpen << "买入  报单状态 ：报单已提交" << endl;
	//		cerr << "OnRtnOrder | 合约: <" << pOrder->InstrumentID << "> | 卖持可用 : " << ShortEnClose[pOrder->InstrumentID] << endl;
	//		LOG(INFO) << "OnRtnOrder | 合约: <" << pOrder->InstrumentID << "> | 卖持可用 : " << ShortEnClose[pOrder->InstrumentID] << endl;
	//	}
	//	if (pOrder->Direction == THOST_FTDC_D_Sell)
	//	{
	//		string isOpen = "";
	//		AskORDER_REF[pOrder->InstrumentID] = pOrder->OrderRef;
	//if (pOrder->CombOffsetFlag[0] != THOST_FTDC_OF_Open)
	//		{
	//			LongEnClose[pOrder->InstrumentID]--;
	//		}
	//		cerr << " OnRtnOrder |合约：<" << pOrder->InstrumentID << "> | 方向 ：" << isOpen << "卖出  报单状态 ：报单已提交" << endl;
	//		LOG(INFO) << " OnRtnOrder |合约：<" << pOrder->InstrumentID << "> | 方向 ：" << isOpen << "卖出  报单状态 ：报单已提交" << endl;
	//		cerr << "OnRtnOrder | 合约: <" << pOrder->InstrumentID << "> | 买持可用 : " << ShortEnClose[pOrder->InstrumentID] << endl;
	//		LOG(INFO) << "OnRtnOrder | 合约: <" << pOrder->InstrumentID << "> | 买持可用 : " << ShortEnClose[pOrder->InstrumentID] << endl;
	//	}
	//}
	////报单成交，更正错误修改的可平仓数
	//if (pOrder->OrderStatus == THOST_FTDC_OST_AllTraded)
	//	//&& (pOrder->OrderSubmitStatus != THOST_FTDC_OSS_Accepted))
	//{
	//	if (pOrder->Direction == THOST_FTDC_D_Buy)
	//	{
	//		//BidORDER_REF[pOrder->InstrumentID] = pOrder->OrderRef;
	//		if (pOrder->CombOffsetFlag[0] != THOST_FTDC_OF_Open)
	//		{
	//			ShortEnClose[pOrder->InstrumentID]++;
	//		}
	//	}
	//	if (pOrder->Direction == THOST_FTDC_D_Sell)
	//	{
	//		//	string isOpen = "";
	//		//AskORDER_REF[pOrder->InstrumentID] = pOrder->OrderRef;
	//		if (pOrder->CombOffsetFlag[0] != THOST_FTDC_OF_Open)
	//		{
	//			LongEnClose[pOrder->InstrumentID]++;
	//		}
	//	}
	//}
	//////
	/////撤单提交
	//if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled)
	//{
	//	if (pOrder->Direction == THOST_FTDC_D_Buy)
	//	{
	//		BidORDER_REF[pOrder->InstrumentID] = pOrder->OrderRef;
	//		if (pOrder->CombOffsetFlag[0] != THOST_FTDC_OF_Open)
	//		{
	//			ShortEnClose[pOrder->InstrumentID]++;
	//		}
	//		cerr << " OnRtnOrder |合约：<" << pOrder->InstrumentID << "> | 方向 ：" << isOpen << "买入  撤单状态 ：报单已提交" << endl;
	//		LOG(INFO) << " OnRtnOrder |合约：<" << pOrder->InstrumentID << "> | 方向 ：" << isOpen << "买入  撤单状态 ：报单已提交" << endl;
	//		cerr << "OnRtnOrder | 合约: <" << pOrder->InstrumentID << "> | 卖持可用 : " << ShortEnClose[pOrder->InstrumentID] << endl;
	//		LOG(INFO) << "OnRtnOrder | 合约: <" << pOrder->InstrumentID << "> | 卖持可用 : " << ShortEnClose[pOrder->InstrumentID] << endl;
	//	}
	//	if (pOrder->Direction == THOST_FTDC_D_Sell)
	//	{
	//		AskORDER_REF[pOrder->InstrumentID] = pOrder->OrderRef;
	//		if (pOrder->CombOffsetFlag[0] != THOST_FTDC_OF_Open)
	//		{
	//			LongEnClose[pOrder->InstrumentID]++;
	//		}
	//		cerr << " OnRtnOrder |合约：<" << pOrder->InstrumentID << "> | 方向 ：" << isOpen << "卖出  撤单状态 ：报单已提交" << endl;
	//		LOG(INFO) << " OnRtnOrder |合约：<" << pOrder->InstrumentID << "> | 方向 ：" << isOpen << "卖出  撤单状态 ：报单已提交" << endl;
	//		cerr << "OnRtnOrder | 合约: <" << pOrder->InstrumentID << "> | 买持可用 : " << ShortEnClose[pOrder->InstrumentID] << endl;
	//		LOG(INFO) << "OnRtnOrder | 合约: <" << pOrder->InstrumentID << "> | 买持可用 : " << ShortEnClose[pOrder->InstrumentID] << endl;
	//	}
	//}
}
//**********************************************************************************************
//**********************************************************************************************
//执行宣告录入请求
void CTraderSpi::ReqExecOrderInsert()
{
	//CThostFtdcInputExecOrderField req;
	//memset(&req, 0, sizeof(req));
	/////经纪公司代码
	//strcpy(req.BrokerID, BROKER_ID);
	/////投资者代码
	//strcpy(req.InvestorID, INVESTOR_ID);
	/////合约代码
	//strcpy(req.InstrumentID, INSTRUMENT_ID);
	/////报单引用
	//strcpy(req.ExecOrderRef, EXECORDER_REF);
	/////用户代码
	////	TThostFtdcUserIDType	UserID;
	/////数量
	//req.Volume=1;
	/////请求编号
	////TThostFtdcRequestIDType	RequestID;
	/////业务单元
	////TThostFtdcBusinessUnitType	BusinessUnit;
	/////开平标志
	//req.OffsetFlag=THOST_FTDC_OF_Close;//如果是上期所，需要填平今或平昨
	/////投机套保标志
	//req.HedgeFlag=THOST_FTDC_HF_Speculation;
	/////执行类型
	//req.ActionType=THOST_FTDC_ACTP_Exec;//如果放弃执行则填THOST_FTDC_ACTP_Abandon
	/////保留头寸申请的持仓方向
	//req.PosiDirection=THOST_FTDC_PD_Long;
	/////期权行权后是否保留期货头寸的标记
	//req.ReservePositionFlag=THOST_FTDC_EOPF_UnReserve;//这是中金所的填法，大商所郑商所填THOST_FTDC_EOPF_Reserve
	/////期权行权后生成的头寸是否自动平仓
	//req.CloseFlag=THOST_FTDC_EOCF_AutoClose;//这是中金所的填法，大商所郑商所填THOST_FTDC_EOCF_NotToClose

	//int iResult = pTraderUserApi->ReqExecOrderInsert(&req, ++iRequestID);
	//cerr << "--->>> 执行宣告录入请求: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
}
//询价录入请求
void CTraderSpi::ReqForQuoteInsert()
{
	//CThostFtdcInputForQuoteField req;
	//memset(&req, 0, sizeof(req));
	/////经纪公司代码
	//strcpy(req.BrokerID, BROKER_ID);
	/////投资者代码
	//strcpy(req.InvestorID, INVESTOR_ID);
	/////合约代码
	//strcpy(req.InstrumentID, INSTRUMENT_ID);
	/////报单引用
	//strcpy(req.ForQuoteRef, EXECORDER_REF);
	/////用户代码
	////	TThostFtdcUserIDType	UserID;

	//int iResult = pTraderUserApi->ReqForQuoteInsert(&req, ++iRequestID);
	//cerr << "--->>> 询价录入请求: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
}
//报价录入请求
string CTraderSpi::ReqQuoteInsert(TThostFtdcOrderSysIDType ForQuoteSysID, TThostFtdcPriceType LIMIT_PRICE, string InstrumentID, int volume, double spreed)
{
	CThostFtdcInputQuoteField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, reqLoginField.BrokerID);
	strcpy(req.InvestorID, reqLoginField.UserID);
	///合约代码
	strcpy(req.InstrumentID, (char*)InstrumentID.data());

	iNextOrderRef++;

	sprintf(ORDER_REF, "%d", iNextOrderRef);

	///报单引用
	strcpy(req.QuoteRef, ORDER_REF);

	///报单引用
	//strcpy(req.QuoteRef, QUOTE_REF);
	///卖价格
	req.AskPrice = LIMIT_PRICE + spreed;
	///买价格
	req.BidPrice = LIMIT_PRICE - spreed;
	///卖数量
	req.AskVolume = volume;
	///买数量
	req.BidVolume = volume;
	///请求编号
	//TThostFtdcRequestIDType	RequestID;
	///业务单元
	//TThostFtdcBusinessUnitType	BusinessUnit;	
	
	///卖开平标志
	req.AskOffsetFlag=THOST_FTDC_OF_Open;
	///买开平标志
	req.BidOffsetFlag=THOST_FTDC_OF_Open;
	///卖投机套保标志
	req.AskHedgeFlag=THOST_FTDC_HF_Speculation;
	///买投机套保标志
	req.BidHedgeFlag=THOST_FTDC_HF_Speculation;
	
	 strcpy(req.ForQuoteSysID, ForQuoteSysID);
	
	 //req.ForQuoteSysID = ForQuoteSysID;
	
	int iResult = pTraderUserApi->ReqQuoteInsert(&req, ++iRequestID);
	cerr << "--->>> ReqQuoteInsert: " << iResult << ((iResult == 0) ? ", Success" : ", Fail") << endl;
	LOG(WARNING) << "--->>> ReqQuoteInsert: " << iResult << ((iResult == 0) ? ", Success" : ", Fail") << endl;

	return  ((iResult == 0) ? ORDER_REF : "error");

}
void CTraderSpi::OnRspExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//如果执行宣告正确，则不会进入该回调
	cerr << "--->>> " << "OnRspExecOrderInsert" << endl;
	IsErrorRspInfo(pRspInfo);
}
void CTraderSpi::OnRspForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//如果询价正确，则不会进入该回调
	cerr << "--->>> " << "OnRspForQuoteInsert" << endl;
	IsErrorRspInfo(pRspInfo);
}
void CTraderSpi::OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//如果报价正确，则不会进入该回调
	cerr << "--->>> " << "OnRspQuoteInsert" << endl;
	IsErrorRspInfo(pRspInfo);
}
void CTraderSpi::ReqExecOrderAction(CThostFtdcExecOrderField *pExecOrder)
{
	//static bool EXECORDER_ACTION_SENT = false;		//是否发送了报单
	//if (EXECORDER_ACTION_SENT)
	//	return;

	//CThostFtdcInputExecOrderActionField req;
	//memset(&req, 0, sizeof(req));

	/////经纪公司代码
	//strcpy(req.BrokerID,pExecOrder->BrokerID);
	/////投资者代码
	//strcpy(req.InvestorID,pExecOrder->InvestorID);
	/////执行宣告操作引用
	////TThostFtdcOrderActionRefType	ExecOrderActionRef;
	/////执行宣告引用
	//strcpy(req.ExecOrderRef,pExecOrder->ExecOrderRef);
	/////请求编号
	////TThostFtdcRequestIDType	RequestID;
	/////前置编号
	//req.FrontID=FRONT_ID;
	/////会话编号
	//req.SessionID=SESSION_ID;
	/////交易所代码
	////TThostFtdcExchangeIDType	ExchangeID;
	/////执行宣告操作编号
	////TThostFtdcExecOrderSysIDType	ExecOrderSysID;
	/////操作标志
	//req.ActionFlag=THOST_FTDC_AF_Delete;
	/////用户代码
	////TThostFtdcUserIDType	UserID;
	/////合约代码
	//strcpy(req.InstrumentID,pExecOrder->InstrumentID);

	//int iResult = pTraderUserApi->ReqExecOrderAction(&req, ++iRequestID);
	//cerr << "--->>> 执行宣告操作请求: "  << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
	//EXECORDER_ACTION_SENT = true;
}

void CTraderSpi::ReqQuoteAction(CThostFtdcQuoteField *pQuote)
{
	//static bool QUOTE_ACTION_SENT = false;		//是否发送了报单
	//if (QUOTE_ACTION_SENT)
	//	return;

	CThostFtdcInputQuoteActionField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strcpy(req.BrokerID, pQuote->BrokerID);
	///投资者代码
	strcpy(req.InvestorID, pQuote->InvestorID);
	///报价操作引用
	//TThostFtdcOrderActionRefType	QuoteActionRef;
	///报价引用
	strcpy(req.QuoteRef,pQuote->QuoteRef);
	///请求编号
	//TThostFtdcRequestIDType	RequestID;
	///前置编号
	req.FrontID=FRONT_ID;
	///会话编号
	req.SessionID=SESSION_ID;
	///交易所代码
	//TThostFtdcExchangeIDType	ExchangeID;
	///报价操作编号
	//TThostFtdcOrderSysIDType	QuoteSysID;
	///操作标志
	req.ActionFlag=THOST_FTDC_AF_Delete;
	///用户代码
	//TThostFtdcUserIDType	UserID;
	///合约代码
	strcpy(req.InstrumentID,pQuote->InstrumentID);

	int iResult = pTraderUserApi->ReqQuoteAction(&req, ++iRequestID);
	cerr << "--->>> ReqQuoteAction: " << iResult << ((iResult == 0) ? ",Success" : ",Fail") << endl;
	LOG(WARNING)<< "--->>> ReqQuoteAction: " << iResult << ((iResult == 0) ? ",Success" : ",Fail") << endl;
	
	//QUOTE_ACTION_SENT = true;
}
void CTraderSpi::OnRspExecOrderAction(CThostFtdcInputExecOrderActionField *pInpuExectOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//正确的撤单操作，不会进入该回调
	cerr << "--->>> " << "OnRspExecOrderAction" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CTraderSpi::OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInpuQuoteAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//正确的撤单操作，不会进入该回调
	cerr << "--->>> " << "OnRspQuoteAction" << endl;
	IsErrorRspInfo(pRspInfo);
}
//执行宣告通知
void CTraderSpi::OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
	//pExecOrder->
	//
	//
	//
	//if (IsMyExecOrder(pExecOrder))
	//{
 //      if (pExecOrder->ExecResult == THOST_FTDC_OER_Canceled)
	//   if (pExecOrder->OffsetFlag != THOST_FTDC_OF_Open)
	//   {
	//	   LongEnClose[pExecOrder->InstrumentID]++;
	//    }
 //    
	//}
	//cerr << "撤单 | 合约: <" << pExecOrder->InstrumentID << "> | 可用 : " << InstrumentStatus[pExecOrder->InstrumentID] << endl;
	//LOG(INFO) << "撤单 | 合约: <" << pExecOrder->InstrumentID << "> | 可用 : " << InstrumentStatus[pTrade->InstrumentID] << endl;
	//cerr << "--->>> " << "撤单" << endl;
 }
//询价通知
void CTraderSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	//上期所中金所询价通知通过该接口返回；只有做市商客户可以收到该通知
	cerr << "--->>> " << "OnRtnForQuoteRsp"  << endl;
}
//报价通知
void CTraderSpi::OnRtnQuote(CThostFtdcQuoteField *pQuote)
{
	//cerr << "--->>> " << "OnRtnQuote"  << endl;
	g_lockqueue.lock();

	Msg MsgQuote;

	//MsgType Type = RtnTrade;

	MsgQuote.Msg_Type = RtnQuote;

	MsgQuote.RtnQuote = *pQuote;

	MsgQueue.push(MsgQuote);

	g_lockqueue.unlock();
	
	cerr << "[OnRtnQuote]|<" << pQuote->InstrumentID << ">|Bid:" << pQuote->BidPrice 
		<< "|" << pQuote->BidVolume << "|Ask:" << pQuote->AskPrice<<"|" << pQuote->AskVolume<<endl;
	LOG(WARNING) << " [OnRtnQuote]|<" << pQuote->InstrumentID << ">|Bid:" << pQuote->BidPrice
		<< "|" << pQuote->BidVolume << "|Ask:" << pQuote->AskPrice << "|" << pQuote->AskVolume << endl;;
	
	
	//if (IsMyQuote(pQuote))
	//{
	//	if (IsTradingQuote(pQuote))
	//		ReqQuoteAction(pQuote);
	//	else if (pQuote->QuoteStatus == THOST_FTDC_OST_Canceled)
	//		cout << "--->>> 报价撤单成功" << endl;
	//}
}

void CTraderSpi:: OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << "OnFrontDisconnected" << endl;
	cerr << "--->>> Reason = " << nReason << endl;
}
		
void CTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "--->>> " << "OnHeartBeatWarning" << endl;
	cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}
/// 
void CTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspError" << endl;
	IsErrorRspInfo(pRspInfo);
}


// 如果ErrorID != 0, 说明收到了错误的响应 AVAILABLE
bool CTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应

	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
	{
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
		LOG(INFO) << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;

	}
   return bResult;
}

bool CTraderSpi::IsMyOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->FrontID == FRONT_ID) &&
			(pOrder->SessionID == SESSION_ID) &&
			(strcmp(pOrder->OrderRef, ORDER_REF) == 0));
}

///报单录入错误回报
void CTraderSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{

	cerr << "--->>> " << "OnErrRtnOrderInsert" << endl;
	IsErrorRspInfo(pRspInfo);
}


///报单操作错误回报
void CTraderSpi::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
	//
}
bool CTraderSpi::IsMyExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
	return ((pExecOrder->FrontID == FRONT_ID) &&
		(pExecOrder->SessionID == SESSION_ID) &&
		(strcmp(pExecOrder->ExecOrderRef, EXECORDER_REF) == 0));
}

bool CTraderSpi::IsMyQuote(CThostFtdcQuoteField *pQuote)
{
	return ((pQuote->FrontID == FRONT_ID) &&
		(pQuote->SessionID == SESSION_ID) &&
		(strcmp(pQuote->QuoteRef, QUOTE_REF) == 0));
}

bool CTraderSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}

bool CTraderSpi::IsTradingExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
	return (pExecOrder->ExecResult != THOST_FTDC_OER_Canceled);
}

bool CTraderSpi::IsTradingQuote(CThostFtdcQuoteField *pQuote)
{
	return (pQuote->QuoteStatus != THOST_FTDC_OST_Canceled);
}


