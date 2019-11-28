#pragma once

#include <algorithm>
#include "../UIFrame/UIFrame.h"

class CalculatorSurface : public EditorSurface
{
	class ResultCom : public UIComponent
	{
	public:
		ResultCom(const std::string& result) :
			m_result(result) {}

		void OnPrint()
		{
			if (!m_result.empty())
				cp.DisplaySpace(4).DisplayLine("√ 计算结果为：")
				.DisplaySpace(8).DisplayLine(m_result);
		}

	private:
		const std::string& m_result;
	};

public:

	CalculatorSurface() :
		EditorSurface("大数计算器", "请输入两个操作数来互相加减")
	{
		auto judgeNumChar = [](char c) {return isdigit((unsigned char)c) || c == '-'; };
		auto judgeNumStr = [](std::string& input, std::string& err) {
			size_t fd = input.find_first_not_of('0');
			if (fd == input.npos) //前面是0的话排除
				input = "0";
			else if (fd != 0)
				input = input.substr(fd);
			/*检查‘-’是否符合规范*/
			fd = 0;
			for (auto i : input)//检查多重‘-’
				if (i == '-')
					if (fd == input.npos)
					{
						err = "过多的‘-’,格式不符合规范";
						return false;
					}
					else
						fd = input.npos;
			if (fd == input.npos)//检查‘-’是不是在开头
				if (input[0] != '-')
				{
					err = "‘-’应该在数字前面";
					return false;
				}
			return true;
		};
		AddEditableData("Num1", "左操作数", "请输入一个整数", judgeNumChar, judgeNumStr, false);
		AddEditableData("Operator", "运算符", "‘+’或者‘-’", [](char c) {
			return c == '+' || c == '-';
			}, [](std::string& input, std::string& err) {
				if (input.size() != 1)
				{
					err = "请正确输入单个运算符";
					return false;
				}
				return true;
			});
		AddEditableData("Num2", "右操作数", "请输入一个整数", judgeNumChar, judgeNumStr);
		Add(new ResultCom(m_result));
	}

private:
	std::string m_result;

	virtual bool OnPostData(const std::unordered_map<std::string, std::string>& mappingData)
	{
		std::string num1 = mappingData.at("Num1");
		std::string num2 = mappingData.at("Num2");
		const std::string& op = mappingData.at("Operator");

		if (op == "-") //相减
		{
			if (num1[0] == '-') //num1是负数
			{
				num1 = num1.substr(1);
				if (num2[0] == '-') //num2是负数
				{
					num2 = num2.substr(1);
					m_result = CalculateSub(num2, num1, false);//-l-(-r)->r-l
				}
				else //num2是正数
					m_result = CalculateAdd(num1, num2, true);//-l-r->-(r+l)
			}
			else //num1是正数
				if (num2[0] == '-') //num2是负数
				{
					num2 = num2.substr(1);
					m_result = CalculateAdd(num1, num2, false);//(l-(-r))->l+r
				}
				else //num2是正数
					m_result = CalculateSub(num1, num2, false);//-l+r->r-l
		}
		else //+
		{
			if (num1[0] == '-') //num1是负数
			{
				num1 = num1.substr(1);
				if (num2[0] == '-') //num2是负数
				{
					num2 = num2.substr(1);
					m_result = CalculateAdd(num1, num2, true);//-l+(-r)->-(l+r)
				}
				else //num2是正数
					m_result = CalculateSub(num2, num1, false);//-l+r->r-l
			}
			else //num1是正数
				if (num2[0] == '-') //num2是负数
				{
					num2 = num2.substr(1);
					m_result = CalculateSub(num1, num2, false);//l-r
				}
				else //num2是正数
					m_result = CalculateAdd(num1, num2, false); //l+r
		}
		return false;
	}

	/*传入一个较大数和一个较小数与运算方式计算，返回计算结果*/
	std::string CalculateBase(std::string bigger, std::string smaller, bool isAdd)
	{
		std::string result;
		result.reserve(bigger.size());
		std::reverse(bigger.begin(), bigger.end());
		std::reverse(smaller.begin(), smaller.end());
		if (isAdd)
			for (size_t i = 0; i < bigger.size(); ++i)
				result += bigger[i] + (i < smaller.size() ? smaller[i] - '0' : 0);
		else
			for (size_t i = 0; i < bigger.size(); ++i)
				result += bigger[i] - (i < smaller.size() ? smaller[i] - '0' : 0);

		for (size_t i = 0; i < result.size(); ++i)
		{
			if (result[i] > '9') //大于9，进位
			{
				if (i + 1 < result.size())
					++result[i + 1];
				else
					result += '1';
				result[i] -= 10;
			}
			else if (result[i] < '0') //小于0，退位
			{
				--result[i + 1];
				result[i] += 10;
			}
		}
		std::reverse(result.begin(), result.end());
		auto fd = result.find_first_not_of('0');
		if (fd == result.npos)
			result = "0";
		else if (fd != 0)
			result = result.substr(fd);
		return result;
	}

	/*返回两个数字的大小关系*/
	int TestBigger(const std::string& test, const std::string& other)
	{
		if (test.size() != other.size())
			return (int)test.size() - (int)other.size();
		for (size_t i = 0; i < test.size(); ++i)
			if (test[i] != other[i])
				return (int)test[i] - (int)other[i];
		return 0;
	}

	std::string CalculateAdd(const std::string& left, const std::string& right, bool isNeg)
	{
		return (isNeg ? "-" : "") + (TestBigger(left, right) > 0 ? CalculateBase(left, right, true) : CalculateBase(right, left, true));
	}

	std::string CalculateSub(const std::string& left, const std::string& right, bool isNeg)
	{
		int res = TestBigger(left, right);
		if (res == 0)
			return "0";
		else if (res > 0) //第一个大
			return (isNeg ? "-" : "") + CalculateBase(left, right, false);//45-5
		return (isNeg ? "" : "-") + CalculateBase(right, left, false); //5-6=-(6-5)
	}

};
