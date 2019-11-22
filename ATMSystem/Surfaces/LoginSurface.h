#pragma once

#include "../UIFrame/UIFrame.h"
#include "../BankSystem/UserManager.h"

class LoginSurface :public EditorSurface
{
public:
	LoginSurface() :
		EditorSurface("登陆用户", "欢迎你，请登录系统以开始使用业务功能")
	{
		AddEditableData("CardNum", "银行卡号", "",
			[](char c) {
				return isdigit((unsigned char)c);
			}, [](std::string& input, std::string& errMsg) {
				if (!UserManager::CheckCardNum(input))
				{
					errMsg = "卡号不符合规范";
					return false;
				}
				if (!UserManager::GetInstance().ExistUser(input))
				{
					errMsg = "用户不存在";
					return false;
				}
				return true;
			});
		AddEditableData("Password", "密码", "密码最低8位，必须包含：数字、字母、下划线，且倒数第三位必须是‘@’字符",
			[](char c) {
				return isalnum((unsigned char)c) || c == '_' || c == '@';
			}, [&](std::string& input, std::string& errMsg) {
				if (UserManager::CheckPassword(input))
					return true;
				errMsg = "密码不符合规范";
				return false;
			}, true);
	}
private:
	virtual bool OnPostData(const std::unordered_map<std::string, std::string>& datas)
	{
		try
		{
			if (!UserManager::GetInstance().Login(datas.at("CardNum"), datas.at("Password")))
			{
				cp.DisplayError("密码错误，请重试");
				return false;
			}
		}
		catch (User::TooManyAttemptsException e)
		{
			cp.DisplayError("尝试次数过多，将冻结一天:" + std::string(e.what()));
			return false;
		}
		catch (FileOperationException e)
		{
			cp.DisplayError("文件系统错误:" + std::string(e.what()));
			return false;
		}
		cp.DisplayOK("欢迎回来:" + datas.at("CardNum") + " 卡的持有者");
		return true;
	}
};
