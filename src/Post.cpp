#include "Post.hpp"
#include "Response.hpp"

Post::Post(Response &objectRef) : response(objectRef) {}

Post::~Post(void) {}

void Post::handleRequest()
{}

void Post::upload()
{
	// Handle file upload logic here
}
