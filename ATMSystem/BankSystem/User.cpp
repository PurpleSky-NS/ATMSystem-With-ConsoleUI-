#include "User.h"
User::User(const std::string& cardNum, const std::string& passWord, double money, unsigned short tryTimes) :
	m_cardNum(cardNum), m_passWord(passWord), m_money(money), m_tryTimes(tryTimes), m_billing(cardNum)
{
	if (m_tryTimes == TRY_LOCK_TIMES) //如果已经上锁，看看时间是否足够解锁
		TryUnlock();
}
void User::Lock() throw(FileOperationException, TooManyAttemptsException)
{
	if (m_tryTimes < TRY_LOCK_TIMES) //只有尝试次数到达上限才上锁
		return;
	std::ofstream lockFileOut(m_cardNum + ".lock");
	if (!lockFileOut.is_open())
		throw FileOperationException(m_cardNum + ".lock 上锁失败，文件无法打开");
	time_t currentTime = CurrentTimeStape();
	lockFileOut.write((const char*)& currentTime, sizeof(currentTime));
	lockFileOut.close();
	throw TooManyAttemptsException("尝试次数过多，冻结一天，冻结时间为[" + TimeStr(currentTime) + "]");
}
void User::TryUnlock() throw(FileOperationException, TooManyAttemptsException)
{
	std::ifstream lockFileIn(m_cardNum + ".lock");
	if (!lockFileIn.is_open()) //如果没有上锁文件，直接解锁
	{
		m_tryTimes = 0;
		SaveUser();
		return;
	}
	time_t lastTime = 0, curentTime = CurrentTimeStape();
	lockFileIn.read((char*)& lastTime, sizeof(lastTime));
	lockFileIn.close();
	if (curentTime - lastTime > 24 * 60 * 60) //如果时间大于一天，解锁
	{
		m_tryTimes = 0;
		std::ofstream(m_cardNum + ".lock");//截断文件
		SaveUser();
		return;
	}
	throw TooManyAttemptsException("你已被冻结，冻结时间为[" + TimeStr(lastTime) + "]");
}
bool User::TryLogin(const std::string& passWord) throw(FileOperationException, TooManyAttemptsException)
{
	if (m_tryTimes == TRY_LOCK_TIMES)//如果已经上锁，尝试能否解锁
		TryUnlock();
	SaveUser();
	if (passWord == m_passWord)
	{
		m_tryTimes = 0;
		SaveUser();
		return true;
	}
	if ((m_tryTimes = (++m_tryTimes > TRY_LOCK_TIMES ? TRY_LOCK_TIMES : m_tryTimes)) == TRY_LOCK_TIMES) //3次尝试了，加上锁文件
	{
		SaveUser();
		Lock();
	}
	SaveUser();
	return false;
}

bool User::WithdrawMoney(double v)
{
	if (m_money - v < 0)
		return false;
	m_money -= v;
	m_billing.Add({ Billing::Withdraw,CurrentTimeStape(),v ,m_money });
	SaveUser();
	return true;
}

bool User::SaveMoney(double v)
{
	if (v < 0)
		return false;
	m_money += v;
	m_billing.Add({ Billing::Save,CurrentTimeStape(),v ,m_money });
	SaveUser();
	return true;
}

User* User::LoadUser(const std::string& cardNum)throw(FileOperationException)
{
	std::ifstream uf(cardNum + ".user");
	if (!uf.is_open())
		throw FileOperationException(cardNum + ".user 文件打开失败，无法读取该文件");
	std::string passWord;
	double money;
	unsigned short tryTimes;
	uf >> passWord >> money >> tryTimes;
	uf.close();
	if (passWord.empty())
		throw FileOperationException(cardNum + ".user 文件数据损坏");
	return new User(cardNum, passWord, money, tryTimes);
}

void User::SaveUser() const throw(FileOperationException)
{
	std::ofstream uf(m_cardNum + ".user");
	if (!uf.is_open())
		throw FileOperationException(m_cardNum + ".user 文件打开失败，无法写入该文件");
	uf << m_passWord << std::endl
		<< m_money << std::endl
		<< m_tryTimes;
	uf.close();
}
