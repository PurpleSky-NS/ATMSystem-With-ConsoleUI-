#pragma once

#include <stdexcept>
#include <string>
#include <fstream>
#include <ctime>
#include <vector>
#include "SimpleException.h"

class Billing
{
public:

	enum BillingType
	{
		Withdraw = 0,
		Save
	};

	struct BillingMessage
	{
		BillingType type;
		time_t time;
		double value;
		double leftValue;
	};

	Billing(const std::string& cardNum);
	Billing(const Billing&) = delete;
	Billing(Billing&&) = default;

	void Add(const BillingMessage& msg) throw(FileOperationException);

	inline const std::string& GetCardNum()const;

	inline const std::vector<BillingMessage>& GetBillings()const;

	/*从文件载入，文件不存在会导致失败*/
	bool LoadBilling();

private:
	std::string m_cardNum;
	std::vector<BillingMessage> m_billings;

	void SaveBilling(const BillingMessage& msg) throw(FileOperationException);

};
inline const std::string& Billing::GetCardNum()const
{
	return m_cardNum;
}

inline const std::vector<Billing::BillingMessage>& Billing::GetBillings() const
{
	return m_billings;
}
