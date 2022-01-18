#include "client/gtk/graph/Property.h"

using namespace GMixer;

void Property::addField(const std::string& name, const std::string& property, GType type)
{
    field pair {type, property};
    try{
        properties.at(name) = pair;
    } catch(const std::out_of_range& oor){
        if(!properties.emplace(name, pair).second)
            printf("Could not insert field %s in property %s\n", name.c_str(), getName().c_str());
    }
}
void Property::updateField(std::string&& name, const std::string& new_value)
{
    if(!signal_property_update.empty() && signal_property_update(this, name, new_value))
        addField(name, new_value, getType(name));
}

GType Property::getType(const std::string& name) const
{
    try{
        return properties.at(name).first;
    } catch(const std::out_of_range& oor){
        return {};
    }
}

const std::string Property::getField(std::string&& name) const
{
    try{
        return properties.at(name).second;
    } catch(const std::out_of_range& oor){
        return "";
    }
}

void PropertyList::destroy()
{
    for(auto val : m_list) 
        if(val->dependent) delete val;

    delete this;
}

void PropertyList::add(Property* prop)
{
    m_list.emplace_front(prop);
}
