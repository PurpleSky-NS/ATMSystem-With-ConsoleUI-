#pragma once

#include <fstream>
#include <unordered_set>
#include "SimpleException.h"
#include "User.h"

class UserManager
{
public:
	class UserExistedException :public std::runtime_error
	{
	public:
		UserExistedException() = default;
		UserExistedException(const std::string& msg) :
			runtime_error(msg) {}
	};
	class UserNotLoginException :public std::runtime_error
	{
	public:
		UserNotLoginException() = default;
		UserNotLoginException(const std::string& msg) :
			runtime_error(msg) {}
	};

	static UserManager& GetInstance();

	/*检查卡号是否符合标准*/
	static bool CheckCardNum(const std::string& cardNum);

	/*检查密码是否符合标准*/
	static bool CheckPassword(const std::string& password);

	/*登陆，返回剩余几次机会，若为0则代表登陆成功，会抛出TooManyAttemptsException异常*/
	unsigned Login(const std::string& cardNum, const std::string& passWord) throw(User::TooManyAttemptsException, FileOperationException);

	/*登出并保存用户信息*/
	void Logout();

	/*是否存在该用户*/
	bool ExistUser(const std::string& cardNum)const;

	/*注册，会抛出CardNumHasExisted异常,返回注册的对象指针，若为false则为注册失败*/
	bool Register(const std::string& cardNum, const std::string& passWord) throw(UserExistedException, FileOperationException);

	/*获取登陆用户*/
	inline User& GetLoginUser()const throw(UserNotLoginException);

	inline bool IsLogin()const;

	inline bool Empty()const;

	inline size_t Size()const;

private:
	static UserManager instance;

	std::unordered_set<std::string> m_users;
	User* m_loginUser = nullptr;

	UserManager();
	UserManager(const UserManager&) = delete;
	UserManager(UserManager&&) = delete;

	/*检查用户是否合法*/
	static bool Check(const User& user);

	/*加载所有用户名*/
	void Load() throw(FileOperationException);
};

inline User& UserManager::GetLoginUser()const throw(UserNotLoginException)
{
	if (m_loginUser == nullptr)
		throw UserNotLoginException("用户未登录");
	return *m_loginUser;
}

inline bool UserManager::IsLogin() const
{
	return m_loginUser != nullptr;
}

inline bool UserManager::Empty() const
{
	return m_users.empty();
}

inline size_t UserManager::Size() const
{
	return m_users.size();
}
