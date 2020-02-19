#include "File.hpp"
#include "FileWatch.hpp"

#include <cassert>
#include <cstring>
#include <sys/inotify.h>
#include <limits.h>
#include <unistd.h>
#include <vector>
#include <future>
#include <thread>
#include <chrono>
#include <mutex>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#pragma GCC optimize ("O3")

std::atomic<bool> watchers_thread_initialized{false};
std::mutex watchers_map_mutex;
static std::unordered_map<int /*wd*/, std::weak_ptr<FileWatcher>> watchers;
static std::unique_ptr<std::thread> watchers_thread;
static int inotify_fd{-1};

void initialize_inotify()
{
  if (inotify_fd != -1) return;

  inotify_fd = inotify_init();
  if (inotify_fd == -1)
  {
    perror("watchers inotify_init");
    fprintf(stderr, "inotify_init returned -1");
  }
}

std::unordered_set<FileEvent> get_events(uint32_t mask)
{
  std::unordered_set<FileEvent> events;
  if (mask & IN_ACCESS)         events.emplace(Access);  
  if (mask & IN_ATTRIB)         events.emplace(Other);
  if (mask & IN_CLOSE_NOWRITE)  events.emplace(CloseNoWrite);
  if (mask & IN_CLOSE_WRITE)    events.emplace(CloseWrite);
  if (mask & IN_CREATE)         events.emplace(Create);
  if (mask & IN_DELETE)         events.emplace(Delete);
  if (mask & IN_DELETE_SELF)    events.emplace(Delete);
  if (mask & IN_IGNORED)        events.emplace(Other);
  if (mask & IN_ISDIR)          events.emplace(Other);
  if (mask & IN_MODIFY)         events.emplace(Modify);
  if (mask & IN_MOVE_SELF)      events.emplace(Moved);
  if (mask & IN_MOVED_FROM)     events.emplace(Moved);
  if (mask & IN_MOVED_TO)       events.emplace(Moved);
  if (mask & IN_OPEN)           events.emplace(Open);
  if (mask & IN_Q_OVERFLOW)     events.emplace(Other);
  if (mask & IN_UNMOUNT)        events.emplace(Other);

  return events;
}

void print_inotify_event(struct inotify_event evt)
{
  printf("wd=%d\t", evt.wd);
  printf("mask=");
  if (evt.mask & IN_ACCESS)        printf("IN_ACCESS ");
  if (evt.mask & IN_ATTRIB)        printf("IN_ATTRIB ");
  if (evt.mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
  if (evt.mask & IN_CLOSE_WRITE)   printf("IN_CLOSE_WRITE ");
  if (evt.mask & IN_CREATE)        printf("IN_CREATE ");
  if (evt.mask & IN_DELETE)        printf("IN_DELETE ");
  if (evt.mask & IN_DELETE_SELF)   printf("IN_DELETE_SELF ");
  if (evt.mask & IN_IGNORED)       printf("IN_IGNORED ");
  if (evt.mask & IN_ISDIR)         printf("IN_ISDIR ");
  if (evt.mask & IN_MODIFY)        printf("IN_MODIFY ");
  if (evt.mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
  if (evt.mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
  if (evt.mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
  if (evt.mask & IN_OPEN)          printf("IN_OPEN ");
  if (evt.mask & IN_Q_OVERFLOW)    printf("IN_Q_OVERFLOW ");
  if (evt.mask & IN_UNMOUNT)       printf("IN_UNMOUNT ");
  printf("\n");
}

std::pair<std::vector<struct inotify_event>, size_t> read_inotify()
{
  std::vector<struct inotify_event> inotify_events(32);

  ssize_t num = read(
      inotify_fd, 
      inotify_events.data(), 
      inotify_events.size()*sizeof(struct inotify_event)
    ); 
  if (num == 0) {
    fprintf(stderr, "inotify read returned 0!\n");
  }
  if (num == -1) {
    fprintf(stderr, "inotify error: read returned -1!\n");
  }
  //printf("Read %zu bytes from inotify read.\n", num);
  size_t num_of_events = num/sizeof(struct inotify_event);
  assert(num_of_events <= inotify_events.size());

  return {inotify_events, num_of_events};
}

void check_watchers() 
{
  assert(inotify_fd != -1);
  watchers_thread_initialized = true;
  printf("Checking watchers...\n");

  while (watchers.size() > 0)
  {
    //printf("Checking %zu watchers...\n", watchers.size());
    const auto &[inotify_events, num_of_events] = read_inotify();

    if (!watchers_map_mutex.try_lock()) continue;

    if (watchers.empty()) break;

    //printf("num_of_events=%lu\n", num_of_events);
    for (size_t i = 0; i < num_of_events; i++)
    {
      struct inotify_event evt = inotify_events.at(i);
      auto wd = evt.wd;

      assert(watchers.size() > 0);
      if (!watchers.contains(wd)) break;

      auto watcher = watchers.at(wd).lock();
      if (watcher == nullptr) break;
      
      auto events = get_events(evt.mask);
      assert(!events.empty());
      for (const auto &e : events) { 
        watcher->add_event(e);
      }
      //print_inotify_event(evt);
    }

    for (const auto &wd_watcher : watchers)
    {
      const auto watcher = wd_watcher.second.lock();
      if (watcher == nullptr) continue;

      if (watcher->has_pending_events()) {
        watcher->invoke();
      }
    }

    watchers_map_mutex.unlock();
  }
  watchers_thread_initialized = false;
  printf("Watchers thread done.\n");
}

void initialize_watchers_thread()
{
  assert(inotify_fd != -1);
  if (!watchers_thread_initialized.load() && watchers_thread == nullptr)
  {
    watchers_thread_initialized = true;
    watchers_thread = std::make_unique<std::thread>(check_watchers);
  }
}

void try_kill_watchers_thread()
{
  if (watchers_thread && watchers.empty()) {
    printf("Waiting for watchers thread..\n");
    watchers_thread->join();
    watchers_thread = nullptr;
  }
}

std::shared_ptr<FileWatcher> FileWatcher::add(const File &file, FileCallback callback)
{
  initialize_inotify();
  std::shared_ptr<FileWatcher> file_watcher;
  {
    for (const auto &wd_watcher : watchers)
    {
      const auto watcher = wd_watcher.second.lock();
      if (watcher->file.get_name() == file.get_name()) {
        watcher->callbacks.push_back(callback);
        printf("Added callback to existing watcher.\n");
        file_watcher = watcher;
        break;
      }
    }
    if (file_watcher == nullptr) 
    {
      file_watcher.reset(new FileWatcher(file, callback) );
      watchers.emplace(file_watcher->wd, file_watcher);
      printf("Created new watcher.\n");
    }

    assert(file_watcher != nullptr);
  }
  initialize_watchers_thread();

  return file_watcher;
}

FileWatcher::FileWatcher(const File &file, FileCallback callback)
  : file{file}, fd{file.get_fd()}
{
  callbacks.push_back(callback);

  wd = inotify_add_watch(inotify_fd, file.get_name().data(), IN_ALL_EVENTS);
  if (wd == -1) 
  {
    fprintf(stderr, "Cannot inotify_add_watch\n");
  }
  //printf("Added FileWatcher wd=%d for file fd=%d\n", wd, fd);
  assert(wd != -1);
}

FileWatcher::~FileWatcher()
{
  std::scoped_lock<std::mutex> lock(watchers_map_mutex);
  printf("Removing watcher %p for file %s\n", this, file.get_name().data());
  assert(inotify_fd != -1);
  watchers.erase(wd);
  inotify_rm_watch(inotify_fd, wd);
  try_kill_watchers_thread();
}

void FileWatcher::invoke()
{
  for (const auto &callback : callbacks)
  {
    callback(file, events);
  }
  events.clear();
}
