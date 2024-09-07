#ifndef PRINTREQUESTINFO_HPP
#define PRINTREQUESTINFO_HPP

#include <iostream>
#include <map>
#include <string>
#include "Request.hpp"

class PrintRequestInfo
{
	public:
		static void printRequestInfo(RequestInfo& request);
		static void	printVectorChar(std::vector<char> &vec, std::string header, std::string path);

	private:
		static const char* pathTypeToString(e_pathType pathType);
		static const char* httpMethodActionToString(e_httpMethodActions action);
};

#endif // PRINTREQUESTINFO_HPP
