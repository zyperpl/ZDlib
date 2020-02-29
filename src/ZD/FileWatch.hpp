#pragma once

#include <memory>
#include <list>
#include <vector>
#include <unordered_set>
#include <functional>

class File;
class FileWatcher;

struct inotify_event;

enum FileEvent
{
  Access,
  CloseNoWrite,
  CloseWrite,
  Create,
  Delete,
  Modify,
  Moved,
  Open,
  Other
};

typedef std::function<void(const File &, std::unordered_set<FileEvent>)>
  FileCallback;

class FileWatcher
{
public:
  ~FileWatcher();

  static bool supported;

protected:
  FileWatcher(const File &file, FileCallback callback);
  static std::shared_ptr<FileWatcher> add(
    const File &file, FileCallback callback);

  void invoke();
  inline void add_event(const FileEvent &ev) { events.insert(ev); }
  inline bool has_pending_events() { return !events.empty(); }

  const File &file;
  std::list<FileCallback> callbacks;
  std::unordered_set<FileEvent> events;

private:
  int wd { -1 };
  int fd { -1 };

  friend void check_watchers();

  friend class File;
  friend class FileWatcherHandle;
};
