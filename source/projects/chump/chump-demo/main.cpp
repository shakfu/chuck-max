/* 
 * experimental code to integrate chump with chuck-max
 *
 */

#include <array>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <algorithm> 
#include <cctype>
#include <locale>

#include "chuck_version.h"
#include "manager.h"
#include "util.h"

#define BASE_URL "https://chuck.stanford.edu/release/chump/manifest/"

namespace fs = std::filesystem;


struct ChuckQuery {
	fs::path chuck_exe;

	ChuckQuery();
	void ltrim(std::string &s);
	void rtrim(std::string &s);
	std::string exec(std::string cmd);
	fs::path get_chuck_exe();
	std::string query_chuck(std::string query, bool trim);
};

ChuckQuery::ChuckQuery() {
	this->chuck_exe = get_chuck_exe();
}


// Trim from the start (in place)
void ChuckQuery::ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// Trim from the end (in place)
void ChuckQuery::rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

std::string ChuckQuery::exec(std::string cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

fs::path ChuckQuery::get_chuck_exe()
{
	fs::path cwd = fs::current_path();
	fs::path chuck_exe = cwd / "chuck";
	if (!fs::exists(chuck_exe)) {
		throw std::runtime_error("chuck not found!");
	}
	return chuck_exe;
}

std::string ChuckQuery::query_chuck(std::string query, bool trim = true)
{
	std::string cmd = (this->chuck_exe.string() + " --query:" + query);
	std::string res = this->exec(cmd);
	if (trim) {
		this->rtrim(res);
	}
	return res;
}

int main()
{
	ChuckQuery cq;
	std::string version = cq.query_chuck("version");
	ChuckVersion cv = ChuckVersion(version);
	std::cout << cv << std::endl;
	std::cout << version << std::endl;
	std::string chugin_api_version = cq.query_chuck("version_chugin_api");
	ApiVersion av = ApiVersion(chugin_api_version);
	std::cout << av << std::endl;
	std::cout << chugin_api_version << std::endl;

	// get chump dir
	fs::path path = chumpDir();
	// create it
	fs::create_directories(path);
	// get manifest url
	std::string manifest_url = manifestURL(BASE_URL);
	// manager pointer
	Manager *manager = NULL;
	// build manager and run command
	fs::path pkg_path = chumpDir() / "manifest.json";

 	try {
    	// create manager
    	manager =
        	new Manager(pkg_path.string(), path, cv, av, whichOS(), whichArch(),
                    manifest_url, true);

    	if (!validate_manifest(pkg_path)) {
      		throw std::invalid_argument("Manifest file (" + pkg_path.string() +
            	                      "/manifest.json) is invalid");
    	}
  	} catch (const std::exception &e) {
    	// print error message
    	std::cerr << e.what() << std::endl;
    	// done
    	return 1;
  	}
}

