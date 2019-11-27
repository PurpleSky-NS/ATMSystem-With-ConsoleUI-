#pragma once

#include <sstream>
#include "../UIFrame/UIFrame.h"
#include "../BankSystem/UserManager.h"

class SaveSurface :public EditorSurface
{
public:
	SaveSurface() :
		EditorSurface("存入金额", "请输入存入的金额")
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
private:
	bool OnPostData(const std::unordered_map<std::string, std::string>& mappingData)
	{
		std::stringstream ss;
		double v = -1;
		ss << mappingData.at("Money");
		ss >> v;
		ss.str("");
		ss.clear();
		if (v < 0)
			return false;
		if (!UserManager::GetInstance().IsLogin())
		{
			cp.DisplayError("您尚未登陆，请返回登陆，才能使用当前功能");
			return true;
		}
		User& user = UserManager::GetInstance().GetLoginUser();
		if (!user.SaveMoney(v))
			cp.DisplayError("存入失败");
		else
		{
			ss << "您已存入" << std::fixed << std::setprecision(2) << v << "元";
			cp.DisplayOK(ss.str());
		}
		return false;
	}
};
