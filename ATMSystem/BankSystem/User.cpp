#include "User.h"
User::User(const std::string& cardNum, const std::string& passWord, double money, unsigned short tryTimes) :
	m_cardNum(cardNum), m_passWord(passWord), m_money(money), m_tryTimes(tryTimes), m_billing(cardNum)
{}
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
		remove(std::string(m_cardNum + ".lock").c_str());
		m_tryTimes = 0;
		SaveUser();
		return;
	}
	throw TooManyAttemptsException("你已被冻结，剩余冻结时间为[" + DiffTimeStr((lastTime + 24 * 60 * 60) - curentTime) + "]");
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
	if (floor((m_money - v + 0.005) * 100) < 0) //两位小数精度
		return false;
	m_money -= v;
	m_money = (m_money < 0 ? 0 : m_money); //两位小数精度
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

User* User::LoadUser(std::istream& fi)
{
	std::string cardNum, passWord, buf;
	double money;
	unsigned short tryTimes;
	fi >> cardNum >> passWord;
	fi.get();
	fi.read((char*)& money, sizeof(money));
	fi.read((char*)& tryTimes, sizeof(tryTimes));
	if (!cardNum.empty() && !passWord.empty())
		return new User(cardNum, passWord, money, tryTimes);
	return nullptr;
}

void User::SaveUser(std::ostream& fo) const
{
	fo << m_cardNum << ' ' << m_passWord << ' ';
	fo.write((const char*)& m_money, sizeof(m_money));
	fo.write((const char*)& m_tryTimes, sizeof(m_tryTimes));
	fo << std::endl;
}

void User::SaveUser() const throw(FileOperationException)
{
	bool isFound = false;
	std::fstream usrFile("users.usr", std::ios::in | std::ios::out | std::ios::binary);
	if (!usrFile.is_open())
		throw FileOperationException("users.usr 文件无法打开并且无法创建");
	std::string card, passwrod;
	while (usrFile >> card >> passwrod) //线性查找自己的数据段
	{
		usrFile.get(); //跳空格
		if (card == m_cardNum) //找到了就写
		{
			usrFile.seekp(usrFile.tellg(), std::ios::beg);
			usrFile.write((const char*)& m_money, sizeof(m_money));
			usrFile.write((const char*)& m_tryTimes, sizeof(m_tryTimes));
			isFound = true;
			break;
		}
		else
			usrFile.seekg(sizeof(m_money) + sizeof(m_tryTimes), std::ios::cur);
	}
	if (!isFound)
	{
		usrFile.seekp(0, std::ios::end);
		SaveUser(usrFile);
	}
	usrFile.close();
}
