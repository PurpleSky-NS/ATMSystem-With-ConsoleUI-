#pragma once

#include "../UIFrame/UIFrame.h"
#include "RegisterSurface.h"
#include "LoginSurface.h"
#include "WithdrawSurface.h"
#include "SaveSurface.h"
#include "QueryMoneySurface.h"
#include "BillingSurface.h"
#include "CalculatorSurface.h"

class MainSurface : public MenuSurface
{
public:
	MainSurface() :
		MenuSurface("主菜单")
	{
		AddItem("用户注册", []() {SurfaceManager::GetInstance().Start(new RegisterSurface); });
		AddItem("用户登录", []() {SurfaceManager::GetInstance().Start(new LoginSurface); });
		AddItem("取钱功能", [&]() {
			if (CheckLogin())
				SurfaceManager::GetInstance().Start(new WithdrawSurface);
			});
		AddItem("存钱功能", [&]() {
			if (CheckLogin())
				SurfaceManager::GetInstance().Start(new SaveSurface);
			});
		AddItem("收支明细", [&]() {
			if (CheckLogin())
				SurfaceManager::GetInstance().Start(new BillingSurface); });
		AddItem("查询余额", [&]() {
			if (CheckLogin())
				SurfaceManager::GetInstance().Start(new QueryMoneySurface); });
		AddItem("计算器", []() {SurfaceManager::GetInstance().Start(new CalculatorSurface); });
	}

private:
	bool CheckLogin()
	{
		if (!UserManager::GetInstance().IsLogin())
		{
			cp.DisplayError("请先登录才能使用软件功能");
			Refresh();
			return false;
		}
		return true;
	}
};
