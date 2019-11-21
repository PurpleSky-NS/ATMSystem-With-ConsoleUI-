#pragma once

#include "../UIFrame/UIFrame.h"
#include "../BankSystem/UserManager.h"

class RegisterSurface :public EditorSurface
{
public:
	RegisterSurface() :
		EditorSurface("注册用户", "欢迎你，填写以下信息来注册成为我们的新用户，以便于使用我们的服务~")
	{
		AddEditableData("CardNum", "银行卡号", "10到13位的纯数字",
			[](char c) {
				return isdigit((unsigned char)c);
			}, [](std::string& input, std::string& errMsg) {
				if (!UserManager::CheckCardNum(input))
				{
					errMsg = "卡号不符合规范";
					return false;
				}
				if (UserManager::GetInstance().ExistUser(input))
				{
					errMsg = "该用户已存在";
					return false;
				}
				return true;
			});
		AddEditableData("Password", "密码", "必须包含：数字、字母、下划线，且倒数第三位必须是‘@’字符",
			[](char c) {
				return isalnum((unsigned char)c) || c == '_' || c == '@';
			}, [&](std::string& input, std::string& errMsg) {
				if (UserManager::CheckPassword(input))//test
					return true;
				errMsg = "密码不符合规范";
				if (GetInputData("ConfirmPassword") != input)
					SetErrorText("ConfirmPassword", "两次密码输入不一致");
				return false;
			}, true);
		AddEditableData("ConfirmPassword", "确认密码", "必须包含：数字、字母、下划线，且倒数第三位必须是‘@’字符",
			[](char c) {
				return isalnum((unsigned char)c) || c == '_' || c == '@';
			}, [&](std::string& input, std::string& errMsg) {
				if (GetInputData("Password") == input)
					return true;
				errMsg = "两次密码输入不一致";
				return false;
			}, true);
	}
private:
	virtual bool OnPostData(const std::unordered_map<std::string, std::string>& datas)
	{
		try
		{
			if (!UserManager::GetInstance().Register(datas.at("CardNum"), datas.at("Password")))
			{
				cp.DisplayError("提交信息异常，注册失败");
				return false;
			}
		}
		catch (UserManager::UserExistedException e)
		{
			cp.DisplayError("卡号已经存在，无法注册");
			return false;
		}
		catch (FileOperationException e)
		{
			cp.DisplayError("文件系统错误:" + std::string(e.what()));
			return false;
		}
		cp.DisplayOK("恭喜你注册成功");
		return true;
	}
};
