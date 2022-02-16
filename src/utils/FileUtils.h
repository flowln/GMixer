#pragma once

#include <glibmm/refptr.h>
#include <giomm/file.h>

namespace FileUtils{
    struct file_info{
        const gchar* name;
        const gchar* command;

        public:
            file_info(const gchar* name, const gchar* command)
                : name(name), command(command)
            {}
    };

    std::shared_ptr<file_info> parseFile(Glib::RefPtr<Gio::File>); 
    bool saveFile(const Glib::ustring&, const file_info); 
}
