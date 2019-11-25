#pragma once

#include <stdexcept>
#include <string>
#include <fstream>
#include "Billing.h"
#include "TimeUtil.h"
#include "SimpleException.h"

static const unsigned TRY_LOCK_TIMES = 3;

class User
{
	friend class UserManager;
public:
	class TooManyAttemptsException :public std::runtime_error
	{
	public:
		TooManyAttemptsException() = default;
		TooManyAttemptsException(const std::string& msg) :
			runtime_error(msg) {}
	};

	/*加载用户*/
	static User* LoadUser(std::istream& fi);

	/*会抛出TooManyAttemptsException异常，试过多次之后，每次都会抛出该异常，密码正确会清除计数*/
	bool TryLogin(const std::string& passWord) throw(FileOperationException, TooManyAttemptsException);

	/*取钱*/
	bool WithdrawMoney(double v);

	/*存钱*/
	bool SaveMoney(double v);

	inline const std::string& GetCardNum()const;

	inline unsigned short GetTryTimes()const;

	inline double GetMoney()const;

	inline const Billing& GetBilling()const;

	/*存储用户信息*/
	void SaveUser(std::ostream& fo)const;

	/*存储已存在用户信息，如果没有，存到最后*/
	void SaveUser()const throw(FileOperationException);

private:
	unsigned short m_tryTimes;
	double m_money;
	std::string m_cardNum;
	std::string m_passWord;
	Billing m_billing;

	User(const std::string& cardNum, const std::string& passWord, double money = 0, unsigned short tryTimes = 0);

	void Lock() throw(FileOperationException, TooManyAttemptsException);

	void TryUnlock() throw(FileOperationException, TooManyAttemptsException);

};
inline const std::string& User::GetCardNum()const
{
	return m_cardNum;
}
inline unsigned short User::GetTryTimes()const
{
	return m_tryTimes;
}

inline double User::GetMoney() const
{
	return m_money;
}

inline const Billing& User::GetBilling() const
{
	return m_billing;
}
