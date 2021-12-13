#pragma once

#include <glibmm/refptr.h>
#include <giomm/file.h>

namespace FileUtils{
    struct file_info{
        gchar* name;
        gchar* command;

        public:
            file_info(gchar* name, gchar* command) 
                : name    (std::move(name))
                , command (std::move(command))
            {}
            ~file_info(){ delete name; delete command; }
    };

    std::unique_ptr<file_info> parseFile(Glib::RefPtr<Gio::File>); 
}
