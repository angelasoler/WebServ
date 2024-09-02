#ifndef DELETE_HPP
# define DELETE_HPP

# include "Request.hpp"
# include "IHttpMethod.hpp"

class Response;

class Delete : public IHttpMethod
{
	public:
		Delete(Response &objectRef);
		~Delete(void);

		int handleRequest(void);
	private:
		Response &response;
		bool		hasWritePermission(const std::string &path);
		bool		deleteDirectory(void);
		void	buildBody(void);
};

#endif // DELETE_HPP
