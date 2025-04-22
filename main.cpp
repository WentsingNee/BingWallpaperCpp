#include <kerbal/container/avl_ordered.hpp>
#include <kerbal/container/static_vector.hpp>
#include <kerbal/type_traits/integral_constant.hpp>

#include <cpr/cpr.h>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <SQLiteCpp/SQLiteCpp.h>

#include <iostream>
#include <filesystem>


struct BingImg
{
		std::string startdate;
		std::string url;
		std::string copyright;
		std::string copyrightlink;
		std::string hsh;
};

struct BingImgExtract
{
		typedef std::string const & key_type;

		key_type operator()(const BingImg & img) const noexcept
		{
			return img.startdate;
		}
};

using MaxNum = kerbal::type_traits::integral_constant<std::size_t, 10>;
using BingImgSet = kerbal::container::avl_ordered<BingImg, BingImgExtract>;

BingImgSet getBingImgSet()
{
	BingImgSet bingImgSet;

	kerbal::container::static_vector<cpr::AsyncResponse, MaxNum::value> asrs;

	for (size_t i = 0; i < asrs.max_size(); ++i) {
		asrs.emplace_back(
				cpr::GetAsync(
					cpr::Url{"https://www.bing.com/HPImageArchive.aspx"},
					cpr::Parameters{
						{"format", "js"},
						{"idx",    std::to_string(i)},
						{"n",      "1"},
						{"mkt",    "en-Us"}
				}));
	}
	for (auto & asr : asrs) {
		cpr::Response r = asr.get();
		nlohmann::json json;
		try {
			json = nlohmann::json::parse(r.text);
		} catch (const nlohmann::detail::parse_error & e) {
			std::cerr << "error occurred with receive str: " << r.text << std::endl;
			continue;
		}
		const auto &img = json["images"][0];

		bingImgSet.insert_unique(BingImg{
				.startdate = img["startdate"],
				.url = "http://www.bing.com/" + (std::string)img["url"],
				.copyright = img["copyright"],
				.copyrightlink = img["copyrightlink"],
				.hsh = img["hsh"],
		});
	}

	return bingImgSet;
}



int main()
{
	using std::cout;
	using std::endl;

	std::filesystem::create_directories("./img");

	BingImgSet bingImgSet = getBingImgSet();

	cout << "download: " << bingImgSet.size() << " pictures" << endl;
	cout << endl;

	SQLite::Database db("bingimg.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
	db.exec(
			"CREATE TABLE IF NOT EXISTS img("
			"	startdate TEXT UNIQUE,"
			"	copyright TEXT,"
			"	url TEXT,"
			"	search TEXT,"
			"	hsh TEXT"
			")"
	);

	for (const auto &e : bingImgSet) {
		cout << fmt::format(
			"startdate: {}\n"
			"copyright: {}\n"
			"url: {}\n"
			"search: {}\n"
			"hsh: {}\n",
			e.startdate,
			e.copyright,
			e.url,
			e.copyrightlink,
			e.hsh
		);

		auto r = cpr::Get(cpr::Url{e.url});
		std::ofstream img(fmt::format("./img/{}.jpg", e.startdate), std::ios::out);
		img << r.text << std::flush;

		SQLite::Statement insert_cmd(db, "INSERT INTO img (startdate, copyright, url, search, hsh) VALUES (?, ?, ?, ?, ?)");
		insert_cmd.bind(1, e.startdate);
		insert_cmd.bind(2, e.copyright);
		insert_cmd.bind(3, e.url);
		insert_cmd.bind(4, e.copyrightlink);
		insert_cmd.bind(5, e.hsh);
		try {
			insert_cmd.exec();
		} catch (std::exception const & e) {
			std::cerr << e.what() << std::endl;
		}

		cout << endl;
	}
}
