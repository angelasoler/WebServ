#ifndef POST_HPP
# define POST_HPP

# include "Request.hpp"
# include "IHttpMethod.hpp"

class Response;

class Post : public IHttpMethod
{
	public:
		Post(Response &objectRef);
		~Post(void);

		int handleRequest(void);
	private:
		Response 	&response;
		std::string uploadPath;

		std::string	getFileName(int index);
		bool		isValidRoute(void);
		int			upload(void);
		void		buildBody(void);
		int			uploadMultipart(void);
};
bool	writeFile(const std::string& content, const std::string& fileName);
void	add_post_log(const std::string& content);

#endif // POST_HPP
