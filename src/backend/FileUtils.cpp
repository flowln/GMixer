#include "backend/FileUtils.h"

#include <glibmm/keyfile.h>

namespace FileUtils{
    std::unique_ptr<file_info> parseFile(Glib::RefPtr<Gio::File> file)
    {
        auto keyfile = Glib::KeyFile::create();
        if(!keyfile->load_from_file(file->get_path())){
            g_printerr("[ERROR] Could not load file %s.", file->get_basename().c_str());
            return {};
        }
        
        gchar* name = g_strdup(keyfile->get_string("Basic", "Name").c_str());
        gchar* command = g_strdup(keyfile->get_string("Basic", "Command").c_str());

        return std::make_unique<file_info>(name, command);
    }
}
