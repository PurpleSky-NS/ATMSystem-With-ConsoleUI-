#include "Billing.h"
Billing::Billing(const std::string& cardNum) :
	m_cardNum(cardNum)
{
	LoadBilling();
}

void Billing::Add(const BillingMessage& msg) throw(FileOperationException)
{
	SaveBilling(msg);
	m_billings.push_back(msg);
}

inline bool Billing::LoadBilling()
{
	std::ifstream bf(m_cardNum + ".bill");
	if (!bf.is_open())
		return false;
	BillingMessage bm;
	while (bf.read((char*)& bm, sizeof(BillingMessage)))
		if (bf.gcount() == sizeof(BillingMessage))
			m_billings.push_back(bm);
	bf.close();
	return true;
}

void Billing::SaveBilling(const BillingMessage& msg) throw(FileOperationException)
{
	std::ofstream bf(m_cardNum + ".bill", std::ios::app);
	if (!bf.is_open()) //文件不存在
	{
		bf.open(m_cardNum + ".bill");//创建文件
		if (!bf.is_open())//创建失败
			throw FileOperationException(m_cardNum + ".bill 文件打开失败，无法写入");
	}
	bf.write((const char*)& msg, sizeof(BillingMessage));
	bf.close();
}
