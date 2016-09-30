
#include "common.h"

//*************ȫ�ֱ���*************************************************************

int iRequestID = 0; // ������

double spreed = 2.0;  //�۲�

int volume=1;

int iNextOrderRef = 0;  ///����Ӧ�ñ��
 
queue<CThostFtdcForQuoteRspField> ForQuoteRspField;//ѯ�۶���

map<string, CThostFtdcDepthMarketDataField> MarketDataField;//��Լ����

map<string, double> payoff; //��Լ����

map<string, string> BidORDER_REF_present;//�򱨼�����

map<string, string> AskORDER_REF_present;//����������

map<string, string> Bid_refill;// �򱨼۲���

map<string, string> Ask_refill;// �����۲���

map<string, double> InstrumentMap; //���ĺ�ԼMap

vector<pair<string,double> > Instrumentlist; //���ĺ�Լlist

std::mutex   g_lockqueue;//�̻߳�����

std::condition_variable cv; // ȫ����������.

queue<Msg> MsgQueue;   ///��Ϣ����

map<string,CThostFtdcOrderField> OrderMap;//ί���б�

map<string, CThostFtdcQuoteField> QuoteMap;//Q���б�

vector<CThostFtdcInputOrderField> InputOrderList;//ί��¼��

vector<CThostFtdcTradeField> TradeList; //�ɽ��б�;

vector<CThostFtdcInputOrderActionField> InputOrderActionList;//ί�в����б�

vector<CThostFtdcInvestorPositionField> InvestorPositionList;//�ֲ��б�

bool InitFinished = false;



///����ƽ����
int CheckEnClose(string InstrumentID, TThostFtdcDirectionType Direction)
{
	bool isLong;
	TThostFtdcPosiDirectionType PosiDirectionType;

	if (Direction == THOST_FTDC_D_Sell)
	{
		PosiDirectionType = THOST_FTDC_PD_Long;
		isLong = true;
	}
	else
	{
		PosiDirectionType = THOST_FTDC_PD_Short;
		isLong = false;

	}
	for (CThostFtdcInvestorPositionField& InvestorPosition : InvestorPositionList)
	{
		if ((InvestorPosition.PosiDirection == PosiDirectionType)
			&& (strcmp(InvestorPosition.InstrumentID,InstrumentID.c_str())==0))
		{
		
			return (isLong ? InvestorPosition.Position - InvestorPosition.ShortFrozen : InvestorPosition.Position - InvestorPosition.LongFrozen);
		}
	}
	return 0;
}

//�ֲָ���
void PositionChange(string InstrumentID, TThostFtdcDirectionType Direction, TThostFtdcOffsetFlagType OffsetFlag, int Volume)
{

	TThostFtdcPosiDirectionType PosiDirectionType;
	//����
	if (OffsetFlag == THOST_FTDC_OF_Open)
	{
		if (Direction == THOST_FTDC_D_Buy)
		{
			PosiDirectionType = THOST_FTDC_PD_Long;
		}
		else
		{
			PosiDirectionType = THOST_FTDC_PD_Short;

		}
		bool isFinded = false;
		
		for (CThostFtdcInvestorPositionField& InvestorPosition : InvestorPositionList)
		{
			if ((InvestorPosition.PosiDirection == PosiDirectionType)
				&& (strcmp(InvestorPosition.InstrumentID, InstrumentID.c_str()) == 0))
			{
				InvestorPosition.Position += Volume;
				isFinded = true;
			}
		}
		if (!isFinded)
		{
			///
			CThostFtdcInvestorPositionField newPosition;
			newPosition.PosiDirection = PosiDirectionType;
			strcpy(newPosition.InstrumentID, InstrumentID.c_str());
			newPosition.Position = 0;
			newPosition.LongFrozen = 0;
			newPosition.ShortFrozen = 0;
			newPosition.Position += Volume;
			InvestorPositionList.push_back(newPosition);
		}
	} 
	
	//ƽ��
	else
	{
		if (Direction == THOST_FTDC_D_Buy)
		{
			PosiDirectionType = THOST_FTDC_PD_Short;
		}
		else
		{
			PosiDirectionType = THOST_FTDC_PD_Long;

		}
		for (CThostFtdcInvestorPositionField& InvestorPosition : InvestorPositionList)
		{
			if ((InvestorPosition.PosiDirection == PosiDirectionType)
				&& (strcmp(InvestorPosition.InstrumentID, InstrumentID.c_str()) == 0))
			{
				InvestorPosition.Position = InvestorPosition.Position-Volume;				
			}
		}
	}
}

//�ֲֶ���
void PositionFrozen(string InstrumentID, TThostFtdcDirectionType Direction, int Volume)
{
	bool isLong;
	TThostFtdcPosiDirectionType PosiDirectionType;
	
	//if (OffsetFlag == THOST_FTDC_OF_Open)
	//{
	//	/*if (Direction == THOST_FTDC_D_Buy)
	//	{
	//		PosiDirectionType = THOST_FTDC_PD_Long;
	//		isLong = true;
	//	}
	//	else
	//	{
	//		PosiDirectionType = THOST_FTDC_PD_Short;
	//		isLong = false;
	//	}
	//	for (CThostFtdcInvestorPositionField InvestorPosition : InvestorPositionList)
	//	{
	//		if ((InvestorPosition.PosiDirection == PosiDirectionType)
	//			&& (strcmp(InvestorPosition.InstrumentID, InstrumentID.c_str()) == 0))
	//		{
	//			if (isLong)
	//				InvestorPosition.LongFrozen += Volume;
	//			else
	//				InvestorPosition.ShortFrozen += Volume;
	//		}
	//	}*/
	//}
	//else //ƽ��
	//if (OffsetFlag != THOST_FTDC_OF_Open)
	//{
		if (Direction == THOST_FTDC_D_Buy)
		{
			PosiDirectionType = THOST_FTDC_PD_Short;
			isLong = true;
		}
		else
		{
			PosiDirectionType = THOST_FTDC_PD_Long ;
			isLong = false;

		}
		for (CThostFtdcInvestorPositionField& InvestorPosition : InvestorPositionList)
		{
			if ((InvestorPosition.PosiDirection == PosiDirectionType)
				&& (strcmp(InvestorPosition.InstrumentID, InstrumentID.c_str()) == 0))
			{
				if (isLong)
					InvestorPosition.LongFrozen = InvestorPosition.LongFrozen + Volume;
				else
					InvestorPosition.ShortFrozen = InvestorPosition.ShortFrozen+ Volume;
			}
		}
	//}
}


//ί��״̬��ѯ
void OrderCheck(string InstrumentID, TThostFtdcDirectionType Direction, int Volume)
{
	/*bool isLong;
	TThostFtdcPosiDirectionType PosiDirectionType;
	if (Direction == THOST_FTDC_D_Buy)
	{
		PosiDirectionType = THOST_FTDC_PD_Long;
		isLong = true;
	}
	else
	{
		PosiDirectionType = THOST_FTDC_PD_Short;
		isLong = false;

	}
	for (CThostFtdcInvestorPositionField InvestorPosition : InvestorPositionList)
	{
		if ((InvestorPosition.PosiDirection == PosiDirectionType)
			&& (strcmp(InvestorPosition.InstrumentID, InstrumentID.c_str()) == 0))
		{
			if (isLong)
				InvestorPosition.LongFrozen += Volume;
			else
				InvestorPosition.ShortFrozen += Volume;
		}
	}*/
}

//ע�⣺���ַ���Ϊ��ʱ��Ҳ�᷵��һ�����ַ���  
void split(std::string& s, std::string& delim, std::vector< std::string >* ret)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		ret->push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last>0)
	{
		ret->push_back(s.substr(last, index - last));
	}
}

vector<string> Strsplit(string str, string separator)
{
	vector<string> result;
	int cutAt;
	while ((cutAt = str.find_first_of(separator)) != str.npos)
	{
		if (cutAt > 0)
		{
			result.push_back(str.substr(0, cutAt));
		}
		str = str.substr(cutAt + 1);
	}
	if (str.length() > 0)
	{
		result.push_back(str);
	}
	return result;
}