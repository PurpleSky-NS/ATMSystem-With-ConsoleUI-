#pragma once

#include "../UIFrame/UIFrame.h"
#include "../BankSystem/UserManager.h"

class QueryMoneySurface : public Surface
{

	class TextCom :public UIComponent
	{
	public:

		TextCom(const User& user) :
			m_user(user) {}

		void OnPrint()
		{

			cp.DisplayTab(2).DisplayLine("余额查询").DisplayLine()
				.Display("欢迎您 ").Display(m_user.GetCardNum()).DisplayLine(" 的卡持有者").DisplayLine()
				.DisplaySpace(4).Display("您目前的余额为： ").DisplayPrecision(m_user.GetMoney(), 2).DisplayLine(" 元")
				.DisplayLine()
				.DisplayLine("按任意键可退出当前界面");
		}

		bool WillRegisterEvnet()const
		{
			return true;
		}

		void OnEvent(char input)
		{
			if (input > 0)
				SurfaceManager::GetInstance().CloseTop();
		}

	private:
		const User& m_user;
	};

public:

	QueryMoneySurface()
	{
		Add(new TextCom(UserManager::GetInstance().GetLoginUser()));
	}

};
