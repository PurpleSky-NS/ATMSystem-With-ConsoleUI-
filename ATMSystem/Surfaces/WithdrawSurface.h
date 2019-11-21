#pragma once

#include <sstream>
#include "../UIFrame/UIFrame.h"
#include "../BankSystem/UserManager.h"

class WithdrawSurface : public MenuSurface
{

	class MoneyInputSurface : public EditorSurface
	{
	public:
		MoneyInputSurface(WithdrawSurface& surface) :
			EditorSurface("其他金额", "请输入其他金额"), m_withdrawSurface(surface)
		{
			AddEditableData("Money", "金额", "请输入不得超过两位小数", [](char input) {
				return isdigit((unsigned char)input) || input == '.';
				}, [](std::string& input, std::string& err) {
					/*判断重复小数*/
					bool repDot = false;
					for (auto i : input)
						if (i == '.')
						{
							if (repDot)
							{
								err = "输入不符合规范";
								return false;
							}
							else
								repDot = true;
						}
					/*检查小数位数*/
					auto fd = input.rfind('.');
					if (fd != input.npos)//找到小数点
						if (input.size() - fd > 3)//如果小数超过2位
						{
							err = "小数不得超过两位";
							return false;
						}
					return true;
				});
		}

		bool OnPostData(const std::unordered_map<std::string, std::string>& mappingData)
		{
			std::stringstream ss;
			double v = -1;
			ss << mappingData.at("Money");
			ss >> v;
			if (v < 0)
				return false;
			m_withdrawSurface.Withdraw(v, false);
			return true;
		}
	private:
		WithdrawSurface& m_withdrawSurface;
	};

public:
	WithdrawSurface() :
		MenuSurface("请选择你要取的金额")
	{
		AddItem("100", [&]() {Withdraw(100); });
		AddItem("200", [&]() {Withdraw(200); });
		AddItem("300", [&]() {Withdraw(300); });
		AddItem("400", [&]() {Withdraw(400); });
		AddItem("500", [&]() {Withdraw(500); });
		AddItem("1000", [&]() {Withdraw(1000); });
		AddItem("其他金额", [&]() {SurfaceManager::GetInstance().Start(new MoneyInputSurface(*this)); });
	}

	void Withdraw(double v, bool isRefresh = true)
	{
		if (!UserManager::GetInstance().IsLogin())
		{
			SurfaceManager::GetInstance().CloseTop();
			cp.DisplayError("您尚未登陆，请返回登陆，才能使用当前功能");
			return;
		}
		User& user = UserManager::GetInstance().GetLoginUser();
		if (!user.WithdrawMoney(v))
			cp.DisplayError("你的余额不足");
		else
		{
			std::stringstream ss;
			ss << "您已取出" << v << "元";
			cp.DisplayOK(ss.str());
		}
		if (isRefresh)
			Refresh();
	}
};
