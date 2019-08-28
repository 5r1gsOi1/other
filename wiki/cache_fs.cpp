
#include "cache_fs.h"

namespace wiki {
struct PageInfo {
  std::string title;
  struct Revision {
    Page::Revision::Header header;
    std::string uuid;

    bool operator<(const Revision& other) const {
      return this->header.timestamp < other.header.timestamp;
    }
  };
  std::set<Revision> revisions;
};

std::optional<PageInfo> ReadPageInfoFromFile(const std::string& filename) {
  std::ifstream stream{filename};
  if (not stream.is_open()) {
    return std::nullopt;
  }
  PageInfo page_info{};
  std::string line{};
  std::getline(stream, line);
  page_info.title = line;
  enum class State : int {
    timestamp = 0,
    user = 1,
    user_id = 2,
    size = 3,
    uuid = 4
  };
  State state{State::timestamp};
  PageInfo::Revision revision{};
  while (std::getline(stream, line)) {
    switch (state) {
      case State::timestamp:
        revision.header.timestamp = line;
        state = State::user;
        continue;
      case State::user:
        revision.header.user = line;
        state = State::user_id;
        continue;
      case State::user_id:
        revision.header.user_id = std::stoi(line);
        state = State::size;
        continue;
      case State::size:
        revision.header.size = std::stoi(line);
        state = State::uuid;
        continue;
      case State::uuid:
        revision.uuid = line;
        state = State::timestamp;
        page_info.revisions.insert(revision);
        continue;
    }
  }
  return std::optional<PageInfo>{page_info};
}

void SavePageInfoToFile(const PageInfo& page_info,
                        const std::string& filename) {
  std::ofstream stream(filename);
  if (stream.is_open()) {
    stream << page_info.title << std::endl;
    for (auto& r : page_info.revisions) {
      stream << r.header.timestamp << std::endl
             << r.header.user << std::endl
             << r.header.user_id << std::endl
             << r.header.size << std::endl
             << r.uuid << std::endl;
    }
    stream << std::endl;
  }
}

std::optional<wiki::Page::Revision> PageCacheFS::GetLastRevision(
    const std::string& page_name) {
  auto dir{index_.find(page_name)};
  if (dir.has_value()) {
    std::string page_info_filename{directory_ + "/" + dir.value() + "/" +
                                   kPageInfoFileName};
    auto page_info{ReadPageInfoFromFile(page_info_filename)};
    if (page_info.has_value()) {
      if (not page_info.value().revisions.empty()) {
        auto& last_revision{--page_info.value().revisions.end()};
        auto content{LoadFileFromDisk(directory_ + "/" + dir.value() + "/" +
                                      last_revision->uuid)};
        if (content.has_value()) {
          return wiki::Page::Revision{last_revision->header, content.value()};
        }
      }
    }
  }
  return std::nullopt;
}

std::optional<wiki::Page::Revision> PageCacheFS::GetRevision(
    const std::string& page_name, const std::string& timestamp) {
  auto dir{index_.find(page_name)};
  if (dir.has_value()) {
    std::string page_info_filename{directory_ + "/" + dir.value() + "/" +
                                   kPageInfoFileName};
    auto page_info{ReadPageInfoFromFile(page_info_filename)};
    if (page_info.has_value()) {
      auto it{std::find_if(page_info.value().revisions.begin(),
                           page_info.value().revisions.end(),
                           [timestamp](const PageInfo::Revision& r) {
                             return r.header.timestamp == timestamp;
                           })};
      if (it != page_info.value().revisions.end()) {
        auto content{
            LoadFileFromDisk(directory_ + "/" + dir.value() + "/" + it->uuid)};
        if (content.has_value()) {
          return wiki::Page::Revision{it->header, content.value()};
        }
      }
    }
  }
  return std::nullopt;
}

std::optional<std::set<std::string>> PageCacheFS::GetAvailableTimestamps(
    const std::string& page_name) {
  auto dir{index_.find(page_name)};
  if (dir.has_value()) {
    std::string page_info_filename{directory_ + "/" + dir.value() + "/" +
                                   kPageInfoFileName};
    auto page_info{ReadPageInfoFromFile(page_info_filename)};
    if (page_info.has_value()) {
      std::set<std::string> timestamps;
      for (const auto& rv : page_info.value().revisions) {
        timestamps.insert(rv.header.timestamp);
      }
      return std::move(timestamps);
    }
  }
  return std::nullopt;
}

bool PageCacheFS::AddRevision(const std::string& page_name,
                              const Page::Revision& revision,
                              const bool override_existing) {
  auto page_uuid{index_.find(page_name)};
  if (page_uuid.has_value()) {
    std::string page_info_filename{directory_ + "/" + page_uuid.value() + "/" +
                                   kPageInfoFileName};
    auto page_info{ReadPageInfoFromFile(page_info_filename)};
    if (page_info.has_value()) {
      auto it{std::find_if(page_info.value().revisions.begin(),
                           page_info.value().revisions.end(),
                           [revision](const PageInfo::Revision& r) {
                             return r.header.timestamp ==
                                    revision.header.timestamp;
                           })};
      std::string revision_uuid{};
      auto new_page_info{page_info.value()};
      if (it == page_info.value().revisions.end()) {
        // TODO: check non-existence of this file
        revision_uuid = GenerateUUID();
        new_page_info.revisions.insert(
            PageInfo::Revision{revision.header, revision_uuid});
        SaveFileToDisk(
            directory_ + "/" + page_uuid.value() + "/" + revision_uuid,
            revision.content);
        SavePageInfoToFile(new_page_info, page_info_filename);
        return true;
      } else if (override_existing) {
        SaveFileToDisk(directory_ + "/" + page_uuid.value() + "/" + it->uuid,
                       revision.content);
        SavePageInfoToFile(new_page_info, page_info_filename);
        return true;
      } else {
        return false;  // revision exists and no override flag supplied
      }
    } else {
      // for some reason _info.txt is invalid or absent so we create a new one
      auto revision_uuid{GenerateUUID()};
      PageInfo new_page_info{
          page_name, {PageInfo::Revision{revision.header, revision_uuid}}};
      SaveFileToDisk(directory_ + "/" + page_uuid.value() + "/" + revision_uuid,
                     revision.content);
      SavePageInfoToFile(new_page_info, directory_ + "/" + page_uuid.value() +
                                            "/" + kPageInfoFileName);
      return true;
    }
  } else {
    auto page_uuid{GenerateUUID()};
    auto revision_uuid{GenerateUUID()};
    PageInfo new_page_info{page_name, {{revision.header, revision_uuid}}};
    CreateDirectoryIfNotExists(directory_ + "/" + page_uuid);
    SaveFileToDisk(directory_ + "/" + page_uuid + "/" + revision_uuid,
                   revision.content);
    SavePageInfoToFile(new_page_info,
                       directory_ + "/" + page_uuid + "/" + kPageInfoFileName);
    index_.add(page_name, page_uuid);
    return true;
  }
}
}  // namespace wiki
