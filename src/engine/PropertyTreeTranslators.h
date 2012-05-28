#ifndef PROPERTYTREETRANSLATORS_H
#define PROPERTYTREETRANSLATORS_H

#include "EngineTypedefs.h"
#include "IrrlichtTools.h"

namespace boost
{
namespace property_tree
{

struct VectorTranslator
{
    typedef std::string             internal_type;
    typedef irr::core::vector3df    external_type;

    boost::optional<external_type> get_value( internal_type const &v )
    {
        irr::core::vector3df vec;
        std::stringstream stream;
        stream << v;
        stream >> vec;
        return vec;
    }

    boost::optional<internal_type> put_value( external_type const& v )
    {        std::stringstream stream;
        stream << v;
        return stream.str();
    }
};

template<>
struct translator_between<std::string, irr::core::vector3df>
{
    typedef VectorTranslator type;
};

}
}

#endif // PROPERTYTREETRANSLATORS_H
