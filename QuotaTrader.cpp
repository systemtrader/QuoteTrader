#include "QuotaTrader.h"
#include "common.h"
#include "Config.h"

//回调类
CMdSpi* pMDUserSpi = nullptr;

CTraderSpi* pTraderUserSpi = nullptr;

string MDFRONT_ADDR;       //行情地址
string TRADERFRONT_ADDR;   //交易地址
string INVESTOR_ID;        //用户
string PASSWORD;           //密码
string BROKERID;           // 经纪公司代码
string StrSpreed;         //价差
string StrVolume;         //报单手数
string strUserProductInfo; //用户端产品信息
string	strAuthCode; ///认证码

///读取合约配置文件
void  SysInit()
{
	const char ConfigFile[] = "config.txt";
	Config configSettings(ConfigFile);

	MDFRONT_ADDR = configSettings.Read("MDAddress", MDFRONT_ADDR);

	TRADERFRONT_ADDR = configSettings.Read("TraderAddress", TRADERFRONT_ADDR);

	BROKERID = configSettings.Read("brokerID", BROKERID);

	INVESTOR_ID = configSettings.Read("user", INVESTOR_ID);

	PASSWORD = configSettings.Read("pwd", PASSWORD);

	strUserProductInfo = configSettings.Read("UserProductInfo", strUserProductInfo);

	strAuthCode = configSettings.Read("AuthCode", strAuthCode);

	StrSpreed = configSettings.Read("spreed", StrSpreed);
	stringstream convert;
	convert << StrSpreed;
	convert >> spreed;

	StrVolume = configSettings.Read("volume", StrVolume);
	stringstream convertvol;
	convertvol << StrVolume;
	convertvol >> volume; 

	//memset(&req, 0, sizeof(req));
	//strcpy(Authenticate.BrokerID, BROKER_ID);
	//strcpy(Authenticate.UserID, INVESTOR_ID);
	//strcpy(Authenticate.UserProductInfo\, PASSWORD);
	//读取配置文件代码
	//std::vector <string> list;
	ifstream inf;
	inf.open("instruments.csv");
	string s;
	while (getline(inf, s))
	{
		vector<string> listStr = Strsplit(s, ",");	
		
		double SettlementPrice;
		stringstream sstr(listStr[1]);
		sstr >> SettlementPrice;	
		
		InstrumentMap.insert(map<string, double>::value_type(listStr[0], SettlementPrice));
		
		//.push_back(make_pair(listStr[0], SettlementPrice));	
		
		CThostFtdcDepthMarketDataField DepthMarketData;		
		
		DepthMarketData.LastPrice = -1;
		DepthMarketData.AskPrice1 = -1;
		DepthMarketData.BidPrice1 = -1;

		//创建对应的map key
		MarketDataField.insert(map<string, CThostFtdcDepthMarketDataField>::value_type(s, DepthMarketData));	
		
		payoff.insert(map<string, int>::value_type(s, 0));
		
		//买报价引用
		BidORDER_REF_present.insert(map<string, string>::value_type(s, ""));
		
		//卖报价引用
		AskORDER_REF_present.insert(map<string, string>::value_type(s, ""));


		//if (s.length()>6)
		//	s.substr(8, 4);
	}
	inf.close();
}
//打印持仓
bool  CheckPositions(string Instrment)
{
	
	     //cerr << "----------------------------Position-----------------------------------"<< endl;
	//LOG(INFO) << "----------------------------Position-----------------------------------" << endl;	
	for (CThostFtdcInvestorPositionField InvestorPosition : InvestorPositionList)
	{
		if (strcmp(InvestorPosition.InstrumentID, Instrment.c_str()) == 0 && InvestorPosition.Position > 0)  //存在持仓，才打印持仓记录
		{
			
			//InvestorPositionList.push_back(*pInvestorPosition);
			//cerr << "[Position]|<" << InvestorPosition.InstrumentID << ">|(2.Long 3.Short):"
			//	<< InvestorPosition.PosiDirection << "|Position:" << InvestorPosition.Position
			///*	<< "|Frozen:"
			//	<< (InvestorPosition.PosiDirection == '2' ? InvestorPosition.ShortFrozen : InvestorPosition.LongFrozen)*/
			//	<< "|Enable:"
			//	<< InvestorPosition.Position - (InvestorPosition.PosiDirection == '2' ? InvestorPosition.ShortFrozen : InvestorPosition.LongFrozen)
			//	<< endl;
			///if 
			
			LOG(INFO)<< "[Position]|<" << InvestorPosition.InstrumentID << ">|(2.Long 3.Short):"
				<< InvestorPosition.PosiDirection << "|Position:" << InvestorPosition.Position
			/*	<< "|Frozen:"
				<< (InvestorPosition.PosiDirection == '2' ? InvestorPosition.ShortFrozen : InvestorPosition.LongFrozen)*/
				<< "|Enable:"
				<< InvestorPosition.Position - (InvestorPosition.PosiDirection == '2' ? InvestorPosition.ShortFrozen : InvestorPosition.LongFrozen)
				<< endl;
			
	/*		cerr << "[Position]|<" << InvestorPosition.InstrumentID << ">|(2.Long3.Short):" << InvestorPosition.PosiDirection << "|Position:" << InvestorPosition.Position
				<< "|LongFrozen:" << InvestorPosition.LongFrozen << "|ShortFrozen:" << InvestorPosition.ShortFrozen << endl;
	   LOG(INFO) << "[Position]|<" << InvestorPosition.InstrumentID << ">|(2.Long3.Short):" << InvestorPosition.PosiDirection << "|Position:" << InvestorPosition.Position
				<< "|LongFrozen:" << InvestorPosition.LongFrozen << "|ShortFrozen:" << InvestorPosition.ShortFrozen << endl;*/
		}
	}

       //	 cerr << "----------------------------------------------------------------------" << endl;
	LOG(INFO) << "----------------------------------------------------------------------" << endl;
	return false;
}
///报单线程 双边报价策略
//void QuotaStrategy()
//{
//	//	/// typedef char TThostFtdcOrderStatusType;  TFtdcOrderStatusType是一个报单状态类型
//	
//	//#define THOST_FTDC_OST_AllTraded '0' 全部成交 
//	//	///
//	//#define THOST_FTDC_OST_PartTradedQueueing '1' 部分成交还在队列中
//	//	///
//	//#define THOST_FTDC_OST_PartTradedNotQueueing '2' 部分成交不在队列中
//	//	///
//	//#define THOST_FTDC_OST_NoTradeQueueing '3' 未成交还在队列中
//	//	///
//	//#define THOST_FTDC_OST_NoTradeNotQueueing '4' 未成交不在队列中
//	//	///
//	//#define THOST_FTDC_OST_Canceled '5' 撤单
//	//	///
//	//#define THOST_FTDC_OST_Unknown 'a' 未知
//	//	///
//	//#define THOST_FTDC_OST_NotTouched 'b' 尚未触发
//	//	///
//	//#define THOST_FTDC_OST_Touched 'c' 已触发
//
//	//市场存在买卖报价	
//	for (pair<string,double> InstrumentPair : Instrumentlist)
//	{
//		
//		string InstrumentID = InstrumentPair.first;
//		TThostFtdcOrderStatusType AskOrderStatus;
//		TThostFtdcOrderStatusType BidOrderStatus;
//		double AskPrice=0;
//		double BidPrice=0;
//		
//		//入场
//		if ((MarketDataField[InstrumentID].BidPrice1 > 0 )&&( MarketDataField[InstrumentID].AskPrice1 > 0)&&
//			(MarketDataField[InstrumentID].BidPrice1 <10000000) && (MarketDataField[InstrumentID].AskPrice1 <10000000))
//		{
//			//卖委托
//			if (AskORDER_REF_present[InstrumentID] != "")
//			{
//				if (OrderMap.count(AskORDER_REF_present[InstrumentID])>0)
//				{
//					AskOrderStatus = OrderMap[AskORDER_REF_present[InstrumentID]].OrderStatus;
//					AskPrice = OrderMap[AskORDER_REF_present[InstrumentID]].LimitPrice;
//				}
//				else
//				{    ///委托被CTP退回
//					AskOrderStatus = 'e';
//				}
//			}
//			else///没有开始报单
//				AskOrderStatus = THOST_FTDC_OST_AllTraded;
//
//		
//			//买委托
//			if (BidORDER_REF_present[InstrumentID] != "")
//			{		
//
//				if (OrderMap.count(BidORDER_REF_present[InstrumentID])>0)
//				{
//					BidOrderStatus = OrderMap[BidORDER_REF_present[InstrumentID]].OrderStatus;
//					BidPrice = OrderMap[BidORDER_REF_present[InstrumentID]].LimitPrice;
//				}
//				else
//				{    ///委托被CTP退回
//
//					BidOrderStatus = 'e';
//				}
//			}
//			else///没有开始报单
//				BidOrderStatus = THOST_FTDC_OST_AllTraded;			
//			
//			///双边都成交重新报单
//			if ((BidOrderStatus == THOST_FTDC_OST_AllTraded) && (AskOrderStatus == THOST_FTDC_OST_AllTraded))
//			{
//				
//				//#define THOST_FTDC_D_Buy 
//				//#define THOST_FTDC_D_Sell		
//				stringstream bidstr;				
//				pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Buy, MarketDataField[InstrumentID].BidPrice1 - spreed/2 , InstrumentID,volume);
//				bidstr << iNextOrderRef;
//				bidstr >> BidORDER_REF_present[InstrumentID];		
//				
//				stringstream askstr;
//				pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Sell, MarketDataField[InstrumentID].AskPrice1 + spreed / 2, InstrumentID, volume);;
//				askstr << iNextOrderRef;
//				askstr >> AskORDER_REF_present[InstrumentID];
//			}			
//			///买入报单成交而卖出报单没有成交
//			if ((BidOrderStatus == THOST_FTDC_OST_AllTraded) && (AskOrderStatus == THOST_FTDC_OST_NoTradeQueueing))
//			{
//				//#define THOST_FTDC_D_Buy 
//				//#define THOST_FTDC_D_Sell			
//				
//				//是否已经是改单
//				
//			if (Ask_refill[InstrumentID].compare(AskORDER_REF_present[InstrumentID])!= 0)				
//				{		
//					//
//			    //pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Sell, MarketDataField[InstrumentID].AskPrice1 + spreed / 2, InstrumentID);	
//				//撤单
//			    pTraderUserSpi->ReqOrderAction(&OrderMap[AskORDER_REF_present[InstrumentID]]);
//				
//				}
//			}
//			///买入报单成交而卖出报单已撤
//			if ((BidOrderStatus == THOST_FTDC_OST_AllTraded) && (AskOrderStatus == THOST_FTDC_OST_Canceled))
//			{
//				
//				//#define THOST_FTDC_D_Buy 
//				//#define THOST_FTDC_D_Sell
//				//是否已经是改单
//				if (Ask_refill[InstrumentID].compare(AskORDER_REF_present[InstrumentID]) != 0)
//				{
//					stringstream ss;				
//					pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Sell, BidPrice + spreed, InstrumentID, volume);
//					ss << iNextOrderRef;
//					ss >> AskORDER_REF_present[InstrumentID];
//					Ask_refill[InstrumentID] = AskORDER_REF_present[InstrumentID];
//				}
//			}
//		
//			///卖出报单成交而买入报单没有成交
//			if ((AskOrderStatus == THOST_FTDC_OST_AllTraded) && (BidOrderStatus == THOST_FTDC_OST_NoTradeQueueing))
//			{
//				//#define THOST_FTDC_D_Buy
//				//#define THOST_FTDC_D_Sell 
//				//是否已经是改单				
//				if (Bid_refill[InstrumentID].compare(BidORDER_REF_present[InstrumentID])!=0)
//				{
//					//撤单
//				   pTraderUserSpi->ReqOrderAction(&OrderMap[BidORDER_REF_present[InstrumentID]]);
//				
//			     }
//			}
//			///卖出报单成交而买入报单已撤
//			if ((AskOrderStatus == THOST_FTDC_OST_AllTraded) && (BidOrderStatus == THOST_FTDC_OST_Canceled))
//			{
//				//#define THOST_FTDC_D_Buy
//				//#define THOST_FTDC_D_Sell 
//
//				//是否已经是改单			
//
//				if (Bid_refill[InstrumentID].compare(BidORDER_REF_present[InstrumentID]) != 0)
//				{
//					stringstream ss;				
//					pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Buy, AskPrice - spreed, InstrumentID, volume);
//					ss << iNextOrderRef;
//					ss >> BidORDER_REF_present[InstrumentID];
//					Bid_refill[InstrumentID] = BidORDER_REF_present[InstrumentID];
//				}
//
//			}
//
//		}
//	}
//}

///报价线程
void QuoteThread(CThostFtdcForQuoteRspField ForQuoteRsp) //  SubscribeForQuoteRsp
{
	stringstream ss;
	ss << pTraderUserSpi->FRONT_ID;
	string FrontID = ss.str();

	ss.clear();
	ss << pTraderUserSpi->SESSION_ID;
	string SessionID = ss.str();
	
	////Quote单发送			
	string QuoteRef=pTraderUserSpi->ReqQuoteInsert(ForQuoteRsp.ForQuoteSysID,
		InstrumentMap[ForQuoteRsp.InstrumentID], ForQuoteRsp.InstrumentID, volume, spreed);

	Sleep(15000);

	pTraderUserSpi->ReqQuoteAction(&(QuoteMap[FrontID + "_" + SessionID + "_" + QuoteRef]));
	//撤销quota单    
}
//回应询价主线程
void RespondInquirie() //  SubscribeForQuoteRsp
{
	while (true)
	{
		if (ForQuoteRspField.size()> 0)
		{
			g_lockqueue.lock();
			CThostFtdcForQuoteRspField ForQuoteRsp = ForQuoteRspField.front();//询价
			ForQuoteRspField.pop();
		    g_lockqueue.unlock();
			//string InstrumentID			
			std::thread quo(QuoteThread,ForQuoteRsp);
			quo.detach();
			///pTraderUserSpi->ReqQuoteInsert(THOST_FTDC_D_Sell, MarketDataField[InstrumentID].AskPrice1 + spreed / 2, InstrumentID, volume);;

		}
	}
}
///消息处理主线程
void mProcess()
{
	while (true)
	{
		if (!MsgQueue.empty())
		{
			Msg msg;
			g_lockqueue.lock();
			
			msg = MsgQueue.front();

			//cerr <<      "----------------------------MSG_BEGIN-------------------------------------" << endl;
			//LOG(INFO) << "----------------------------MSG_BEGIN-------------------------------------" << endl;
			
			//cerr <<      "<消息>||类型(0.委托1.成交2.委托录入3.撤单):" << msg.Msg_Type << "|消息数:" << MsgQueue.size() << endl;
			//LOG(INFO) << "<消息>||类型(0.委托1.成交2.委托录入3.撤单):" << msg.Msg_Type << "|消息数:" << MsgQueue.size() << endl;		

		    cerr << "[Msg] || Size of MsgQueue:" << MsgQueue.size() << endl;
			//LOG(INFO) << "[Msg] || Size of MsgQueue:" << MsgQueue.size() << endl;	
			MsgQueue.pop();			
			g_lockqueue.unlock();			
			//pTraderUserSpi->IsErrorRspInfo(&msg.RspInfo);
			//消息队列处理//
			
			
			
			switch (msg.Msg_Type)
			{
					// 委托回报
				case RtnOrder:
				{
					
					g_lockqueue.lock();
					//根据委托回报的状态计算**********************************************************************				
					TThostFtdcOrderStatusType OrderStatus = 'n';//原委托状态，无委托初始值为n				
					
					int Volume = -1;
					//查找委托列表中是否存在此委托，确定此委托是否为新委托
					stringstream ss;
					ss << msg.RtnOrder.FrontID;
					string FrontID = ss.str();

					ss.clear();
					ss << msg.RtnOrder.SessionID;
					string SessionID = ss.str();

					if (OrderMap.count(FrontID + "_" + SessionID + "_" + string(msg.RtnOrder.OrderRef)) > 0)
					{
						//LOG(INFO) << "OrderStatus:" << OrderStatus << "|Volume" << Volume << endl;
						
						
						OrderStatus = OrderMap[FrontID + "_" + SessionID + "_" + string(msg.RtnOrder.OrderRef)].OrderStatus;
                        ////此次成交量	
						Volume = OrderMap[FrontID + "_" + SessionID + "_" + string(msg.RtnOrder.OrderRef)].VolumeTotal - msg.RtnOrder.VolumeTotal;
								
					}					
					LOG(INFO) << "OrderStatusChange:" << OrderStatus << "--->" << msg.RtnOrder.OrderStatus << "|Volume:" << Volume << endl;
					
					//委托状态改变 || 成交状态改变
					if (OrderStatus!= msg.RtnOrder.OrderStatus || Volume>0)
					{					
						///委托录入 冻结相应的持仓		
						if (OrderStatus == 'n')
						{
							LOG(INFO) << "-------------------------------OrderStatus == 'n'-----------------------------" << endl;
							LOG(INFO) << "OrderID:" << FrontID + "_" + SessionID + "_" + string(msg.RtnOrder.OrderRef) << "|"
								<< "OrderStatus:" << msg.RtnOrder.OrderStatus << "|"
								<< "VolumeTotalOriginal:" << msg.RtnOrder.VolumeTotalOriginal << "|"
								<< "VolumeTotal:" << msg.RtnOrder.VolumeTotal << "|"
								<< "VolumeTraded:" << msg.RtnOrder.VolumeTraded << "|" 
						     	<< "CombOffsetFlag:" << msg.RtnOrder.CombOffsetFlag << "|" << endl;

							//TThostFtdcOffsetFlagType
							if (msg.RtnOrder.CombOffsetFlag[0]!=THOST_FTDC_OF_Open)
								PositionFrozen(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, msg.RtnOrder.VolumeTotalOriginal);
							LOG(INFO) << "--------------------------------------------------------------------------------" << endl;
				      	 }						
						
						//委托全部撤单 恢复冻结的持仓					
						if (msg.RtnOrder.OrderStatus == THOST_FTDC_OST_Canceled)
						{
							LOG(INFO) << "-------------------------------THOST_FTDC_OST_Canceled---------------------------" << endl;
							LOG(INFO) << "OrderID:" << FrontID + "_" + SessionID + "_" + string(msg.RtnOrder.OrderRef) << "|"
								<< "OrderStatus:" << msg.RtnOrder.OrderStatus << "|"
								<< "VolumeTotalOriginal:" << msg.RtnOrder.VolumeTotalOriginal << "|"
								<< "VolumeTotal:" << msg.RtnOrder.VolumeTotal << "|"
								<< "VolumeTraded:" << msg.RtnOrder.VolumeTraded << "|" 
								<< "CombOffsetFlag:" << msg.RtnOrder.CombOffsetFlag << "|" << endl;
							if (msg.RtnOrder.CombOffsetFlag[0]!=THOST_FTDC_OF_Open)
								PositionFrozen(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, -msg.RtnOrder.VolumeTotal);
							LOG(INFO) << "-----------------------------------------------------------------------------------" << endl;
						}

						// 委托全部成交 恢复冻结的持仓 计算持仓
						if (msg.RtnOrder.OrderStatus == THOST_FTDC_OST_AllTraded)
						{
							LOG(INFO) << "-------------------------------THOST_FTDC_OST_AllTraded-----------------------------" << endl;
							LOG(INFO)<< "OrderID:" << FrontID + "_" + SessionID + "_" + string(msg.RtnOrder.OrderRef) << "|"
								<< "OrderStatus:" << msg.RtnOrder.OrderStatus << "|"
								<< "VolumeTotalOriginal:" << msg.RtnOrder.VolumeTotalOriginal << "|"
								<< "VolumeTotal:" << msg.RtnOrder.VolumeTotal << "|"
								<< "VolumeTraded:" << msg.RtnOrder.VolumeTraded << "|" 
								<< "CombOffsetFlag:" << msg.RtnOrder.CombOffsetFlag << "|" << endl;
							
							PositionChange(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, msg.RtnOrder.CombOffsetFlag[0], Volume);
							if (msg.RtnOrder.CombOffsetFlag[0]!= THOST_FTDC_OF_Open)
								PositionFrozen(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, -Volume);
							LOG(INFO) << "-------------------------------------------------------------------------------------" << endl;
						}
					
						//部分成交
						if (msg.RtnOrder.OrderStatus == THOST_FTDC_OST_PartTradedQueueing)
						{
							LOG(INFO) << "-------------------------------THOST_FTDC_OST_PartTradedQueueing----------------------" << endl;
							LOG(INFO) <<"OrderID:" << FrontID + "_" + SessionID + "_" + string(msg.RtnOrder.OrderRef) << "|"
								<< "OrderStatus:" << msg.RtnOrder.OrderStatus << "|"
								<< "VolumeTotalOriginal:" << msg.RtnOrder.VolumeTotalOriginal << "|"
								<< "VolumeTotal:" << msg.RtnOrder.VolumeTotal << "|"
								<< "VolumeTraded:" << msg.RtnOrder.VolumeTraded << "|" 
								<< "CombOffsetFlag:" << msg.RtnOrder.CombOffsetFlag << "|" << endl;
							PositionChange(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, msg.RtnOrder.CombOffsetFlag[0], Volume);
							if (msg.RtnOrder.CombOffsetFlag[0]!=THOST_FTDC_OF_Open)
								PositionFrozen(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, -Volume);
							LOG(INFO) << "-------------------------------------------------------------------------------------" << endl;
						}
						//部分撤单
						if (msg.RtnOrder.OrderStatus == THOST_FTDC_OST_PartTradedNotQueueing)
						{
							LOG(INFO) << "------------------------------THOST_FTDC_OST_PartTradedNotQueueing-------------------" << endl;
							LOG(INFO)<<"OrderID:" << FrontID + "_" + SessionID + "_" + string(msg.RtnOrder.OrderRef) << "|"
								<< "OrderStatus:" << msg.RtnOrder.OrderStatus << "|" 
							          << "VolumeTotalOriginal:" << msg.RtnOrder.VolumeTotalOriginal << "|" 
									  << "VolumeTotal:" << msg.RtnOrder.VolumeTotal << "|"
									  << "VolumeTraded:" << msg.RtnOrder.VolumeTraded << "|"
									  << "CombOffsetFlag:" << msg.RtnOrder.CombOffsetFlag << "|" << endl;
							
							if (msg.RtnOrder.CombOffsetFlag[0]!=THOST_FTDC_OF_Open)
							///PositionChange(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, msg.RtnOrder.CombOffsetFlag[0], msg.RtnOrder.VolumeTraded);
							PositionFrozen(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, -msg.RtnOrder.VolumeTotal);
							
							LOG(INFO) << "---------------------------------------------------------------------------------------" << endl;
						}
					  
						
						/*	cerr <<     "<委托>||编号:" << msg.RtnOrder.OrderSysID << "|<" << msg.RtnOrder.InstrumentID
							<< ">方向(0.买1.卖):" << msg.RtnOrder.Direction << "手数:" << msg.RtnOrder.VolumeTotalOriginal
							<< "|开平(0.开1.平):" << msg.RtnOrder.CombOffsetFlag[0]
							<< "|价格:" << msg.RtnOrder.LimitPrice
							<< "|状态:" << msg.RtnOrder.OrderStatus<< endl;						
						
						LOG(INFO) << "<委托>||编号:" << msg.RtnOrder.OrderSysID << "|<" << msg.RtnOrder.InstrumentID
							<< ">方向(0.买1.卖):" << msg.RtnOrder.Direction << "|手数:" << msg.RtnOrder.VolumeTotalOriginal
							<< "|开平(0.开1.平):" << msg.RtnOrder.CombOffsetFlag[0]
							<< "|价格:" << msg.RtnOrder.LimitPrice
							<< "|状态:" << msg.RtnOrder.OrderStatus << endl;	*/					
						///计算持仓
						if(CheckPositions(msg.RtnOrder.InstrumentID))
						{
							//
						}
						//更新委托列表中的状态
						OrderMap[FrontID + "_" + SessionID + "_" + string(msg.RtnOrder.OrderRef)] = msg.RtnOrder;
				/*		cerr << "----------------------------MSG_END-------------------------------------" << endl;							
				   LOG(INFO) << "----------------------------MSG_END-------------------------------------" << endl;*/
						//执行策略
						//QuotaStrategy();						
					}				
					
					g_lockqueue.unlock();
				
					
					break;
				};		
				// 成交回报
				case RtnQuote:
				{
					stringstream ss;
					ss << msg.RtnQuote.FrontID;
					string FrontID = ss.str();

					ss.clear();
					ss << msg.RtnQuote.SessionID;
					string SessionID = ss.str();

					QuoteMap[FrontID +"_"+SessionID + "_" + msg.RtnQuote.QuoteRef] = msg.RtnQuote;
					
					//g_lockqueue.lock();

					//TradeList.push_back(msg.RtnTrade);

					//PositionChange(msg.RtnTrade.InstrumentID, msg.RtnTrade.Direction, msg.RtnTrade.OffsetFlag, msg.RtnTrade.Volume);

					//PositionFrozen(msg.RtnTrade.InstrumentID, msg.RtnTrade.Direction, msg.RtnTrade.OffsetFlag, -msg.RtnTrade.Volume);

					//LogpInvestorPosition();

					//g_lockqueue.unlock();

					/*	cerr << "<成交>||编号:" << msg.RtnTrade.OrderSysID << "|<" << msg.RtnTrade.InstrumentID
					<< ">方向(0.买1.卖):" << msg.RtnTrade.Direction << "手数:" << msg.RtnTrade.Volume
					<< "|开平(0.开1.平):" << msg.RtnTrade.OffsetFlag
					<< "|价格:" << msg.RtnTrade.Price << endl;

					LOG(INFO) << "<成交>||编号:" << msg.RtnTrade.OrderSysID << "|<" << msg.RtnTrade.InstrumentID
					<< ">方向(0.买1.卖):" << msg.RtnTrade.Direction << "手数:" << msg.RtnTrade.Volume
					<< "|开平(0.开1.平):" << msg.RtnTrade.OffsetFlag
					<< "|价格:" << msg.RtnTrade.Price << endl;

					cerr << "----------------------------MSG_END-------------------------------------" << endl;
					LOG(INFO) << "----------------------------MSG_END-------------------------------------" << endl;*/

					break;
				};
				
				// 成交回报
				case RtnTrade:
				{			
					//g_lockqueue.lock();

					//TradeList.push_back(msg.RtnTrade);
				
					//PositionChange(msg.RtnTrade.InstrumentID, msg.RtnTrade.Direction, msg.RtnTrade.OffsetFlag, msg.RtnTrade.Volume);
				
					//PositionFrozen(msg.RtnTrade.InstrumentID, msg.RtnTrade.Direction, msg.RtnTrade.OffsetFlag, -msg.RtnTrade.Volume);
				
					//LogpInvestorPosition();
				
					//g_lockqueue.unlock();

				/*	cerr << "<成交>||编号:" << msg.RtnTrade.OrderSysID << "|<" << msg.RtnTrade.InstrumentID
						<< ">方向(0.买1.卖):" << msg.RtnTrade.Direction << "手数:" << msg.RtnTrade.Volume
						<< "|开平(0.开1.平):" << msg.RtnTrade.OffsetFlag
						<< "|价格:" << msg.RtnTrade.Price << endl;

					LOG(INFO) << "<成交>||编号:" << msg.RtnTrade.OrderSysID << "|<" << msg.RtnTrade.InstrumentID
						<< ">方向(0.买1.卖):" << msg.RtnTrade.Direction << "手数:" << msg.RtnTrade.Volume
						<< "|开平(0.开1.平):" << msg.RtnTrade.OffsetFlag
						<< "|价格:" << msg.RtnTrade.Price << endl;
					
					cerr << "----------------------------MSG_END-------------------------------------" << endl;
					LOG(INFO) << "----------------------------MSG_END-------------------------------------" << endl;*/

					break;
				};
			
				//报单录入
				case InputOrder:
				{
					break;
				};
				// 报单操作录入
				case InputOrderAction:
				{
					break;
				}
			}

		
		}
		 
	}
}
void main(void)
{	
	google::SetStderrLogging(google::GLOG_FATAL);
	//LOG
	google::InitGoogleLogging("Quote");  //参数为自己的可执行文件名 		

	google::SetLogDestination(google::GLOG_INFO, "./Log/LOG");

	google::SetLogDestination(google::GLOG_WARNING, "./Log/QUOTE");

	//google::SetLogDestination(google::GLOG_ERROR, "./Log/Position");
	//
	//google::SetLogDestination(V(int, default = 0), "./Log/Position");
	//google::SetLogDestination(google::GLOG_ERROR, "./Log/Postion");

	//V(int, default = 0);　//只记录此错误级别和更高错误级别的 VLOG 日志信息
	//
	//google::log_dir(string, default = "");　　//设置日志文件输出目录 

	//google::SetLogDestination(VLOG, "./Log/Portion");

	FLAGS_max_log_size = 100;  //最大日志大小为 50MB

	SysInit();//初始化全局变量

	TThostFtdcCommandTypeType MdFront;

	TThostFtdcCommandTypeType TraderFront;

	//TThostFtdcBrokerIDType	BROKER_ID = "7080";			
	
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(CThostFtdcReqUserLoginField));
	memcpy(req.BrokerID, BROKERID.c_str(), sizeof(TThostFtdcBrokerIDType));
	memcpy(req.UserID, INVESTOR_ID.c_str(), sizeof(TThostFtdcUserIDType));
	memcpy(req.Password, PASSWORD.c_str(), sizeof(TThostFtdcPasswordType));	
	//客户端登录认证信息
	CThostFtdcReqAuthenticateField  Authenticate;
	memset(&Authenticate, 0, sizeof(CThostFtdcReqUserLoginField));
	memcpy(Authenticate.BrokerID, BROKERID.c_str(), sizeof(TThostFtdcBrokerIDType));
	memcpy(Authenticate.UserID, INVESTOR_ID.c_str(), sizeof(TThostFtdcUserIDType));
	memcpy(Authenticate.UserProductInfo, strUserProductInfo.c_str(), sizeof(TThostFtdcProductInfoType));
	memcpy(Authenticate.AuthCode, strAuthCode.c_str(), sizeof(TThostFtdcAuthCodeType));	
	
	memset(&MdFront, 0, sizeof(TThostFtdcCommandTypeType));
	memcpy(MdFront, MDFRONT_ADDR.c_str(), sizeof(TThostFtdcCommandTypeType));

	memset(&TraderFront, 0, sizeof(TThostFtdcCommandTypeType));
	memcpy(TraderFront, TRADERFRONT_ADDR.c_str(), sizeof(TThostFtdcCommandTypeType));

	//初始化交易UserApi
	CThostFtdcTraderApi *pTraderUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();			// 创建UserApi

	pTraderUserSpi = new CTraderSpi(pTraderUserApi, req,Authenticate);

	pTraderUserApi->RegisterSpi((CThostFtdcTraderSpi*)pTraderUserSpi);			// 注册事件类

	pTraderUserApi->SubscribePublicTopic(THOST_TERT_RESTART);				// 注册公有流
	pTraderUserApi->SubscribePrivateTopic(THOST_TERT_RESTART);				// 注册私有流
	pTraderUserApi->RegisterFront(TraderFront);							// connect
	pTraderUserApi->Init();

	// 初始化行情UserApi
	CThostFtdcMdApi* pMDUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			// 创建UserApi

	pMDUserSpi = new CMdSpi(pMDUserApi, req);

	pMDUserApi->RegisterSpi(pMDUserSpi);						// 注册事件类

	pMDUserApi->RegisterFront(MdFront);					// connect

	pMDUserApi->Init();

	//等待系统初始化完成
	while (!InitFinished)
	  Sleep(1000);	
	//执行策略
	//QuotaStrategy();		
	//守护线程	
	std::thread thmProcess(mProcess);
	thmProcess.detach();	
	//回应询价线程
	std::thread thRespondInquirie(RespondInquirie);		
	thRespondInquirie.join();

}




