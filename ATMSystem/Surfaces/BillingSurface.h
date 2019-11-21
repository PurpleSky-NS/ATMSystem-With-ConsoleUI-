#pragma once

#include "../UIFrame/UIFrame.h"
#include "../BankSystem/UserManager.h"
#include "../BankSystem/TimeUtil.h"

class BillingSurface : public TableSurface<Billing::BillingMessage>
{
	class BillingAdapter :public ItemContainer
	{
	public:

		BillingAdapter(const User& user) :
			m_items(user.GetBilling().GetBillings()) {}

		virtual const Billing::BillingMessage& Get(size_t pos)
		{
			return m_items[pos];
		}

		virtual size_t Size()const
		{
			return m_items.size();
		}

		virtual size_t Empty()const
		{
			return m_items.empty();
		}

	private:
		const std::vector<Billing::BillingMessage>& m_items;
	};

public:

	BillingSurface() :
		TableSurface("收支明细"), m_billingsAdapter(UserManager::GetInstance().GetLoginUser())
	{
		SetContainer(&m_billingsAdapter);
	}


	virtual bool OnItemPrint(const Billing::BillingMessage& item, bool isLastItem)
	{
		cp.DisplayRoom((item.type == Billing::Withdraw ? "支出" : "收入"), 9)
			.DisplayRoom(TimeStr(item.time), 22)
			.DisplayRoom(20).DisplayPrecision(item.value, 2)
			.DisplayRoom(20).DisplayPrecision(item.leftValue, 2)
			.DisplayLine().DisplayLine();
		return true;
	}

	/*显示子项标题的时候调用*/
	virtual bool OnItemTitlePrint()
	{
		cp.DisplayRoom("账单类型", 9)
			.DisplayRoom("交易时间", 22)
			.DisplayRoom("交易金额", 20)
			.DisplayRoom("余额", 20)
			.DisplayLine().DisplayLine();
		return false;
	}

private:

	BillingAdapter m_billingsAdapter;

};
