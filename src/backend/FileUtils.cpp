#include "backend/FileUtils.h"

#include <glibmm/keyfile.h>

namespace FileUtils{
    std::unique_ptr<file_info> parseFile(Glib::RefPtr<Gio::File> file)
    {
        auto keyfile = Glib::KeyFile::create();
        
        try{
            keyfile->load_from_file(file->get_path());
        }
        catch(GFileError& e){
            g_printerr("[ERROR] Unable to load file %s.\n", file->get_basename().c_str()); 
            return nullptr;
        }
        catch(Glib::KeyFileError& e){
            g_printerr("[ERROR] File %s is not of valid format.\n", file->get_basename().c_str());
            return nullptr;
        }

        gchar* name = g_strdup(keyfile->get_string("Basic", "Name").c_str());
        gchar* command = g_strdup(keyfile->get_string("Basic", "Command").c_str());

        return std::make_unique<file_info>(name, command);
    }

    bool saveFile(const Glib::ustring& file, const file_info info)
    {
        auto keyfile = Glib::KeyFile::create();
        keyfile->set_string("Basic", "Name", info.name);
        keyfile->set_string("Basic", "Command", info.command);

        try {
            keyfile->save_to_file(file);
        } catch (GFileError& e) {
            g_printerr("[ERROR] Could not save pipeline at file %s.\n", file.c_str());
            return false;
        }

        return true;
    }
}
