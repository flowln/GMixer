#pragma once

#include <giomm/file.h>
#include <glibmm/refptr.h>

namespace FileUtils {
struct file_info {
    const char* name;
    const char* command;

   public:
    file_info(const char* name, const char* command) : name(name), command(command) {}
};

std::shared_ptr<file_info> parseFile(Glib::RefPtr<Gio::File>);
bool saveFile(const Glib::ustring&, const file_info);
}  // namespace FileUtils
