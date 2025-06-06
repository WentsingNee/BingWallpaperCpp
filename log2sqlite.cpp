/**
 * @file       log2sqlite.cpp
 * @brief
 * @date       2023-02-27
 * @author     Peter
 * @copyright
 *      Peter of [ThinkSpirit Laboratory](http://thinkspirit.org/)
 *   of [Nanjing University of Information Science & Technology](http://www.nuist.edu.cn/)
 *   all rights reserved
 */

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>

#include <SQLiteCpp/SQLiteCpp.h>


int main(int argc, char * argv[])
{
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " in-log.txt out.db" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	char const * in_log = argv[1];
	char const * out_db = argv[2];

	SQLite::Database db(out_db, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
	db.exec(
			"CREATE TABLE IF NOT EXISTS img("
			"	startdate TEXT UNIQUE,"
			"	copyright TEXT,"
			"	url TEXT,"
			"	search TEXT,"
			"	hsh TEXT"
			")"
	);

	std::string s; {
		std::ifstream fin(in_log);
		while (fin) {
			s += fin.get();
		}
	}

//	std::string s = R"(startdate: 20200111
//copyright: 楚格峰下的艾布湖，德国巴伐利亚 (© Marc Hohenleitner/Huber/eStock Photo)
//url: http://www.bing.com//th?id=OHR.Zugspitze_ZH-CN1831794930_1920x1080.jpg&rf=LaDigue_1920x1080.jpg&pid=hp
//search: https://www.bing.com/search?q=%E8%89%BE%E5%B8%83%E6%B9%96&form=hpcapt&mkt=zh-cn
//hsh: f735d6829768887f6d17e84389700418
//
//startdate: 20200112
//copyright: 北京颐和园昆明湖上的十七孔桥，中国 (© Jia Wang/Getty Images)
//url: http://www.bing.com//th?id=OHR.SeventeenSolstice_ZH-CN4901756341_1920x1080.jpg&rf=LaDigue_1920x1080.jpg&pid=hp
//search: https://www.bing.com/search?q=%E5%8D%81%E4%B8%83%E5%AD%94%E6%A1%A5&form=hpcapt&mkt=zh-cn
//hsh: 1ce556e2436538e9de7b6ff4404a8191
//)";


	std::regex rgx(
			"startdate: (.*)\n"
			"copyright: (.*)\n"
			"url: (.*)\n"
			"search: (.*)\n"
			"hsh: (.*)\n"
	);

	auto sbegin = s.cbegin();
	auto send = s.cend();
	int cnt = 0;
	while (true) {
		std::smatch match;
		if (!std::regex_search(sbegin, send, match, rgx)) {
			break;
		}
		const std::string & startdate = match[1];
		const std::string & copyright = match[2];
		const std::string & url = match[3];
		const std::string & search = match[4];
		const std::string & hsh = match[5];

		std::cout << "startdate: " << startdate << "\n"
				  << "copyright: " << copyright << "\n"
				  << "url: " << url << "\n"
				  << "search: " << search << "\n"
				  << "hsh: " << hsh << "\n";
		std::cout << std::endl;

		SQLite::Statement insert_cmd(db, "INSERT INTO img (startdate, copyright, url, search, hsh) VALUES (?, ?, ?, ?, ?)");
		insert_cmd.bind(1, startdate);
		insert_cmd.bind(2, copyright);
		insert_cmd.bind(3, url);
		insert_cmd.bind(4, search);
		insert_cmd.bind(5, hsh);
		try {
			insert_cmd.exec();
		} catch (std::exception const & e) {
			std::cerr << e.what() << std::endl;
		}

		sbegin += match.length();
		++cnt;
	}

	std::cout << cnt << std::endl;
}
