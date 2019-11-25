﻿#include "UserManager.h"
UserManager UserManager::instance;
UserManager& UserManager::GetInstance()
{
	return instance;
}

bool UserManager::CheckCardNum(const std::string& cardNum)
{
	if (!(cardNum.size() >= 10 && cardNum.size() <= 13)) //卡号10-13位
		return false;
	for (auto i : cardNum)
		if (!isdigit(i))  //卡号纯数字
			return false;
	return true;
}

bool UserManager::CheckPassword(const std::string& password)
{
	if (password.size() < 8) //密码最低8位
		return false;
	unsigned char mask = 0;
	for (size_t i = 0; i < password.size(); ++i)
		if (isalpha(password[i])) //字母
			mask |= 1;
		else if (isdigit(password[i])) //数字
			mask |= 2;
		else if (password[i] == '_') //下划线
			mask |= 4;
		else if (password[i] == '@' && i == password.size() - 3) //倒数第三位必须是@
			mask |= 8;
		else
			return false;
	return mask == 15; //0b1111，满足数字&&字母&&下划线
}

unsigned UserManager::Login(const std::string& cardNum, const std::string& passWord) throw(User::TooManyAttemptsException, FileOperationException)
{
	auto fd = m_users.find(cardNum);
	if (fd == m_users.end())
		return TRY_LOCK_TIMES;
	User* user = (*fd).second;
	if (!user->TryLogin(passWord))//尝试登陆
		return TRY_LOCK_TIMES - user->GetTryTimes();
	Logout();//登出原用户
	m_loginUser = user;
	return 0;
}

void UserManager::Logout()
{
	m_loginUser = nullptr;
}

bool UserManager::ExistUser(const std::string& cardNum) const
{
	return m_users.find(cardNum) != m_users.end();
}

bool UserManager::Register(const std::string& cardNum, const std::string& passWord) throw(UserExistedException, FileOperationException)
{
	if (ExistUser(cardNum))
		throw UserExistedException("用户名已存在");
	User* user = new User(cardNum, passWord);
	if (!Check(*user))
		return false;
	m_users.insert({ cardNum,user });
	std::ofstream usf("users.usr", std::ios::app | std::ios::binary);
	if (!usf.is_open())//没打开，创建文件
	{
		usf.open("users.usr");
		if (usf.is_open())//还没打开，文件系统错误
			throw FileOperationException("users.usr 文件无法打开并且无法创建");
	}
	user->SaveUser(usf);
	usf.close();
	return true;
}

UserManager::UserManager()
{
	Load();
}

UserManager::~UserManager()
{
	for (auto& i : m_users)
		delete i.second;
}

bool UserManager::Check(const User& user)
{
	return CheckCardNum(user.m_cardNum) && CheckPassword(user.m_passWord);
}

void UserManager::Load()throw(FileOperationException)
{
	std::ifstream usf("users.usr", std::ios::binary);
	if (!usf.is_open())//没打开
		return;
	User* user = nullptr;
	while ((user = User::LoadUser(usf)) != nullptr)
		m_users.insert({ user->m_cardNum,user });
	usf.close();
}
