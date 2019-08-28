/*
#include "analysis.h"
//#include "tasks/charts/detailed/afi_detailed_chart.h"

void CheckForDuplicates(const Pages& sorted_pages) {
  auto it = sorted_pages.begin();
  while (it != sorted_pages.end()) {
    it = std::adjacent_find(it, sorted_pages.end(),
                            [](const Page& a, const Page& b) -> bool {
                              return a.second == b.second;
                            });
    if (it != sorted_pages.end()) {
      std::cout << it->first << "  ---  " << (it + 1)->first << "  ---  "
                << "[[" << it->second << "]]\n";
      ++it;
    }
  }
}
/*
template <typename ParseFunctionType>
Pages GetListOfNominatedArticles(const GeneralParameters& parameters,
                                 const std::string& project_page,
                                 const Date& date,
                                 ParseFunctionType& parse_function) {
  WikiServerCacherExtended server_cacher(parameters);
  auto page_text = server_cacher.get(project_page, date);
  return parse_function(date, page_text);
}

Pages CreatePagesFromWikiPages(const wiki::Pages& wiki_pages) {
  Pages pages;
  pages.reserve(wiki_pages.size());
  for (auto p : wiki_pages) {
    pages.push_back(std::make_pair(Date(), p.title));
  }
  return pages;
}
/*
void CheckAfiForDuplicates(const GeneralParameters& parameters,
                           const Date& date) {
  const std::string kProjectPageName = "Википедия:К улучшению";
  WikiServerCacherExtended server_cacher(parameters);
  auto page_text = server_cacher.get(kProjectPageName, date);
  auto pages = ParseImproveMainPage(date, page_text);
  auto ComparePagesOnlyByTitle = [](const Page& a, const Page& b) -> bool {
    return a.second < b.second;
  };
  std::sort(pages.begin(), pages.end(), ComparePagesOnlyByTitle);
  CheckForDuplicates(pages);

  const std::string kCategoryName =
      "Категория:Википедия:Статьи для срочного улучшения";
  auto category_wiki_pages =
      server_cacher.InternalServer().GetPagesInCategory(kCategoryName);
  std::sort(category_wiki_pages.begin(), category_wiki_pages.end(),
            [](const wiki::Page& a, const wiki::Page& b) -> bool {
              return a.title < b.title;
            });
  auto category_pages = CreatePagesFromWikiPages(category_wiki_pages);
  Pages diff1;
  std::set_difference(
      category_pages.begin(), category_pages.end(), pages.begin(), pages.end(),
      std::inserter(diff1, diff1.begin()), ComparePagesOnlyByTitle);

  auto ComparePagesOnlyByDate = [](const Page& a, const Page& b) -> bool {
    return a.first < b.first;
  };
  std::sort(diff1.begin(), diff1.end(), ComparePagesOnlyByDate);

  for (const auto& p : diff1) {
    std::cout << p.first << "  ---  [[" << p.second << "]]\n";
  }

  std::cout << "\n\nDIFF 2" << std::endl;
  Pages diff2;
  std::set_difference(pages.begin(), pages.end(), category_pages.begin(),
                      category_pages.end(), std::inserter(diff2, diff2.begin()),
                      ComparePagesOnlyByTitle);
  std::sort(diff2.begin(), diff2.end(), ComparePagesOnlyByDate);

  for (const auto& p : diff2) {
    std::cout << "[[ВП:К улучшению/" << p.first.ToStringInRussian() << "#"
              << p.second << "|" << p.first << "]]  ---  [[" << p.second
              << "]]\n";
  }
}//*/
