#include "Delete.hpp"
#include "Response.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cstdio>
#include <cerrno>

Delete::Delete(Response &objectRef) : response(objectRef) {}

Delete::~Delete(void) {}

void Delete::buildBody(void)
{
    std::string body = response.getDefaultPage();
    response.setBody(body);
}

int Delete::handleRequest(void)
{
	if (response.requestInfo.permissions.notFound)
		return 404;
    else if (response.requestInfo.pathType == File)
    {
        // Verifica permissões no diretório pai
        std::string parentDir = response.requestInfo.fullPath.substr(0, response.requestInfo.fullPath.find_last_of('/'));
        if (!hasWritePermission(parentDir))
        {
            return 403;  // Forbidden
        }

        // Tenta apagar o arquivo
        if (remove(response.requestInfo.fullPath.c_str()) == 0)
        {
            return 204;  // No Content
        }
        else
        {
            return 500;  // Internal Server Error
        }
    }
    else if (!endsWith(response.requestInfo.requestedRoute, "/"))
    {
        return 409;  // Conflict
    }
    else if (!response.requestInfo.permissions.write)
    {
        return 403;  // Forbidden
    }
    else if (deleteDirectory())
    {
        return 204;  // No Content
    }
    else
    {
        return 500;  // Internal Server Error
    }
}

bool Delete::deleteDirectory(void)
{
    // Verifica permissões no diretório
    if (!hasWritePermission(response.requestInfo.fullPath))
    {
        return false;  // Falta permissão
    }

    DIR *dir = opendir(response.requestInfo.fullPath.c_str());
    if (!dir)
        return false;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string entryName = entry->d_name;

        if (entryName == "." || entryName == "..")
        {
            continue;
        }

        std::string fullPath = response.requestInfo.fullPath + "/" + entryName;

        struct stat entryStat;
        if (stat(fullPath.c_str(), &entryStat) == 0)
        {
            if (S_ISDIR(entryStat.st_mode))
            {
                // Exclui subdiretórios recursivamente
                Delete subDirDelete(response);
                response.requestInfo.fullPath = fullPath;
                if (!subDirDelete.deleteDirectory())
                {
                    closedir(dir);
                    return false;
                }
            }
            else
            {
                // Exclui arquivo
                if (remove(fullPath.c_str()) != 0)
                {
                    closedir(dir);
                    return false;
                }
            }
        }
        else
        {
            closedir(dir);
            return false;
        }
    }

    closedir(dir);
    // Remove o diretório vazio
    if (rmdir(response.requestInfo.fullPath.c_str()) != 0)
        return false;

    return true;
}

bool Delete::hasWritePermission(const std::string &path)
{
    struct stat pathStat;
    if (stat(path.c_str(), &pathStat) != 0)
        return false;

    // Verifica permissões de escrita
    return (pathStat.st_mode & S_IWUSR) || (pathStat.st_mode & S_IWGRP) || (pathStat.st_mode & S_IWOTH);
}
