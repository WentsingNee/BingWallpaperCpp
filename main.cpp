#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <kerbal/container/array.hpp>
#include <kerbal/container/flat_ordered.hpp>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
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
		const std::string & operator()(const BingImg & img) const noexcept
		{
			return img.startdate;
		}
};

using BingImgSet = kerbal::container::flat_ordered<BingImg, std::string, std::less<std::string>, BingImgExtract>;

BingImgSet getBingImgSet()
{
	using MaxNum = kerbal::type_traits::integral_constant<size_t, 10>;
	BingImgSet bingImgSet; {
		bingImgSet.reserve(MaxNum::value);
	}
	std::mutex bingImgSetMutex;

	kerbal::container::array<std::thread, MaxNum::value> threads;

	for (size_t i = 0; i < threads.max_size(); ++i) {
		std::thread & thread = threads[i];
		thread = std::thread([i, &bingImgSet, &bingImgSetMutex](){
			auto r = cpr::Get(cpr::Url{"https://www.bing.com/HPImageArchive.aspx"},
							  cpr::Parameters{
									  {"format", "js"},
									  {"idx",    std::to_string(i + 1)},
									  {"n",      "1"},
									  {"mkt",    "en-Us"}
							  });

			const nlohmann::json json = nlohmann::json::parse(r.text);
			const auto &img = json["images"][0];

			std::lock_guard<std::mutex> guard(bingImgSetMutex);
			bingImgSet.unique_insert(BingImg{
					.startdate = img["startdate"],
					.url = "http://www.bing.com/" + (std::string)img["url"],
					.copyright = img["copyright"],
					.copyrightlink = img["copyrightlink"],
					.hsh = img["hsh"],
			});
		});
	}

	for (std::thread & thread : threads) {
		thread.join();
	}

	return bingImgSet;
}

int main()
{
	using std::cout;
	using std::endl;

	BingImgSet bingImgSet = getBingImgSet();

	cout << "download: " << bingImgSet.size() << " pictures" << endl;
	cout << endl;

	std::filesystem::create_directories("./img");

	for (const auto &e : bingImgSet) {
		cout << "startdate: " << e.startdate << "\n"
				<< "copyright: " << e.copyright << "\n"
				<< "url: " << e.url << "\n"
				<< "search: " << e.copyrightlink << "\n"
				<< "hsh: " << e.hsh << "\n";

		auto r = cpr::Get(cpr::Url{e.url});
		std::ofstream img("./img/" + e.startdate + ".jpg", std::ios::out);
		img << r.text << std::flush;

		cout << endl;
	}
}
